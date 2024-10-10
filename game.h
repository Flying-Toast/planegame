#ifndef __HAVE_GAME_H
#define __HAVE_GAME_H

#include "model.h"
#include "render.h"

struct game {
	struct camera cam;
};

Err game_init(struct game *g);

#endif
