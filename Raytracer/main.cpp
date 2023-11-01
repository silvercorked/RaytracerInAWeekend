
#include <fstream>
#include <iostream>
#include <chrono>

#include "common.hpp"

#include "Color.hpp"
#include "HittableList.hpp"
#include "Sphere.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "BoundingVolumeHierarchy.hpp"
#include "Texture.hpp"
#include "Quad.hpp"

auto randomSpheres() -> void {
	auto start = std::chrono::high_resolution_clock::now();
	// World
	HittableList world;

	auto groundMaterial = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
	world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMaterial));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto chooseMat = randomDouble();
			Point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());
			if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<Material> sphereMat;
				if (chooseMat < 0.8) { // Diffuse
					auto albedo = Color::random() * Color::random();
					sphereMat = make_shared<Lambertian>(albedo);
					auto center2 = center + Vec3(0, randomDouble(0, 0.5), 0);
					world.add(make_shared<Sphere>(center, center2, 0.2, sphereMat));
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

	world = HittableList(make_shared<BoundingVolumeHierarchyNode>(world));

	// Camera
	Camera cam;

	cam.aspectRatio = 16.0 / 9.0;
	cam.imageWidth = 400;
	cam.samplePerPixel = 100;
	cam.maxDepth = 50;

	cam.vfov = 20;
	cam.lookFrom = Point3(13, 2, 3);
	cam.lookAt = Point3(0, 0, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0.02;
	cam.focusDistance = 10.0;

	cam.render(world);

	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

auto twoSpheres() -> void {
	auto start = std::chrono::high_resolution_clock::now();
	HittableList world;

	auto checker = make_shared<CheckerTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

	world.add(make_shared<Sphere>(Point3(0, -10, 0), 10, make_shared<Lambertian>(checker)));
	world.add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<Lambertian>(checker)));
	
	Camera cam;
	cam.aspectRatio = 16.0 / 9.0;
	cam.imageWidth = 400;
	cam.samplePerPixel = 100;
	cam.maxDepth = 50;

	cam.vfov = 20;
	cam.lookFrom = Point3(13, 2, 3);
	cam.lookAt = Point3(0, 0, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(world);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

auto earth() -> void {
	auto start = std::chrono::high_resolution_clock::now();
	auto earthTexture = make_shared<ImageTexture>("earthmap.jpg");
	auto earthSurface = make_shared<Lambertian>(earthTexture);
	auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2, earthSurface);

	Camera cam;
	cam.aspectRatio = 16.0 / 9.0;
	cam.imageWidth = 400;
	cam.samplePerPixel = 100;
	cam.maxDepth = 50;

	cam.vfov = 20;
	cam.lookFrom = Point3(0, 0, 12);
	cam.lookAt = Point3(0, 0, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(HittableList(globe));
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

auto twoPerlinSpheres() -> void {
	auto start = std::chrono::high_resolution_clock::now();
	HittableList world;

	auto perlinTexture = make_shared<NoiseTexture>(4);

	world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(perlinTexture)));
	world.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(perlinTexture)));

	Camera cam;
	cam.aspectRatio = 16.0 / 9.0;
	cam.imageWidth = 400;
	cam.samplePerPixel = 100;
	cam.maxDepth = 50;

	cam.vfov = 20;
	cam.lookFrom = Point3(13, 2, 3);
	cam.lookAt = Point3(0, 0, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(world);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

auto quads() -> void {
	auto start = std::chrono::high_resolution_clock::now();
	HittableList world;
	auto leftRed = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
	auto backGreen = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
	auto rightBlue = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
	auto upperOrange = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
	auto lowerTeal = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));
	world.add(make_shared<Quad>(Point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0), leftRed));
	world.add(make_shared<Quad>(Point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), backGreen));
	world.add(make_shared<Quad>(Point3(3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), rightBlue));
	world.add(make_shared<Quad>(Point3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upperOrange));
	world.add(make_shared<Quad>(Point3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0, -4), lowerTeal));

	Camera cam;
	cam.aspectRatio = 1.0;
	cam.imageWidth = 400;
	cam.samplePerPixel = 100;
	cam.maxDepth = 50;

	cam.vfov = 80;
	cam.lookFrom = Point3(0, 0, 9);
	cam.lookAt = Point3(0, 0, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(world);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

int main() {
	switch (5) {
		case 1: randomSpheres(); break;
		case 2: twoSpheres(); break;
		case 3: earth(); break;
		case 4: twoPerlinSpheres(); break;
		case 5: quads(); break;
	}
}
