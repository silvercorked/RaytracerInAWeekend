#pragma once

#include "Vec3.hpp"

#include <fstream>

auto writeColor(std::ofstream& out, Color pixelColor) -> void {
	out << static_cast<int>(255.999 * pixelColor.x()) << ' '
		<< static_cast<int>(255.999 * pixelColor.y()) << ' '
		<< static_cast<int>(255.999 * pixelColor.z()) << '\n';
}
