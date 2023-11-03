#pragma once

#include "common.hpp"

/*
	Store "volume" in bounding box defined by an interval in x, y and z (thus axis aligned).
	Can compare with a ray to determine if it hits this box,
		if it hits, objects inside the volume can be compared to the ray to check for a hit
		if not, all objects inside the volume will not be hit by the ray and can be skipped
		- To do this comparison, need to find solutions to Ray equation P(t) = A + t * B
			- for example, in x, x0 = Ax + t0 * Bx -> t0 = (x0 - Ax) / Bx
								 x1 = Ax + t1 * Bx -> t1 = (x1 - Ax) / Bx
			t0 - t1 represents the interval in which the ray overlaps the X axis of this bounding box in terms of Ray length, t.
			if done for all three axies and if these three intervals then have any overlap, the ray enters this bouding box
*/
struct AxisAlignedBoundingBox {
	Interval x, y, z;

	AxisAlignedBoundingBox() {} // empty w/ empty intervals
	AxisAlignedBoundingBox(const Interval& ix, const Interval& iy, const Interval& iz)
		: x{ ix }, y{ iy }, z{ iz } {}
	AxisAlignedBoundingBox(const Point3& a, const Point3& b) {
		// treat two points a and b as extrema for bounding box, so we don't require a particular min/max coordinate order.
		x = Interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
		y = Interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
		z = Interval(fmin(a[2], b[2]), fmax(a[2], b[2]));
	}
	AxisAlignedBoundingBox(const AxisAlignedBoundingBox& box0, const AxisAlignedBoundingBox& box1) {
		x = Interval(box0.x, box1.x);
		y = Interval(box0.y, box1.y);
		z = Interval(box0.z, box1.z);
	}

	// return an box that has no side narrower than a static amount
	auto pad() {
		static const double delta = 0.0001;
		Interval nX = this->x.size() >= delta ? x : x.expand(delta);
		Interval nY = this->y.size() >= delta ? y : y.expand(delta);
		Interval nZ = this->z.size() >= delta ? z : z.expand(delta);
		return AxisAlignedBoundingBox(nX, nY, nZ);
	}
	auto axis(int n) const -> const Interval& {
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}
	auto hit(const Ray& r, Interval rT) const -> bool {
		for (int a = 0; a < 3; a++) {
			auto invD = 1 / r.direction()[a];	// 1 / b in axis x, y, or z
			auto orig = r.origin()[a];			// a	 in axis x, y, or z
			auto ax = this->axis(a);			// interval of axis (x, y, or z)
			auto t0 = (ax.min - orig) * invD;
			auto t1 = (ax.max - orig) * invD;
			if (invD < 0)
				std::swap(t0, t1);
			if (t0 > rT.min) rT.min = t0;
			if (t1 < rT.max) rT.max = t1;
			if (rT.max <= rT.min) // overlap interval doesn't exist
				return false;
		}
		return true;
	}
};

auto operator+(const AxisAlignedBoundingBox& bbox, const Vec3& offset) -> AxisAlignedBoundingBox {
	return AxisAlignedBoundingBox(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}
auto operator+(const Vec3& offset, const AxisAlignedBoundingBox& bbox) -> AxisAlignedBoundingBox {
	return bbox + offset;
}
