#ifndef __HAVE_ERROR_H
#define __HAVE_ERROR_H

typedef enum {
	ERR_OK = 0,
	ERR_FS,
	ERR_PARSEOBJ,
	ERR_GL,

	MAX_ERR,
} Err;

#endif
