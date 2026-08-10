#ifndef __OS_SYSCALL_H__
#define __OS_SYSCALL_H__

int syscall_handler(int syscall, void *args);

#define SYSCALL_READ 0
#define SYSCALL_WRITE 1
#define SYSCALL_OPEN 2
#define SYSCALL_CLOSE 3
#define SYSCALL_CLONE 4
#define SYSCALL_EXEC 5

#endif