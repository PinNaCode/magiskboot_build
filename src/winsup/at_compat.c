#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "internal/assert.h"

ssize_t readlinkat (int dirfd, const char *__restrict path,
                            char *__restrict buf, size_t len) {
    TODO(readlinkat)
    return -1;
}

int       symlinkat (const char *oldpath, int newdirfd, const char *newpath) {
    TODO(symlinkat)
    return -1;
}

int     linkat (int dirfd1, const char *path1, int dirfd2, const char *path2, int flags) {
    TODO(linkat)
    return -1;
}

int unlinkat (int dirfd, const char *pathname, int flags) {
    TODO(unlinkat)
    return -1;
}

int openat (int dirfd, const char *pathname, int flags, mode_t mode) {
    TODO(openat)
    return -1;
}

int faccessat (int dirfd, const char *path, int mode, int flags) {
    TODO(faccessat)
    return -1;
}

int fstatat (int dirfd, const char *__restrict pathname, struct stat *__restrict buf, int flags) {
    TODO(fstatat)
    return -1;
}

int mkdirat (int dirfd, const char *pathname, mode_t mode) {
    TODO(mkdirat)
    return -1;
}

int renameat (int olddirfd, const char *oldpath, int newdirfd, const char * newpath) {
    TODO(renameat)
    return -1;
}
