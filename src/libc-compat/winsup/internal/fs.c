#include <assert.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "acl.h"
#include "assert.h"
#include "fd.h"

#include "fs.h"

// Origin: https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ne-minwinbase-file_info_by_handle_class
// XXX: mingw may set this to 0x10 wrongly in some condition, so let's provide our own version of this enum for now
#define __FileCaseSensitiveInfo     (0x17)

_Static_assert(__FileCaseSensitiveInfo == 0x17, "FileCaseSensitiveInfo is set to a wrong value");

#define LOG_TAG                     "fs_internal"

static bool enforce_case;

void __ensure_case_sensitive(const char *path, bool file) {
    if (!path)
        return;  // invalid call

    if (file) {
        // find out parent directory and re-run

        char *tmp = strdup(path);

        if (!tmp) {
#ifndef NDEBUG
            perror("strdup");
#endif
            assert(0);
        }

        const char *parent = dirname(tmp);

        __ensure_case_sensitive(parent, false);

        free(tmp);

        return;
    }

    struct stat buf;

    if (stat(path, &buf) < 0 || !S_ISDIR(buf.st_mode) || access(path, W_OK) != 0)
        return;  // path is not a valid directory or inaccessible

    HANDLE h = INVALID_HANDLE_VALUE;
    bool success = false;

    int fd;

    if ((fd = __open_dir_fd(path, GENERIC_READ, FILE_SHARE_VALID_FLAGS, 0)) < 0) {
open_dir_fd_failed:
#ifndef NDEBUG
        LOG_ERR("__open_dir_fd failed: %s", win_strerror(GetLastError()));
#endif

        goto quit;
    }

    if ((h = (HANDLE) _get_osfhandle(fd)) == INVALID_HANDLE_VALUE) {
get_handle_failed:
#ifndef NDEBUG
        LOG_ERR("_get_osfhandle failed");
#endif

        goto quit;
    }

    fd = -1;  // ownership transferred

    FILE_CASE_SENSITIVE_INFO fcsi;
    DWORD winerr;

    if (!GetFileInformationByHandleEx(h, __FileCaseSensitiveInfo, &fcsi, sizeof(fcsi))) {
        winerr = GetLastError();

#ifndef NDEBUG
        LOG_ERR("GetFileInformationByHandleEx failed: %s", win_strerror(winerr));
#endif

diag_and_quit:
#ifdef NDEBUG
        if (!enforce_case)
            goto quit;  // dont show diagnosis info if user requested to disable check
#endif

        switch (winerr) {
            case ERROR_INVALID_PARAMETER:
                LOG_ERR("Detected: You may be on an OS version that doesn't support case sensitivity settings.");
                break;
            case ERROR_NOT_SUPPORTED:
                LOG_ERR("Detected: Windows Subsystem for Linux may not be enabled on this machine.");
                break;
            case ERROR_ACCESS_DENIED:
                LOG_ERR("Detected: You may not have enough rights to have this directory case sensitive with current user.");
                break;
            case ERROR_DIR_NOT_EMPTY:
                LOG_ERR("Detected: The directory is not empty, due to Windows limitation, please place magiskboot binary and image files outside of your working directory.\n"
                            "Or, use fsutil to set the case sensitivity before filling this directory with files.");
                break;
            default:
                // unknown error !?
                LOG_ERR("Note: Unable to determine reason, try using fsutil to set the case sensitivity manually and see.");
                break;
        }

        goto quit;
    }

    if (fcsi.Flags & FILE_CS_FLAG_CASE_SENSITIVE_DIR)
        goto done;  // already case sensitive, skip

    // reopen with write permission
    CloseHandle(h);
    h = NULL;

    if ((fd = __open_dir_fd(path, GENERIC_WRITE, FILE_SHARE_VALID_FLAGS, 0)) < 0)
        goto open_dir_fd_failed;

    if ((h = (HANDLE) _get_osfhandle(fd)) == INVALID_HANDLE_VALUE)
        goto get_handle_failed;

    fd = -1;

    __ensure_path_access(path, FILE_DELETE_CHILD);  // undocumented but required for non-administrator?

    fcsi.Flags |= FILE_CS_FLAG_CASE_SENSITIVE_DIR;

    if (!SetFileInformationByHandle(h, __FileCaseSensitiveInfo, &fcsi, sizeof(fcsi))) {
        winerr = GetLastError();

#ifndef NDEBUG
        LOG_ERR("SetFileInformationByHandle failed: %s", win_strerror(winerr));
#endif

        goto diag_and_quit;
    }

done:
    success = true;

quit:
    if (!(fd < 0))
        close(fd);

    if (h != INVALID_HANDLE_VALUE)
        CloseHandle(h);

    if (!success && enforce_case) {
        LOG_ERR("Friendly error: Unable to ensure case sensitivity of the directory '%s'\n"
                    "Now the program will stop in order to avoid any potential incorrect file operations.\n"
                    "Please run 'fsutil.exe file setCaseSensitiveInfo <path> enable' manually for this directory and try again.\n"
                    "You may want to make sure Windows Subsystem for Linux is enabled on this machine.\n"
                    "On OS version older than Windows 10 1803, reactOS and wine, this feature may not be available.\n"
                    "To disable this check, set the environment variable MAGISKBOOT_WINSUP_NOCASE to 1 before running this program.\n"
                    "Make sure you understand what are you doing before disabling the check.", path);

        abort();
    }

    return;
}

__attribute__((constructor)) static void __init_enforce_mode(void) {
    enforce_case = !getenv("MAGISKBOOT_WINSUP_NOCASE");
}
