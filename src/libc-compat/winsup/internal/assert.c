#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "assert.h"

#ifndef NDEBUG
#  define LOG_TAG             "assert_internal"
#endif

static char __win_strerror_buf[64 * 1024 - 1];

const char *win_strerror(DWORD winerr) {
    if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                       winerr, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                       __win_strerror_buf, sizeof(__win_strerror_buf), NULL)) {
#ifndef NDEBUG
        LOG_ERR("FormatMessage for WinError %ld failed (WinError %ld)", winerr, GetLastError());
#endif
        return "Unknown error (win_strerror failed)";
    }

    return __win_strerror_buf;
}
