
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
#include "ConstantMedium.hpp"
#include "BoundingVolumeHierarchy.hpp"

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
	cam.background = Color(0.7, 0.8, 1.0);

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
	cam.background = Color(0.7, 0.8, 1.0);

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
	cam.background = Color(0.7, 0.8, 1.0);

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
	cam.background = Color(0.7, 0.8, 1.0);

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
	cam.background = Color(0.7, 0.8, 1.0);

	cam.vfov = 80;
	cam.lookFrom = Point3(0, 0, 9);
	cam.lookAt = Point3(0, 0, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(world);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

auto simpleLight() -> void {
	auto start = std::chrono::high_resolution_clock::now();
	HittableList world;

	auto pertext = make_shared<NoiseTexture>(4);
	world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
	world.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

	auto diffLight = make_shared<DiffuseLight>(Color(4, 4, 4)); // going outside 0-1 range to scale light intensity
	world.add(make_shared<Sphere>(Point3(0, 7, 0), 2, diffLight));
	world.add(make_shared<Quad>(Point3(3, 1, -2), Vec3(2, 0, 0), Vec3(0, 2, 0), diffLight));

	Camera cam;
	cam.aspectRatio = 16.0 / 9.0;
	cam.imageWidth = 400;
	cam.samplePerPixel = 100;
	cam.maxDepth = 50;
	cam.background = Color(0.0, 0.0, 0.0);

	cam.vfov = 20;
	cam.lookFrom = Point3(26, 3, 6);
	cam.lookAt = Point3(0, 2, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(world);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

auto cornellBox() -> void {
	auto start = std::chrono::high_resolution_clock::now();
	HittableList world;

	auto red = make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
	auto white = make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
	auto green = make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
	auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

	world.add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
	world.add(make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
	world.add(make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
	world.add(make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.add(make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
	world.add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

	shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 330, 165), white);
	box1 = make_shared<Rotate>(box1, Vec3(0, 15, 0)); // 0 degrees in x, 15 degrees in y, 0 degrees in z
	box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
	world.add(box1);
	shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), white);
	box2 = make_shared<Rotate>(box2, Vec3(0, -18, 0));
	box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
	world.add(box2);

	Camera cam;
	cam.aspectRatio = 1.0;
	cam.imageWidth = 600;
	cam.samplePerPixel = 64;
	cam.maxDepth = 50;
	cam.background = Color(0.0, 0.0, 0.0);

	cam.vfov = 40;
	cam.lookFrom = Point3(278, 278, -800);
	cam.lookAt = Point3(278, 278, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(world);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

auto cornellSmoke() -> void {
	auto start = std::chrono::high_resolution_clock::now();
	HittableList world;

	auto red = make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
	auto white = make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
	auto green = make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));

	world.add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
	world.add(make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
	world.add(make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
	world.add(make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.add(make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
	world.add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

	shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 330, 165), white);
	box1 = make_shared<Rotate>(box1, Vec3(0, 15, 0)); // 0 degrees in x, 15 degrees in y, 0 degrees in z
	box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
	shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), white);
	box2 = make_shared<Rotate>(box2, Vec3(0, -18, 0));
	box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
	
	world.add(make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
	world.add(make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));

	Camera cam;
	cam.aspectRatio = 1.0;
	cam.imageWidth = 600;
	cam.samplePerPixel = 400;
	cam.maxDepth = 50;
	cam.background = Color(0.0, 0.0, 0.0);

	cam.vfov = 40;
	cam.lookFrom = Point3(278, 278, -800);
	cam.lookAt = Point3(278, 278, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(world);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Time(ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;
}

void finalScene(int imageWidth, int samplesPerPixel, int maxDepth) {
	HittableList boxes1;
	auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

	int boxesPerSide = 20;
	for (int i = 0; i < boxesPerSide; i++) {
		for (int j = 0; j < boxesPerSide; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = randomDouble(1, 101);
			auto z1 = z0 + w;

			boxes1.add(box(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
		}
	}

	HittableList world;

	world.add(make_shared<BoundingVolumeHierarchyNode>(boxes1));

	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
	world.add(make_shared<Quad>(Point3(123, 554, 147), Vec3(300, 0, 0), Vec3(0, 0, 265), light));

	auto center1 = Point3(400, 400, 200);
	auto center2 = center1 + Vec3(30, 0, 0);
	auto sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
	world.add(make_shared<Sphere>(center1, center2, 50, sphere_material));

	world.add(make_shared<Sphere>(Point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
	world.add(make_shared<Sphere>(
		Point3(0, 150, 145), 50, make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)
	));

	auto boundary = make_shared<Sphere>(Point3(360, 150, 145), 70, make_shared<Dielectric>(1.5));
	world.add(boundary);
	world.add(make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
	boundary = make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
	world.add(make_shared<ConstantMedium>(boundary, .0001, Color(1, 1, 1)));

	auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("earthmap.jpg"));
	world.add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
	auto pertext = make_shared<NoiseTexture>(0.1);
	world.add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

	HittableList boxes2;
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<Sphere>(Point3::random(0, 165), 10, white));
	}

	world.add(make_shared<Translate>(
		make_shared<Rotate>(
			make_shared<BoundingVolumeHierarchyNode>(boxes2), Vec3(0, 15, 0)),
		Vec3(-100, 270, 395)
	));

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.imageWidth = imageWidth;
	cam.samplePerPixel = samplesPerPixel;
	cam.maxDepth = maxDepth;
	cam.background = Color(0, 0, 0);

	cam.vfov = 40;
	cam.lookFrom = Point3(478, 278, -600);
	cam.lookAt = Point3(278, 278, 0);
	cam.vUp = Vec3(0, 1, 0);

	cam.defocusAngle = 0;

	cam.render(world);
}


int main() {
	switch (7) {
		case 1: randomSpheres(); break;
		case 2: twoSpheres(); break;
		case 3: earth(); break;
		case 4: twoPerlinSpheres(); break;
		case 5: quads(); break;
		case 6: simpleLight(); break;
		case 7: cornellBox(); break;
		case 8: cornellSmoke(); break;
		case 9: finalScene(800, 7500, 40); break;
		default: finalScene(400, 250, 4); break;
	}
}
