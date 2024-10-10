#include "game.h"
#include "model.h"
#include <GLES3/gl32.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdbool.h>

void process_event(const SDL_Event *evt) {
	if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)
		exit(0);
}

int main(void) {
	Err err;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_Init(SDL_INIT_VIDEO))
		errx(1, "SDL_Init: %s", SDL_GetError());

	if (!IMG_Init(IMG_INIT_JPG))
		errx(1, "IMG_Init: %s", IMG_GetError());

	SDL_Window *window = SDL_CreateWindow(
		"planegame",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		100, 100, // width/height ignored; FULLSCREEN_DESKTOP uses current display res
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN
	);
	if (!window)
		errx(1, "SDL_CreateWindow: %s", SDL_GetError());

	if (!SDL_GL_CreateContext(window))
		errx(1, "SDL_GL_CreateContext: %s", SDL_GetError());

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);

	struct game game;
	if ((err = game_init(&game)))
		errx(1, "game_init: %d", err);

	glClearColor(0.157, 0.173, 0.204, 1.0);
	for (;;) {
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			if (evt.type == SDL_QUIT)
				goto quit;
			process_event(&evt);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		if ((err = render(&game)))
			errx(1, "render: %d", err);
		SDL_GL_SwapWindow(window);
	}

quit:
	SDL_DestroyWindow(window);
	model_cleanup();

	SDL_Quit();
	return 0;
}
