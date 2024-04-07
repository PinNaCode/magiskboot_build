#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define WIN32_MEAN_AND_LEAN
#include <windows.h>

#include "../include/winsup/acl_compat.h"
#include "../include/winsup/stat_compat.h"

#include "internal/errno.h"

#include "../include/winsup/acl_compat.h"

#ifndef NDEBUG
#  include <stdio.h>
#endif

int fchmod (int fd, mode_t mode) {
    HANDLE h = (HANDLE) _get_osfhandle(fd);

    if (h == INVALID_HANDLE_VALUE)
        return -1;  // errno is already set to EBADF by _get_osfhandle

    BY_HANDLE_FILE_INFORMATION hfi = {};

    if (!GetFileInformationByHandle(h, &hfi)) {
        __set_errno_via_winerr(GetLastError());

#ifndef NDEBUG
        perror("GetFileInformationByHandle");
#endif

        return -1;
    }

    // ref: https://github.com/reactos/reactos/blob/7f346b1aa350942540c7dee9fce3869bea2a72a6/sdk/lib/crt/stdio/file.c#L764

    DWORD new_attr = (mode & (S_IWUSR | S_IWGRP | S_IWOTH)) ? (hfi.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY) : (hfi.dwFileAttributes | FILE_ATTRIBUTE_READONLY);

    if (new_attr == hfi.dwFileAttributes)
        return 0;  // attr unchanged

    FILE_BASIC_INFO info = {};

    info.FileAttributes = new_attr;

    if (!SetFileInformationByHandle(h, FileBasicInfo, &info, sizeof(info))) {
        __set_errno_via_winerr(GetLastError());

#ifndef NDEBUG
        perror("SetFileInformationByHandle");
#endif

        return -1;
    }

    return 0;
}

int     chown (const char *path, uid_t owner, gid_t group) {
    if (access(path, F_OK) != 0)
        return -1;

    return 0;  // FIXME: stub
}

int     fchown (int fildes, uid_t owner, gid_t group) {
    struct stat buf;

    if (fstat(fildes, &buf) < 0)
        return -1;

    return 0;  // FIXME: stub
}

int lchown (const char *path, uid_t owner, gid_t group) {
    struct stat buf;

    if (lstat(path, &buf) < 0)
        return -1;

    return 0;  // FIXME: stub
}
