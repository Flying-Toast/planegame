#ifndef __HAVE_MODEL_H
#define __HAVE_MODEL_H

#include "error.h"
#include <stddef.h>

enum modelkey {
	MODEL_MONKEY,
};

struct vert {
	float pos[3];
};

struct model {
	struct vert *verts;
	size_t nverts;
};

Err getmodel(enum modelkey key, const struct model **out);

#endif
