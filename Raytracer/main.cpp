
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

	auto R = cos(pi / 4);

	auto materialGround = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
	auto materialCenter = make_shared<Lambertian>(Color(0.3, 0.2, 0.5));
	auto materialLeft = make_shared<Dielectric>(1.5);
	auto materialRight = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

	world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100, materialGround));	// simluated floor
	world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5, materialCenter));		// actual circle
	world.add(make_shared<Sphere>(Point3(-1.0, 0, -1), 0.5, materialLeft));		// actual circle
	world.add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), -0.4, materialLeft)); // hollow sphere
	world.add(make_shared<Sphere>(Point3(1.0, 0, -1), 0.5, materialRight));		// actual circle

	// Camera
	Camera cam;

	cam.aspectRatio = 16.0 / 9.0;
	cam.imageWidth = 800;
	cam.samplePerPixel = 50;
	cam.maxDepth = 50;

	cam.vfov = 20;
	cam.lookFrom = Point3(-2, 2, 1);
	cam.lookAt = Point3(0, 0, -1);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 10;
	cam.focusDistance = 3.4;

	cam.render(world);

	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::microseconds>(end - start) << std::endl;
}