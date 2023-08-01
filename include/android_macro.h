#ifndef _MAGISKBOOT_BUILD_ANDROID_MACRO
#define _MAGISKBOOT_BUILD_ANDROID_MACRO

#define __call_bypassing_fortify(x) x

#ifdef _WIN32
#ifndef __printflike
#define __printflike(fmt, var) __attribute__((format(printf,fmt,var)))
#endif
#endif

#endif /* _MAGISKBOOT_BUILD_ANDROID_MACRO */
