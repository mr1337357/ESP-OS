#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include "lib.h"

#define SYSCALL_READ 0
#define SYSCALL_WRITE 1
#define SYSCALL_OPEN 2
#define SYSCALL_CLOSE 3
#define SYSCALL_CLONE 4
#define SYSCALL_EXEC 5

typedef struct
{
  int fd;
  int len;
  uint8_t *buffer;
} fileop;

struct exec_args
{
    int (*syscall)(int, void*);
    int argc;
    char **argv;
};

char **__environ;

int (*do_syscall)(int callnum, void *arg);

void main(int argc, char **argv);

void _start(struct exec_args *startargs)//void *syscall_handle, int argc, char **argv)
{
   __environ = 0;
   do_syscall = startargs->syscall;
   //struct { int callnum; void (*handler)(int, void *);} eventcall = { 0, event_handler};
   //do_syscall = syscall_handle;
   //do_syscall(0, &eventcall);
   main(startargs->argc,startargs->argv);
}

int write(int fd, const void *buff, size_t len)
{
   
   fileop fop = { .fd = fd, .buffer = (void *)buff, .len = len}; //drop the const (the kernel won't write here so it should be ok)
   return do_syscall(SYSCALL_WRITE,&fop);
}

int read(int fd, void *buff, size_t len)
{
   return -1;
}

int open(char *name, int mode)
{
   return -1;
}

int close(int fd)
{
   return -1;
}

size_t strlen(const char *msg)
{
   int i;
   for(i=0;*msg++;i++)
   {
   }
   return i;
}

void print(char *message)
{
   write(1,message,strlen(message));
}


int execve(const char *path, char *const argv[], char *const envp[])
{
   struct {const char *fname;  char * const *argv; char * const *env;} execcall = {.fname = path, .argv = argv, .env = envp};
   return do_syscall(SYSCALL_EXEC, &execcall);
}

int execv(const char *path, char *const argv[])
{
   return execve(path, argv, __environ);
}

__attribute((weak)) void event_handler(int event, void *arg)
{
   (void) event;
   (void) arg;
}
