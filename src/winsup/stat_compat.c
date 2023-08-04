#include <sys/stat.h>

#include "internal/assert.h"

int _stat_stub(const char *name, struct stat *buf) {
    TODO(stat)
    return -1;
}

int _fstat_stub(int fd, struct stat *buf) {
    TODO(fstat)
    return -1;
}
