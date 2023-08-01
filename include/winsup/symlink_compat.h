#ifndef _MAGISKBOOT_BUILD_WINSUP_SYMLINK_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_SYMLINK_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/stat.h>

ssize_t readlink (const char *__restrict path,
                          char *__restrict buf, size_t len);

ssize_t readlinkat (int dirfd, const char *__restrict path,
                            char *__restrict buf, size_t len);

int lstat (const char *__restrict path, struct stat *__restrict buf);

int     symlink (const char *name1, const char *name2);

int       symlinkat (const char *, int, const char *);

int     linkat (int dirfd1, const char *path1, int dirfd2, const char *path2, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_SYMLINK_COMPAT */
