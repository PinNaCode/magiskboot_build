#ifndef _MAGISKBOOT_BUILD_WINSUP_MKDIR_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_MKDIR_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

int _mkdir_stub (const char *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_MKDIR_COMPAT */
