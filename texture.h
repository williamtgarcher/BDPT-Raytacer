#ifndef TEXTUREH
#define TEXTUREH

#include "vec3.h"

class texture {
public:
	virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
	vec3 color;

	constant_texture() {}
	constant_texture(vec3 c) : color(c) {}

	virtual vec3 value(float u, float v, const vec3& p) const {
		return color;
	}
};

class checker_texture : public texture {
public:
	texture* odd;
	texture* even;
	const float f = 10.0;

	checker_texture() {}
	checker_texture(texture* t0, texture* t1, float freq = 10.0f) : even(t0), odd(t1), f(freq) {}

	virtual vec3 value(float u, float v, const vec3& p) const {
		float sines = sin(f * p[0]) * sin(f * p[1]) * sin(f * p[2]);
		return (sines < 0) ? odd->value(u, v, p) : even->value(u, v, p);
	}
};


class image_texture : public texture {
public:
	unsigned char* data;
	int nx, ny;

	image_texture() {}
	image_texture(unsigned char *pixels, int A, int B) : data(pixels), nx(A), ny(B) {}

	virtual vec3 value(float u, float v, const vec3& p) const;
};

vec3 image_texture::value(float u, float v, const vec3& p) const {
	int i = u * nx;
	int j = (1 - v) * ny - 0.001;

	i = (i < 0) ? 0 : i;
	j = (j < 0) ? 0 : j;

	i = (i > nx - 1) ? nx - 1 : i;
	j = (j > nx - 1) ? nx - 1 : j;

	int index = 3 * i + 3 * nx * j;
	float r = int(data[index]) / 255.0f;
	float g = int(data[index + 1]) / 255.0f;
	float b = int(data[index + 2]) / 255.0f;

	return vec3(r, g, b);
}

#endif