#include <stdint.h>
#include <stdio.h>

#include "os_syscall.h"
#include "os_thread.h"
#include "os_filedes.h"

typedef struct
{
  int fd;
  int len;
  uint8_t *buffer;
} fileop;

typedef struct 
{
   void *entry;
   void *threadarg;
} clone_args;

typedef struct
{
   char *fname;
   char **argv;
   char **env;
} exec_args; //lol we just un-consted their memory 

int syscall_file_read(fileop *fop)
{
    os_thread *current = os_threads_get_current_thread();
    if(fop->fd < 0 || fop->fd >= 10)
    {
        return -1;
    }
    if(current->filedes_list[fop->fd] < 0)
    {
        return -1;
    }
    return os_file_read(current->filedes_list[fop->fd],fop->buffer,fop->len);
}

int syscall_file_write(fileop *fop)
{
    os_thread *current = os_threads_get_current_thread();
    if(fop->fd < 0 || fop->fd >= 10)
    {
        return -1;
    }
    if(current->filedes_list[fop->fd] < 0)
    {
        return -1;
    }
    return os_file_write(current->filedes_list[fop->fd],fop->buffer,fop->len);
}

int syscall_clone(clone_args *args)
{
   return os_threads_create(args->entry,args->threadarg);
}

int syscall_exec(exec_args *args)
{
   int argc;
   if(args == 0 || args->argv == 0)
   {
      return -1;
   }
   for(argc = 0; args->argv[argc] != 0; argc++)
   {
   }
   os_threads_exec(args->fname, argc, args->argv);
   return -1;
}


int syscall_handler(int syscall, void *args)
{
    printf("syscall_handler(%d, %p)\r\n",syscall, args);
    switch(syscall)
    {
        case SYSCALL_READ:
            return syscall_file_read(args);
            break;
        case SYSCALL_WRITE:
            return syscall_file_write(args);
            break;
        case SYSCALL_CLONE:
            return syscall_clone(args);
            break;
        case SYSCALL_EXEC:
            return syscall_exec(args);
            break;
    }
    return 0;
}
