#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.14159265358979323846;

// Utility Functions
inline auto degreesToRadians(double degrees) -> double {
	return degrees * pi / 180.0;
}
inline auto randomDouble() -> double { // returns a random real in [0,1)
	static std::uniform_real_distribution<double> dist(0, 1.0);
	static std::mt19937 gen;
	return dist(gen);
}
inline auto randomDouble(double min, double max) -> double { // returns a random real in [min, max)
	return min + (max - min) * randomDouble();
}
inline auto randomInt(int min, int max) -> int {
	return static_cast<int>(randomDouble(min, max + 1));
}

// Common Headers
#include "Interval.hpp"
#include "Vec3.hpp"
#include "Ray.hpp"
