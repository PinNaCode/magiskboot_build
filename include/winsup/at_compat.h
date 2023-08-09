#ifndef _MAGISKBOOT_BUILD_WINSUP_AT_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_AT_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int unlinkat (int dirfd, const char *pathname, int flags);

int openat (int dirfd, const char *pathname, int flags, mode_t mode);

int faccessat (int dirfd, const char *path, int mode, int flags);

int fstatat (int dirfd, const char *__restrict pathname, struct stat *__restrict buf, int flags);

int mkdirat (int dirfd, const char *pathname, mode_t mode);

int    renameat (int, const char *, int, const char *);

#define AT_REMOVEDIR 1
#define AT_SYMLINK_NOFOLLOW 2

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_AT_COMPAT */
