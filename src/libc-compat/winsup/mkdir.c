#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/winsup/mkdir_compat.h"

int _mkdir_stub (const char *path, mode_t mode) {
    if (mkdir(path) < 0)
        return -1;

    if (chmod(path, mode) < 0) {
        int old_errno = errno;
        assert(!rmdir(path));  // mkdir() just succeed, how come
        errno = old_errno;
        return -1;
    }

    return 0;
}
