#pragma once

#include "common.hpp"

struct Material; // forward declaration

struct HitRecord {
	Point3 p;
	Vec3 Normal;
	shared_ptr<Material> matPtr;
	double t;
	bool frontFace;

	/*
		Normal is always pointing outward of sphere, but sometimes ray
		is going through sphere to hit surface
		and sometimes its hitting the surface from the outside.
		To keep track of which way the ray hits, we can set a bool
		from the ray and the normal. This function sets frontFace correctly.

		To check which, can use dot product. a . b = ||a||*||b||*cos({angle between a and b})
			- ||a|| * ||b|| will always be positive, so don't affect the following
			- cos(theta) will be negative if angle > pi/2 (90 degrees)
				- if angle > pi/2, must have been inside the circle for at least a tad bit before hitting
			- => if a . b is negative, angle > pi/2 => inside, otherwise outside
		Choice is to have normal always oppose Ray direction
			- if ray inside, normal points out
			if ray outside, normal points in
	*/
	inline void setFaceNormal(const Ray& r, const Vec3& outwardNormal) {
		this->frontFace = dot(r.direction(), outwardNormal) < 0;	// true if inside, false otherwise
		this->Normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

/*
	Sometimes useful to have t_min and t_max where a hit is occuring rather than just one t
	Normal of the closest is the only one that matters
*/
struct Hittable {
	virtual auto hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const -> bool = 0;
};
