#pragma once

#include "common.hpp"
#include "Hittable.hpp"
#include "HittableList.hpp"


/*

*/
class BoundingVolumeHierarchyNode : public Hittable {
	shared_ptr<Hittable> left;
	shared_ptr<Hittable> right;
	AxisAlignedBoundingBox bbox;

public:
	BoundingVolumeHierarchyNode(const HittableList& list)
		: BoundingVolumeHierarchyNode(list.objects, 0, list.objects.size()) {}
	BoundingVolumeHierarchyNode(
		const std::vector<shared_ptr<Hittable>>& srcObjects,
		size_t start,
		size_t end
	) {
		auto objects = srcObjects; // modifiable copy

		int axis = randomInt(0, 2);
		size_t objectSpan = end - start;

		if (objectSpan == 1) {
			this->left = objects[start];
			this->right = this->left;
		}
		else if (objectSpan == 2) {
			if (BoundingVolumeHierarchyNode::boxCompare(objects[start], objects[start + 1], axis)) {
				this->left = objects[start];
				this->right = objects[start + 1];
			}
			else {
				this->left = objects[start + 1];
				this->right = objects[start];
			}
		}
		else {
			std::sort(objects.begin() + start, objects.begin() + end, [&axis](
				const shared_ptr<Hittable> a,
				const shared_ptr<Hittable> b
			) -> bool {
				return BoundingVolumeHierarchyNode::boxCompare(a, b, axis);
			});
			auto mid = start + objectSpan / 2;
			this->left = make_shared<BoundingVolumeHierarchyNode>(objects, start, mid);
			this->right = make_shared<BoundingVolumeHierarchyNode>(objects, mid, end);
		}
		this->bbox = AxisAlignedBoundingBox(this->left->boundingBox(), this->right->boundingBox());
	}

	auto hit(const Ray& r, Interval rT, HitRecord& rec) const -> bool override {
		if (!this->bbox.hit(r, rT)) return false;
		bool hitLeft = this->left->hit(r, rT, rec);
		bool hitRight = this->right->hit(
			r,
			Interval(rT.min, hitLeft ? rec.t : rT.max),
			rec
		);
		return hitLeft || hitRight;
	}
	auto boundingBox() const -> AxisAlignedBoundingBox override {
		return this->bbox;
	}

private:
	static auto boxCompare(
		const shared_ptr<Hittable> a,
		const shared_ptr<Hittable> b,
		int axisIndex
	) -> bool {
		return a->boundingBox().axis(axisIndex).min < b->boundingBox().axis(axisIndex).min;
	}
};
