#include "model.h"
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ARRAY_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))

#define CACHEMODEL(path, storage, out) \
	({ \
		if (storage.verts == NULL) { \
			Err __err = readobj(path, &storage); \
			if (__err) \
				return __err; \
		} \
		*out = &storage; \
		ERR_OK; \
	})

static Err readobj(const char *path, struct model *out);

static struct model monkey;

Err getmodel(enum modelkey key, const struct model **out) {
	switch (key) {
		case MODEL_MONKEY:
			return CACHEMODEL("data/monkey.obj", monkey, out);
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

static size_t countlines(const char *string, const char *starts_with) {
	size_t n = 0, swlen = strlen(starts_with);
	bool first = true;
	while (*string) {
		if (first && strneq(string, starts_with, swlen))
			n++;

		first = *string == '\n';
		string++;
	}
	return n;
}

static bool advance_past_line_start(const char **string, const char *starts_with) {
	size_t swlen = strlen(starts_with);
	const char *s = *string;
	bool ret = true;

	while (!strneq(s, starts_with, swlen)) {
		if (*s == '\0') {
			ret = false;
			goto eos;
		}

		while (*(s++) != '\n')
			;
	}
	s += swlen;
eos:
	*string = s;
	return ret;
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

static Err parseobj(const char *s, struct model *out) {
	size_t nverts = countlines(s, "v ");
	out->nverts = nverts;
	// we leak this; it gets cached in getmodel()
	struct vert *verts = malloc(sizeof(struct vert) * nverts);

	for (size_t i = 0; i < nverts; i++) {
		if (!advance_past_line_start(&s, "v "))
			goto err_free;

		size_t npos = ARRAY_LEN(verts[i].pos);
		for (size_t pi = 0; pi < npos; pi++) {
			if (parsefloat(&s, &verts[i].pos[pi]))
				goto err_free;
			if (pi != npos - 1 && *(s++) != ' ')
				goto err_free;
		}
	}

	out->verts = verts;
	return ERR_OK;
err_free:
	free(out->verts);
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
