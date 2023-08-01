#include <sys/types.h>
#include <sys/stat.h>

ssize_t readlink (const char *__restrict path,
                          char *__restrict buf, size_t len) {
    return -1;
}

ssize_t readlinkat (int dirfd, const char *__restrict path,
                            char *__restrict buf, size_t len) {
    return -1;
}

int lstat (const char *__restrict path, struct stat *__restrict buf) {
    return -1;
}

int     symlink (const char *name1, const char *name2) {
    return -1;
}

int       symlinkat (const char *oldpath, int newdirfd, const char *newpath) {
    return -1;
}

int     linkat (int dirfd1, const char *path1, int dirfd2, const char *path2, int flags) {
    return -1;
}
