#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_cache.h"

#include "os_thread.h"
#include "os_psram.h"
#include "os_elf.h"

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

struct thread_data
{
    void (*entry)(void *);
    void *arg;
};

int syscall_dummy(int syscall, void *arg)
{
    printf("syscalled %d\n",syscall);
    //while(1);
    return 0;
}

void _os_threads_launcher(void *arg)
{
    struct thread_data *thrd;
    struct thread_data thrd_local;
    TaskHandle_t mytask;
    int pid = os_threads_getpid();
    thrd = arg;
    thrd_local = *thrd;
    free(thrd);
    thrd_local.entry(thrd_local.arg);
    mytask = threads[pid].task;
    os_psram_thread_free(pid);
    threads[pid].task = 0;
    vTaskDelete(threads[pid].task);
}

os_thread *os_threads_create(void *entry,void *threadarg)
{
    struct thread_data *thrd;
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
        thrd = heap_caps_malloc(sizeof(struct thread_data), MALLOC_CAP_SPIRAM);
        thrd->entry = entry;
        thrd->arg = threadarg;
        oldpid = os_threads_getpid();
        threads[i].code = threads[oldpid].code;
        threads[i].heap = threads[oldpid].heap;
        os_psram_duplicate_thread(oldpid,i);
        xTaskCreate(_os_threads_launcher, "app", 4096, thrd, tskIDLE_PRIORITY, &threads[i].task);

    }
    return 0;
}

void Cache_WriteBack_All();

void os_threads_exec(char *filename)
{
    struct thread_data *thrd;
    void *entry;
    int pid = os_threads_getpid();
    TaskHandle_t oldtask;
    oldtask = threads[pid].task;
    //unload memory
    os_psram_thread_free(pid);
    threads[pid].code = 0;
    threads[pid].heap = 0;
    entry = (void *)elf_load(filename);
    Cache_WriteBack_All();
    //esp_cache_msync((void *)(((uint32_t)entry)&~0xFF), 0x1000000, 0);
    if(entry)
    {
        thrd = heap_caps_malloc(sizeof(struct thread_data), MALLOC_CAP_SPIRAM);
        thrd->entry = entry;
        thrd->arg = (void *)syscall_dummy;
        xTaskCreate(_os_threads_launcher, "app", 4096, thrd, tskIDLE_PRIORITY, &threads[pid].task);
    }
    vTaskDelete(oldtask);
}