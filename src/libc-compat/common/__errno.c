#include <errno.h>

int *__errno (void) {
#ifdef HAVE___errno_location
  return __errno_location();
#elif defined(HAVE___error)
  return __error();
#elif defined(HAVE__errno)
  return _errno();
#else
#  error "Unreachable"
#endif
}
