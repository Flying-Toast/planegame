#ifndef __HAVE_MODEL_H
#define __HAVE_MODEL_H

#include "error.h"
#include "linalg.h"
#include <GLES3/gl32.h>
#include <stddef.h>

enum modelkey {
	MODEL_MONKEY,
};

struct vert {
	vec3 pos;
	vec2 st;
	vec3 norm;
};

struct model {
	struct vert *verts;
	size_t nverts;

	GLuint vbo;
	GLuint vao;
	GLuint tex;
	GLuint shader;
};

Err getmodel(enum modelkey key, const struct model **out);
void model_cleanup(void);
void model_bind(const struct model *m);

#endif
