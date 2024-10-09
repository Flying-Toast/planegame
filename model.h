#ifndef __HAVE_MODEL_H
#define __HAVE_MODEL_H

#include "error.h"
#include <GLES3/gl32.h>
#include <stddef.h>

enum modelkey {
	MODEL_MONKEY,
};

struct vec2 {
	float coords[2];
};

struct vec3 {
	float coords[3];
};

struct vert {
	size_t point_idx;
	size_t uv_idx;
	size_t norm_idx;
};

struct face {
	struct vert verts[3];
};

struct model {
	struct vec3 *points;
	size_t npoints;

	struct vec3 *norms;
	size_t nnorms;

	struct vec2 *uvs;
	size_t nuvs;

	struct face *faces;
	size_t nfaces;

	GLuint shader;
};

Err getmodel(enum modelkey key, const struct model **out);
void model_cleanup(void);

#endif
