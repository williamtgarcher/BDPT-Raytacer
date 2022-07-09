#ifndef AARECTH
#define AARECTH

#include "hitable.h"
#include "random.h"

class xy_rect : public hitable {
public:
	material* mp;
	float x0, x1, y0, y1, z;

	xy_rect() {}
	xy_rect(float _x0, float _x1, float _y0, float _y1, float _z, material* mat) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), z(_z), mp(mat) { surface_area = (_x1 - _x0) * (_y1 - _y0); }

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow = false) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(x0, y0, z - 0.0001), vec3(x1, y1, z + 0.0001));
		return true;
	}

	virtual void random_on_surface(hit_record& rec, float& area) const {
		rec.u = random();
		rec.v = random();
		rec.p = vec3(x0 + (x1 - x0) * rec.u, y0 + (y1 - y0) * rec.v, z);
		rec.normal = vec3(0, 0, 1);
		rec.mat_ptr = mp;
		area = surface_area;
	}
};


class yz_rect : public hitable {
public:
	material* mp;
	float y0, y1, z0, z1, x;

	yz_rect() {}
	yz_rect(float _y0, float _y1, float _z0, float _z1, float _x, material* mat) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), x(_x), mp(mat) { surface_area = (_y1 - _y0) * (_z1 - _z0); }

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow = false) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(x - 0.0001, y0, z0), vec3(x + 0.0001, y1, z1));
		return true;
	}

	virtual void random_on_surface(hit_record& rec, float& area) const {
		rec.u = random();
		rec.v = random();
		rec.p = vec3(x, y0 + (y1 - y0) * rec.u, z0 + (z1 - z0) * rec.v);
		rec.normal = vec3(1, 0, 0);
		rec.mat_ptr = mp;
		area = surface_area;
	}
};


class xz_rect : public hitable {
public:
	material* mp;
	float z0, z1, x0, x1, y;

	xz_rect() {}
	xz_rect(float _x0, float _x1, float _z0, float _z1, float _y, material* mat) : z0(_z0), z1(_z1), x0(_x0), x1(_x1), y(_y), mp(mat) { surface_area = (_x1 - _x0) * (_z1 - _z0); }

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow = false) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(x0, y - 0.0001, z0), vec3(x1, y + 0.0001, z1));
		return true;
	}

	virtual void random_on_surface(hit_record& rec, float& area) const {
		rec.u = random();
		rec.v = random();
		rec.p = vec3(x0 + (x1 - x0) * rec.u, y, z0 + (z1 - z0) * rec.v);
		rec.normal = vec3(0, 1, 0);
		rec.mat_ptr = mp;
		area = surface_area;
	}
};




bool xy_rect::hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow) const {
	float t = (z - r.origin.e[2]) / r.direction.e[2];
	if (t < tmin || t > tmax) {
		return false;
	}
	//vec3 p = r.point_at_parameter(t);
	vec3 p = r.origin + t * r.direction;
	if (p.e[0] < x0 || p.e[0] > x1 || p.e[1] < y0 || p.e[1] > y1) {
		return false;
	}
	rec.u = (p.e[0] - x0) / (x1 - x0);
	rec.v = (p.e[1] - y0) / (y1 - y0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = p;
	rec.normal = vec3(0, 0, 1);
	return true;
}


bool yz_rect::hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow) const {
	float t = (x - r.origin.e[0]) / r.direction.e[0];
	if (t < tmin || t > tmax) {
		return false;
	}
	//vec3 p = r.point_at_parameter(t);
	vec3 p = r.origin + t * r.direction;
	if (p.e[1] < y0 || p.e[1] > y1 || p.e[2] < z0 || p.e[2] > z1) {
		return false;
	}
	rec.u = (p.e[1] - y0) / (y1 - y0);
	rec.v = (p.e[2] - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = p;
	rec.normal = vec3(1, 0, 0);
	return true;
}


bool xz_rect::hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow) const {
	float t = (y - r.origin.e[1]) / r.direction.e[1];
	if (t < tmin || t > tmax) {
		return false;
	}
	//vec3 p = r.point_at_parameter(t);
	vec3 p = r.origin + t * r.direction;
	if (p.e[2] < z0 || p.e[2] > z1 || p.e[0] < x0 || p.e[0] > x1) {
		return false;
	}
	rec.u = (p.e[2] - z0) / (z1 - z0);
	rec.v = (p.e[0] - x0) / (x1 - x0);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = p;
	rec.normal = vec3(0, 1, 0);
	return true;
}


#endif