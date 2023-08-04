#ifndef _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ASSERT
#define _MAGISKBOOT_BUILD_WINSUP_INTERNAL_ASSERT

void abort_unimpl(const char*);

#define TODO(x)             \
{                           \
    abort_unimpl(#x);       \
}

#endif
