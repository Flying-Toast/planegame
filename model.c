#include "model.h"
#include "util.h"
#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr[0])))

static Err readobj(const char *path, struct model *out);

struct objvert {
	size_t point_idx;
	size_t uv_idx;
	size_t norm_idx;
};

struct objface {
	struct objvert verts[3];
};

bool shaders_initialized = false;
static GLuint default_shader;
// NOTE: add new models to model_cleanup()
static struct model monkey;

static Err loadshader(const char *path, GLenum type, GLuint *id_out) {
	char *src;
	Err err = ERR_OK;

	if ((err = readtostring(path, &src)))
		return err;

	GLuint id = glCreateShader(type);
	glShaderSource(id, 1, (const char **) &src, NULL);
	glCompileShader(id);

	GLint ok = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		char infolog[1024] = {0};
		glGetShaderInfoLog(id, sizeof(infolog), NULL, infolog);
		LOGF("shader compilation error: %s", infolog);
		err = ERR_GL;
		goto err_free;
	}

	*id_out = id;
err_free:
	free(src);
	return err;
}

static void freemodel(struct model *m) {
	if (m->verts == NULL)
		return;
	free(m->verts);
	m->verts = NULL;
}

void model_cleanup(void) {
	freemodel(&monkey);
	if (shaders_initialized) {
		glDeleteProgram(default_shader);
		shaders_initialized = false;
	}
}

static Err init_shaders(void) {
	Err err = ERR_OK;
	GLuint vertshad, fragshad;

	err = loadshader("data/shaders/vert.glsl", GL_VERTEX_SHADER, &vertshad);
	if (err)
		return err;

	err = loadshader("data/shaders/frag.glsl", GL_FRAGMENT_SHADER, &fragshad);
	if (err)
		return err;

	default_shader = glCreateProgram();
	glAttachShader(default_shader, vertshad);
	glAttachShader(default_shader, fragshad);

	glLinkProgram(default_shader);
	GLint ok = GL_FALSE;
	glGetProgramiv(default_shader, GL_LINK_STATUS, &ok);
	if (!ok) {
		char infolog[1024] = {0};
		glGetProgramInfoLog(default_shader, sizeof(infolog), NULL, infolog);
		LOGF("shader link error: %s", infolog);
		err = ERR_GL;
		goto err_delete;
	}

	goto out;
err_delete:
	glDeleteProgram(default_shader);
out:
	glDeleteShader(vertshad);
	glDeleteShader(fragshad);
	return err;
}

static Err cachemodel(
	struct model *storage,
	const char *objpath,
	const struct model **out
) {
	if (storage->verts == NULL) {
		Err err = readobj(objpath, storage);
		if (err)
			return err;
	}

	if (!shaders_initialized) {
		Err err = init_shaders();
		if (err)
			return err;
		shaders_initialized = true;
	}

	storage->shader = default_shader;
	*out = storage;
	return ERR_OK;
}

Err getmodel(enum modelkey key, const struct model **out) {
	switch (key) {
	case MODEL_MONKEY:
		return cachemodel(&monkey, "data/monkey.obj", out);
	}
	errx(1, "invalid modelkey %d\n", key);
}

static bool strneq(const char *a, const char *b, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (a[i] == '\0' || b[i] == '\0' || a[i] != b[i])
			return false;
	}
	return true;
}

// Advance string the the first character of the next line.
// Return false iff end of string is reached.
static bool nextline(const char **string) {
	const char *s = *string;
	bool ret = false;
	while (*s) {
		if (*(s++) == '\n') {
			ret = true;
			break;
		}
	}
	*string = s;
	return ret && *s;
}

static void countlines(
	const char *string,
	size_t *npoints_out,
	size_t *nfaces_out,
	size_t *nuvs_out,
	size_t *nnorms_out
) {
	size_t npoints = 0, nfaces = 0, nuvs = 0, nnorms = 0;
	do {
		if (strneq(string, "v ", 2))
			npoints++;
		else if (strneq(string, "f ", 2))
			nfaces++;
		else if (strneq(string, "vt ", 3))
			nuvs++;
		else if (strneq(string, "vn ", 3))
			nnorms++;
	} while (nextline(&string));
	*npoints_out = npoints;
	*nfaces_out = nfaces;
	*nuvs_out = nuvs;
	*nnorms_out = nnorms;
}

static Err parsefloat(const char **string, float *out) {
	const char *s = *string;

	if (*s == '\0')
		return ERR_PARSEOBJ;

	char *endptr;
	float parsed = strtof(s, &endptr);
	if (endptr == s)
		return ERR_PARSEOBJ;

	*out = parsed;
	*string = endptr;
	return ERR_OK;
}

static Err parsefloats(const char **string, size_t n, float *outlist) {
	const char *s = *string;

	for (size_t i = 0; i < n; i++) {
		if (parsefloat(&s, &outlist[i]))
			return ERR_PARSEOBJ;
		if (i != n - 1 && (!*s || *(s++) != ' '))
			return ERR_PARSEOBJ;
	}

	*string = s;
	return ERR_OK;
}

