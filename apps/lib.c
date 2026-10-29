#include <stdint.h>
//#include <string.h>
#include <stddef.h>
//#include <unistd.h>

#include "lib.h"

#define SYSCALL_READ 0
#define SYSCALL_WRITE 1
#define SYSCALL_OPEN 2
#define SYSCALL_CLOSE 3
#define SYSCALL_CLONE 4
#define SYSCALL_EXEC 5
#define SYSCALL_WAITPID 6
#define SYSCALL_CHDIR 7
#define SYSCALL_GETCWD 8
#define SYSCALL_MALLOC 9
#define SYSCALL_FREE 10


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
    char **envp;
};

char **__environ;

int (*do_syscall)(int callnum, void *arg);

void main(int argc, char **argv);

void _start(struct exec_args *startargs)//void *syscall_handle, int argc, char **argv)
{
   do_syscall = startargs->syscall;
   //struct { int callnum; void (*handler)(int, void *);} eventcall = { 0, event_handler};
   //do_syscall = syscall_handle;
   //do_syscall(0, &eventcall);
   __environ = startargs->envp;
   main(startargs->argc,startargs->argv);
}

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

int open(char *name, int mode)
{
   fileop fop = { .fd = mode, .buffer = (void *)name, .len = 0}; //drop the const (the kernel won't write here so it should be ok)
   return do_syscall(SYSCALL_OPEN,&fop);
}

int close(int fd)
{
   fileop fop = { .fd = fd, .buffer = 0, .len = 0}; //drop the const (the kernel won't write here so it should be ok)
   return do_syscall(SYSCALL_CLOSE,&fop);
}

size_t strlen(const char *msg)
{
   int i;
   for(i=0;*msg++;i++)
   {
   }
   return i;
}

int strncmp(char *a, char *b, int n)
{
   int i;
   for(i=0;i<n;i++)
   {
      if(a[i] == 0 || a[i] != b[i])
      {
         return a[i] - b[i];
      }
   }
   return 0;
}

int strcmp(char *a, char *b)
{
   return strncmp(a,b,strlen(a)+1);
   /*
   while(1)
   {
      if(!*a)
      {
         if(!*b)
         {
            return 0;
         }
         return -1;
      }
      if(*a != *b)
      {
         return *a - *b;
      }
      a++;b++;
   }
   return -2;
   */
}

void print(char *message)
{
   write(1,message,strlen(message));
}

int thread_create(void *entry, void *threadargs)
{
   struct {void *entry; void *args;} threadcall = {.entry = entry, .args = threadargs};
   return do_syscall(SYSCALL_CLONE, &threadcall);
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

int waitpid(int pid, int *status, int options)
{
   struct {int pid;  int *status; int options;} waitpidcall = {.pid = pid, .status = status, .options = options};
   return do_syscall(SYSCALL_WAITPID, &waitpidcall);
}

int chdir(char *path)
{
   return do_syscall(SYSCALL_CHDIR, path);
}

int getcwd(char *path, int len)
{
   struct {char *path; int len;} getcwdcall = {.path = path, .len = len};
   return do_syscall(SYSCALL_GETCWD, &getcwdcall);
}

void *malloc(int size)
{
   void *retp;
   struct {void **retp; int size;} malloccall = {.retp = &retp, .size = size};
   do_syscall(SYSCALL_MALLOC, &malloccall);
   return retp;
}

void free(void *ptr)
{
   do_syscall(SYSCALL_FREE,ptr);
}

char *getenv(char *key)
{
   char **tempenv;
   char *varp;
   if(!__environ)
   {
      return 0;
   }
   tempenv = __environ;
   for(varp = *tempenv++; varp; varp = *tempenv++)
   {
      if(!strcmp())
   }
}

__attribute((weak)) void event_handler(int event, void *arg)
{
   (void) event;
   (void) arg;
}
