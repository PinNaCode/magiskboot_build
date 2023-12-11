#ifndef _MAGISKBOOT_BUILD_LIBBSD_COMPAT
#define _MAGISKBOOT_BUILD_LIBBSD_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

size_t _strlcpy_stub(char *dst, const char *src, size_t siz);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_LIBBSD_COMPAT */
