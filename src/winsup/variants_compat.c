#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int unlinkat (int dirfd, const char *pathname, int flags) {
    return -1;
}

int openat (int dirfd, const char *pathname, int flags, mode_t mode) {
    return -1;
}

DIR *fdopendir(int fd) {
    return NULL;
}

int faccessat (int dirfd, const char *path, int mode, int flags) {
    return -1;
}

int fstatat (int dirfd, const char *__restrict pathname, struct stat *__restrict buf, int flags) {
    return -1;
}

int mkdirat (int dirfd, const char *pathname, mode_t mode) {
    return -1;
}

int renameat (int olddirfd, const char *oldpath, int newdirfd, const char * newpath) {
    return -1;
}
