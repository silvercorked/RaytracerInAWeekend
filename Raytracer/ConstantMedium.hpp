#pragma once

#include "common.hpp"

#include "Hittable.hpp"
#include "Material.hpp"
#include "Texture.hpp"

class ConstantMedium : public Hittable {
	shared_ptr<Hittable> boundary;
	double negInvDensity;
	shared_ptr<Material> phaseFunction;

public:
	ConstantMedium(shared_ptr<Hittable> b, double d, shared_ptr<Texture> a) :
		boundary(b),
		negInvDensity(-1 / d),
		phaseFunction(make_shared<Isotropic>(a))
	{}
	ConstantMedium(shared_ptr<Hittable> b, double d, Color c) :
		boundary(b),
		negInvDensity(-1 / d),
		phaseFunction(make_shared<Isotropic>(c))
	{}

	auto hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool override {
		HitRecord rec1, rec2;
		if (!this->boundary->hit(r, Interval::universe, rec1))
			return false;
		if (!this->boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
			return false;
		if (rec1.t < rayT.min) rec1.t = rayT.min;
		if (rec2.t > rayT.max) rec2.t = rayT.max;
		if (rec1.t >= rec2.t)
			return false;
		if (rec1.t < 0) rec1.t = 0;
		auto rayLen = r.direction().length();
		auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLen;
		auto hitDistance = negInvDensity * log(randomDouble());
		if (hitDistance > distanceInsideBoundary)
			return false;
		rec.t = rec1.t + hitDistance / rayLen;
		rec.p = r.at(rec.t);
		rec.normal = Vec3(1, 0, 0); // arbitrary
		rec.frontFace = true; // arbitrary
		rec.material = this->phaseFunction;
		return true;
	}
	auto boundingBox() const -> AxisAlignedBoundingBox override { return this->boundary->boundingBox(); }
};
