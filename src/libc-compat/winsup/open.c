#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "internal/assert.h"
#include "internal/errno.h"
#include "internal/fd.h"
#include "internal/fs.h"

#define LOG_TAG             "open_compat"

int __cdecl __real_open(const char *path, int oflag, ... );

static int open_impl(const char *path, int oflag, va_list ap) {
    oflag |= O_BINARY;

    struct stat buf;

    if (oflag & O_CREAT) {
        // creating file, cannot be a directory

        // if we are creating a new file, make sure the parent directory is case sensitive
        if (access(path, F_OK) != 0)
            __ensure_case_sensitive(path, true);

        int ret = __real_open(path, oflag, (mode_t) va_arg(ap, int));

        return ret;
    }

    // otherwise the 3rd argument `mode' is unused

    if (stat(path, &buf) < 0)
        // doesn't exist, cannot be a directory
        goto use_mingw;

    if (S_ISDIR(buf.st_mode)) {
        DWORD access = 0, share_mode = 0;

        switch (oflag & O_ACCMODE) {
            case O_RDONLY:  // this is the default
                access = GENERIC_READ;
                share_mode = FILE_SHARE_VALID_FLAGS;
                break;
            case O_WRONLY:
            case O_RDWR:
                // writing to directory fd is nonsense
                errno = EISDIR;
                return -1;
            default:
#ifndef NDEBUG
                LOG_ERR("invalid access mode");
#endif
                errno = EINVAL;
                return -1;
        }

        // ignoring other oflags according to POSIX

        int fd = __open_dir_fd(path, access, share_mode, oflag);

        if (fd < 0) {
            __set_errno_via_winerr(GetLastError());

            return -1;
        }

        return fd;
    }

use_mingw:
    return __real_open(path, oflag);  // not a directory, mingw impl is good
}

int __cdecl __wrap_open(const char *path, int oflag, ... ) {
    va_list ap;
    va_start(ap, oflag);

    int fd = open_impl(path, oflag, ap);

    va_end(ap);

    if (!(fd < 0))
        __fd_cache_oflag(fd, oflag);

    return fd;
}

// for Rust

int __cdecl __wrap__open(const char *path, int oflag, ... ) {
    va_list ap;
    int res;

    va_start(ap, oflag);
    if (oflag & O_CREAT)
        res = open(path, oflag, (mode_t) va_arg(ap, int));
    else
        res = open(path, oflag);
    va_end(ap);

    return res;
}

HANDLE WINAPI __real_CreateFileW(const wchar_t *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                 SECURITY_ATTRIBUTES *lpSecurityAttributes, DWORD dwCreationDisposition,
                                 DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

HANDLE WINAPI __wrap_CreateFileW(const wchar_t *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                 SECURITY_ATTRIBUTES *lpSecurityAttributes, DWORD dwCreationDisposition,
                                 DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    if (!lpFileName || !lpFileName[0])
        goto skip;  // bad arguments

    switch (dwCreationDisposition) {
        case CREATE_ALWAYS:
        case CREATE_NEW:
        case OPEN_ALWAYS:
            // these flags might ask to create a new file
            break;
        case OPEN_EXISTING:
        case TRUNCATE_EXISTING:
            // these flags will always open an existing file
            goto skip;
        default:
            LOG_ERR("FIXME: unhandled dwCreationDisposition %ld", dwCreationDisposition);
            break;
    }

    char path[PATH_MAX + 1];

    if (!WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                             lpFileName, -1, path, sizeof(path), NULL, NULL)) {
        LOG_ERR("WideCharToMultiByte failed: %s", win_strerror(GetLastError()));

        abort();
    }

    // if we are creating a new file, make sure the parent directory is case sensitive
    if (access(path, F_OK) != 0)
        __ensure_case_sensitive(path, true);

skip:
    return __real_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
                              dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
