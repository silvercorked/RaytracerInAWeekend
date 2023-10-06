#pragma once

#include <cmath>
#include <iostream>

using std::sqrt;

struct Vec3 {
	double e[3];

	Vec3() : e{ 0,0,0 } {}
	Vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

	auto x() const -> double { return e[0]; }
	auto y() const -> double { return e[1]; }
	auto z() const -> double { return e[2]; }

	auto operator-() const -> Vec3 { return Vec3(-e[0], -e[1], -e[2]); } // negation
	auto operator[](int i) const -> double { return e[i]; }
	auto operator[](int i) -> double& { return e[i]; }

	auto operator+=(const Vec3& v) -> Vec3& {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}
	auto operator*=(const double t) -> Vec3& {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}
	auto operator/=(const double t) -> Vec3& {
		return *this *= 1 / t;
	}
	auto length() const -> double {
		return sqrt(length_squared());
	}
	auto length_squared() const -> double{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}
	auto nearZero() const -> bool {
		const auto s = 1e-8;
		return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
	}
	inline static auto random() -> Vec3 {
		return Vec3(randomDouble(), randomDouble(), randomDouble());
	}
	inline static auto random(double min, double max) -> Vec3 {
		return Vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
	}
};

using Point3 = Vec3;
using Color = Vec3;

// Vec3 utilities
// these operator definitions affect conversions.
// Vec3 + Vec3, no diff
// Vec3 + {0,0,0}, no diff
// {0,0,0} + Vec3, left side converted to vec3 if these outside, but if member functions, won't be
inline auto operator<<(std::ostream& out, const Vec3& v) -> std::ostream& {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}
inline auto operator+(const Vec3& u, const Vec3& v) -> Vec3 { // vector addition -> (new)
	return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}
inline auto operator-(const Vec3& u, const Vec3& v) -> Vec3 { // vector subtraction -> (new)
	return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}
inline auto operator*(const Vec3 & u, const Vec3 & v) -> Vec3 { // matrix multiplication -> (new)
	return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}
inline auto operator*(double u, const Vec3& v) -> Vec3 { // scalar multiplication -> (new)
	return Vec3(u * v.e[0], u * v.e[1], u * v.e[2]);
}
inline auto operator*(Vec3 v, double t) -> Vec3 { // scalar multplication (other side) -> (new)
	return t * v;
}
inline auto operator/(Vec3 v, double t) -> Vec3 { // scalar division -> (new)
	return (1 / t) * v;
}
inline auto dot(const Vec3& u, const Vec3& v) -> double { // dot product (inner product)
	return u.e[0] * v.e[0]
		+ u.e[1] * v.e[1]
		+ u.e[2] * v.e[2];
}
inline auto cross(const Vec3& u, const Vec3& v) -> Vec3 { // cross product (vector product) -> (new)
	return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
inline auto unitVector(Vec3 v) -> Vec3 { // to unit length -> (new)
	return v / v.length();
}

/*
   V\  |N /R       | B 
	 \ | /         |
	__\|/__________|
	   \
	    \
		 \V
	R is v + 2 * B. Assume N is a unit vector. length of B = v . n
	v points inwards, so minus sign to point outwards
*/
auto reflect(const Vec3& v, const Vec3& n) -> Vec3 {
	return v - 2 * dot(v, n) * n;
}

/*
	Snell's Law:
	sin theta1 = eta2/eta1 * sin theta2
	Output ray, R', can be divided into parts parallel to normal and parts perpendicular to normal. R = input ray
	Rperp = eta2/eta1 * (R + cos theta2 * n) -> eta2/eta * (R + (-R * n) * n)
	Rparallel = -sqrt(1-abs(rPerp)^2) * n

	Ray's inside material with higher refractive index, no solution to snell's law, no refraction
*/
auto refract(const Vec3& uv, const Vec3& n, double etaiOverEtat) {
	auto cosTheta = fmin(dot(-uv, n), 1.0);
	Vec3 rOutPerp = etaiOverEtat * (uv + cosTheta * n);
	Vec3 rOutParallel = -sqrt(fabs(1.0 - rOutPerp.length_squared())) * n;
	return rOutPerp + rOutParallel;
}

auto randomInUnitSphere() -> Vec3 {
	while (true) {
		auto p = Vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}
auto randomInHemisphere(const Vec3& normal) -> Vec3 {
	Vec3 inUnitSphere = randomInUnitSphere();
	// in same hemisphere as normal. if dot(inUnitSphere, normal) > 0.0, negate inUnitSphere
	return inUnitSphere * -(dot(inUnitSphere, normal) > 0.0); // maybe turns into a cmov?
}
auto randomUnitVector() -> Vec3 {
	return unitVector(randomInUnitSphere());
}
