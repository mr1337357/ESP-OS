#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "os_thread.h"
#include "os_psram.h"

SemaphoreHandle_t os_thread_mutex;

os_thread threads[OS_THREADS_MAX_THREADS];

void os_threads_init()
{
    os_thread_mutex = xSemaphoreCreateMutex();
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

os_thread *os_threads_get_current_thread()
{
    int pid = os_threads_getpid();
    if(pid > 0)
    {
        return &threads[pid];
    }
    return 0;
}

os_thread *os_threads_get_thread(int pid)
{
    if(pid > 0)
    {
        return &threads[pid];
    }
    return 0;
}

void _os_threads_launcher(void *arg)
{
    TaskHandle_t mytask;
    int pid = os_threads_getpid();
    //(void)(*entry)(void)
    //Thread returned. Cleanup.
    mytask = threads[pid].task;
    os_psram_thread_free(pid);
    threads[pid].task = 0;
    vTaskDelete(threads[pid].task);
}

os_thread *os_threads_create(void *entry,void *threadarg)
{
    int oldpid;
    int i;
    for(i=0;i<OS_THREADS_MAX_THREADS;i++)
    {
        if(!threads[i].task)
        {
            break;
        }
    }
    if(i < OS_THREADS_MAX_THREADS)
    {
        oldpid = os_threads_getpid();
        threads[i].code = threads[oldpid].code;
        threads[i].heap = threads[oldpid].heap;
        os_psram_duplicate_thread(oldpid,i);
        xTaskCreate(_os_threads_launcher, "app", 4096, entry, tskIDLE_PRIORITY, &threads[i].task);

    }
    return 0;
}

void os_threads_exec(char *filename)
{
    int pid = os_threads_getpid();
    TaskHandle_t oldtask;
    oldtask = threads[pid].task;
    //unload memory
    os_psram_thread_free(pid);
    threads[pid].code = 0;
    threads[pid].heap = 0;

}