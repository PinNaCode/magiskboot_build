#include "../../include/winsup/acl_compat.h"

int _chmod_stub(const char *path, int mode) {
    return -1;
}

int fchmod (int fd, mode_t mode) {
    return -1;
}

int     chown (const char *path, uid_t owner, gid_t group) {
    return -1;
}

int     fchown (int fildes, uid_t owner, gid_t group) {
    return -1;
}
