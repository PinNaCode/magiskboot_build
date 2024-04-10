#ifndef _MAGISKBOOT_BUILD_WINSUP_DIRENT_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_DIRENT_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>

#define DT_UNKNOWN 0
#define DT_REG 1
#define DT_LNK 2
#define DT_DIR 3
#define DT_FIFO 4
#define DT_SOCK 5
#define DT_BLK 6
#define DT_CHR 7

typedef struct _DIR_wrap _DIR_stub;

struct _dirent_stub
{
        long            d_ino;          /* Always zero. */
        unsigned char   d_type;         /* file type. this may not be supported
                                             by all the file systems */
        unsigned short  d_reclen;       /* Always zero. */
        unsigned short  d_namlen;       /* Length of name in d_name. */
        char            d_name[260]; /* [FILENAME_MAX] */ /* File name. */
};

struct _DIR_wrap {
    DIR *dirp;
    int fd;
};

_DIR_stub *_opendir_stub (const char*);

_DIR_stub *fdopendir(int fd);

struct _dirent_stub* _readdir_stub (_DIR_stub*);

int _closedir_stub (_DIR_stub*);

void _rewinddir_stub (_DIR_stub*);

long _telldir_stub (_DIR_stub*);

void _seekdir_stub (_DIR_stub*, long);

int dirfd (_DIR_stub*);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_DIRENT_COMPAT */
