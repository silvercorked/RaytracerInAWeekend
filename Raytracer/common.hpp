#pragma once

#include <cmath>
#include <limits>
#include <memory>

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

// Common Headers
#include "Vec3.hpp"
#include "Ray.hpp"
