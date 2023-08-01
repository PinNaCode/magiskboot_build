#ifndef _MAGISKBOOT_BUILD_WINSUP_IO_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_IO_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <io.h>

ssize_t _read_stub(int fd, void *buf, size_t count);

ssize_t _write_stub(int fd, const void *buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_IO_COMPAT */
