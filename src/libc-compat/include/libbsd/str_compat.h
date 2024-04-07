#ifndef _MAGISKBOOT_BUILD_LIBBSD_STR_COMPAT
#define _MAGISKBOOT_BUILD_LIBBSD_STR_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

size_t strlcpy(char *dst, const char *src, size_t siz);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_LIBBSD_STR_COMPAT */
