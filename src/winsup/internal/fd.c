#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../../../include/winsup/stat_compat.h"

#include "errno.h"

#ifndef NDEBUG
#include "assert.h"

#define LOG_TAG             "fd_internal"
#define LOG_ERR(...)        log_err(LOG_TAG, __VA_ARGS__);
#endif

char *__fd_get_path(int fd) {
    HANDLE h = (HANDLE) _get_osfhandle(fd);

    if (h == INVALID_HANDLE_VALUE) {
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
    }

    char *buf = malloc(PATH_MAX);

    if (!buf)
        return NULL;

    if (!GetFinalPathNameByHandle(h, buf, PATH_MAX, 0)) {
#ifndef NDEBUG
        LOG_ERR("GetFinalPathNameByHandle failed: %s", win_strerror(GetLastError()))
#endif
        free(buf);

        return NULL;
    }

    size_t buf_len = strlen(buf);
    size_t path_len;

    if (!strncmp(buf, "\\\\?\\UNC\\", 8)) {
        path_len = buf_len - 8;
        memmove(buf + 2, buf + 8, path_len);
        buf[path_len + 2] = '\0';
    } else if (!strncmp(buf, "\\\\?\\", 4)) {
        path_len = buf_len - 4;
        memmove(buf, buf + 4, path_len);
        buf[path_len] = '\0';
    }

    return buf;
}

int __open_dir_fd(const char *path, DWORD access, DWORD share_mode, int flags) {
    HANDLE h;
    int fd;

    if ((h = CreateFile(path, access, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL)) == INVALID_HANDLE_VALUE)
        return -1;

    if ((fd = _open_osfhandle((intptr_t) h, flags)) < 0) {
#ifndef NDEBUG
        LOG_ERR("_open_osfhandle failed")
#endif
        SetLastError(ERROR_INVALID_PARAMETER);  // EINVAL

        CloseHandle(h);

        return -1;
    }

    // don't close the original handle, the ownership is transferred to the fd

    return fd;
}

int __open_symlink_fd(const char *path, DWORD access, DWORD share_mode, int flags) {
    HANDLE h;
    int fd;

    if ((h = CreateFile(path, access, share_mode, NULL, OPEN_EXISTING,
            FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL)) == INVALID_HANDLE_VALUE)
        return -1;

    if ((fd = _open_osfhandle((intptr_t) h, flags)) < 0) {
#ifndef NDEBUG
        LOG_ERR("_open_osfhandle failed")
#endif
        SetLastError(ERROR_INVALID_PARAMETER);  // EINVAL

        CloseHandle(h);

        return -1;
    }

    // don't close the original handle, the ownership is transferred to the fd

    return fd;
}

bool __is_dirfd(int fd) {
    struct stat buf;

    if (_fstat_stub(fd, &buf) < 0)
        goto not;  // errno is set by fstat

    if (S_ISDIR(buf.st_mode))
        return true;

not:
    errno = ENOTDIR;
    return false;
}

char *__get_path_at(int dirfd, const char *name) {
    if (!__is_dirfd(dirfd))
        return NULL;  // errno is set by this call

    char *parent = __fd_get_path(dirfd);

    if (!parent) {
        __set_errno_via_winerr(GetLastError());

        return NULL;
    }

    char *buff = malloc(PATH_MAX);

    if (!buff)
        return NULL;

    snprintf(buff, PATH_MAX, "%s/%s", parent, name);

    return buff;
}
