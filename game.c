#include "game.h"
#include <SDL2/SDL.h>

static void init_cam(struct camera *c) {
	c->transform = ID4;
}

Err game_init(struct game *g) {
	Err err = ERR_OK;

	init_cam(&g->cam);

	return err;
}

void game_tick(struct game *g, float dt) {
	////////////////////////
	g->cam.transform = ID4;
	static float ffff = 1;
	mat4muls3x3(&g->cam.transform, 1000.0/ffff);
	ffff += dt;
	////////////////////////
}

void game_handle_evt(const SDL_Event *e) {
}
