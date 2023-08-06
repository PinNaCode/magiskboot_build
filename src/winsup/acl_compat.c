#include <unistd.h>

#include "../../include/winsup/acl_compat.h"

#include "internal/assert.h"

int _chmod_stub(const char *path, int mode) {
    return chmod(path, mode);  // TODO: implement a real POSIX chmod
}

int fchmod (int fd, mode_t mode) {
    TODO(fchmod)
    return -1;
}

int     chown (const char *path, uid_t owner, gid_t group) {
    return 0;  // TODO: implement
}

int     fchown (int fildes, uid_t owner, gid_t group) {
    TODO(fchown)
    return -1;
}
