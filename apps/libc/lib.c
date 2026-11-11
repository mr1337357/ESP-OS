#include <string.h>
#include <stddef.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

#include "lib.h"

struct _reent _re;

struct _reent *__getreent()
{
   return &_re;
}

struct exec_args
{
    int (*syscall)(int, void*);
    int argc;
    char **argv;
    char **envp;
};


int (*do_syscall)(int callnum, void *arg);

void main(int argc, char **argv);

void stdio_init();

void _start(struct exec_args *startargs)//void *syscall_handle, int argc, char **argv)
{
   do_syscall = startargs->syscall;
   //struct { int callnum; void (*handler)(int, void *);} eventcall = { 0, event_handler};
   //do_syscall = syscall_handle;
   //do_syscall(0, &eventcall);
   __environ = startargs->envp;
   stdio_init();
   main(startargs->argc,startargs->argv);
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

int execvpe(const char *file, char *const argv[], char *const envp[])
{
   char pathbuff[256];
   char *path = getenv("PATH");
   if(strchr(file,'/'))
   {
      return execvpe(file,argv,envp);
   }
   if(!path)
   {
      return -1;
   }
   strcpy(pathbuff,path);
   strcat(pathbuff,"/");
   strcat(pathbuff,file);
   return execve(pathbuff,argv,envp);
}

int execvp(const char *file, char *const argv[])
{
   return execvpe(file, argv, __environ);
}


int waitpid(int pid, int *status, int options)
{
   struct {int pid;  int *status; int options;} waitpidcall = {.pid = pid, .status = status, .options = options};
   return do_syscall(SYSCALL_WAITPID, &waitpidcall);
}

void *malloc(size_t size)
{
   void *retp;
   struct {void **retp; int size;} malloccall = {.retp = &retp, .size = size};
   do_syscall(SYSCALL_MALLOC, &malloccall);
   return retp;
}

void *realloc(void *p, size_t size)
{
   void *newbuff = 0;
   if(size != 0)
   {
      newbuff = malloc(size);
   }
   if(newbuff && p)
   {
      memcpy(newbuff,p,size);
   }
   return newbuff;
}

void free(void *ptr)
{
   do_syscall(SYSCALL_FREE,ptr);
}

__attribute((weak)) void event_handler(int event, void *arg)
{
   (void) event;
   (void) arg;
}
