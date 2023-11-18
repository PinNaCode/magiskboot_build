#include <sys/stat.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "internal/assert.h"
#include "internal/fd.h"
#include "internal/errno.h"

int _stat_stub(const char *name, struct stat *buf) {
    return stat(name, buf);
}

int _fstat_stub(int fd, struct stat *buf) {
    char *path = __fd_get_path(fd);

    if (!path) {
        __set_errno_via_winerr(GetLastError());

        return -1;
    }

    return _stat_stub(path, buf);
}

int lstat (const char *__restrict path, struct stat *__restrict buf) {
    TODO(lstat)
    return -1;
}
