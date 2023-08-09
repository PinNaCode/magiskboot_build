#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../../include/winsup/dirent_compat.h"

#include "../../include/winsup/open_compat.h"
#include "../../include/winsup/libbsd_compat.h"
#include "../../include/winsup/at_compat.h"
#include "../../include/winsup/stat_compat.h"

#include "internal/fd.h"
#include "internal/errno.h"

#ifndef O_PATH
#define O_PATH      0
#endif

#define dirent_min(a, b)  (((a) < (b)) ? (a) : (b))

_DIR_stub *_opendir_stub (const char *path) {
    struct stat buf;
    _DIR_stub *res = NULL;
    DIR *real_dirp;
    int fd = _open_stub(path, O_PATH);

    if (fd < 0)
        goto error;

    if (fstat(fd, &buf) < 0)
        goto error;

    if (!S_ISDIR(buf.st_mode)) {
        errno = ENOTDIR;

        goto error;
    }

    if (!(res = malloc(sizeof(_DIR_stub))))
        goto error;

    if (!(real_dirp = opendir(path)))
        goto error;

    res->dirp = real_dirp;
    res->fd = fd;

    return res;

error:
    if (!(fd < 0))
        close(fd);

    if (res)
        free(res);

    // if real_dirp was opened, we shouldn't reach here

    return NULL;
}

_DIR_stub *fdopendir(int fd) {
    struct stat buf;
    _DIR_stub *res = NULL;
    DIR *real_dirp;
    char *path = NULL;

    // fstat would fail if fd is invalid
    // no need to check it here

    if (fstat(fd, &buf) < 0)
        goto error;

    if (!S_ISDIR(buf.st_mode)) {
        errno = ENOTDIR;

        goto error;
    }

    if (!(path = __fd_get_path(fd))) {
        __set_errno_via_winerr(GetLastError());

        goto error;
    }

    if (!(res = malloc(sizeof(_DIR_stub))))
        goto error;

    if (!(real_dirp = opendir(path)))
        goto error;

    res->dirp = real_dirp;
    res->fd = fd;

    return res;

error:
    // real_dirp was not opened
    // so the ownership is not transferred
    // not closing the fd

    if (path)
        free(path);

    if (res)
        free(res);

    return NULL;
}

struct _dirent_stub* _readdir_stub (_DIR_stub* dirp) {
    struct dirent *d;
    static struct _dirent_stub res;  // not thread-safe
    struct stat buf;
    unsigned char type = DT_UNKNOWN;
    int old_errno;

    if (!dirp || !dirp->dirp || dirp->fd < 0) {
        errno = EBADF;

        return NULL;
    }

    if (!(d = readdir(dirp->dirp)))
        return NULL;

    // fstatat should not fail
    // or pretend there is no more entries
    old_errno = errno;

    if (fstatat(dirp->fd, d->d_name, &buf, AT_SYMLINK_NOFOLLOW) < 0) {
        errno = old_errno;

        return NULL;
    }

    if (S_ISREG(buf.st_mode))
        type = DT_REG;
    else if (S_ISLNK(buf.st_mode))
        type = DT_LNK;
    else if (S_ISDIR(buf.st_mode))
        type = DT_DIR;

    // clone data
    res.d_ino = d->d_ino;
    res.d_reclen = d->d_reclen;
    res.d_namlen = strlcpy(res.d_name, d->d_name, dirent_min(sizeof(res.d_name), d->d_namlen));
    res.d_type = type;

    return &res;
}

int _closedir_stub (_DIR_stub *dirp) {
    // try to free maximum resources

    if (!dirp) {
        errno = EBADF;

        return -1;
    }

    if (dirp->dirp)
        closedir(dirp->dirp);

    if (!(dirp->fd < 0))
        close(dirp->fd);

    free(dirp);

    return 0;
}

void _rewinddir_stub (_DIR_stub *dirp) {
    if (!dirp)  // POSIX didn't say to set errno
        return;

    if (dirp->dirp)
        rewinddir(dirp->dirp);

    // Windows doesn't care this
    // just for emulation
    // in case any one try to do sth with it
    if (!(dirp->fd < 0))
        lseek(dirp->fd, 0, SEEK_SET);

    return;
}

long _telldir_stub (_DIR_stub *dirp) {
    if (!dirp || !dirp->dirp) {
        errno = EBADF;

        return -1;
    }

    return telldir(dirp->dirp);
}

void _seekdir_stub (_DIR_stub *dirp, long loc) {
    if (!dirp)  // POSIX didn't say to set errno
        return;

    if (dirp->dirp)
        seekdir(dirp->dirp, loc);

    if (!(dirp->fd < 0))
        lseek(dirp->fd, loc, SEEK_SET);

    return;
}

int dirfd (_DIR_stub* dirp) {
    if (!dirp) {
        errno = EINVAL;

        return -1;
    }

    return dirp->fd;
}
