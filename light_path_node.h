#pragma once

#include "vec3.h"

class hitable;

struct light_path_node {
	vec3 position;
	vec3 radiance;
	vec3 normal;
	hitable* light;
};
