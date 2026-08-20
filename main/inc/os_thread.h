#ifndef __OS_THREAD_H__
#define __OS_THREAD_H__

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define OS_THREADS_MAX_THREADS 16

struct alloc_chain
{
    struct alloc_chain *next;
    uint32_t thread_map;
    uint8_t buffer[];
};

typedef struct alloc_chain alloc_chain_t;

typedef struct
{
    TaskHandle_t task;
    alloc_chain_t *code;
    alloc_chain_t *heap;
    char cwd[256];
    int filedes_list[10];
    SemaphoreHandle_t close_sem;
} os_thread;

void os_threads_init();
int os_threads_getpid();
os_thread *os_threads_get_current_thread();
os_thread *os_threads_get_thread(int pid);
int os_threads_create(void *entry,void *threadarg);
void os_threads_exec(char *filename, int argc, char **argv);

#endif
