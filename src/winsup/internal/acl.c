#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <aclapi.h>
#include <Lmcons.h>

#ifndef NDEBUG
#include <stdio.h>

#include "assert.h"

#define LOG_TAG                     "fs_internal"
#define LOG_ERR(...)                log_err(LOG_TAG, __VA_ARGS__);
#endif

static char userBuff[UNLEN + 1];

int __ensure_path_access(const char *path, DWORD access) {
    char *pathBuff = strdup(path);

    if (!pathBuff) {
#ifndef NDEBUG
        LOG_ERR("strdup failed");
#endif
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);

        return -1;
    }

    SECURITY_DESCRIPTOR *sd = NULL;

    ACL *oldDacl = NULL;  // inside sd, don't free

    ACL *newDacl = NULL;

    DWORD winerr;

    int ret = -1;

    if ((winerr = GetNamedSecurityInfo(path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &oldDacl, NULL, (void **)&sd))) {
        SetLastError(winerr);

#ifndef NDEBUG
        LOG_ERR("GetNamedSecurityInfo failed: %s", win_strerror(winerr))
#endif

        goto exit;
    }

    EXPLICIT_ACCESS ea = {};

    BuildExplicitAccessWithName(&ea, userBuff, access, GRANT_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);

    DWORD old_access = 0;

    if ((winerr = GetEffectiveRightsFromAcl(oldDacl, &ea.Trustee, &old_access))) {
        SetLastError(winerr);

#ifndef NDEBUG
        LOG_ERR("GetNamedSecuirtyInfo failed: %s", win_strerror(winerr))
#endif

        goto exit;
    }

    if (old_access & access)
        goto success;  // already has the desired access

    if ((winerr = SetEntriesInAcl(1, &ea, oldDacl, &newDacl))) {
        SetLastError(winerr);

#ifndef NDEBUG
        LOG_ERR("SetEntriedInAcl failed: %s", win_strerror(winerr))
#endif

        goto exit;
    }

    if ((winerr = SetNamedSecurityInfo(pathBuff, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, newDacl, NULL))) {
        SetLastError(winerr);

#ifndef NDEBUG
        LOG_ERR("SetNamedSecurityInfo failed: %s", win_strerror(winerr))
#endif

        goto exit;
    }

success:
    ret = 0;

exit:
    if (pathBuff)
        free(pathBuff);

    if (sd)
        LocalFree(sd);  // also frees the memory region contains oldDacl

    if (newDacl)
        LocalFree(newDacl);

    return ret;
}

__attribute__((constructor)) static void __init_user_name(void) {
    DWORD dw = sizeof(userBuff);

    if (!GetUserName(userBuff, &dw)) {
        LOG_ERR("GetUserName failed: %s", win_strerror(GetLastError()))

        assert(0);
    }
}
