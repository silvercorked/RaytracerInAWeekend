#pragma once

#include "common.hpp"
#include "AxisAlignedBoundingBox.hpp"

struct Material; // forward declaration

struct HitRecord {
	Point3 p;
	Vec3 normal;
	shared_ptr<Material> material;
	double t;
	double u;
	double v;
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
		this->normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

/*
	Sometimes useful to have t_min and t_max where a hit is occuring rather than just one t
	Normal of the closest is the only one that matters
*/
struct Hittable {
	virtual auto hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool = 0;
	virtual auto boundingBox() const -> AxisAlignedBoundingBox = 0;
};

class Translate : public Hittable {
	shared_ptr<Hittable> obj;
	Vec3 offset;
	AxisAlignedBoundingBox bbox;

public:
	Translate(shared_ptr<Hittable> p, const Vec3& displacement) : obj(p), offset(displacement) {
		this->bbox = this->obj->boundingBox() + this->offset;
	}
	auto hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool override {
		// Move the ray backwards by the offset
		Ray rOffset(r.origin() - this->offset, r.direction(), r.time());
		// Determine where (if any) an intersection occurs along the offset ray on the wrapped obj
		if (!this->obj->hit(rOffset, rayT, rec))
			return false;
		// Move the intersection point forwards by the offset
		rec.p += this->offset;
		return true;
	}
	auto boundingBox() const -> AxisAlignedBoundingBox override { return this->bbox; }
};

class Rotate : public Hittable {
	shared_ptr<Hittable> obj;
	Vec3 rotation;
	AxisAlignedBoundingBox bbox;

public:
	Rotate(shared_ptr<Hittable> p, const Vec3& degrees) : obj(p) {
		this->rotation = Vec3(
			degreesToRadians(degrees.x()),
			degreesToRadians(degrees.y()),
			degreesToRadians(degrees.z())
		);
		this->bbox = this->obj->boundingBox();

		Point3 min(infinity, infinity, infinity);
		Point3 max(-infinity, -infinity, -infinity);

		const float c3 = cos(this->rotation.z());
		const float s3 = sin(this->rotation.z());
		const float c2 = cos(this->rotation.x());
		const float s2 = sin(this->rotation.x());
		const float c1 = cos(this->rotation.y());
		const float s1 = sin(this->rotation.y());

		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++) {
					auto x = i * this->bbox.x.max + (1 - i) * bbox.x.min;
					auto y = j * this->bbox.y.max + (1 - j) * bbox.y.min;
					auto z = k * this->bbox.z.max + (1 - k) * bbox.z.min;
					auto nx = x * (c1 * c3 + s1 * s2 * s3)
						+ y * (c2 * s3)
						+ z * (c1 * s2 * s3 - c3 * s1);
					auto ny = x * (c3 * s2 * s2 - c1 * s3)
						+ y * (c2 * c3)
						+ z * (c1 * c3 * s2 + s1 * s3);
					auto nz = x * (c2 * s1)
						+ y * (-s2)
						+ z * (c1 * c2);

					Vec3 tester(nx, ny, nz);
					for (int c = 0; c < 3; c++) {
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
					}
				}
		this->bbox = AxisAlignedBoundingBox(min, max);
	}
	auto hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool override {
		// change ray from world space to object space
		auto origin = r.origin();
		auto direction = r.direction();

		// y x z Tait-Bryan Angle (Euler Angle) https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		const float c3 = cos(this->rotation.z());
		const float s3 = sin(this->rotation.z());
		const float c2 = cos(this->rotation.x());
		const float s2 = sin(this->rotation.x());
		const float c1 = cos(this->rotation.y());
		const float s1 = sin(this->rotation.y());
		// need to do inverse transform because we're applying to ray, not shape itself
		/* for the inverse, because a rotation matrix is always ornthonormal,
			if R = R(pitch)R(yaw)R(roll), then R^-1 = R^T
				= (R(pitch)R(yaw)R(roll))^T
				= R(roll)^T R(yaw)^T R(pitch)^T
			[ a d g
			  b e h
			  c f i ]
			a = c1 * c3 + s1 * s2 * s3. b = c3 * s1 * s2 - c1 * s3. c = c2 * s1
			d = c2 * s3. e = c2 * c3. f = -s2
			g = c1 * s2 * s3 - c3 * s1. h = c1 * c3 * s2 + s1 * s3. i = c1 * c2
		*/
		Point3 unrotOrigin(
			origin[0] * (c1 * c3 + s1 * s2 * s3)
			+ origin[1] * (c2 * s3)
			+ origin[2] * (c1 * s2 * s3 - c3 * s1),
			origin[0] * (c3 * s2 * s2 - c1 * s3)
			+ origin[1] * (c2 * c3)
			+ origin[2] * (c1 * c3 * s2 + s1 * s3),
			origin[0] * (c2 * s1)
			+ origin[1] * (-s2)
			+ origin[2] * (c1 * c2)
		);
		
		Vec3 unrotDirection(
			direction[0] * (c1 * c3 + s1 * s2 * s3)
			+ direction[1] * (c2 * s3)
			+ direction[2] * (c1 * s2 * s3 - c3 * s1),
			direction[0] * (c3 * s2 * s2 - c1 * s3)
			+ direction[1] * (c2 * c3)
			+ direction[2] * (c1 * c3 * s2 + s1 * s3),
			direction[0] * (c2 * s1)
			+ direction[1] * (-s2)
			+ direction[2] * (c1 * c2)
		);
		
		Ray rRotated(unrotOrigin, unrotDirection, r.time());

		// Determine where (if any) an intersection occurs in object space
		if (!this->obj->hit(rRotated, rayT, rec))
			return false;

		// Change the intersection point from object space to world space
		auto p = rec.p;
		/* Normal transformation to go back
			[ a b c
			  d e f
			  g h i ]
		*/
		Point3 rotP(
			p[0] * (c1 * c3 + s1 * s2 * s3)
			+ p[1] * (c3 * s1 * s2 - c1 * s3)
			+ p[2] * (c2 * s1),
			p[0] * (c2 * s3)
			+ p[1] * (c2 * c3)
			+ p[2] * (-s2),
			p[0] * (c1 * s2 * s3 - c3 * s1)
			+ p[1] * (c1 * c3 * s2 + s1 * s3)
			+ p[2] * (c1 * c2)
		);
		auto normal = rec.normal;
		Vec3 rotNormal(
			normal[0] * (c1 * c3 + s1 * s2 * s3)
			+ normal[1] * (c3 * s1 * s2 - c1 * s3)
			+ normal[2] * (c2 * s1),
			normal[0] * (c2 * s3)
			+ normal[1] * (c2 * c3)
			+ normal[2] * (-s2),
			normal[0] * (c1 * s2 * s3 - c3 * s1)
			+ normal[1] * (c1 * c3 * s2 + s1 * s3)
			+ normal[2] * (c1 * c2)
		);

		rec.p = rotP;
		rec.normal = rotNormal;
		return true;
	}
	auto boundingBox() const -> AxisAlignedBoundingBox override { return this->bbox; }
};
