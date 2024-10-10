#include "game.h"

static void init_cam(struct camera *c) {
	c->transform = ID4;
}

Err game_init(struct game *g) {
	Err err = ERR_OK;

	init_cam(&g->cam);

	return err;
}
