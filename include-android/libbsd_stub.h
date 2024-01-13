#ifndef _MAGISKBOOT_BUILD_LIBBSD_STUB
#define _MAGISKBOOT_BUILD_LIBBSD_STUB

#ifndef __printflike
#  define __printflike(fmt, var) __attribute__((format(printf,fmt,var)))
#endif

#endif /* _MAGISKBOOT_BUILD_LIBBSD_STUB */
