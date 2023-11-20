#ifndef _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ERRNO
#define _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ERRNO

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif /* WIN32_LEAN_AND_MEAN */
#include <Windows.h>
#include <winternl.h>

void __set_errno_via_winerr(DWORD winerr);

void __set_errno_via_ntstatus(NTSTATUS status);

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif /* WIN32_LEAN_AND_MEAN */

#endif /* _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ERRNO */
