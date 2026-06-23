#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"

#include "os_thread.h"


os_thread threads[OS_THREADS_MAX_THREADS];

void os_threads_init()
{
    memset(threads,0,sizeof(threads));
    threads[0].task = xTaskGetCurrentTaskHandle();
}

int os_threads_getpid()
{
    TaskHandle_t current = xTaskGetCurrentTaskHandle();
    int i;
    for(i=0;i<OS_THREADS_MAX_THREADS;i++)
    {
        if(threads[i].task == current)
        {
            return i;
        }
    }
    return -1; //we should probably force a crash here
}

os_thread *os_threads_get_thread()
{
    int pid = os_threads_getpid();
    if(pid > 0)
    {
        return &threads[pid];
    }
    return 0;
}