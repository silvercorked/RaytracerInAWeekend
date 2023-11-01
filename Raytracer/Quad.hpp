#pragma once

#include "common.hpp"
#include "Hittable.hpp"

#include <cmath>

/* it's actually a parallelogram
 Q + v _______ Q + u + v
	  /      /
	 /      /
  Q /______/ Q + u
*/
class Quad : public Hittable {
	Point3 Q;
	Vec3 u, v;
	shared_ptr<Material> mat;
	AxisAlignedBoundingBox bbox;
	Vec3 normal;
	double D;
	Vec3 w;

public:
	Quad(const Point3& _q, const Vec3& _u, const Vec3& _v, shared_ptr<Material> m)
		: Q(_q), u(_u), v(_v), mat(m)
	{
		auto n = cross(u, v); // create a vector normal to u and v
		this->normal = unitVector(n);
		this->D = dot(normal, Q); // Q is on plane, so if n . v = D => n . Q = D
		this->w = n / dot(n, n); // see check hit comment above hit function
		this->setBoundingBox();
	}

	virtual auto setBoundingBox() -> void {
		this->bbox = AxisAlignedBoundingBox(Q, Q + u + v).pad();
	}
	auto boundingBox() const -> AxisAlignedBoundingBox override {
		return this->bbox;
	}
	/*
		Plan:
			1) find plane containing quad
			2) solve ray-plane intersection
			3) determine if hit location is on quad
		Plane: Ax + By + Cz + D = 0
			Ax + By + Cz = D
			n . v = D, where n is a vector perpendicular to the plane and v is from the origin to any position on the plane
			What to know if R(t) = P + td intersects the plane at some t
			n . (P + td) = D
			t = (D - n . P) / (n . d) // if d and n are parallel, denominator is zero, but also means ray will never hit plane
		Check hit:
			if u and v are not parallel (ie, can span R2)
			H = Q + a*u + b*v, where H is a point on the plane (where the ray intersects), and a and b are scalars
			h = H - Q = a * u + b * v
			u x h = u x (a * u + b * v) = a(u x u) + b(u x v), u x u = 0 =>
			u x h = b(u x v)
			v x h = a(v x u)
			a = (n . (v x p)) / (n . (v x u)) // use dot to reduce to scalars and divide to solve
			b = (n . (u x p)) / (n . (u x v))
			swap both cross products in a to get common denominator (a x b = -b x a)
			a = (n . (p x v)) / (n . (u x v))
			w = n / (n . (u x v)) = n / (n . n)
			a = w . (p x v)
			b = w . (u x p)
	*/
	auto hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool override {
		// check if parallel, because no intersection possible so can end
		auto denom = dot(this->normal, r.direction()); // n . d
		if (fabs(denom) < 1e-8) return false;
		// make sure hit is within interval
		auto t = (this->D - dot(this->normal, r.origin())) / denom; // t = (D - n . P) / (n . d)
		if (!rayT.contains(t)) // if outside hit interval, ignore solution
			return false;
		// check if hit on plane is inside plane shape of the quad
		auto intersection = r.at(t); // ray hits plane containing quad at some t within rayT
		Vec3 planeHitPointVector = intersection - Q;
		auto a = dot(this->w, cross(planeHitPointVector, this->v));
		auto b = dot(this->w, cross(this->u, planeHitPointVector));
		if (!isInterior(a, b, rec))
			return false; // was not inside quad
		rec.t = t;
		rec.p = intersection;
		rec.material = this->mat;
		rec.setFaceNormal(r, this->normal);
		return true;
	}
	virtual auto isInterior(double a, double b, HitRecord& rec) const -> bool {
		if (a < 0 || 1 < a || b < 0 || 1 < b) // for quad, 0 <= a <= 1, 0 <= b <= 1
			return false; // not inside
		rec.u = a;
		rec.v = b;
		return true; // is inside and hit loc on quad is just a, b
	}
};
