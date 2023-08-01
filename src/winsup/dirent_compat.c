#include "../../include/winsup/dirent_compat.h"

struct _dirent_stub* _readdir_stub (DIR* dirp) {
    return NULL;
}

int dirfd (DIR* dirp) {
    return -1;
}
