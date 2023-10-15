#pragma once

#include "common.hpp"
#include "Material.hpp"

#include <fstream>

class Camera {
	int imageHeight;			// rendered image height
	Point3 center;				// camera center
	Point3 pixel00Location;		// location of pixel 0, 0
	Vec3 pixelDeltaU;			// offset to pixels to the right
	Vec3 pixelDeltaV;			// offset to pixels below
	Vec3 u, v, w;				// Camera frame basis vectors (looking towards -w)
public:
	double aspectRatio = 1.0;	// Ratio of image width over height
	int imageWidth = 100;		// Rendered image width in pixel count
	int samplePerPixel = 10;	// Count of random samples for each pixel
	int maxDepth = 10;			// Maximum number of ray bounces into scene
	
	double vfov = 90; // Vertical view angle (field of view)
	Point3 lookFrom = Point3(0, 0, -1);	// Point camera is looking from
	Point3 lookAt = Point3(0, 0, 0);		// Point camera is looking at
	Vec3 vUp = Vec3(0, 1, 0);			// Camera-relative "up" direction

	auto render(const Hittable& world) -> void {
		this->initialize();
		std::ofstream outImage;
		outImage.open("out/image.ppm", std::ios::out | std::ios::trunc);
		outImage << "P3\n" << this->imageWidth << ' ' << this->imageHeight << "\n255\n";
		for (int j = 0; j < this->imageHeight; j++) {
			std::cout << "\rScalines remaining: " << (this->imageHeight - j) << ' ' << std::flush;
			for (int i = 0; i < this->imageWidth; i++) {
				Color pixelColor(0, 0, 0);
				for (int sample = 0; sample < samplePerPixel; sample++) {
					Ray r = getRay(i, j);
					pixelColor += rayColor(r, this->maxDepth, world);
				}
				writeColor(outImage, pixelColor, this->samplePerPixel);
			}
		}
		outImage.close();
		std::cout << "\nDone.\n";
	}
private:
	auto initialize() -> void {
		this->imageHeight = static_cast<int>(this->imageWidth / this->aspectRatio);
		this->imageHeight = (this->imageHeight < 1) ? 1 : this->imageHeight;
		
		this->center = this->lookFrom;

		// Temp to avoid recomputation of this->lookFrom - this->lookAt twice
		this->w = this->lookFrom - this->lookAt;

		// Determine viewport dimensions.
		/*
			|   
			|  /|
			| / | h
			|/  |
			|___| -z
			 \theta/2
			  \
			   \
			2 * h = 2 * tan(theta / 2)
		*/
		auto focalLen = (this->w).length();
		auto theta = degreesToRadians(this->vfov);
		auto h = tan(theta / 2);
		auto viewportHeight = 2 * h * focalLen;
		auto viewportWidth = viewportHeight * (static_cast<double>(this->imageWidth) / this->imageHeight);

		// Calculate the u,v,w unit basis vectors for the camera coordinate frame.
		this->w = unitVector(this->w);				// looking toward -w
		this->u = unitVector(cross(this->vUp, w));	// camera right
		this->v = cross(w, u);						// camera up (vUp != v, v is basis vector based on cam orientation, vUp is const)

		// Calculate the vectors acrros the horizontal and down viewport edges.
		auto viewportU = viewportWidth * this->u;	// Vector across viewport horizontal edge
		auto viewportV = viewportHeight * -v;		// Vector down viewport horizontal edge

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		this->pixelDeltaU = viewportU / this->imageWidth;
		this->pixelDeltaV = viewportV / this->imageHeight;

		// Calculate the location of the upper left pixel.
		auto viewportUpperLeft = this->center - (focalLen * this->w) - viewportU / 2 - viewportV / 2;
		this->pixel00Location = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
	}
	auto getRay(int i, int j) const -> Ray {
		// get a randomly sampled camera ray for the pixel at location i,j.
		auto pixelCenter = this->pixel00Location + (i * this->pixelDeltaU) + (j * this->pixelDeltaV);
		auto pixelSample = pixelCenter + pixelSampleSquare();
		auto rayOrigin = this->center;
		auto rayDir = pixelSample - rayOrigin;
		return Ray(rayOrigin, rayDir);
	}
	auto pixelSampleSquare() const -> Vec3 {
		// returns a random point in the square surrouding a pixel at the origin
		auto px = -0.5 + randomDouble();
		auto py = -0.5 + randomDouble();
		return (px * this->pixelDeltaU) + (py * this->pixelDeltaV);
	}
	auto rayColor(const Ray& r, int depth, const Hittable& world) const -> Color {
		HitRecord rec;

		if (depth <= 0) // stop gathering if max depth
			return Color(0, 0, 0);

		if (world.hit(r, Interval(0.001, infinity), rec)) {
			Ray scattered;
			Color attenuation;
			if (rec.material->scatter(r, rec, attenuation, scattered))
				return attenuation * this->rayColor(scattered, depth - 1, world);
			return Color(0, 0, 0);
		}
		Vec3 unitDir = unitVector(r.direction());
		auto a = 0.5 * (unitDir.y() + 1.0);
		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0); // linear interpolate
	}
};
