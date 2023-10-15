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
	Vec3 defocusDiskU;			// Defocus disk horizontal radius
	Vec3 defocusDiskV;			// Defocus disk vertical radius
public:
	double aspectRatio = 1.0;	// Ratio of image width over height
	int imageWidth = 100;		// Rendered image width in pixel count
	int samplePerPixel = 10;	// Count of random samples for each pixel
	int maxDepth = 10;			// Maximum number of ray bounces into scene
	
	double vfov = 90; // Vertical view angle (field of view)
	Point3 lookFrom = Point3(0, 0, -1);	// Point camera is looking from
	Point3 lookAt = Point3(0, 0, 0);		// Point camera is looking at
	Vec3 vUp = Vec3(0, 1, 0);			// Camera-relative "up" direction

	double defocusAngle = 0;			// Variation angle of rays through each pixel
	double focusDistance = 10;			// Distance from Camera lookFrom point to plane of perfect focus

	/* Defocus Blur (Depth of Field) (Thin Lens approximation)
		Focus plane is orthogonal to the camera view direction
		Focus distance is the distance between the camera center and the focus plane
		Viewport lies on the focus plane, centered on the camera view direction vector
		Grid of pixels locations lies inside the viewport (in the 3d world)
		Random image sample locations are chosen from the region around the current pixel location
		Camera fires rays from random points on the lens through the current image sample location
	*/

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

		// Determine viewport dimensions.
		/*
			|   
			|  /|
			| / | h
			|/  |
			|___| -z	(at -z in this image, is the focus plane (and in this model, the image plane is at the same distance)
			 \theta/2					// focal length = focal distance in this model
			  \								// (length -> dist between cam center and image plane)
			   \							// (distance -> dist between cam center and focal plane)
			2 * h = 2 * tan(theta / 2)
		*/
		auto theta = degreesToRadians(this->vfov);
		auto h = tan(theta / 2);
		auto viewportHeight = 2 * h * this->focusDistance; // focal len = focal dist in this case
		auto viewportWidth = viewportHeight * (static_cast<double>(this->imageWidth) / this->imageHeight);

		// Calculate the u,v,w unit basis vectors for the camera coordinate frame.
		this->w = unitVector(this->lookFrom - this->lookAt);				// looking toward -w
		this->u = unitVector(cross(this->vUp, w));	// camera right
		this->v = cross(w, u);						// camera up (vUp != v, v is basis vector based on cam orientation, vUp is const)

		// Calculate the vectors acrros the horizontal and down viewport edges.
		auto viewportU = viewportWidth * this->u;	// Vector across viewport horizontal edge
		auto viewportV = viewportHeight * -v;		// Vector down viewport horizontal edge

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		this->pixelDeltaU = viewportU / this->imageWidth;
		this->pixelDeltaV = viewportV / this->imageHeight;

		// Calculate the location of the upper left pixel.
		auto viewportUpperLeft = this->center - (this->focusDistance * this->w) - viewportU / 2 - viewportV / 2;
		this->pixel00Location = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);
		
		// Calculate the camera defocus disk basis vectors.
		auto defocusRadius = this->focusDistance * tan(degreesToRadians(this->defocusAngle / 2));
		this->defocusDiskU = this->u * defocusRadius;
		this->defocusDiskV = this->v * defocusRadius;
	}
	auto getRay(int i, int j) const -> Ray {
		// get a randomly sampled camera ray for the pixel at location i,j, originating from the camera defocus disk
		auto pixelCenter = this->pixel00Location + (i * this->pixelDeltaU) + (j * this->pixelDeltaV);
		auto pixelSample = pixelCenter + pixelSampleSquare();
		auto rayOrigin = this->defocusAngle <= 0
			? this->center
			: this->defocusDiskSample();
		auto rayDir = pixelSample - rayOrigin;
		return Ray(rayOrigin, rayDir);
	}
	auto pixelSampleSquare() const -> Vec3 {
		// returns a random point in the square surrouding a pixel at the origin
		auto px = -0.5 + randomDouble();
		auto py = -0.5 + randomDouble();
		return (px * this->pixelDeltaU) + (py * this->pixelDeltaV);
	}
	auto defocusDiskSample() const -> Point3 {
		// Returns a random point in the camera defocus disk.
		auto p = randomInUnitDisk();
		return this->center + (p[0] * this->defocusDiskU) + (p[1] * this->defocusDiskV);
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
