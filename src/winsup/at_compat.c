#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>

#include "../../include/winsup/link_compat.h"

#include "internal/assert.h"
#include "internal/fd.h"

ssize_t readlinkat (int dirfd, const char *__restrict path,
                            char *__restrict buf, size_t len) {
    TODO(readlinkat)
    return -1;
}

int       symlinkat (const char *oldpath, int newdirfd, const char *newpath) {
    if (PathIsRelative(newpath) == FALSE) {
        // ignore newdirfd if newpath is absolute
        return symlink(oldpath, newpath);
    }

    char *real_newpath = __get_path_at(newdirfd, newpath);

    if (!real_newpath)
        return -1;  // errno is already set by this call

    int ret = symlink(oldpath, real_newpath);

    free(real_newpath);

    return ret;
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
