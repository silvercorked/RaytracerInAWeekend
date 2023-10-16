#pragma once

#include "Vec3.hpp"

/*
	Ray
		- origin
		- direction
		- f(t) = origin + direction * t
		- time component used in monte carlo simulations 
*/
class Ray {
	Point3 orig;
	Vec3 dir;
	double tm;
	
public:
	Ray() {}
	Ray(const Point3& origin, const Vec3& direction) : orig{ origin }, dir{ direction }, tm{ 0 } {}
	Ray(const Point3& origin, const Vec3& direction, double time)
		: orig{ origin }, dir{ direction }, tm{ time } {}
	
	auto origin() const -> Point3 { return orig; }
	auto direction() const -> Vec3 { return dir; }
	auto time() const -> double { return tm; }

	auto at(double t) const -> Point3 {
		return orig + t * dir;
	}
};
