#include <sys/stat.h>

int _stat_stub(const char *name, struct stat *buf) {
    return -1;
}

int _fstat_stub(int fd, struct stat *buf) {
    return -1;
}
