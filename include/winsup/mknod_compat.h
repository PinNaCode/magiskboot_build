#ifndef _MAGISKBOOT_BUILD_WINSUP_MKNOD_COMPAT
#define _MAGISKBOOT_BUILD_WINSUP_MKNOD_COMPAT

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

int mknod (const char *path, mode_t mode, dev_t dev );

inline dev_t
__gnu_dev_makedev(int maj, int min)
{
        return (((maj) << 16) | ((min) & 0xffff));
}

#define makedev(maj, min) __gnu_dev_makedev(maj, min)

inline int
__gnu_dev_major(dev_t dev)
{
        return (int)(((dev) >> 16) & 0xffff);
}

inline int
__gnu_dev_minor(dev_t dev)
{
        return (int)((dev) & 0xffff);
}

#define major(dev) __gnu_dev_major(dev)
#define minor(dev) __gnu_dev_minor(dev)

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_WINSUP_MKNOD_COMPAT */

