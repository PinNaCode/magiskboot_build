#ifndef _MAGISKBOOT_BUILD_WINSUP_INTERNAL_FD
#define _MAGISKBOOT_BUILD_WINSUP_INTERNAL_FD

#include <stdbool.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

char *__fd_get_path(int fd);

char *__get_path_at(int dirfd, const char *name);

int __open_dir_fd(const char *path, DWORD access, DWORD share_mode, int flags);

bool __is_dirfd(int fd);

int __open_symlink_fd(const char *path, DWORD access, DWORD share_mode, int flags);

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_INTERNAL_FD */
