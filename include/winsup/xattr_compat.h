#ifndef _MAGISKBOOT_BUILD_WINSUP_XATTR_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_XATTR_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

ssize_t getxattr (const char *, const char *, void *, size_t);

ssize_t lgetxattr (const char *, const char *, void *, size_t);

ssize_t fgetxattr (int , const char *, void *, size_t);

int setxattr (const char *, const char *, const void *, size_t , int);

int lsetxattr (const char *, const char *, const void *, size_t , int);

int fsetxattr (int , const char *, const void *, size_t , int);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_XATTR_COMPAT */
