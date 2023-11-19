#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winternl.h>

#include "../../include/winsup/stat_compat.h"
#include "../../include/winsup/link_compat.h"

#include "internal/fd.h"
#include "internal/errno.h"

#define EXE_SUFFIX          (('e' << 2) | ('x' << 1) | 'e')
#define BAT_SUFFIX          (('b' << 2) | ('a' << 1) | 't')
#define CMD_SUFFIX          (('c' << 2) | ('m' << 1) | 'd')
#define COM_SUFFIX          (('c' << 2) | ('o' << 1) | 'm')

int _fstat_stub(int fd, struct stat *buf) {
    // ref: https://github.com/reactos/reactos/blob/455f33077599729c27f1f1347ad2f6329d50d1f3/sdk/lib/crt/stdio/stat64.c
    HANDLE h = (HANDLE) _get_osfhandle(fd);

    if (h == INVALID_HANDLE_VALUE)
        return -1;  // errno is already set to EBADF by _get_osfhandle

    BY_HANDLE_FILE_INFORMATION hfi;

    memset(&hfi, 0, sizeof(hfi));

    if (!GetFileInformationByHandle(h, &hfi)) {
        __set_errno_via_winerr(GetLastError());

        return -1;
    }

    memset(buf, 0, sizeof(struct stat));

    char *path = __fd_get_path(fd);

    int ret = -1;

    DWORD type = GetFileType(h);

    if (type == FILE_TYPE_UNKNOWN) {
        DWORD winerr = GetLastError();

        if (winerr == NO_ERROR) {
            // IDK if this is the correct way to error handle
            errno = ENOENT;
            goto error;
        } else {
            // GetFileType failed
            __set_errno_via_winerr(winerr);

            goto error;
        }
    } else if (type == FILE_TYPE_CHAR) {
        buf->st_mode |= S_IFCHR;
        buf->st_dev = buf->st_rdev = fd;
        buf->st_nlink = 1;
    } else if (type == FILE_TYPE_PIPE) {
        buf->st_mode |= S_IFIFO;
        buf->st_dev = buf->st_rdev = fd;
        buf->st_nlink = 1;
    } else {  // type == FILE_TYPE_DISK
        buf->st_mode |= (S_IRUSR | S_IRGRP | S_IROTH);

        if (hfi.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            buf->st_mode |= S_IFLNK;
        else if (hfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            buf->st_mode |= (S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH);
        else {
            buf->st_mode |= S_IFREG;

            if (path) {
                ssize_t path_len = strlen(path);

                if (path_len >= 4 && path[path_len - 4] == '.') {
                    int ext = (tolower(path[path_len - 3]) << 2) | (tolower(path[path_len - 2]) << 1) | tolower(path[path_len - 1]);

                    if (ext == EXE_SUFFIX || ext == BAT_SUFFIX || ext == CMD_SUFFIX || ext == COM_SUFFIX)
                        buf->st_mode |= (S_IXUSR | S_IXGRP | S_IXOTH);
                }
            }
        }

        buf->st_nlink = hfi.nNumberOfLinks;
    }

    if (!(hfi.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
        buf->st_mode |= (S_IWUSR | S_IWGRP | S_IWOTH);

    if (hfi.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
        if (path) {
            char path_buf[PATH_MAX];
            ssize_t len = readlink(path, path_buf, PATH_MAX);

            if (len > 0)
                buf->st_size = len;
        }
    } else
        buf->st_size = ((__int64) hfi.nFileSizeHigh << 32) + hfi.nFileSizeLow;

    DWORD dw = 0;

    RtlTimeToSecondsSince1970((LARGE_INTEGER *) &hfi.ftLastAccessTime, &dw);
    buf->st_atime = dw;

    dw = 0;

    RtlTimeToSecondsSince1970((LARGE_INTEGER *) &hfi.ftLastWriteTime, &dw);
    buf->st_mtime = buf->st_ctime = dw;

    // should not close fd or handle after stat

    ret = 0;

error:
    if (path)
        free(path);

    return ret;
}

int lstat (const char *__restrict path, struct stat *__restrict buf) {
    int fd = __open_symlink_fd(path, GENERIC_READ, FILE_SHARE_READ, 0);

    if (fd < 0) {
        __set_errno_via_winerr(GetLastError());

        return -1;
    }

    int ret = _fstat_stub(fd, buf);

    close(fd);

    return ret;
}
