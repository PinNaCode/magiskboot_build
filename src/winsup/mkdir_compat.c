#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "internal/fs.h"

int _mkdir_stub (const char *path, mode_t mode) {
    if (mkdir(path) < 0)
        return -1;

    if (chmod(path, mode) < 0) {
        int old_errno = errno;
        rmdir(path);
        errno = old_errno;
        return -1;
    }

    // enable case sensitive for the newly created directory
    __ensure_case_sensitive(path, false);

    return 0;
}
