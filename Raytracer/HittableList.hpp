#pragma once

#include "Hittable.hpp"
#include "AxisAlignedBoundingBox.hpp"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class HittableList : public Hittable {
	AxisAlignedBoundingBox bbox;

public:
	std::vector<shared_ptr<Hittable>> objects;
	
	HittableList() {}
	HittableList(shared_ptr<Hittable> object) {
		this->add(object);
	}

	auto clear() -> void { this->objects.clear(); }
	auto add(shared_ptr<Hittable> object) -> void {
		this->objects.push_back(object);
		this->bbox = AxisAlignedBoundingBox(this->bbox, object->boundingBox());
	}

	virtual auto hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool override;
	auto boundingBox() const -> AxisAlignedBoundingBox override {
		return this->bbox;
	}
};

/*
	Attempts to hit anything in this List. Closests is stored in HitRecord.
*/
auto HittableList::hit(const Ray& r, Interval rayT, HitRecord& rec) const -> bool {
	HitRecord temp_rec;
	bool hitAnything = false;
	auto closestSoFar = rayT.max;
	
	for (const auto& object : this->objects) {
		if (object->hit(r, Interval(rayT.min, closestSoFar), temp_rec)) {
			hitAnything = true;
			closestSoFar = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hitAnything;
}
