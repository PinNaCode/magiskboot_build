#ifndef _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ASSERT
#define _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ASSERT

#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define LOG_ERR(_fmt, ...)      \
    fprintf(stderr, "winsup " LOG_TAG ": " _fmt "\n", ##__VA_ARGS__)

const char* win_strerror(DWORD);

#ifdef WIN32_LEAN_AND_MEAN
#  undef WIN32_LEAN_AND_MEAN
#endif

#endif
