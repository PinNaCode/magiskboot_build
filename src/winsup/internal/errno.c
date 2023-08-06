#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "winerr_map.h"

void __set_errno_via_winerr(DWORD winerr) {
    if (winerr < __winerr_map_size)
        errno = __winerr_map[winerr];
    else
        errno = -1;

    return;
}
