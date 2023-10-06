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
			scatterDir = rec.Normal + randomUnitVector();
		else
			scatterDir = randomInHemisphere(rec.Normal);
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
		Vec3 reflected = reflect(unitVector(rIn.direction()), rec.Normal);
		scattered = Ray(rec.p, reflected + fuzz * randomInUnitSphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.Normal) > 0);
	}
};
