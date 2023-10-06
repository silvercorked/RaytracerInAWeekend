
#include <fstream>
#include <iostream>

#include "common.hpp"

#include "Color.hpp"
#include "HittableList.hpp"
#include "Sphere.hpp"
#include "Camera.hpp"
#include "Material.hpp"

auto rayColor(const Ray& r, const Hittable& world, int depth) -> Color {
	HitRecord rec;

	if (depth <= 0)
		return Color(0, 0, 0);
	if (world.hit(r, 0.001, infinity, rec)) { // 0.001 as min to avoid hitting where we just hit
		Ray scattered;
		Color attenuation;
		if (rec.matPtr->scatter(r, rec, attenuation, scattered))
			return attenuation * rayColor(scattered, world, depth - 1);
		return Color(0, 0, 0);
	}
	Vec3 unitDirection = unitVector(r.direction());
	auto t = 0.5 * (unitDirection.y() + 1.0); // shift y between 0.5 and 1 for some nice values
	return (1.0 - t) * Color{1.0, 1.0, 1.0} + t * Color{0.5, 0.7, 1.0}; // lerp blue and white
}

int main() {
	// Image
	const auto aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	const int samplesPerPixel = 100;
	const int maxDepth = 50;

	// World
	HittableList world;
	auto materialGround = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
	auto materialCenter = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
	auto materialLeft = make_shared<Dielectric>(1.5);
	auto materialRight = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100, materialGround));	// simluated floor
	world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5, materialCenter));		// actual circle
	world.add(make_shared<Sphere>(Point3(-1.0, 0, -1), 0.5, materialLeft));		// actual circle
	world.add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), -0.4, materialLeft)); // hollow sphere
	world.add(make_shared<Sphere>(Point3(1.0, 0, -1), 0.5, materialRight));		// actual circle

	// Camera
	Camera cam;

	// Render
	std::ofstream outImage;
	outImage.open("out/image.ppm", std::ios::out | std::ios::trunc);
	outImage << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
	for (int j = imageHeight - 1; j >= 0; j--) {
		std::cout << "\rScalines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < imageWidth; i++) {
			Color pixelColor(0, 0, 0);
			for (int s = 0; s < samplesPerPixel; s++) {
				auto u = (i + randomDouble()) / (imageWidth - 1);
				auto v = (j + randomDouble()) / (imageHeight - 1);
				Ray r = cam.getRay(u, v);
				pixelColor += rayColor(r, world, maxDepth);
			}
			writeColor(outImage, pixelColor, samplesPerPixel);
		}
	}
	outImage.close();
	std::cout << "\nDone.\n";
}