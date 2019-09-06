#include <errno.h>
#ifndef __SEGGER_LIBC__
#include <sys/stat.h>
#endif

int _lstat(const char *file, struct stat *st) {
#ifdef __SEGGER_LIBC__
  errno = EINVAL;
#else
  errno = ENOSYS;
#endif
  return -1;
}
