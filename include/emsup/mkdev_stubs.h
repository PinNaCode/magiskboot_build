#ifndef _MAGISKBOOT_BUILD_EMSUP_MKDEV_STUBS
#define _MAGISKBOOT_BUILD_EMSUP_MKDEV_STUBS

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#define major(dev) __gnu_dev_major(dev)
#define minor(dev) __gnu_dev_minor(dev)
#define makedev(maj, min) __gnu_dev_makedev(maj, min)

static inline dev_t
__gnu_dev_makedev(int maj, int min)
{
        return (((maj) << 16) | ((min) & 0xffff));
}

static inline int
__gnu_dev_major(dev_t dev)
{
        return (int)(((dev) >> 16) & 0xffff);
}

static inline int
__gnu_dev_minor(dev_t dev)
{
        return (int)((dev) & 0xffff);
}

#ifdef __cplusplus
}
#endif

#endif /* _MAGISKBOOT_BUILD_EMSUP_MKDEV_STUBS */
