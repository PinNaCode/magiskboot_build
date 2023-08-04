#include "../../include/winsup/dirent_compat.h"

#include "internal/assert.h"

struct _dirent_stub* _readdir_stub (DIR* dirp) {
    TODO(readdir)
    return NULL;
}

int dirfd (DIR* dirp) {
    TODO(dirfd)
    return -1;
}
