#include <sys/stat.h>

#include "internal/assert.h"

int _stat_stub(const char *name, struct stat *buf) {
    return stat(name, buf);
}

int _fstat_stub(int fd, struct stat *buf) {
    return fstat(fd, buf);
}

int lstat (const char *__restrict path, struct stat *__restrict buf) {
    TODO(lstat)
    return -1;
}
