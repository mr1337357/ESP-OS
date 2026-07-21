#include <stdint.h>
#include <string.h>

#include "lib.h"

#define FOP_READ 0
#define FOP_WRITE 1
#define FOP_OPEN 2
#define FOP_CLOSE 3

typedef struct
{
  int op;
  int fd;
  int len;
  uint8_t *buffer;
} fileop;

int (*do_syscall)(int callnum, void *arg);

void main(int argc, char **argv);

void _start(void *syscall_handle)
{
   struct { int callnum; void (*handler)(int, void *);} eventcall = { 0, event_handler};
   do_syscall = syscall_handle;
   do_syscall(0, &eventcall);
   main(0,0);
}

int write(int fd, char *buff, int len)
{
   fileop fop;
   fop.op = FOP_WRITE;
   fop.fd = fd;
   fop.buffer = buff;
   fop.len = len;
   return do_syscall(1,&fop);
}

int read(int fd, char *buff, int len)
{
   fileop fop;
   fop.op = FOP_READ;
   fop.fd = fd;
   fop.buffer = buff;
   fop.len = len;
   return do_syscall(1,&fop);
}

int open(char *name, int mode)
{
   fileop fop;
   fop.op = FOP_OPEN;
   fop.fd = mode;
   fop.buffer = name;
   fop.len = 0;
   return do_syscall(1,&fop);
}

int close(int fd)
{
   fileop fop;
   fop.op = FOP_CLOSE;
   fop.fd = fd;
   return do_syscall(1,&fop);
}

void print(char *message)
{
   write(1,message,strlen(message));
}

void delay(int ms)
{
   struct { int callnum; int ms; } delaycall = { 1, ms };
   do_syscall(0, &delaycall);
}

void *malloc(int len)
{
   struct { int callnum; int size; } malloccall = { 2, len};
   return (void *)do_syscall(0, &malloccall);
}

void run(char *fname)
{
   struct { int callnum; char *fname; } runcall = { 3, fname};
   do_syscall(0, &runcall);
}

__attribute((weak)) void event_handler(int event, void *arg)
{
   (void) event;
   (void) arg;
}
