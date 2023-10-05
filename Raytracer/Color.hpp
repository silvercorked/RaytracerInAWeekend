#pragma once

#include "Vec3.hpp"

#include <fstream>

/*
	rather than average cumulative sample ray colors within the same pixel,
	divide here once and allow color to be added to directly
*/
auto writeColor(std::ofstream& out, Color pixelColor, int samplesPerPixel) -> void {
	auto r = pixelColor.x();
	auto g = pixelColor.y();
	auto b = pixelColor.z();

	// divide the color by the number of samples
	auto scale = 1.0 / samplesPerPixel;
	r *= scale;
	g *= scale;
	b *= scale;

	// Write the translated [0, 255] value of each color component
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
