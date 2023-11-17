#include <io.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "internal/fd.h"
#include "internal/errno.h"

#ifndef NDEBUG
#include "internal/assert.h"

#define LOG_TAG             "open_compat"
#define LOG_ERR(...)        log_err(LOG_TAG, __VA_ARGS__);
#endif

int _open_stub(const char *path, int oflag, ... ) {
    struct stat buf;

    if (oflag & O_CREAT) {
        // creating file, cannot be a directory
        va_list ap;
        va_start(ap, oflag);

        int ret = open(path, oflag, (mode_t) va_arg(ap, int));

        va_end(ap);

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
                share_mode = FILE_SHARE_READ;
                break;
            case O_WRONLY:
                access = GENERIC_WRITE;
                share_mode = FILE_SHARE_WRITE | FILE_SHARE_DELETE;
                break;
            case O_RDWR:
                access = GENERIC_READ | GENERIC_WRITE;
                share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
                break;
            default:
#ifndef NDEBUG
                LOG_ERR("invalid access mode for directory");
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
