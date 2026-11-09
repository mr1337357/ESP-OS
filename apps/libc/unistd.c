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

int dup2(int oldfd, int newfd)
{
   struct {int oldfd; int newfd;} dup2call = {.oldfd = oldfd, .newfd = newfd};
   return do_syscall(SYSCALL_DUP2, &dup2call);
}