#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>

#include "errno.h"

#include "fd.h"

#ifndef NDEBUG
#  include "assert.h"

#  define LOG_TAG             "fd_internal"
#endif

#ifndef OPEN_MAX
#  ifndef _UCRT
// this value can be found in Windows SDK source
#    define OPEN_MAX            (8192)
#  else
// Origin: https://github.com/reactos/reactos/blob/f889c29af691e0e956c8d3bae08f7b647c9ff643/sdk/lib/crt/stdio/file.c#L111-L112
#    define OPEN_MAX            (2048)
#  endif
#endif

struct fd_cache_entry {
    int oflag;
};

// TODO: not thread-safe, add a lock if needed
// (currently magiskboot is a sinelg-threaded program)
static struct fd_cache_entry fd_cache[OPEN_MAX];

static size_t fd_cache_max_size = 0;

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
        LOG_ERR("GetFinalPathNameByHandle failed: %s", win_strerror(GetLastError()));
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
        LOG_ERR("_open_osfhandle failed");
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
        LOG_ERR("_open_osfhandle failed");
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

    if (fstat(fd, &buf) < 0)
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

    char *buff = NULL;

    if ((strlen(parent) + 1 + strlen(name)) > PATH_MAX) {
        errno = ENAMETOOLONG;

        goto exit;
    }

    buff = malloc(PATH_MAX + 1);

    if (!buff)
        goto exit;

    snprintf(buff, PATH_MAX + 1, "%s/%s", parent, name);

exit:
    if (parent)
        free(parent);

    return buff;
}

static int __get_osfhandle_oflag_fallback(HANDLE h) {
    OBJECT_BASIC_INFORMATION obi;

    NTSTATUS status = NtQueryObject(h, ObjectBasicInformation, &obi, sizeof(obi), NULL);

    if (!NT_SUCCESS(status)) {
#ifndef NDEBUG
        LOG_ERR("NtQueryObject failed: %s", win_strerror(RtlNtStatusToDosError(status)));
#endif

        return 0;
    }

    // Ref: https://github.com/rust-lang/rust/blob/32f5db98909de7bfb23cad3a48f740b99a19b01c/library/std/src/sys/windows/fs.rs#L245-L248

    if (obi.GrantedAccess & FILE_WRITE_DATA)
        return 0;  // not append mode

    return (obi.GrantedAccess & FILE_APPEND_DATA) ? O_APPEND : 0;
}

int __get_osfhandle_oflag(HANDLE h) {
    int fd;
    int oflag;
    HANDLE other;

    if (h == INVALID_HANDLE_VALUE)
        return 0;

    for (fd = 0; fd < fd_cache_max_size; fd++) {
        if ((oflag = fd_cache[fd].oflag) < 0)
            continue;  // not cached, check next one

        if ((other = (HANDLE) _get_osfhandle(fd)) == INVALID_HANDLE_VALUE) {
            // cache is outdated, fd is already closed

            fd_cache[fd].oflag = -1;

            if (fd == (fd_cache_max_size - 1))
                fd_cache_max_size--;

            continue;
        }

        if (h == other)
            return oflag;
    }

    // not cached, maybe handle opened from Rust with no fd associcated

    return __get_osfhandle_oflag_fallback(h);
}

void __fd_cache_oflag(int fd, int oflag) {
    assert(!(fd < 0));
    assert(fd < OPEN_MAX);

    fd_cache[fd].oflag = oflag & O_APPEND;  // only O_APPEND is useful for opened handle

    if (fd_cache_max_size <= fd)
        fd_cache_max_size = fd + 1;
}

__attribute__((constructor)) static void __init_oflag_cache(void) {
    int fd;

    for (fd = 0; fd < OPEN_MAX; fd++)
        fd_cache[fd].oflag = -1;
}
