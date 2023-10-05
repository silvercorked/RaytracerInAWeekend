
#include <fstream>
#include <iostream>

#include "common.hpp"

#include "Color.hpp"
#include "HittableList.hpp"
#include "Sphere.hpp"

auto rayColor(const Ray& r, const Hittable& world) -> Color {
	HitRecord rec;
	if (world.hit(r, 0, infinity, rec))
		return 0.5 * (rec.Normal + Color(1, 1, 1));
	Vec3 unitDirection = unitVector(r.direction());
	auto t = 0.5 * (unitDirection.y() + 1.0); // shift y between 0.5 and 1 for some nice values
	return (1.0 - t) * Color{1.0, 1.0, 1.0} + t * Color{0.5, 0.7, 1.0}; // lerp blue and white
}

int main() {
	// Image
	const auto aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);

	// World
	HittableList world;
	world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));		// actual circle
	world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));	// simluated floor

	// Camera
	auto viewportHeight = 2.0;
	auto viewportWidth = aspectRatio * viewportHeight;
	auto focalLength = 1.0;

	auto origin = Point3{ 0, 0, 0 };
	auto horizontal = Vec3{ viewportWidth, 0, 0 }; // + in x
	auto vertical = Vec3{0, viewportHeight, 0}; // + in y
	auto lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - Vec3{0, 0, focalLength};

	// Render
	std::ofstream outImage;
	outImage.open("out/image.ppm", std::ios::out | std::ios::trunc);
	outImage << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";
	for (int j = imageHeight - 1; j >= 0; j--) {
		std::cout << "\rScalines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < imageWidth; i++) {
			auto u = ((double) i) / (imageWidth - 1);
			auto v = ((double) j) / (imageHeight - 1);
			Ray r{ origin, lowerLeftCorner + u * horizontal + v * vertical - origin };
			Color pixelColor = rayColor(r, world);
			writeColor(outImage, pixelColor);
		}
	}
	outImage.close();
	std::cout << "\nDone.\n";
}