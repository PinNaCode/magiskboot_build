#ifndef _MAGISKBOOT_BUILD_WINSUP_INTERNAL_FD
#define _MAGISKBOOT_BUILD_WINSUP_INTERNAL_FD

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

char *__fd_get_path(int fd);

int __open_dir_fd(const char *path, DWORD access, DWORD share_mode, int flags);

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_INTERNAL_FD */
