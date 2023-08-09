#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "internal/assert.h"
#include "internal/errno.h"

ssize_t readlink (const char *__restrict path,
                          char *__restrict buf, size_t len) {
    TODO(readlink)
    return -1;
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
