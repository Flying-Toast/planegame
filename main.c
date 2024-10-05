#include "model.h"
#include <GLFW/glfw3.h>
#include <err.h>
#include <stdbool.h>

GLFWwindow *window;

static void process_input(void) {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main(void) {
	if (!glfwInit())
		errx(1, "glfwInit");

	window = glfwCreateWindow(1920, 1080, "planegame", glfwGetPrimaryMonitor(), NULL);
	if (!window)
		errx(1, "glfwCreateWindow");

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);

	glfwMakeContextCurrent(window);

	glClearColor(1.0, 1.0, 0.867, 1.0);
	while (!glfwWindowShouldClose(window)) {
		process_input();
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}
