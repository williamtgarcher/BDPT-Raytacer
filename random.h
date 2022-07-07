#ifndef RANDOMH
#define RANDOMH

#include <cstdlib>
#include "vec3.h"

float random() {
	return float(rand()) / float(RAND_MAX);
}


#endif