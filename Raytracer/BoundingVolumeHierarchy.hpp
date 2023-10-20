#pragma once

#include "common.hpp"
#include "Hittable.hpp"
#include "HittableList.hpp"


/*
	By wrapping objects in a tree of bounding boxes, we can check ray hits
	on bounding boxes, which excludes large portions of objects during ray collision tests.
	It sort of reduced checks from about N to log N because of being a binary tree (b-tree, k=2).
	The constructor for this organizes the hittables into the structure. Likely, this construction process
	which is called every frame, can be heavily optimized to gain 2 effects:
		- reduced construction time
		- reduced hit checks during ray calculations
	The construct used here is an Axis Aligned Bounding box, which is simply calculated
	but possibly larger, and thus causes more ray checks during hit calculations.
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
