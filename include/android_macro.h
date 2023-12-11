#ifndef _MAGISKBOOT_BUILD_ANDROID_MACRO
#define _MAGISKBOOT_BUILD_ANDROID_MACRO

#define __call_bypassing_fortify(x) x

#if !defined(__ANDROID__) && !defined(__APPLE__)
#ifndef __printflike
#define __printflike(fmt, var) __attribute__((format(printf,fmt,var)))
#endif
#endif

#endif /* _MAGISKBOOT_BUILD_ANDROID_MACRO */
