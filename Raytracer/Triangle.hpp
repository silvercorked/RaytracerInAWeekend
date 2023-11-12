#pragma once

#include "common.hpp"
#include "Hittable.hpp"
#include "AxisAlignedBoundingBox.hpp"

/*
	not doing this file until after finishing book 3.
	- big moves with this file:
		- allow all other shapes to be "triangularized", ie turned into a set of triangles
		- include linear interpolation function for some point along a triangle:
			- current thought
							(1, 0, 0)
							|\
							| \
							|  \ (0.5, 0.5, 0)
							|   \
				 (0, 0, 1)  |____\ (0, 1, 0)
				 this vec3 can multiple the 3 colors associated with the vertices:
					ie (color 1, color 2, color 3) * (0.5, 0.5, 0) [3x3 * 3x1] and then add the values in the resulting 3x1 together into a single value
				not sure how to get vec3 yet. will explore later
		- with things triangularized, one step closer to working inside gpu
*/

class Triangle : public Hittable {
	Point3 Q;
	Vec3 u, v;
	shared_ptr<Material> mat;
	AxisAlignedBoundingBox bbox;
	Vec3 normal;
	double D;
	Vec3 w;

public:
	Triangle(const Point3& _q, const Vec3& _u, const Vec3& _v, shared_ptr<Material> m)
		: Q(_q), u(_u), v(_v), mat(m)
	{
		auto n = cross(u, v); // create a vector normal to u and v
		this->normal = unitVector(n);
		this->D = dot(normal, Q); // Q is on plane, so if n . v = D => n . Q = D
		this->w = n / dot(n, n); // see check hit comment above hit function
		this->setBoundingBox();
	}

	virtual auto setBoundingBox() -> void {
		this->bbox = AxisAlignedBoundingBox(Q, Q + u + v).pad(); // bounding box has to be square, so wrong >= half the time cause box v triangle
	}
	auto boundingBox() const -> AxisAlignedBoundingBox override {
		return this->bbox;
	}
	/*
		Plan:
			1) find plane containing triangle
			2) solve ray-plane intersection
			3) determine if hit location is on triangle
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
		// check if hit on plane is inside plane shape of the triangle
		auto intersection = r.at(t); // ray hits plane containing triangle at some t within rayT
		Vec3 planeHitPointVector = intersection - this->Q;
		auto a = dot(this->w, cross(planeHitPointVector, this->v));
		auto b = dot(this->w, cross(this->u, planeHitPointVector));
		if (!isInterior(a, b, rec))
			return false; // was not inside triangle
		rec.t = t;
		rec.p = intersection;
		rec.material = this->mat;
		rec.setFaceNormal(r, this->normal);
		return true;
	}
	virtual auto isInterior(double a, double b, HitRecord& rec) const -> bool {
		if (a < 0 || b < 0 || a + b > 1) // if a or b is negative, definite miss. if a + b <= 1, hit
			return false; // not inside
		rec.u = a;
		rec.v = b;
		return true; // is inside and hit loc on triangle is just a, b
	}
};
