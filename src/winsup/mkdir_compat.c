#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "../../include/winsup/acl_compat.h"

int _mkdir_stub (const char *path, mode_t mode) {
    if (mkdir(path) < 0)
        return -1;

    if (_chmod_stub(path, mode) < 0) {
        int old_errno = errno;
        rmdir(path);
        errno = old_errno;
        return -1;
    }

    return 0;
}
