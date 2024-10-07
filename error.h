#ifndef __HAVE_ERROR_H
#define __HAVE_ERROR_H

#include <stdio.h>

#define LOGF(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__)
#define LOG(msg) fputs(msg "\n", stderr)

typedef enum {
	ERR_OK = 0,
	ERR_FS,
	ERR_PARSEOBJ,
	ERR_GL,

	MAX_ERR,
} Err;

#endif
