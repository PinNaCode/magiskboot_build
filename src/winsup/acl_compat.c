#include "../../include/winsup/acl_compat.h"

#include "internal/assert.h"

int _chmod_stub(const char *path, int mode) {
    TODO(chmod)
    return -1;
}

int fchmod (int fd, mode_t mode) {
    TODO(fchmod)
    return -1;
}

int     chown (const char *path, uid_t owner, gid_t group) {
    TODO(chown)
    return -1;
}

int     fchown (int fildes, uid_t owner, gid_t group) {
    TODO(fchown)
    return -1;
}
