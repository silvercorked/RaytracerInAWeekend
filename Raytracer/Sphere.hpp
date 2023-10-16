#pragma once

#include "Hittable.hpp"
#include "Vec3.hpp"

struct Sphere : public Hittable {
	Point3 center1;
	double radius;
	shared_ptr<Material> material;
	bool isMoving;
	Vec3 centerVec;

	// Stationary Sphere
	Sphere(Point3 _center, double _radius, shared_ptr<Material> _material) :
		center1{_center}, radius{ _radius }, material{ _material }, isMoving{ false } {}
	// Moving Sphere
	Sphere(Point3 _center1, Point3 _center2, double _radius, shared_ptr<Material> _material) :
		center1{ _center1 }, radius{ _radius }, material{ _material }
	{
		this->centerVec = _center2 - _center1;
	};

	auto center(double time) const -> Point3 {
		// linear interpolate from center1 to center 2 by time (t=0 => center1, t=1 => center2
		return center1 + time * centerVec;
	}
	
	virtual auto hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool override;
};

/*
	Sphere: (x - cx)^2 + (y - cy)^2 + (z - cz)^2 = R^2, where cx, cy, and cz are the circle's center location
	C = (cx, cy, cz), P = (x, y, z) => (P-C) * (P-C) = R^2
		if (P-C) . (P-C) < R^2, P is inside the sphere, if > outside, if =, on the sphere
	use Ray equation for P, P(t) = A + t * b
	(A + t * b - C) . (A + t * b - C) = R^2
	t^2*b.b + 2*t*b . (A-C) + (A-C) . (A-C) - R^2 = 0
		0 roots (sqrt(some -))
		1 root (root is zero)
		2 roots (roots are positive)
	simplification of b value in ax^2 + bx + c = 0:
		b in this equations has a factor of 2, taking b = 2h,
		(-b +- sqrt(b^2 - 4*a*c)) / (2*a) -> (-h +- sqrt(h^2 - a*c)) / a
*/
auto Sphere::hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool {
	Point3 center = this->isMoving
		? this->center(r.time()) : this->center1;
	Vec3 oc = r.origin() - center;						// A-C
	auto a = r.direction().lengthSquared();				// b . b (aka, length^2)
	auto half_b = dot(oc, r.direction());				// b = 2 * b . (A-C), this is without the * 2
	auto c = oc.lengthSquared() - this->radius * this->radius;		// (A-C) . (A-C) - R^2
	auto underRadical = half_b * half_b - a * c;
	if (underRadical < 0)								// does not intersect
		return false;
	auto radical = sqrt(underRadical);
														// find nearest root that lies in the acceptable range.
	auto root = (-half_b - radical) / a;
	if (!rayT.surrounds(root)) {					// first hit occurs outside tMin-tMax window
		root = (-half_b + radical) / a;
		if (!rayT.surrounds(root))
			return false;								// hit occurs but not within tMin-tMax window
	}
	rec.t = root;
	rec.p = r.at(rec.t);
	Vec3 outwardNormal = (rec.p - center) / radius;// normal is in direction of P (hit point/root) - C (center) (points at P from C)
	rec.setFaceNormal(r, outwardNormal);
	rec.material = this->material;
	return true;
}
