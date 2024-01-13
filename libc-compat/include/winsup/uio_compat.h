#ifndef _MAGISKBOOT_BUILD_WINSUP_UIO_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_UIO_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <sys/types.h>

struct iovec {
    void *iov_base;
    size_t iov_len;
};

ssize_t readv(int fd, const struct iovec *iov, int iov_cnt);
ssize_t writev(int fd, const struct iovec *iov, int iov_cnt);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_UIO_COMPAT */
