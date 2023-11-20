#include <assert.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winternl.h>

#include "assert.h"

// According to: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationfile
#ifndef FileCaseSensitiveInformation
#define FileCaseSensitiveInformation    (71)
#endif

#define fs_min(a, b)  (((a) < (b)) ? (a) : (b))

#define LOG_TAG             "fs_internal"
#define LOG_ERR(...)        log_err(LOG_TAG, __VA_ARGS__);

static bool enforce_case;

void __ensure_case_sensitive(const char *path, bool file) {
    if (!path)
        return;  // invalid call

    if (file) {
        // find out parent directory and re-run

        char *tmp = strdup(path);

        assert(tmp);

        const char *parent = dirname(tmp);

        __ensure_case_sensitive(parent, false);

        free(tmp);

        return;
    }

    struct stat buf;

    if (stat(path, &buf) < 0 || !S_ISDIR(buf.st_mode) || access(path, W_OK) != 0)
        return;  // path is not a valid directory or inaccessible

    WCHAR path_buf[PATH_MAX];
    size_t chars;
    size_t path_len = strlen(path);

    int err = mbstowcs_s(&chars, path_buf, PATH_MAX, path, fs_min(PATH_MAX - 1, path_len));

    HANDLE h = NULL;
    bool success = false;

    if (err) {
#ifndef NDEBUG
        perror("mbstowcs_s");
#endif

        goto quit;
    }

    UNICODE_STRING nt_path = {};

    if (RtlDosPathNameToNtPathName_U(path_buf, &nt_path, NULL, NULL) == FALSE) {
#ifndef NDEBUG
        LOG_ERR("RtlDosPathNameToNtPathName_U failed")
#endif

        goto quit;
    }

    OBJECT_ATTRIBUTES object;

    InitializeObjectAttributes(&object, &nt_path, 0, NULL, NULL);

    NTSTATUS status;
    IO_STATUS_BLOCK io;

    status = NtOpenFile(&h, GENERIC_READ, &object, &io, FILE_SHARE_VALID_FLAGS, FILE_DIRECTORY_FILE);

    if (!NT_SUCCESS(status)) {
ntopenfile_failed:
#ifndef NDEBUG
        LOG_ERR("NtOpenFile failed: %s", nt_strstatus(status))
#endif

        goto quit;
    }

    FILE_CASE_SENSITIVE_INFO fcsi;

    status = NtQueryInformationFile(h, &io, &fcsi, sizeof(fcsi), FileCaseSensitiveInformation);

    if (!NT_SUCCESS(status)) {
#ifndef NDEBUG
        LOG_ERR("NtQueryInformationFile failed: %s", nt_strstatus(status))
#endif

        goto quit;
    }

    if (fcsi.Flags & FILE_CS_FLAG_CASE_SENSITIVE_DIR)
        goto done;  // already case sensitive, skip

    // reopen with write permission
    NtClose(h);
    h = NULL;
    status = NtOpenFile(&h, GENERIC_WRITE, &object, &io, FILE_SHARE_VALID_FLAGS, FILE_DIRECTORY_FILE);

    if (!NT_SUCCESS(status))
        goto ntopenfile_failed;

    fcsi.Flags = FILE_CS_FLAG_CASE_SENSITIVE_DIR;

    status = NtSetInformationFile(h, &io, &fcsi, sizeof(fcsi), FileCaseSensitiveInformation);

    if (!NT_SUCCESS(status)) {
#ifndef NDEBUG
        LOG_ERR("NtSetInformationFile failed: %s", nt_strstatus(status))
#endif

        goto quit;
    }

done:
    success = true;

quit:
    if (h)
        NtClose(h);

    if (!success && enforce_case) {
        LOG_ERR("An error occurred while ensuring case sensitivity of the directory '%s'\n"
                    "Now the program will stop in order to avoid any potential incorrect file operations.\n"
                    "Please run 'fsutil.exe file setCaseSensitiveInfo <path> enable' manually for this directory and try again.\n"
                    "To disable this check, set the environment variable MAGISKBOOT_WINSUP_NOCASE to 1 before running this program.", path)

        abort();
    }

    return;
}

__attribute__((constructor)) static void __init_enforce_mode(void) {
    enforce_case = !getenv("MAGISKBOOT_WINSUP_NOCASE");

    // FIXME: some of the Rust stuffs don't go through our open() stubs (but mkdir() stub is used)
    //        so need to ensure the CWD is case sensitive on startup
    //        but not if the user makes magiskboot to create files under another directory
    //        a possible workaround is probably to hook CreateFile() and add the checks
    __ensure_case_sensitive(".", false);
}
