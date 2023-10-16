
#include <fstream>
#include <iostream>
#include <chrono>

#include "common.hpp"

#include "Color.hpp"
#include "HittableList.hpp"
#include "Sphere.hpp"
#include "Camera.hpp"
#include "Material.hpp"

int main() {
	auto start = std::chrono::high_resolution_clock::now();
	// World
	HittableList world;

	auto groundMat = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
	world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMat));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto chooseMat = randomDouble();
			Point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());
			if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<Material> sphereMat;
				if (chooseMat < 0.8) { // Diffuse
					auto albedo = Color::random() * Color::random();
					sphereMat = make_shared<Lambertian>(albedo);
					world.add(make_shared<Sphere>(center, 0.2, sphereMat));
				}
				else if (chooseMat < 0.95) { // metal
					auto albedo = Color::random(0.5, 1);
					auto fuzz = randomDouble(0, 0.5);
					sphereMat = make_shared<Metal>(albedo, fuzz);
					world.add(make_shared<Sphere>(center, 0.2, sphereMat));
				}
				else { // glass
					sphereMat = make_shared<Dielectric>(1.5);
					world.add(make_shared<Sphere>(center, 0.2, sphereMat));
				}
			}
		}
	}

	auto material1 = make_shared<Dielectric>(1.5);
	world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));
	auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
	world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));
	auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

	// Camera
	Camera cam;

	cam.aspectRatio = 16.0 / 9.0;
	cam.imageWidth = 1200;
	cam.samplePerPixel = 500;
	cam.maxDepth = 50;

	cam.vfov = 20;
	cam.lookFrom = Point3(13, 2, 3);
	cam.lookAt = Point3(0, 0, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0.6;
	cam.focusDistance = 10.0;

	cam.render(world);

	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::microseconds>(end - start) << std::endl;
}