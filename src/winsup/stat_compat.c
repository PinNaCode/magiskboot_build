#include <sys/stat.h>

#include "internal/assert.h"
#include "internal/fd.h"

int _stat_stub(const char *name, struct stat *buf) {
    return stat(name, buf);
}

int _fstat_stub(int fd, struct stat *buf) {
    char *path = __fd_get_path(fd);

    if (!path)
        return -1;

    return _stat_stub(path, buf);
}

int lstat (const char *__restrict path, struct stat *__restrict buf) {
    TODO(lstat)
    return -1;
}
