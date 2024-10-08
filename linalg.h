#ifndef __HAVE_LINALG_H
#define __HAVE_LINALG_H

#include <stddef.h>

#define ID4 ((mat4) {{ \
		{1.0, 0.0, 0.0, 0.0}, \
		{0.0, 1.0, 0.0, 0.0}, \
		{0.0, 0.0, 1.0, 0.0}, \
		{0.0, 0.0, 0.0, 1.0}, \
	}})

typedef struct {
	float v[2];
} vec2;

typedef struct {
	float v[3];
} vec3;

typedef struct {
	// array of columns
	float v[4][4];
} mat4;

static inline mat4 mat4mul(const mat4 *lhs, const mat4 *rhs) {
	mat4 ret;

	for (size_t col = 0; col < 4; col++) {
		for (size_t row = 0; row < 4; row++) {
			ret.v[col][row] =
				lhs->v[0][row] * rhs->v[row][0] +
				lhs->v[1][row] * rhs->v[row][1] +
				lhs->v[2][row] * rhs->v[row][2] +
				lhs->v[3][row] * rhs->v[row][3];
		}
	}

	return ret;
}

// scale top-left 3x3 submatrix by `s` in-place
static inline void mat4muls3x3(mat4 *m, float s) {
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			m->v[i][j] *= s;
		}
	}
}

#endif
