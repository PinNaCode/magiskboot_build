#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "internal/errno.h"
#include "internal/fd.h"
#include "internal/fs.h"

#include "../include/winsup/open_compat.h"

#ifndef NDEBUG
#  include "internal/assert.h"

#  define LOG_TAG             "open_compat"
#  define LOG_ERR(...)        log_err(LOG_TAG, __VA_ARGS__);
#endif

static int open_impl(const char *path, int oflag, va_list ap) {
    oflag |= O_BINARY;

    struct stat buf;

    if (oflag & O_CREAT) {
        // creating file, cannot be a directory

        // if we are creating a new file, make sure the parent directory is case sensitive
        if (access(path, F_OK) != 0)
            __ensure_case_sensitive(path, true);

        int ret = open(path, oflag, (mode_t) va_arg(ap, int));

        return ret;
    }

    // otherwise the 3rd argument `mode' is unused

    if (stat(path, &buf) < 0)
        // doesn't exist, cannot be a directory
        goto use_mingw;

    if (S_ISDIR(buf.st_mode)) {
        DWORD access = 0, share_mode = 0;

        switch (oflag & O_ACCMODE) {
            case O_RDONLY:  // this is the default
                access = GENERIC_READ;
                share_mode = FILE_SHARE_VALID_FLAGS;
                break;
            case O_WRONLY:
            case O_RDWR:
                // writing to directory fd is nonsense
                errno = EISDIR;
                return -1;
            default:
#ifndef NDEBUG
                LOG_ERR("invalid access mode");
#endif
                errno = EINVAL;
                return -1;
        }

        // ignoring other oflags according to POSIX

        int fd = __open_dir_fd(path, access, share_mode, oflag);

        if (fd < 0) {
            __set_errno_via_winerr(GetLastError());

            return -1;
        }

        return fd;
    }

use_mingw:
    return open(path, oflag);  // not a directory, mingw impl is good
}

int _open_stub(const char *path, int oflag, ... ) {
    va_list ap;
    va_start(ap, oflag);

    int fd = open_impl(path, oflag, ap);

    va_end(ap);

    if (!(fd < 0))
        __fd_cache_oflag(fd, oflag);

    return fd;
}
