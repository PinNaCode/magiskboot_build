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
