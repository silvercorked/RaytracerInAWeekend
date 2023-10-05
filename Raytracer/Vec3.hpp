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