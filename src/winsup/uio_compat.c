/*
 * from QEMU:
 *      https://github.com/qemu/qemu/blob/802341823f1720511dd5cf53ae40285f7978c61b/util/osdep.c#L560
 *
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>
#include <errno.h>

#include "../../include/winsup/uio_compat.h"

static inline ssize_t __uio_impl(int fd, const struct iovec *iov, int iov_cnt, bool do_write) {
    unsigned i = 0;
    ssize_t ret = 0;
    ssize_t off = 0;
    while (i < iov_cnt) {
        ssize_t r = do_write
            ? write(fd, iov[i].iov_base + off, iov[i].iov_len - off)
            : read(fd, iov[i].iov_base + off, iov[i].iov_len - off);
        if (r > 0) {
            ret += r;
            off += r;
            if (off < iov[i].iov_len) {
                continue;
            }
        } else if (!r) {
            break;
        } else if (errno == EINTR) {
            continue;
        } else {
            /* else it is some "other" error,
             * only return if there was no data processed. */
            if (ret == 0) {
                ret = -1;
            }
            break;
        }
        off = 0;
        i++;
    }
    return ret;
}

ssize_t readv(int fd, const struct iovec *iov, int iov_cnt) {
    return __uio_impl(fd, iov, iov_cnt, false);
}

ssize_t writev(int fd, const struct iovec *iov, int iov_cnt) {
    return __uio_impl(fd, iov, iov_cnt, true);
}
