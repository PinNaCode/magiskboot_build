#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>

#include "../../include/winsup/at_compat.h"
#include "../../include/winsup/link_compat.h"
#include "../../include/winsup/mkdir_compat.h"
#include "../../include/winsup/open_compat.h"
#include "../../include/winsup/stat_compat.h"

#include "internal/fd.h"

ssize_t readlinkat (int dirfd, const char *__restrict path,
                            char *__restrict buf, size_t len) {
    if (PathIsRelative(path) == FALSE)
        return readlink(path, buf, len);  // ignore dirfd if path is absolute

    char *real_path = __get_path_at(dirfd, path);

    if (!real_path)
        return -1;  // errno already set by this call

    ssize_t ret = readlink(real_path, buf, len);

    free(real_path);

    return ret;
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
    (void) flags;  // flags is not implemented and unused for now

    int ret = -1;

    char *real_path1 = NULL;
    char *real_path2 = NULL;

    // ignore dirfd if path is absolute

    if (PathIsRelative(path1) == FALSE)
        real_path1 = strdup(path1);
    else {
        real_path1 = __get_path_at(dirfd1, path1);  // errno is set by this call

        if (!real_path1)
            goto error;
    }

    if (PathIsRelative(path2) == FALSE)
        real_path2 = strdup(path2);
    else {
        real_path2 = __get_path_at(dirfd2, path2);  // errno is set by this call

        if (!real_path2)
            goto error;
    }

    ret = link(real_path1, real_path2);

error:
    if (real_path1)
        free(real_path1);

    if (real_path2)
        free(real_path2);

    return ret;
}

int unlinkat (int dirfd, const char *pathname, int flags) {
    int (*unlinkat_funcptr)(const char *pathname);

    if (flags == AT_REMOVEDIR)
        unlinkat_funcptr = rmdir;
    else if (flags == 0)
        unlinkat_funcptr = unlink;
    else {
        // POSIX say should not allow unknown flags
        errno = EINVAL;
        return -1;
    }

    if (PathIsRelative(pathname) == FALSE) {
        // ignore dirfd if pathname is absolute
        return unlinkat_funcptr(pathname);
    }

    char *real_pathname = __get_path_at(dirfd, pathname);

    if (!real_pathname)
        return -1;  // errno already set by the call

    int ret = unlinkat_funcptr(real_pathname);

    free(real_pathname);

    return ret;
}

int openat (int dirfd, const char *pathname, int flags, ...) {
    char *real_pathname;

    if (PathIsRelative(pathname) == FALSE)
        real_pathname = strdup(pathname);
    else {
        real_pathname = __get_path_at(dirfd, pathname);

        if (!real_pathname)
            return -1;  // errno already set by the call
    }

    int ret;

    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode_t mode = (mode_t) va_arg(ap, int);
        va_end(ap);
        ret = _open_stub(pathname, flags, mode);
    } else
        ret = _open_stub(pathname, flags);

    free(real_pathname);

    return ret;
}

int faccessat (int dirfd, const char *path, int mode, int flags) {
    if (flags == AT_SYMLINK_NOFOLLOW) {
        struct stat buf;

        if (fstatat(dirfd, path, &buf, AT_SYMLINK_NOFOLLOW) < 0)
            return -1;

        mode &= (R_OK | W_OK | X_OK);

        if (mode == 0)
            goto have_access;  // F_OK, others are ignores and treat as F_OK

        if ((mode & R_OK) && !(buf.st_mode & (S_IRUSR | S_IRGRP | S_IROTH)))
            return -1;

        if ((mode & W_OK) && !(buf.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)))
            return -1;

        if ((mode & X_OK) && !(buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
            return -1;

have_access:
        return 0;
    } else if (flags) {
        errno = EINVAL;
        return -1;
    }

    if (PathIsRelative(path) == FALSE)
        return access(path, mode);  // ignore dirfd if path is absolute

    char *real_path = __get_path_at(dirfd, path);

    if (!real_path)
        return -1;  // errno already set by this call

    int ret = access(real_path, mode);

    free(real_path);

    return ret;
}

int fstatat (int dirfd, const char *__restrict pathname, struct stat *__restrict buf, int flags) {
    int (*fstatat_funcptr)(const char *path, struct stat *buf);

    if (flags == AT_SYMLINK_NOFOLLOW)
        fstatat_funcptr = lstat;
    else if (flags == 0)
        fstatat_funcptr = stat;
    else {
        errno = EINVAL;
        return -1;
    }

    if (PathIsRelative(pathname) == FALSE) {
        // ignore dirfd if pathname is absolute
        return fstatat_funcptr(pathname, buf);
    }

    char *real_pathname = __get_path_at(dirfd, pathname);

    if (!real_pathname)
        return -1;  // errno already set by the call

    int ret = fstatat_funcptr(real_pathname, buf);

    free(real_pathname);

    return ret;
}

int mkdirat (int dirfd, const char *pathname, mode_t mode) {
    if (PathIsRelative(pathname) == FALSE)
        return _mkdir_stub(pathname, mode);  // ignore dirfd if pathname is absolute

    char *real_pathname = __get_path_at(dirfd, pathname);

    if (!real_pathname)
        return -1;  // errno already set by this call

    int ret = _mkdir_stub(real_pathname, mode);

    free(real_pathname);

    return ret;
}

int renameat (int olddirfd, const char *oldpath, int newdirfd, const char * newpath) {
    int ret = -1;

    char *real_oldpath = NULL;
    char *real_newpath = NULL;

    // ignore dirfd if path is absolute

    if (PathIsRelative(oldpath) == FALSE)
        real_oldpath = strdup(oldpath);
    else {
        real_oldpath = __get_path_at(olddirfd, oldpath);  // errno is set by this call

        if (!real_oldpath)
            goto error;
    }

    if (PathIsRelative(newpath) == FALSE)
        real_newpath = strdup(newpath);
    else {
        real_newpath = __get_path_at(newdirfd, newpath);  // errno is set by this call

        if (!real_newpath)
            goto error;
    }

    ret = rename(real_oldpath, real_newpath);

error:
    if (real_oldpath)
        free(real_oldpath);

    if (real_newpath)
        free(real_newpath);

    return ret;
}
