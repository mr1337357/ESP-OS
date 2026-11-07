#include <fcntl.h>
#include "lib.h"

typedef struct
{
  int fd;
  int len;
  uint8_t *buffer;
} fileop;

int write(int fd, const void *buff, size_t len)
{
   
   fileop fop = { .fd = fd, .buffer = (void *)buff, .len = len}; //drop the const (the kernel won't write here so it should be ok)
   return do_syscall(SYSCALL_WRITE,&fop);
}

int read(int fd, void *buff, size_t len)
{
   fileop fop = { .fd = fd, .buffer = (void *)buff, .len = len}; //drop the const (the kernel won't write here so it should be ok)
   return do_syscall(SYSCALL_READ,&fop);
}

int open(const char *path, int flags, ...)
{
   fileop fop = { .fd = flags, .buffer = (void *)path, .len = 0}; //drop the const (the kernel won't write here so it should be ok)
   return do_syscall(SYSCALL_OPEN,&fop);
}

int close(int fd)
{
   fileop fop = { .fd = fd, .buffer = 0, .len = 0}; //drop the const (the kernel won't write here so it should be ok)
   return do_syscall(SYSCALL_CLOSE,&fop);
}
