#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../../include/winsup/open_compat.h"

#ifndef O_CLOEXEC
#define O_CLOEXEC               O_NOINHERIT
#endif

// From musl:
//      https://github.com/bminor/musl/blob/f314e133929b6379eccc632bef32eaebb66a7335/src/stdio/__fmodeflags.c
//      https://github.com/bminor/musl/blob/f314e133929b6379eccc632bef32eaebb66a7335/src/stdio/fopen.c#L12-L16
static int __fmodeflags(const char *mode)
{
    if (!strchr("rwa", *mode))
        // check if mode is valid
        return -1;

    int flags;

    if (strchr(mode, '+')) flags = O_RDWR;
    else if (*mode == 'r') flags = O_RDONLY;
    else flags = O_WRONLY;

    if (strchr(mode, 'x')) flags |= O_EXCL;
    if (strchr(mode, 'e') || strchr(mode, 'N')) flags |= O_CLOEXEC;
    if (*mode != 'r') flags |= O_CREAT;
    if (*mode == 'w') flags |= O_TRUNC;
    if (*mode == 'a') flags |= O_APPEND;

    flags |= O_BINARY;

    return flags;
}

static int __fixup_fmode(const char *old_mode, char *mode) {
    int flags = __fmodeflags(old_mode);

    if (flags < 0)
        return -1;

    bool plus = false;
    int ptr = 0;

    if (flags & O_RDWR) {
        plus = true;

        if (flags & O_APPEND)
            mode[ptr] = 'a';
        else if (flags & O_TRUNC)
            mode[ptr] = 'w';
        else if (!(flags & O_CREAT))
            mode[ptr] = 'r';
        else
            return -1;
    } else if (flags & O_WRONLY) {
        if (flags & O_TRUNC)
            mode[ptr] = 'w';
        else if (flags & O_APPEND)
            mode[ptr] = 'a';
        else
            return -1;
    } else  // O_RDONLY
        mode[ptr] = 'r';

    ptr++;

    if (mode[0] != 'r' && !(flags & O_CREAT))
        return -1;

    if (plus) {
        mode[ptr] = '+';
        ptr++;
    }

    mode[ptr] = 'b';
    ptr++;

    // fd already opened, ignore nonsense O_EXCL and O_CLOEXEC

    mode[ptr] = '\0';

    return 0;
}

FILE *_fdopen_stub(int fd, const char *mode) {
    char new_mode[16];

    if (__fixup_fmode(mode, new_mode) < 0) {
        errno = EINVAL;
        return NULL;
    }

    return fdopen(fd, new_mode);
}

FILE *_fopen_stub(const char *__restrict pathname, const char *__restrict mode) {
    int flags = __fmodeflags(mode);

    if (flags < 0) {
        errno = EINVAL;
        return NULL;
    }

    int fd;

    if (flags & O_CREAT)
        fd = _open_stub(pathname, flags, 0666);
    else
        fd = _open_stub(pathname, flags);

    if (fd < 0)
        return NULL;

    return _fdopen_stub(fd, mode);
}
