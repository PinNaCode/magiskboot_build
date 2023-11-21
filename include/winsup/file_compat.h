#ifndef _MAGISKBOOT_BUILD_WINSUP_FILE_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_FILE_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

FILE *_fopen_stub(const char *__restrict pathname, const char *__restrict mode);

FILE *_fdopen_stub(int fd, const char *mode);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_FILE_COMPAT */
