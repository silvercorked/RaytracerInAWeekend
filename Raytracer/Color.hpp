#pragma once

#include "Vec3.hpp"

#include <fstream>

using Color = Vec3;

/*
	rather than average cumulative sample ray colors within the same pixel,
	divide here once and allow color to be added to directly
*/
auto writeColor(std::ofstream& out, Color pixelColor, int samplesPerPixel) -> void {
	auto r = pixelColor.x();
	auto g = pixelColor.y();
	auto b = pixelColor.z();

	// divide the color by the number of samples and gamma-correct for gamma=2.0
	auto scale = 1.0 / samplesPerPixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	// Write the translated [0, 255] value of each color component
	static const Interval intesity(0, 0.999);
	out << static_cast<int>(256 * intesity.clamp(r)) << ' '
		<< static_cast<int>(256 * intesity.clamp(g)) << ' '
		<< static_cast<int>(256 * intesity.clamp(b)) << '\n';
}
