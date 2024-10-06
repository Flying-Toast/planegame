#include "model.h"
#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr[0])))

static Err readobj(const char *path, struct model *out);

static struct model monkey;

static Err cachemodel(const char *path, struct model *storage, const struct model **out) {
	if (storage->faces == NULL) {
		Err err = readobj(path, storage);
		if (err)
			return err;
	}
	*out = storage;
	return ERR_OK;
}

Err getmodel(enum modelkey key, const struct model **out) {
	switch (key) {
		case MODEL_MONKEY:
			return cachemodel("data/monkey.obj", &monkey, out);
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

static Err parseindex(const char **string, size_t *out) {
	const char *s = *string;

	if (*s == '\0')
		return ERR_PARSEOBJ;

	char *endptr;
	size_t parsed = strtoul(s, &endptr, 10);
	if (endptr == s)
		return ERR_PARSEOBJ;

	*out = parsed;
	*string = endptr;
	return ERR_OK;
}

static Err parsevert(const char **string, struct vert *out) {
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

static Err parseverts(const char **string, size_t n, struct vert *outlist) {
	const char *s = *string;

	for (size_t i = 0; i < n; i++) {
		if (parsevert(&s, &outlist[i]))
			return ERR_PARSEOBJ;
		if (i != n - 1 && (!*s || *(s++) != ' '))
			return ERR_PARSEOBJ;
	}

	*string = s;
	return ERR_OK;
}

static Err parseobj(const char *s, struct model *out) {
	size_t npoints, nfaces, nuvs, nnorms;
	countlines(s, &npoints, &nfaces, &nuvs, &nnorms);
	// we leak these; the model gets cached in getmodel()
	struct vec3 *points = malloc(sizeof(struct vec3) * npoints);
	struct vec3 *norms = malloc(sizeof(struct vec3) * nnorms);
	struct face *faces = malloc(sizeof(struct face) * nfaces);
	struct vec2 *uvs = malloc(sizeof(struct vec2) * nuvs);

	size_t curpoint = 0, curface = 0, curuv = 0, curnorm = 0;

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
			if (parseverts(&s, 3, faces[curface].verts))
				goto err_free;
			curface++;
		}
	} while(nextline(&s));

	out->points = points;
	out->npoints = npoints;
	out->norms = norms;
	out->nnorms = nnorms;
	out->faces = faces;
	out->nfaces = nfaces;
	out->uvs = uvs;
	out->nuvs = nuvs;
	return ERR_OK;
err_free:
	free(points);
	free(norms);
	free(faces);
	free(uvs);
	return ERR_PARSEOBJ;
}

static Err readobj(const char *path, struct model *out) {
	int fd;
	Err err = ERR_OK;
	struct stat st;

	if ((fd = open(path, O_RDONLY)) == -1)
		return ERR_READOBJ;

	if (fstat(fd, &st) == -1 || !S_ISREG(st.st_mode))
		goto out_close;

	char *buf = malloc(st.st_size);
	if (read(fd, buf, st.st_size) == -1)
		goto out_free;

	err = parseobj(buf, out);
out_free:
	free(buf);
out_close:
	close(fd);
	return err;
}
