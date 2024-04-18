#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <emscripten.h>
#include <emscripten/bind.h>

/* 20 ms */
#define  MBB_CONIO_DELAY            (20)

/* 10 characters */
#define  MBB_CONOUT_THRESHOLD       (10)

extern int __mbb_main(int argc, char **argv);

// defined in src/Magisk/native/src/boot/main.cpp
// renamed by our -Dmain=__mbb_main in CMakeLists.stubs.txt
static int (*main_ptr)(int argc, char **argv) = __mbb_main;

__attribute__((noreturn)) extern "C" void __real_exit(int status);

__attribute__((noreturn)) static void (*exit_ptr)(int status) = __real_exit;

static int (*vprintf_ptr)(const char *format, va_list ap) = vprintf;

static int (*vfprintf_ptr)(FILE *stream, const char *format, va_list ap) = vfprintf;

extern "C" {
    __attribute__((noreturn)) void __wrap_exit(int status) {
        exit_ptr(status);
    }

    int __wrap_printf(const char *format, ...) {
        va_list ap;
        int res;

        va_start(ap, format);
        res = vprintf_ptr(format, ap);
        va_end(ap);

        return res;
    }

    int __wrap_iprintf(const char *format, ...) {
        va_list ap;
        int res;

        va_start(ap, format);
        res = vprintf_ptr(format, ap);
        va_end(ap);

        return res;
    }

    int __wrap_fprintf(FILE *stream, const char *format, ...) {
        va_list ap;
        int res;

        va_start(ap, format);
        res = vfprintf_ptr(stream, format, ap);
        va_end(ap);

        return res;
    }

    int __wrap_fiprintf(FILE *stream, const char *format, ...) {
        va_list ap;
        int res;

        va_start(ap, format);
        res = vfprintf_ptr(stream, format, ap);
        va_end(ap);

        return res;
    }
}

static void __exit_hook(int status) {
    EM_ASM({
        Module['mbb_main_cb']($0);
    }, status);
}

static int __main_wrap(int argc, char **argv) {
    int res;

    res = __mbb_main(argc, argv);

    // asyncify breaks retval, so we do this to inform js side
    __exit_hook(res);

    return 0;
}

__attribute__((noreturn)) static void __exit_wrap(int status) {
    __exit_hook(status);
    __real_exit(status);
}

static void __xprintf_hook(const char *s) {
    // when should we update UI?

    if (strlen(s) >= MBB_CONOUT_THRESHOLD)
        goto do_sleep;  // when the line is long enough

    if (strchr(s, '\n') || strchr(s, '\r'))
        goto do_sleep;  // when there is a newline char in the str

    return;

do_sleep:
    // sleeping is even more expensive to perf
    // so only do this when needed
    emscripten_sleep(MBB_CONIO_DELAY);  // note this needs -sASYNCIFY in LDFLAGS
}

static int __vprintf_wrap(const char *format, va_list ap) {
    int res;

    res = vprintf(format, ap);

    // give browser a chance to update UI
    __xprintf_hook(format);

    return res;
}

static int __vfprintf_wrap(FILE *stream, const char *format, va_list ap) {
    int res;

    res = vfprintf(stream, format, ap);

    switch (fileno(stream)) {
        case STDOUT_FILENO:
        case STDERR_FILENO:
            __xprintf_hook(format);  // same as above
            break;
        default:
            break;
    }

    return res;
}

int main(int argc, char **argv) {
    return main_ptr(argc, argv);
}

static void __enable_conio_hack(void) {
    main_ptr = __main_wrap;

    exit_ptr = __exit_wrap;

    vprintf_ptr = __vprintf_wrap;
    vfprintf_ptr = __vfprintf_wrap;
}

EMSCRIPTEN_BINDINGS(conio_hack) {
    emscripten::function("mbb_enable_conio_hack", &__enable_conio_hack);
}

__attribute__((constructor)) static void __init_io_mode(void) {
    // disable line buffering
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
}