// subtracts one from the output to make it zero-based
static Err parseindex(const char **string, size_t *out) {
	const char *s = *string;

	if (*s == '\0')
		return ERR_PARSEOBJ;

	char *endptr;
	unsigned long parsed = strtoul(s, &endptr, 10);
	if (endptr == s)
		return ERR_PARSEOBJ;

	if (parsed == 0) {
		LOG("index is 0 but .obj indices should start at 1");
		return ERR_PARSEOBJ;
	}

	*out = parsed - 1;
	*string = endptr;
	return ERR_OK;
}

static Err parseobjvert(const char **string, struct objvert *out) {
	const char *s = *string;

	if (*s == '\0')
		return ERR_PARSEOBJ;

	size_t parts[3];
	for (size_t i = 0; i < ARRAY_LEN(parts); i++) {
		if (parseindex(&s, &parts[i]))
			return ERR_PARSEOBJ;
		if (i != ARRAY_LEN(parts) - 1 && (!*s || *(s++) != '/'))
			return ERR_PARSEOBJ;
	}

	out->point_idx = parts[0];
	out->uv_idx = parts[1];
	out->norm_idx = parts[2];
	*string = s;
	return ERR_OK;
}

static Err parseobjverts(const char **string, size_t n, struct objvert *outlist) {
	const char *s = *string;

	for (size_t i = 0; i < n; i++) {
		if (parseobjvert(&s, &outlist[i]))
			return ERR_PARSEOBJ;
		if (i != n - 1 && (!*s || *(s++) != ' '))
			return ERR_PARSEOBJ;
	}

	*string = s;
	return ERR_OK;
}

static Err parseobj(const char *s, struct model *out) {
	Err err = ERR_OK;
	size_t npoints, nfaces, nuvs, nnorms;
	countlines(s, &npoints, &nfaces, &nuvs, &nnorms);
	struct vec3 *points = malloc(sizeof(struct vec3) * npoints);
	struct vec3 *norms = malloc(sizeof(struct vec3) * nnorms);
	struct objface *faces = malloc(sizeof(struct objface) * nfaces);
	struct vec2 *uvs = malloc(sizeof(struct vec2) * nuvs);

	size_t curpoint = 0, curface = 0, curuv = 0, curnorm = 0;
	size_t lineno = 0;

	do {
		if (strneq(s, "v ", 2)) {
			s += 2; // skip "v "
			assert(curpoint < npoints);
			if (parsefloats(&s, 3, points[curpoint].coords))
				goto err_free;
			curpoint++;
		} else if (strneq(s, "vt ", 3)) {
			s += 3; // skip "vt "
			assert(curuv < nuvs);
			if (parsefloats(&s, 2, uvs[curuv].coords))
				goto err_free;
			curuv++;
		} else if (strneq(s, "vn ", 3)) {
			s += 3; // skip "vn "
			assert(curnorm < nnorms);
			if (parsefloats(&s, 3, norms[curnorm].coords))
				goto err_free;
			curnorm++;
		} else if (strneq(s, "f ", 2)) {
			s += 2; // skip "f "
			assert(curface < nfaces);
			if (parseobjverts(&s, 3, faces[curface].verts))
				goto err_free;
			for (size_t i = 0; i < 3; i++) {
				struct objvert *v = &faces[curface].verts[i];
				if (v->norm_idx >= nnorms) {
					LOGF(
						"norm idx %lu out of range (max %lu)"
						, v->norm_idx
						, nnorms - 1
					);
					goto err_free;
				}
				if (v->point_idx >= npoints) {
					LOGF(
						"point idx %lu out of range (max %lu)"
						, v->point_idx
						, npoints - 1
					);
					goto err_free;
				}
				if (v->uv_idx >= nuvs) {
					LOGF(
						"uv idx %lu out of range (max %lu)"
						, v->uv_idx
						, nuvs - 1
					);
					goto err_free;
				}
			}
			curface++;
		}
	} while(lineno++, nextline(&s));

	size_t nverts = nfaces * 3;
	struct vert *verts = malloc(sizeof(struct vert) * nverts);
	for (size_t fi = 0; fi < nfaces; fi++) {
		for (size_t vi = 0; vi < 3; vi++) {
			const struct objvert *overt = &faces[fi].verts[vi];
			struct vert *v = &verts[fi * 3 + vi];
			v->norm = norms[overt->norm_idx];
			v->pos = points[overt->point_idx];
			v->uv = uvs[overt->uv_idx];
		}
	}
	out->verts = verts;
	out->nverts = nverts;
	goto out;
err_free:
	err = ERR_PARSEOBJ;
	LOGF("error in obj file line %lu", lineno);
out:
	free(points);
	free(norms);
	free(faces);
	free(uvs);
	return err;
}

static Err readobj(const char *path, struct model *out) {
	char *buf;
	Err err = ERR_OK;

	if ((err = readtostring(path, &buf))) {
		LOGF("failed to read %s", path);
		return err;
	}

	if ((err = parseobj(buf, out))) {
		LOGF("failed to parse obj from %s", path);
		goto err_free;
	}

err_free:
	free(buf);
	return err;
}
