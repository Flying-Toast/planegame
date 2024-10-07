#include "model.h"
#include <GLES3/gl32.h>
#include <SDL2/SDL.h>
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

	atexit(SDL_Quit);

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

	/////////////////// DELETEME DELETEMELKEAFKEAjvlk
	const struct model *foobar123;
	err = getmodel(MODEL_MONKEY, &foobar123);
	if (err)
		errx(1, "whoopsie doopsie: %d", err);
	GLuint prog = foobar123->shader;
	glUseProgram(prog);
	GLfloat triangle_vertices[] = {
	    0.0,  0.8, 0.0,
	   -0.8, -0.8, 0.0,
	    0.8, -0.8, 0.0,
	};
	GLint attb = glGetAttribLocation(prog, "pos");
	if (attb == -1)
		errx(1, "fjewakfjewaoif");
	///////////////////

	glClearColor(0.157, 0.173, 0.204, 1.0);
	for (;;) {
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			if (evt.type == SDL_QUIT)
				goto quit;
			process_event(&evt);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		/////////////////// DELETEME DELETEMELKEAFKEAjvlk
		glEnableVertexAttribArray(attb);
		glVertexAttribPointer(
			attb,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			triangle_vertices
		);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		/////////////////// DELETEME DELETEMELKEAFKEAjvlk
		SDL_GL_SwapWindow(window);
	}
quit:
	return 0;
}
