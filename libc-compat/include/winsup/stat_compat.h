#ifndef _MAGISKBOOT_BUILD_WINSUP_STAT_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_STAT_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/stat.h>

#ifdef S_IFBLK
#  undef S_IFBLK
#endif
#define S_IFBLK 0x6000
#define S_IFLNK 0xA000

#define     S_ISLNK(m)      (((m)&S_IFMT) == S_IFLNK)

int lstat (const char *__restrict path, struct stat *__restrict buf);

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_STAT_COMPAT */
