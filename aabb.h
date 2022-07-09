#ifndef AABBH
#define AABBH

#include "ray.h"


// Replaces built-in fmax and fmin. Quite a bit faster, as doesn't check NaNs, exceptions, etc.
inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }


// Axis-Aligned Bounding-Box (AABB)
class aabb {
public:
	vec3 _min;
	vec3 _max;

	aabb() {}
	aabb(const vec3& a, const vec3& b, float padding = 0.0) {
		//_min = a; _max = b;
		_min = a - padding * vec3(1, 1, 1);
		_max = b + padding * vec3(1, 1, 1);
	}

	bool hit(const ray& r, float tmin, float tmax) const {
		for (int a = 0; a < 3; a++) {
			float invD = 1.0f / r.direction.e[a];
			float t0 = (_min.e[a] - r.origin.e[a]) * invD;
			float t1 = (_max.e[a] - r.origin.e[a]) * invD;
			if (invD < 0.0f) {
				std::swap(t0, t1);
			}
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;
			if (tmax <= tmin) {
				return false;
			}
		}
		return true;
	}
};


aabb surrounding_box(aabb box0, aabb box1) {
	vec3 _min(ffmin(box0._min[0], box1._min[0]), ffmin(box0._min[1], box1._min[1]), ffmin(box0._min[2], box1._min[2]));
	vec3 _max(ffmax(box0._max[0], box1._max[0]), ffmax(box0._max[1], box1._max[1]), ffmax(box0._max[2], box1._max[2]));
	return aabb(_min, _max);
}




#endif