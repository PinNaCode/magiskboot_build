#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <pthread.h>

#ifndef NDEBUG
#define LOG_TAG             "assert_internal"
#define LOG_ERR(...)        log_err(LOG_TAG, __VA_ARGS__);
#endif

void log_err(const char *tag, const char *fmt, ...) {
    va_list ap;

    fprintf(stderr, "winsup %s: ", tag);

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");

    fflush(stderr);
}

void abort_unimpl(const char *s) {
    log_err("ABORT", "%s is not implemented", s);
    abort();
}

static char __win_strerror_buf[64 * 1024 - 1];
static pthread_mutex_t __win_strerror_lck;
static const char *__unknown_strerror = "Unknown error";

__attribute__((constructor)) static void __init_win_strerr_lck(void) {
    pthread_mutex_init(&__win_strerror_lck, NULL);
}

const char *win_strerror(DWORD winerr) {
    pthread_mutex_lock(&__win_strerror_lck);
    if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                       winerr, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                       __win_strerror_buf, sizeof(__win_strerror_buf), NULL)) {
#ifndef NDEBUG
        LOG_ERR("FormatMessage failed (WinError %ld)", GetLastError());
#endif
        return __unknown_strerror;
    }
    pthread_mutex_unlock(&__win_strerror_lck);

    return __win_strerror_buf;
}
