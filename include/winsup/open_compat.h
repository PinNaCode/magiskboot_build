#ifndef _MAGISKBOOT_BUILD_WINSUP_OPEN_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_OPEN_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

int _open_stub(const char *path, int oflag, ... );

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_OPEN_COMPAT */
