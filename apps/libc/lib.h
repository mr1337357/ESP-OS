#ifndef __LIB_H__
#define __LIB_H__

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
#define SYSCALL_READDIR 11
#define SYSCALL_DUP2 12

extern int (*do_syscall)(int callnum, void *arg);

extern char **__environ;

#endif