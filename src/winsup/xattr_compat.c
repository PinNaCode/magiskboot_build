#include <sys/types.h>

#include "internal/assert.h"

ssize_t getxattr (const char *path, const char *name, void *value, size_t size) {
    TODO(getxattr)
    return -1;
}

ssize_t lgetxattr (const char *path, const char *name, void *value, size_t size) {
    TODO(lgetxattr)
    return -1;
}

ssize_t fgetxattr (int fd, const char *name, void *value, size_t size) {
    TODO(fgetxattr)
    return -1;
}

int setxattr (const char *path, const char *name, const void *value, size_t size, int flags) {
    TODO(setxattr)
    return -1;
}

int lsetxattr (const char *path, const char *name, const void *value, size_t size, int flags) {
    TODO(lsetxattr)
    return -1;
}

int fsetxattr (int fd, const char *name, const void *value, size_t size, int flags) {
    TODO(fsetxattr)
    return -1;
}
