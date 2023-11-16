#include <limits.h>
#include <string.h>
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifndef NDEBUG
#include "assert.h"

#define LOG_TAG             "fd_internal"
#define LOG_ERR(...)        log_err(LOG_TAG, __VA_ARGS__);
#endif

static const char *PATH_PFX = "\\\\?\\";
static const char *UNC_PATH_PFX = "\\\\?\\UNC\\";

static size_t path_pfx_len;
static size_t unc_path_pfx_len;

__attribute__((constructor)) static void __init_data(void) {
    path_pfx_len = strlen(PATH_PFX);
    unc_path_pfx_len = strlen(UNC_PATH_PFX);
}

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

    size_t buf_len = strlen(buf);
    size_t path_len;

    if (!strncmp(buf, UNC_PATH_PFX, unc_path_pfx_len)) {
        path_len = buf_len - unc_path_pfx_len;
        memmove(buf + 2, buf + unc_path_pfx_len, path_len);
        buf[path_len + 2] = '\0';
    } else if (!strncmp(buf, PATH_PFX, path_pfx_len)) {
        path_len = buf_len - path_pfx_len;
        memmove(buf, buf + path_pfx_len, path_len);
        buf[path_len] = '\0';
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