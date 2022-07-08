#pragma once


struct light_path_node {
	vec3 position;
	vec3 radiance;
	vec3 normal;
};


class scene_lights {
public:
	hitable** light_hitables;
	float* light_areas;
	float* light_weights;
	int n_lights = 0;


	scene_lights(hitable** list, int list_size) {
		hit_record rec;
		float area;
		float total = 0;
		int n = 0;
		for (int i = 0; i < list_size; i++) {
			list[i]->random_on_surface(rec, area);
			if (rec.mat_ptr->is_light) {
				n++;
				total += list[i]->surface_area;
				//total += list[i]->surface_area * rec.mat_ptr->emitted(ray(rec.p, -rec.normal), rec, rec.u, rec.v, rec.p).length();
			}
		}
		light_hitables = new hitable * [n];
		light_areas = new float[n];
		light_weights = new float[n];

		float factor = 1 / total;
		for (int i = 0; i < list_size; i++) {
			list[i]->random_on_surface(rec, area);
			if (rec.mat_ptr->is_light) {
				light_hitables[n_lights] = list[i];
				light_areas[n_lights] = list[i]->surface_area;
				light_weights[n_lights++] = factor * list[i]->surface_area;
				//light_weights[n_lights++] = factor * list[i]->surface_area * rec.mat_ptr->emitted(ray(rec.p, -rec.normal), rec, rec.u, rec.v, rec.p).length();
			}
		}
	}
	scene_lights(hitable_list* world) : scene_lights(world->list, world->list_size) {}


	light_path_node start_light_path() const {
		light_path_node output;
		hit_record rec;
		float area;
		float choice = random();
		for (int i = 0; i < n_lights; i++) {
			if (light_weights[i] > choice || i == n_lights - 1) {
				light_hitables[i]->random_on_surface(rec, area);
				output.normal = rec.normal;
				output.position = rec.p;
				output.radiance = light_areas[i] * rec.mat_ptr->emitted(ray(rec.p, -rec.normal), rec, rec.u, rec.v, rec.p);
				output.radiance /= light_weights[i];//?
				return output;
			}
			choice -= light_weights[i];
		}
	}
};





vec3 l_path_contributions(vec3 position, vec3 normal, hitable* world, light_path_node* l_path, int l_nodes) {
	vec3 output(0, 0, 0);
	hit_record rec;
	for (int i = 0; i < l_nodes; i++) {
		vec3 connection = l_path[i].position - position;
		float squared_distance = connection.squared_length();
		float distance = sqrt(squared_distance);
		ray shadow(position, connection / distance);
		if (dot(shadow.direction, normal) > 0 && dot(shadow.direction, l_path[i].normal) < 0) {
			if (!world->hit(shadow, 0.001, distance - 0.001, rec, true)) {
				if (i == 0) {
					output += l_path[i].radiance * abs(dot(shadow.direction, normal) * dot(shadow.direction, l_path[i].normal)) / squared_distance;
				}
				else {
					output += l_path[i].radiance * abs(dot(shadow.direction, l_path[i].normal));
				}
			}
		}
	}
	return output;
}