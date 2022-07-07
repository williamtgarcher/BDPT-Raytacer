#ifndef CAMERAH
#define CAMERAH

#include "ray.h"
#include "random.h"

vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0 * vec3(random(), random(), 0) - vec3(1, 1, 0);
	} while (p.squared_length() >= 1.0);
	return p;
}

class camera {
public:
	vec3 u, v, w;
	float lens_radius;

	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 origin;

	camera(vec3 position, vec3 view_direction, vec3 vup, float v_fov, float aspect, float aperture = 0.0, float focus_dist = 1.0, bool look_at = true) {
		float theta = v_fov * M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		lens_radius = aperture / 2;
		
		// If look_at is False, it uses view_direction as the vector the camera should point along. 
		// If look_at is True, it uses view_direction as a target coordinate to point the camera at.
		w = -unit_vector(look_at ? (view_direction - position) : view_direction);
		//w = unit_vector(position - view_direction);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		origin = position;
		lower_left_corner = origin - focus_dist * ((half_width * u) + (half_height * v) + w);
		horizontal = 2 * focus_dist * half_width * u;
		vertical = 2 * focus_dist * half_height * v;
	}

	ray get_ray(float s, float t) {
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = rd[0] * u + rd[1] * v;
		vec3 offset_origin = origin + offset;
		return ray(offset_origin, lower_left_corner + s * horizontal + t * vertical - offset_origin);
	}
};

#endif // !CAMERAH