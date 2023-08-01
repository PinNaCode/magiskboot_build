#ifndef _MAGISKBOOT_BUILD_WINSUP_DIRENT_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_DIRENT_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>

struct _dirent_stub
{
        long            d_ino;          /* Always zero. */
        unsigned char   d_type;         /* file type. this may not be supported
                                             by all the file systems */
        unsigned short  d_reclen;       /* Always zero. */
        unsigned short  d_namlen;       /* Length of name in d_name. */
        char            d_name[260]; /* [FILENAME_MAX] */ /* File name. */
};

#define DT_REG 1
#define DT_LNK 2
#define DT_DIR 3

struct _dirent_stub* _readdir_stub (DIR*);

int dirfd (DIR*);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_DIRENT_COMPAT */
