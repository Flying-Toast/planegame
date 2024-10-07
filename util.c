#include "util.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

Err readtostring(const char *path, char **outptr) {
	int fd;
	Err err = ERR_OK;
	struct stat st;

	if ((fd = open(path, O_RDONLY)) == -1) {
		LOGF("can't open file %s: %s", path, strerror(errno));
		return ERR_FS;
	}

	if (fstat(fd, &st) == -1) {
		LOGF("failed to stat fd of %s: %s", path, strerror(errno));
		err = ERR_FS;
		goto out_close;
	}

	if (!S_ISREG(st.st_mode)) {
		LOGF("%s is not regular file", path);
		err = ERR_FS;
		goto out_close;
	}

	char *buf = calloc(st.st_size + 1, 1);
	if (read(fd, buf, st.st_size) == -1)
		goto out_free;

	*outptr = buf;
	goto out_close;
out_free:
	free(buf);
out_close:
	close(fd);
	return err;
}
