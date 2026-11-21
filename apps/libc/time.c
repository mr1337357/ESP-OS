#include <time.h>
#include <unistd.h>

#include "lib.h"



unsigned int sleep(unsigned int seconds)
{
    struct timespec ts;
    ts.tv_nsec = 0;
    ts.tv_sec = seconds;
    do_syscall(SYSCALL_NANOSLEEP,&ts);
    return 0;
}

int usleep(useconds_t usec)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = usec * 1000;
    do_syscall(SYSCALL_NANOSLEEP,&ts);
    return 0;
}