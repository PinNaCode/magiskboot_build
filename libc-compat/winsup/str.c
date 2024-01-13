#include <string.h>

#include "../include/winsup/str_compat.h"

// by deltamind106 from:
//    https://stackoverflow.com/questions/52988769/writing-own-memmem-for-windows
void *memmem(const void *haystack, int haystacklen, const void *needle, int needlelen) {
    unsigned char *csrc = (unsigned char *)haystack;
    unsigned char *ctrg = (unsigned char *)needle;
    unsigned char *tptr, *cptr;
    int searchlen;
    int ndx = 0;

    if (!haystack || !haystacklen || !needle || !needlelen)
        return NULL;

    while (ndx <= haystacklen) {
        cptr = &csrc[ndx];
        if ((searchlen = haystacklen - ndx - needlelen + 1) <= 0) {
            return NULL;
        }
        if ((tptr = memchr(cptr, *ctrg, searchlen)) == NULL) {
            return NULL;
        }
        if (memcmp(tptr, ctrg, needlelen) == 0) {
            return tptr;
        }
        ndx += tptr - cptr + 1;
    }

    return NULL;
}
