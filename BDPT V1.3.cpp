#define _USE_MATH_DEFINES

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <string>
#include <chrono>

#include "sphere.h"
#include "aarect.h"
#include "box.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "random.h"
#include "light_path.h"
#include "bvh.h"

using namespace std;



hitable_list* cornell_box() {
	hitable** list = new hitable * [100];
	int i = 0;
	material* red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material* white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material* green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material* light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	material* glass = new dielectric(1.5);
	material* aluminium = new metal(vec3(0.8, 0.85, 0.88), 0.1);
	material* dimlight = new diffuse_light(new constant_texture(vec3(5, 5, 5)));

	list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
	//list[i++] = new flip_normals(new xz_rect(213, 275, 227, 275, 554, light));//
	//list[i++] = new flip_normals(new xz_rect(213, 275, 290, 332, 554, light));//
	//list[i++] = new flip_normals(new xz_rect(280, 343, 227, 332, 554, light));//

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	//list[i++] = new xz_rect(20, 110, 20, 110, 1, dimlight);//
	//list[i++] = new xz_rect(500, 540, 500, 540, 1, dimlight);//
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	//list[i++] = new sphere(vec3(190, 90, 190), 90, glass);
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	return new hitable_list(list, i);
}






vec3 color(const ray& r, hitable* world, int depth, light_path_node* l_path, int l_nodes) {
	hit_record rec;
	// t_min is slightly > 0 to prevent reflected rays colliding with the object they reflect from at very small t
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			vec3 reflected = color(scattered, world, depth + 1, l_path, l_nodes);
			if (!rec.mat_ptr->is_specular) {
				reflected += l_path_contributions(rec.p, rec.normal, world, l_path, l_nodes);
				return emitted + attenuation * reflected / (l_nodes + 1);
			}
			return emitted + attenuation * reflected;
		}
		else {
			return emitted;
		}
	}
	else {
		return vec3(0, 0, 0);
	}
}


vec3 color_first(const ray& r, hitable* world, light_paths* l_paths, const int ss) {
	hit_record rec;
	// t_min is slightly > 0 to prevent reflected rays colliding with the object they reflect from at very small t
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

		if (!rec.mat_ptr->is_rough) {
			if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
				return emitted + attenuation * color_first(scattered, world, l_paths, ss);
			}
			else {
				return emitted;
			}
		}
		
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			int nodes;
			light_path_node* path = l_paths->new_path(nodes);
			vec3 reflected;
			vec3 col(0, 0, 0);

			reflected = color(scattered, world, 0, path, nodes);
			if (!rec.mat_ptr->is_specular) {
				reflected += l_path_contributions(rec.p, rec.normal, world, path, nodes);
				col += emitted + attenuation * reflected / (nodes + 1);
			}
			else {
				col += emitted + attenuation * reflected;
			}

			for (int i = 1; i < ss; i++) {
				rec.mat_ptr->scatter(r, rec, attenuation, scattered);
				path = l_paths->new_path(nodes);

				reflected = color(scattered, world, 0, path, nodes);
				if (!rec.mat_ptr->is_specular) {
					reflected += l_path_contributions(rec.p, rec.normal, world, path, nodes);
					col += emitted + attenuation * reflected / (nodes + 1);
				}
				else {
					col += emitted + attenuation * reflected;
				}
			}
			return col / float(ss);
		}
		else {
			return emitted;
		}
	}
	else {
		return vec3(0, 0, 0);
	}

}



int main() {
	chrono::system_clock::time_point tp = chrono::system_clock::now();
	chrono::system_clock::duration dtn = tp.time_since_epoch();
	string output_filename = "renders/Image [" + to_string(dtn.count()) + "].ppm";
	ofstream ImageFile("render.ppm");

	const int nx = 400, ny = 400;
	const int ns = 20;
	const int aa = 4;
	const int ss = ns / aa;
	ImageFile << "P3\n" << nx << " " << ny << "\n255\n";

	hitable_list* world_list = cornell_box();
	scene_lights* lights = new scene_lights(world_list);
	//hitable* world = new bvh_node(world_list->list, world_list->list_size);
	hitable* world = new hitable_list(world_list->list, world_list->list_size);
	light_paths* l_paths = new light_paths(world, lights, 10000, 10);

	srand(dtn.count());

	vec3 lookfrom(278, 278, -800); vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny));

	int row_buffer[nx][3];
	for (int j = ny - 1; j >= 0; j--) {
		cerr << "\rScanlines remaining: " << (j + 1) << " (" << to_string(float(100 * (j + 1)) / float(ny)) << "%)" << string(20, ' ') << flush;

#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < nx; i++) {

			vec3 col(0, 0, 0);

			for (int s = 0; s < aa; s++) {
				float u = (float(i) + random()) / float(nx);
				float v = (float(j) + random()) / float(ny);

				ray r = cam.get_ray(u, v);

				/*int nodes;
				light_path_node* path = l_paths->new_path(nodes);
				col += color(r, world, 0, path, nodes);*/
				col += color_first(r, world, l_paths, ss);
			}

			col /= float(aa);


			row_buffer[i][0] = min(int(255.99 * sqrt(col[0])), 255);
			row_buffer[i][1] = min(int(255.99 * sqrt(col[1])), 255);
			row_buffer[i][2] = min(int(255.99 * sqrt(col[2])), 255);
		}
		for (int i = 0; i < nx; i++) {
			ImageFile << row_buffer[i][0] << " " << row_buffer[i][1] << " " << row_buffer[i][2] << "\n";
		}
	}

	ImageFile.close();
	chrono::system_clock::time_point end_tp = chrono::system_clock::now();
	cerr << "\rRender complete." << string(20, ' ') << "(" << to_string((end_tp - tp).count() / 1e7) << "s)" << flush;
	std::ifstream src("render.ppm", std::ios::binary);
	std::ofstream dst(output_filename, std::ios::binary);
	dst << src.rdbuf();
	return 0;
}