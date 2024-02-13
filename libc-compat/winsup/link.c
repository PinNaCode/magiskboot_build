#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef NDEBUG
#  include <stdio.h>
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ntdef.h>
#include <winioctl.h>

#include "internal/errno.h"
#include "internal/fd.h"

#include "../include/winsup/link_compat.h"

ssize_t readlink (const char *__restrict path,
                          char *__restrict buf, size_t len) {
    if (!(len > 0)) {
        errno = EINVAL;
        return -1;
    }

    int fd;

    if ((fd = __open_symlink_fd(path, GENERIC_READ, FILE_SHARE_VALID_FLAGS, 0)) < 0) {
        __set_errno_via_winerr(GetLastError());

        return -1;
    }

    HANDLE h;

    if ((h = (HANDLE) _get_osfhandle(fd)) == INVALID_HANDLE_VALUE) {
        // errno is already set to EBADF by _get_osfhandle

        close(fd);  // ownership not transferred

        return -1;
    }

    ssize_t ret = -1;

    REPARSE_DATA_BUFFER *buff = malloc(MAXIMUM_REPARSE_DATA_BUFFER_SIZE);

    if (!buff)
        goto error;

    DWORD bytes;

    if (!DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, NULL, 0, buff, MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &bytes, NULL)) {
        __set_errno_via_winerr(GetLastError());

#ifndef NDEBUG
        perror("DeviceIoControl");
#endif

        goto error;
    }

    if (buff->ReparseTag != IO_REPARSE_TAG_SYMLINK) {
        // not a symlink
        errno = EINVAL;
        goto error;
    }

    WCHAR *ws_ptr;
    size_t ws_len;

    // From windows-libc:
    //     https://github.com/SibiSiddharthan/windows-libc/blob/dcb7a2a5889d5bd54e6241dbf6d469114e8a3302/src/unistd/readlink.c#L76-L98
    if (buff->SymbolicLinkReparseBuffer.PrintNameLength != 0) {
        ws_ptr = (WCHAR *) ((CHAR *) (buff->SymbolicLinkReparseBuffer.PathBuffer) + buff->SymbolicLinkReparseBuffer.PrintNameOffset);
        ws_len = buff->SymbolicLinkReparseBuffer.PrintNameLength;
    } else if (buff->SymbolicLinkReparseBuffer.SubstituteNameLength != 0) {
        ws_ptr = (WCHAR *) ((CHAR *) (buff->SymbolicLinkReparseBuffer.PathBuffer) + buff->SymbolicLinkReparseBuffer.SubstituteNameOffset);
        ws_len = buff->SymbolicLinkReparseBuffer.SubstituteNameLength;
    } else {
        // this should not happen
        assert(0);
        return -1;  // never reached
    }

    size_t chars;
    errno_t err = wcstombs_s(&chars, buf, len, ws_ptr, MIN(len - 1, ws_len / sizeof(WCHAR)));

    if (err) {
        errno = err;
        goto error;
    }

    ret = chars - 1;  // not including the terminating NULL

error:
    if (h != INVALID_HANDLE_VALUE)
        CloseHandle(h);  // no need to close fd since handle already owns it

    if (buff)
        free(buff);

    return ret;
}

int link(const char *path1, const char *path2) {
    struct stat buf;

    if (stat(path1, &buf) < 0)
        return -1;

    if (S_ISDIR(buf.st_mode)) {
        errno = EPERM;  // not supported

        return -1;
    }

    if (!CreateHardLink(path2, path1, NULL)) {
        __set_errno_via_winerr(GetLastError());

        return -1;
    }

    return 0;
}

int     symlink (const char *name1, const char *name2) {
    /*
     * this is not perfect
     *
     * this doesn't map well to Windows API,
     * because it depend on the link type.
     *
     * a wrong type can make the symlink unusable
     * this also applies when the target doesn't exist
     *  or changes its type after calling symlink
     *
     * for now we just ignore these
     */

    struct stat buf;
    // prefer non-privileged access when possible
    DWORD flags = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

    if (stat(name1, &buf) < 0)
        goto not_directory;

    if (S_ISDIR(buf.st_mode))
        flags |= SYMBOLIC_LINK_FLAG_DIRECTORY;

not_directory:
    if (!CreateSymbolicLink(name2, name1, flags)) {
        __set_errno_via_winerr(GetLastError());

        return -1;
    }

    return 0;
}
