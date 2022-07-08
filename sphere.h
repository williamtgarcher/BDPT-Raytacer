#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "material.h"

void get_sphere_uv(const vec3& p, float& u, float& v) {
	float phi = atan2f(p[2], p[0]);
	float theta = asinf(p[1]);
	u = ((phi / M_PI) + 1) / 2;
	v = (theta / M_PI) + 0.5;
}



class sphere : public hitable {
public:
	vec3 center;
	float radius;
	material* mat_ptr;

	sphere() {}
	sphere(vec3 cen, float r, material* m) : center(cen), radius(r), mat_ptr(m) { surface_area = 4 * M_PI * r * r; };

	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec, bool shadow = false) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	virtual void random_on_surface(hit_record& rec, float& area) const {
		rec.normal = random_unit_vector();
		rec.p = center + radius * rec.normal;
		rec.mat_ptr = mat_ptr;
		get_sphere_uv(rec.p, rec.u, rec.v);
		area = M_PI * radius * radius;
	}
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec, bool shadow) const {
	vec3 oc = r.origin - center;

	// As sphere::hit is one of the most called functions, the calls to ray.origin() and ray.direction() take up a significant amount of time. 
	// Replacing these with just accessing the ray's A and B attributes directly leads to a render time reduction of ~25%
	float a = r.direction.squared_length();
	float b = dot(oc, r.direction);
	float c = oc.squared_length() - radius * radius;

	float discriminant = b * b - a * c;
	if (discriminant > 0) {

		float temp = (-b - sqrt(discriminant)) / a; // lower root
		if (t_min < temp && temp < t_max) {
			rec.t = temp;
			//rec.p = r.point_at_parameter(rec.t);
			rec.p = r.origin + rec.t * r.direction;
			rec.normal = (rec.p - center) / radius;
			get_sphere_uv(rec.normal, rec.u, rec.v);
			rec.mat_ptr = mat_ptr;
			return true;
		}

		temp = (-b + sqrt(discriminant)) / a; // higher root
		if (t_min < temp && temp < t_max) {
			rec.t = temp;
			//rec.p = r.point_at_parameter(rec.t);
			rec.p = r.origin + rec.t * r.direction;
			rec.normal = (rec.p - center) / radius;
			get_sphere_uv(rec.normal, rec.u, rec.v);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}

	return false;
}


bool sphere::bounding_box(float t0, float t1, aabb& box) const {
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

#endif