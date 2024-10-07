#include "util.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

Err readtostring(const char *path, char **outptr) {
	int fd;
	Err err = ERR_OK;
	struct stat st;

	if ((fd = open(path, O_RDONLY)) == -1)
		return ERR_FS;

	if (fstat(fd, &st) == -1 || !S_ISREG(st.st_mode))
		goto out_close;

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
