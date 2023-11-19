#include <unistd.h>

#include "../../include/winsup/acl_compat.h"

#include "internal/errno.h"
#include "internal/fd.h"

int fchmod (int fd, mode_t mode) {
    char *path = __fd_get_path(fd);

    if (!path) {
        __set_errno_via_winerr(GetLastError());

        return -1;
    }

    int ret = chmod(path, mode);

    free(path);

    return ret;
}

int     chown (const char *path, uid_t owner, gid_t group) {
    return 0;  // TODO: implement
}

int     fchown (int fildes, uid_t owner, gid_t group) {
    char *path = __fd_get_path(fildes);

    if (!path) {
        __set_errno_via_winerr(GetLastError());

        return -1;
    }

    int ret = chown(path, owner, group);

    free(path);

    return ret;
}
