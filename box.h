#ifndef BOXH
#define BOXH

#include "hitable_list.h"
#include "aarect.h"

class box : public hitable {
public:
	vec3 pmin, pmax;
	hitable* list_ptr;

	box() {}
	box(const vec3& p0, const vec3& p1, material* ptr) {
		for (int i = 0; i < 3; i++) {
			pmin.e[i] = min(p0.e[i], p1.e[i]); pmax.e[i] = max(p0.e[i], p1.e[i]);
		}
		hitable** list = new hitable * [6];
		list[0] = new flip_normals(new xy_rect(pmin.e[0], pmax.e[0], pmin.e[1], pmax.e[1], pmin.e[2], ptr));
		list[1] = new xy_rect(pmin.e[0], pmax.e[0], pmin.e[1], pmax.e[1], pmax.e[2], ptr);
		list[2] = new flip_normals(new xz_rect(pmin.e[0], pmax.e[0], pmin.e[2], pmax.e[2], pmin.e[1], ptr));
		list[3] = new xz_rect(pmin.e[0], pmax.e[0], pmin.e[2], pmax.e[2], pmax.e[1], ptr);
		list[4] = new flip_normals(new yz_rect(pmin.e[1], pmax.e[1], pmin.e[2], pmax.e[2], pmin.e[0], ptr));
		list[5] = new yz_rect(pmin.e[1], pmax.e[1], pmin.e[2], pmax.e[2], pmax.e[0], ptr);
		list_ptr = new hitable_list(list, 6);
		surface_area = 2 * (list[1]->surface_area + list[3]->surface_area + list[5]->surface_area);
	}

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow = false) const {
		return list_ptr->hit(r, tmin, tmax, rec, shadow);
	}
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(pmin, pmax);
		return true;
	}

	virtual void random_on_surface(hit_record& rec, float& area) const {
		list_ptr->random_on_surface(rec, area);
	}
};

#endif