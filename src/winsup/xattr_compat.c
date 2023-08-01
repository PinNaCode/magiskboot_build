#include <sys/types.h>

ssize_t getxattr (const char *path, const char *name, void *value, size_t size) {
    return -1;
}

ssize_t lgetxattr (const char *path, const char *name, void *value, size_t size) {
    return -1;
}

ssize_t fgetxattr (int fd, const char *name, void *value, size_t size) {
    return -1;
}

int setxattr (const char *path, const char *name, const void *value, size_t size, int flags) {
    return -1;
}

int lsetxattr (const char *path, const char *name, const void *value, size_t size, int flags) {
    return -1;
}

int fsetxattr (int fd, const char *name, const void *value, size_t size, int flags) {
    return -1;
}
