#ifndef BVHH
#define BVHH

#include "hitable.h"
#include "ray.h"
#include "aabb.h"
#include "random.h"


class bvh_node : public hitable {
public:
	aabb box;
	hitable* left;
	hitable* right;

	bvh_node() {}
	bvh_node(hitable** l, int n);

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow = false) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	virtual float visible_area_fraction(const light_path_node& node, const ray& shadow) const {
		// This should never be called
		return 0;
	}
};


bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
	b = box;
	return true;
}

int box_x_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable* ah = *(hitable**)a;
	hitable* bh = *(hitable**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right)) {
		cerr << "No bounding box in bvh_node constructor.\n";
	}
	return (box_left._min[0] < box_right._min[0]) ? -1 : 1;
}
int box_y_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable* ah = *(hitable**)a;
	hitable* bh = *(hitable**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right)) {
		cerr << "No bounding box in bvh_node constructor.\n";
	}
	return (box_left._min[1] < box_right._min[1]) ? -1 : 1;
}
int box_z_compare(const void* a, const void* b) {
	aabb box_left, box_right;
	hitable* ah = *(hitable**)a;
	hitable* bh = *(hitable**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right)) {
		cerr << "No bounding box in bvh_node constructor.\n";
	}
	return (box_left._min[2] < box_right._min[2]) ? -1 : 1;
}

bool bvh_node::hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow) const {
	if (box.hit(r, tmin, tmax)) {
		hit_record left_rec, right_rec;
		bool hit_left = left->hit(r, tmin, tmax, left_rec);
		bool hit_right = right->hit(r, tmin, tmax, right_rec);

		/*if (hit_left && hit_right) {
			rec = (left_rec.t < right_rec.t) ? left_rec : right_rec;
		}
		else if (hit_left) {
			rec = left_rec;
		}
		else if (hit_right) {
			rec = right_rec;
		}
		else {
			return false;
		}
		return true;*/

		if (hit_left && hit_right) {
			rec = (left_rec.t < right_rec.t) ? left_rec : right_rec;
			return true;
		}
		else if (hit_left) {
			rec = left_rec;
			return true;
		}
		else if (hit_right) {
			rec = right_rec;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}


bvh_node::bvh_node(hitable** l, int n) {
	int axis = int(3 * random());
	if (axis == 0) {
		qsort(l, n, sizeof(hitable*), box_x_compare);
	}
	else if (axis == 1) {
		qsort(l, n, sizeof(hitable*), box_y_compare);
	}
	else {
		qsort(l, n, sizeof(hitable*), box_z_compare);
	}

	if (n == 1) {
		left = right = l[0];
	}
	else if (n == 2) {
		left = l[0];
		right = l[1];
	}
	else {
		left = new bvh_node(l, n / 2);
		right = new bvh_node(l + n / 2, n - n / 2);
	}

	aabb box_left, box_right;
	if (!left->bounding_box(0, 0, box_left) || !right->bounding_box(0, 0, box_right)) {
		cerr << "No bounding box in bvh_node constructor.\n";
	}
	box = surrounding_box(box_left, box_right);
}



#endif