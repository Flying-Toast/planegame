#ifndef __HAVE_GAME_H
#define __HAVE_GAME_H

#include "model.h"
#include "render.h"
#include <SDL2/SDL.h>

struct game {
	struct camera cam;
};

Err game_init(struct game *g);
void game_tick(struct game *g, float dt);
void game_handle_evt(const SDL_Event *e);

#endif
