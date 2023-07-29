#ifndef _MAGISKBOOT_BUILD_SENDFILE_COMPAT
#define _MAGISKBOOT_BUILD_SENDFILE_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

void _sendfile_stub(int out_fd, int in_fd, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_SENDFILE_COMPAT */
