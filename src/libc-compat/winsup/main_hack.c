// HACK:
//   native paths like \\?\C:\windows doesn't get passed correctly
//   to main() when compiled with MSYS2's compiler and MinGW-runtime.
//
//   so until the root cause is discovered, this hack will be used.

#include <assert.h>
#include <stdlib.h>
#include <wchar.h>

#include <windows.h>

#include "internal/assert.h"

#define LOG_TAG             "main_hack"

// defined in:
//   src/Magisk/native/src/boot/main.cpp

int __real_main(int argc, char **argv);

int __wrap_main(int argc, char **argv) {
    int argc_new, i, len, res;
    wchar_t **argv_w;
    char **argv_new;

    // get wide argv

    argv_w = CommandLineToArgvW(GetCommandLineW(), &argc_new);
    if (!argv_w) {
        LOG_ERR("CommandLineToArgvW failed: %s", win_strerror(GetLastError()));

        abort();
    }

    // convert it to system code page

    argv_new = calloc(argc_new + 1, sizeof(char *));
    assert(argv_new);

    for (i = 0; i < argc_new; i++) {
        len = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                                  argv_w[i], -1, NULL, 0, NULL, NULL);
        if (!len) {
wcstombs_failed:
            LOG_ERR("WideCharToMultiByte failed: %s", win_strerror(GetLastError()));

            abort();
        }

        argv_new[i] = malloc(len);
        assert(argv_new[i]);

        if (!WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, argv_w[i],
                                 -1, argv_new[i], len, NULL, NULL))
            goto wcstombs_failed;
    }

    // the last arg is always a NULL entry,
    // which indicates the end of our argv
    argv_new[argc_new] = NULL;

    LocalFree(argv_w);

    res = __real_main(argc_new, argv_new);

    for (i = 0; i < argc_new; i++)
        free(argv_new[i]);

    free(argv_new);

    return res;
}
