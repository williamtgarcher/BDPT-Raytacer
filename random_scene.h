#ifndef RANDOMSCENEH
#define RANDOMSCENEH

#include "sphere.h"
#include "hitable_list.h"
#include "material.h"
#include "random.h"

#include "bvh.h"


hitable* random_scene() {
	int n = 500;
	hitable** list = new hitable * [n + 1];

	texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)), 10);
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));

	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {

			float choose_mat = random();
			vec3 center(a + 0.9 * random(), 0.2, b + 0.9 * random());

			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {

				if (choose_mat < 0.8) {// diffuse
					list[i++] = new sphere(center, 0.2, new lambertian(vec3(random() * random(), random() * random(), random() * random())));
				}
				else if (choose_mat < 0.95) {// metal
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5, 0.5, 0.5) + 0.5 * vec3(random(), random(), random()), 0.5 * random()));
				}
				else {// glass
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}

		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	//return new hitable_list(list, i);
	return new bvh_node(list, i, 0, 0);
}


#endif