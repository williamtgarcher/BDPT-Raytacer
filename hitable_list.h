#ifndef HITABLELISTH
#define HITABLELISTH

#include "hitable.h"

class hitable_list : public hitable {
public:
	hitable** list;
	int list_size;

	hitable_list() {}
	hitable_list(hitable** l, int n) {
		list = l; list_size = n;
		for (int i = 0; i < n; i++) {
			surface_area += list[i]->surface_area;
		}
	}

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	virtual void random_on_surface(hit_record& rec, float& area) const {
		list[min(int(list_size * random()), list_size - 1)]->random_on_surface(rec, area);
	}

	virtual float visible_area_fraction(const light_path_node& node, const ray& shadow) const {
		// This should never be called
		float visible_area = 0;
		float total_area = 0;
		for (int i = 0; i < list_size; i++) {
			visible_area += list[i]->visible_area_fraction(node, shadow) * surface_area;
			total_area += surface_area;
		}
		return visible_area / total_area;
	}
};

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec, bool shadow) const {
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;

	for (int i = 0; i < list_size; i++) {
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec, shadow)) {
			if (shadow) { return true; }
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}


bool hitable_list::bounding_box(float t0, float t1, aabb& box) const {
	if (list_size < 1) {
		return false;
	}

	aabb temp_box;
	bool first_true = list[0]->bounding_box(t0, t1, temp_box);
	if (!first_true) {
		return false;
	}
	else {
		box = temp_box;
	}
	for (int i = 1; i < list_size; i++) {
		if (list[i]->bounding_box(t0, t1, temp_box)) {
			box = surrounding_box(box, temp_box);
		}
		else {
			return false;
		}
	}
	return false;
}

#endif