#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "internal/assert.h"

int unlinkat (int dirfd, const char *pathname, int flags) {
    TODO(unlinkat)
    return -1;
}

int openat (int dirfd, const char *pathname, int flags, mode_t mode) {
    TODO(openat)
    return -1;
}

DIR *fdopendir(int fd) {
    TODO(fdopendir)
    return NULL;
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
