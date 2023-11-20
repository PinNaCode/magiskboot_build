#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "../../include/winsup/open_compat.h"

#ifndef O_CLOEXEC
#define O_CLOEXEC   0
#endif

// From musl:
//      https://github.com/bminor/musl/blob/f314e133929b6379eccc632bef32eaebb66a7335/src/stdio/__fmodeflags.c
static inline int __fmodeflags(const char *mode)
{
	int flags;
	if (strchr(mode, '+')) flags = O_RDWR;
	else if (*mode == 'r') flags = O_RDONLY;
	else flags = O_WRONLY;
	if (strchr(mode, 'x')) flags |= O_EXCL;
	if (strchr(mode, 'e')) flags |= O_CLOEXEC;
	if (*mode != 'r') flags |= O_CREAT;
	if (*mode == 'w') flags |= O_TRUNC;
	if (*mode == 'a') flags |= O_APPEND;
	return flags;
}

FILE *_fopen_stub(const char *__restrict pathname, const char *__restrict mode) {
    // ref: https://github.com/bminor/musl/blob/f314e133929b6379eccc632bef32eaebb66a7335/src/stdio/fopen.c#L12-L16
	if (!strchr("rwa", *mode)) {
        // check if mode is valid
		errno = EINVAL;
		return 0;
	}

	int flags = __fmodeflags(mode);

    int fd;

    if (flags & O_CREAT)
        fd = _open_stub(pathname, flags, 0666);
    else
        fd = _open_stub(pathname, flags);

    if (fd < 0)
        return NULL;

    return fdopen(fd, mode);
}
