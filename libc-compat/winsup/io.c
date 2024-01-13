#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>  // for _fmode
#include <sys/types.h>
#include <unistd.h>

#include "../include/winsup/io_compat.h"

ssize_t _read_stub(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t _write_stub(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}

// From libnt:
//      https://github.com/svoboda18/libnt/blob/f3027ff6261a1bfdbcec32de09ac6987aca80f22/libnt.c#L308-L324
__attribute__((constructor)) static void __init_io_mode(void) {
    _fmode = O_BINARY;
    _setmode(_fileno(stdin), O_BINARY);
    _setmode(_fileno(stdout), O_BINARY);
    _setmode(_fileno(stderr), O_BINARY);
}
