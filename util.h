#ifndef __HAVE_UTIL_H
#define __HAVE_UTIL_H

#include "error.h"

// Caller frees *outptr
Err readtostring(const char *path, char **outptr);

#endif
