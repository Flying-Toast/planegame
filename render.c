#include "game.h"
#include "render.h"
#include <GLES3/gl32.h>

static Err rendermodel(const struct game *g, const struct model *m) {
	GLint transform_uni;

	if ((transform_uni = glGetUniformLocation(m->shader, "transform"))) {
		LOG("can't get transform uniform");
		return ERR_GL;
	}
	glUniformMatrix4fv(
		glGetUniformLocation(m->shader, "transform"),
		1,
		GL_FALSE,
		(GLfloat *) &g->cam.transform.v
	);

	model_bind(m);
	glDrawArrays(GL_TRIANGLES, 0, m->nverts);

	return ERR_OK;
}

Err render(const struct game *g) {
	Err err = ERR_OK;

	const struct model *monkey;
	if ((err = getmodel(MODEL_MONKEY, &monkey)))
		return err;

	if ((err = rendermodel(g, monkey)))
		return err;

	return err;
}
