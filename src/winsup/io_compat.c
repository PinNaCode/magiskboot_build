#include <sys/types.h>
#include <io.h>

ssize_t _read_stub(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t _write_stub(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}
