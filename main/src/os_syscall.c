#include <stdint.h>
#include <stdio.h>

#include "os_syscall.h"

//HACK!!!
typedef struct
{
  int fd;
  int len;
  uint8_t *buffer;
} fileop;

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
            if(fop->fd == 1)
            {
                printf("%s",fop->buffer);
            }
            break;
        case SYSCALL_CLONE:
            break;
        case SYSCALL_EXEC:
            break;
    }
    return 0;
}