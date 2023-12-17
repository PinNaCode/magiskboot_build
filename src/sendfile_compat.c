#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

#define sf_min(a, b)  (((a) < (b)) ? (a) : (b))

#define sf_buf_size     8192

void _sendfile_stub(int out_fd, int in_fd, size_t count) {
    char sf_buf[sf_buf_size];
    size_t n_read, n_write, n_write_new;
    off_t n_left = count;

    while (n_left) {
        n_read = read(in_fd, &sf_buf, sf_min(n_left, sf_buf_size));
        if (n_read == 0)
            break;
        if (n_read < 0) {
            if (errno == EINTR)  // operation interrupted by signal
                continue;  // retry the read
#ifndef NDEBUG
            perror("read");
#endif
            break;
        }
        n_write = write(out_fd, &sf_buf, n_read);
        assert(n_write != 0);
        if (n_write < 0) {
            if (errno == EINTR)  // operation interrupted by signal
                n_write = 0;  // allow retrying the write
            else {
#ifndef NDEBUG
                perror("write");
#endif
                return;
            }
        }
        while (n_write < n_read) {
            n_write_new = write(out_fd, (&sf_buf) + n_write, n_read - n_write);
            assert(n_write_new != 0);
            if (n_write_new < 0) {
                if (errno == EINTR)  // operation interrupted by signal
                    continue;  // retry the write
#ifndef NDEBUG
                perror("write");
#endif
                return;
            }
            n_write += n_write_new;
        }
        n_left -= n_write;
    }

    return;
}
