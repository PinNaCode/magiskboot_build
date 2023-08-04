#include <stdio.h>
#include <stdlib.h>

void abort_unimpl(const char *s) {
    fprintf(stderr, "magiskboot_build ABORT: %s is not implemented\n", s);
    fflush(stderr);
    abort();
}
