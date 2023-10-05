#pragma once

#include "Vec3.hpp"

/*
	Ray
		- origin
		- direction
		- f(t) = origin + direction * t
*/
struct Ray {
	Point3 orig;
	Vec3 dir;
	
	Ray() {};
	Ray(const Point3& origin, const Vec3& direction) : orig{ origin }, dir{ direction } {};
	
	auto origin() const -> Point3 { return orig; }
	auto direction() const -> Vec3 { return dir; }

	auto at(double t) const -> Point3 {
		return orig + t * dir;
	}
};
