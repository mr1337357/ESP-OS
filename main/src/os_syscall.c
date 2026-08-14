#include <stdint.h>
#include <stdio.h>

#include "os_syscall.h"
#include "os_thread.h"
#include "os_filedes.h"

//HACK!!!
typedef struct
{
  int fd;
  int len;
  uint8_t *buffer;
} fileop;

int syscall_file_read()
{

    return -1;
}

int syscall_file_write(fileop *fop)
{
    printf("syscall_file_write(%p)\n",fop);
    printf("syscall_file_write fd %d\n",fop->fd);
    os_thread *current = os_threads_get_current_thread();
    if(fop->fd < 0 || fop->fd >= 10)
    {
        return -1;
    }
    if(current->filedes_list[fop->fd] < 0)
    {
        return -1;
    }
    printf("current->filedes_list[fop->fd] %d\n",current->filedes_list[fop->fd]);
    return os_file_write(current->filedes_list[fop->fd],fop->buffer,fop->len);
}

int syscall_handler(int syscall, void *args)
{
    printf("syscall_handler(%d, %p)\r\n",syscall, args);
    switch(syscall)
    {
        case SYSCALL_READ:
            break;
        case SYSCALL_WRITE:
            /*HACK! DO NOT USE IN PRODUCTION*/
            fileop *fop = args;
            return syscall_file_write(fop);
            break;
        case SYSCALL_CLONE:
            break;
        case SYSCALL_EXEC:
            break;
    }
    return 0;
}