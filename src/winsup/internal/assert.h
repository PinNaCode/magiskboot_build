#ifndef _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ASSERT
#define _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ASSERT

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

void abort_unimpl(const char*);

void log_err(const char*, const char*, ...);

const char* win_strerror(DWORD);

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

#define TODO(x)             \
{                           \
    abort_unimpl(#x);       \
}

#endif
