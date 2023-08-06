#include <limits.h>
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef NDEBUG
#include "assert.h"

#define LOG_TAG             "fd_internal"
#define LOG_ERR(...)        log_err(LOG_TAG, __VA_ARGS__);
#endif

char *__fd_get_path(int fd) {
    char *buf = malloc(PATH_MAX);

    if (!buf)
        return NULL;

    if (!GetFinalPathNameByHandle((HANDLE) _get_osfhandle(fd), buf, PATH_MAX, 0)) {
#ifndef NDEBUG
        LOG_ERR("GetFinalPathNameByHandle failed: %s", win_strerror(GetLastError()))
#endif
        free(buf);

        return NULL;
    }

    return buf;
}

int __open_dir_fd(const char *path, DWORD access, DWORD share_mode, int flags) {
    HANDLE h;
    int fd;

    if ((h = CreateFile(path, access, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL)) == INVALID_HANDLE_VALUE) {
#ifndef NDEBUG
        LOG_ERR("CreateFile failed: %s", win_strerror(GetLastError()));
#endif

        return -1;
    }

    if ((fd = _open_osfhandle((intptr_t) h, flags)) < 0) {
#ifndef NDEBUG
        LOG_ERR("_open_osfhandle failed")
#endif
        SetLastError(ERROR_INVALID_PARAMETER);  // EINVAL

        return -1;
    }

    // don't close the original handle, the ownership is transferred to the fd

    return fd;
}
