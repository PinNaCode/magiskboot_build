#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int mknod (const char *path, mode_t mode, dev_t dev ) {
    if (access(path, R_OK) == 0) {
        errno = EEXIST;

        return -1;
    }

    // no easy way to implement mknod properly on Win32
    // since we don't have device files in normal filesystem
    errno = ENOTSUP;  // maybe set a listed errno in man page?
    return -1;
}
