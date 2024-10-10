#ifndef __HAVE_RENDER_H
#define __HAVE_RENDER_H

#include "error.h"
#include "linalg.h"

struct game;

struct camera {
	mat4 transform;
};

Err render(const struct game *g);

#endif
