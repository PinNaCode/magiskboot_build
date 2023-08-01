#ifndef _MAGISKBOOT_BUILD_WINSUP_STR_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_STR_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

void *memmem(const void *haystack, int haystacklen, const void *needle, int needlelen);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_STR_COMPAT */
