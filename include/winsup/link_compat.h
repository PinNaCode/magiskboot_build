#ifndef _MAGISKBOOT_BUILD_WINSUP_LINK_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_LINK_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/stat.h>
#include <sys/types.h>

ssize_t readlink (const char *__restrict path,
                          char *__restrict buf, size_t len);

int link(const char *path1, const char *path2);

int     symlink (const char *name1, const char *name2);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_LINK_COMPAT */
