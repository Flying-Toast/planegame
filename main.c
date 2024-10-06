#include "model.h"
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <err.h>
#include <stdbool.h>

void process_event(const SDL_Event *evt) {
 if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)
	 exit(0);
}

int main(void) {
	if (SDL_Init(SDL_INIT_VIDEO))
		errx(1, "SDL_Init: %s", SDL_GetError());

	SDL_Window *window = SDL_CreateWindow(
		"planegame",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1920, 1080,
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN
	);
	if (!window)
		errx(1, "SDL_CreateWindow: %s", SDL_GetError());

	if (!SDL_GL_CreateContext(window))
		errx(1, "SDL_GL_CreateContext: %s", SDL_GetError());

	glClearColor(1.0, 1.0, 0.867, 1.0);
	for (;;) {
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			if (evt.type == SDL_QUIT)
				goto quit;
			process_event(&evt);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window);
	}
quit:
	return 0;
}
