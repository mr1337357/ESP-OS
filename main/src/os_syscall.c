#include <stdint.h>
#include <stdio.h>

uint32_t syscall_handler(int syscall, void *args)
{
    printf("syscall_handler(%d, %p)\r\n",syscall, args);
    return 0;
}