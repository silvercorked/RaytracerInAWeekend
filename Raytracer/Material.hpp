#pragma once

#include "common.hpp"

constexpr const bool USE_LAMBERTIAN_DIFFUSE = true;

struct HitRecord; // forward declaration

struct Material {
	virtual auto scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const -> bool = 0;
};

// Diffuse Material
struct Lambertian : public Material {
	Color albedo;

	Lambertian(const Color& a) : albedo{ a } {}

	virtual auto scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const -> bool override {
		Vec3 scatterDir;
		if constexpr (USE_LAMBERTIAN_DIFFUSE)
			scatterDir = rec.Normal + randomUnitVector();	// Lambertian image 2 unit sphere tangent to the surface. pick sphere
		else												// on same normal side, get random vector that is within, then go from 
			scatterDir = randomInHemisphere(rec.Normal);	// hit point to random vector. else case is no Lambertian
		if (scatterDir.nearZero()) scatterDir = rec.Normal; // avoid generating a zero vector
		scattered = Ray(rec.p, scatterDir);
		attenuation = albedo;
		return true;
	}
};

// Metallic Material
struct Metal : public Material {
	Color albedo;
	double fuzz;

	Metal(const Color& a, double f) : albedo{ a }, fuzz{f < 1 ? f : 1} {}

	virtual auto scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const -> bool override {
		Vec3 reflected = reflect(unitVector(rIn.direction()), rec.Normal);	// metallic rays are reflected
		scattered = Ray(rec.p, reflected + fuzz * randomInUnitSphere());	// jiggle a bit to cause increasing fuzziness w/ anti-aliasing
		attenuation = albedo;
		return (dot(scattered.direction(), rec.Normal) > 0);
	}
};

struct Dielectric : public Material {
	double ir; // index of refraction

	Dielectric(double indexOfRefraction) : ir{ indexOfRefraction } {}

	virtual auto scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const -> bool override {
		attenuation = Color(1.0, 1.0, 1.0);
		double refractionRatio = rec.frontFace ? (1.0 / ir) : ir;
		Vec3 unitDir = unitVector(rIn.direction());
		// if eta2 > eta1 broken inequality
		// eta2/eta1 * sin theta2 > 1.0 (as sin theta1 cant be bigger than 1).
		// in these cases, must reflect (total internal reflection)
		double cosTheta = fmin(dot(-unitDir, rec.Normal), 1.0);	// trig R * n = cos theta
		double sinTheta = sqrt(1.0 - cosTheta * cosTheta);		// trig sin theta = sqrt(1-cos^2(theta))
		bool cannotRefract = refractionRatio * sinTheta > 1.0;
		Vec3 dir;
		if (cannotRefract || reflectance(cosTheta, refractionRatio) > randomDouble())
			dir = reflect(unitDir, rec.Normal);
		else
			dir = refract(unitDir, rec.Normal, refractionRatio);
		scattered = Ray(rec.p, dir);
		return true;
	}
private:
	static auto reflectance(double cosine, double refractiveIndex) -> double {
		// Schlick's approximation cause real equation is nasty
		auto r0 = (1 - refractiveIndex) / (1 + refractiveIndex);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};
