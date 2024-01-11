#ifndef _MAGISKBOOT_BUILD_ANDROID_MACRO
#define _MAGISKBOOT_BUILD_ANDROID_MACRO

#ifndef __call_bypassing_fortify
#define __call_bypassing_fortify(x) x
#endif

#if !defined(__APPLE__) && !defined(__printflike)
#define __printflike(fmt, var) __attribute__((format(printf,fmt,var)))
#endif

#endif /* _MAGISKBOOT_BUILD_ANDROID_MACRO */
