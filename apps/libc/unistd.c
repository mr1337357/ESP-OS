#include <unistd.h>

#include "lib.h"

int chdir(const char *path)
{
   return do_syscall(SYSCALL_CHDIR, (void *)path);
}

char *getcwd(char *path, size_t len)
{
   struct {char *path; int len;} getcwdcall = {.path = path, .len = len};
   do_syscall(SYSCALL_GETCWD, &getcwdcall);
   return path;
}