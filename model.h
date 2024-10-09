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
	struct vec3 pos;
	struct vec2 uv;
	struct vec3 norm;
};

struct model {
	struct vert *verts;
	size_t nverts;

	GLuint shader;
};

Err getmodel(enum modelkey key, const struct model **out);
void model_cleanup(void);

#endif
