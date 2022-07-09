#ifndef HITABLEH
#define HITABLEH


#include "ray.h"
#include "aabb.h"

class material;

struct hit_record {
	float t;
	float u;
	float v;
	vec3 p;
	vec3 normal;
	material* mat_ptr;
};

class hitable {
public:
	float surface_area = 0;

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec, bool shadow = false) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;

	virtual void random_on_surface(hit_record& rec, float& area) const { return; };
};


class flip_normals : public hitable {
public:
	hitable* ptr;

	flip_normals(hitable* p) : ptr(p) { surface_area = ptr->surface_area; }

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec, bool shadow = false) const {
		if (ptr->hit(r, t_min, t_max, rec, shadow)) {
			rec.normal = -rec.normal;
			return true;
		}
		else {
			return false;
		}
	}
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		return ptr->bounding_box(t0, t1, box);
	}

	virtual void random_on_surface(hit_record& rec, float& area) const {
		ptr->random_on_surface(rec, area);
		rec.normal = -rec.normal;
	}
};


class translate : public hitable {
public:
	hitable* ptr;
	vec3 offset;

	translate(hitable* p, const vec3& displacement) : ptr(p), offset(displacement) { surface_area = ptr->surface_area; }

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec, bool shadow = false) const {
		ray moved_r(r.origin - offset, r.direction);
		if (ptr->hit(moved_r, t_min, t_max, rec, shadow)) {
			rec.p += offset;
			return true;
		}
		else {
			return false;
		}
	}
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		if (ptr->bounding_box(t0, t1, box)) {
			box = aabb(box._min + offset, box._max + offset);
			return true;
		}
		else {
			return false;
		}
	}

	virtual void random_on_surface(hit_record& rec, float& area) const {
		ptr->random_on_surface(rec, area);
		rec.p += offset;
	}
};


class rotate_y : public hitable {
public:
	hitable* ptr;
	float sin_theta, cos_theta;
	bool hasbox;
	aabb bbox;

	rotate_y(hitable* p, float angle) : ptr(p) {
		surface_area = ptr->surface_area;

		float radians = (M_PI / 180.0) * angle;
		sin_theta = sin(radians);
		cos_theta = cos(radians);
		hasbox = ptr->bounding_box(0, 1, bbox);
		vec3 _min(FLT_MAX, FLT_MAX, FLT_MAX);
		vec3 _max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					float x = i * bbox._min.e[0] + (1 - i) * bbox._max.e[0];
					float y = j * bbox._min.e[1] + (1 - j) * bbox._max.e[1];
					float z = k * bbox._min.e[2] + (1 - k) * bbox._max.e[2];

					float newx = cos_theta * x + sin_theta * z;
					float newz = -sin_theta * x + cos_theta * z;
					vec3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++) {
						_min.e[c] = (tester.e[c] < _min.e[c]) ? tester.e[c] : _min.e[c];
						_max.e[c] = (tester.e[c] > _max.e[c]) ? tester.e[c] : _max.e[c];
					}
				}
			}
		}

		bbox = aabb(_min, _max);
	}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec, bool shadow = false) const {
		vec3 origin = r.origin;
		vec3 direction = r.direction;
		origin.e[0] = cos_theta * r.origin.e[0] - sin_theta * r.origin.e[2];
		origin.e[2] = sin_theta * r.origin.e[0] + cos_theta * r.origin.e[2];
		direction.e[0] = cos_theta * r.direction.e[0] - sin_theta * r.direction.e[2];
		direction.e[2] = sin_theta * r.direction.e[0] + cos_theta * r.direction.e[2];
		ray rotated_r(origin, direction);
		if (ptr->hit(rotated_r, t_min, t_max, rec, shadow)) {
			vec3 p = rec.p;
			vec3 normal = rec.normal;
			p.e[0] = cos_theta * rec.p.e[0] + sin_theta * rec.p.e[2];
			p.e[2] = -sin_theta * rec.p.e[0] + cos_theta * rec.p.e[2];
			normal.e[0] = cos_theta * rec.normal.e[0] + sin_theta * rec.normal.e[2];
			normal.e[2] = -sin_theta * rec.normal.e[0] + cos_theta * rec.normal.e[2];
			rec.p = p;
			rec.normal = normal;
			return true;
		}
		else {
			return false;
		}
	}
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = bbox;
		return hasbox;
	}

	virtual void random_on_surface(hit_record& rec, float& area) const {
		ptr->random_on_surface(rec, area);
		vec3 origin = rec.p;
		vec3 direction = rec.normal;
		origin.e[0] = cos_theta * rec.p.e[0] - sin_theta * rec.p.e[2];
		origin.e[2] = sin_theta * rec.p.e[0] + cos_theta * rec.p.e[2];
		direction.e[0] = cos_theta * rec.normal.e[0] - sin_theta * rec.normal.e[2];
		direction.e[2] = sin_theta * rec.normal.e[0] + cos_theta * rec.normal.e[2];
		rec.p = origin;
		rec.normal = direction;
	}
};

#endif // !HITABLEH