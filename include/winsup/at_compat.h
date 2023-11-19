#ifndef _MAGISKBOOT_BUILD_WINSUP_AT_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_AT_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define AT_REMOVEDIR 1
#define AT_SYMLINK_NOFOLLOW 2

ssize_t readlinkat (int dirfd, const char *__restrict path,
                            char *__restrict buf, size_t len);

int       symlinkat (const char *, int, const char *);

int     linkat (int dirfd1, const char *path1, int dirfd2, const char *path2, int flags);

int unlinkat (int dirfd, const char *pathname, int flags);

int openat (int dirfd, const char *pathname, int flags, ...);

int faccessat (int dirfd, const char *path, int mode, int flags);

int fstatat (int dirfd, const char *__restrict pathname, struct stat *__restrict buf, int flags);

int mkdirat (int dirfd, const char *pathname, mode_t mode);

int    renameat (int, const char *, int, const char *);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_AT_COMPAT */
