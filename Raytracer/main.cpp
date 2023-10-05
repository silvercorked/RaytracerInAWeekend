
#include <fstream>
#include <iostream>

#include "Vec3.h"
#include "Color.h"
#include "Ray.h"

/*
	Sphere: (x - cx)^2 + (y - cy)^2 + (z - cz)^2 = R^2, where cx, cy, and cz are the circle's center location
	C = (cx, cy, cz), P = (x, y, z) => (P-C) * (P-C) = R^2
		if (P-C) . (P-C) < R^2, P is inside the sphere, if > outside, if =, on the sphere
	use Ray equation for P, P(t) = A + t * b
	(A + t * b - C) . (A + t * b - C) = R^2
	t^2*b.b + 2*t*b . (A-C) + (A-C) . (A-C) - R^2 = 0
		0 roots (sqrt(some -))
		1 root (root is zero)
		2 roots (roots are positive)
*/
bool hitSphere(const Point3& center, double radius, const Ray& r) {
	Vec3 oc = r.origin() - center; // A-C
	auto a = dot(r.direction(), r.direction());	// b . b 
	auto b = 2.0 * dot(oc, r.direction());		// 2 * b . (A-C)
	auto c = dot(oc, oc) - radius * radius;		// (A-C) . (A-C) - R^2
	auto underRadical = b * b - 4 * a * c;
	return underRadical >= 0;
}

Color rayColor(const Ray& r) {
	if (hitSphere(Point3(0, 0, -1), 0.5, r))	// red sphere at 0,0,-1 with r = 0.5
		return Color(1, 0, 0);					// where the red is from
	Vec3 unitDirection = unitVector(r.direction());
	auto t = 0.5 * (unitDirection.y() + 1.0); // shift y between 0.5 and 1 for some nice values
	return (1.0 - t) * Color{1.0, 1.0, 1.0} + t * Color{0.5, 0.7, 1.0}; // lerp blue and white
}

int main() {


	// Image
	const auto aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);

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
			Color pixelColor = rayColor(r);
			writeColor(outImage, pixelColor);
		}
	}
	outImage.close();
	std::cout << "\nDone.\n";
}