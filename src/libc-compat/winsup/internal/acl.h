#ifndef _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ACL
#define _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ACL

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

int __ensure_path_access(const char *path, DWORD access);

#ifdef WIN32_LEAN_AND_MEAN
#  undef WIN32_LEAN_AND_MEAN
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ACL */
