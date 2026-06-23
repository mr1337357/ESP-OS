#include <stdio.h>

#include "esp_psram.h"
#include "esp_heap_caps.h"

#include "os_thread.h"

alloc_chain_t *global_heap_list = 0;

void os_psram_init()
{
    esp_err_t ret;
    ret = esp_psram_init();
    if(ret != ESP_OK)
    {
        printf("mem err %d\n",ret);
        return;
    }
    size_t size = esp_psram_get_size();
    printf("mem size %d\n" ,size);
}

void *os_psram_code_malloc(int size)
{
    alloc_chain_t *temp;
    alloc_chain_t *newmem;
    os_thread *thread = os_threads_get_thread();
    newmem = heap_caps_malloc(size + 8, MALLOC_CAP_SPIRAM);
    newmem->next = 0;
    newmem->thread_map = (1<<os_threads_getpid());
    if(thread->code == 0)
    {
        thread->code = newmem;
        return &newmem->buffer;
    }
    for(temp = thread->code;temp->next; temp = temp->next);
    temp->next = newmem;
    return &newmem->buffer;
}

void *os_psram_heap_malloc(int size)
{
    alloc_chain_t *temp;
    alloc_chain_t *newmem;
    os_thread *thread = os_threads_get_thread();
    newmem = heap_caps_malloc(size + 8, MALLOC_CAP_SPIRAM);
    newmem->next = 0;
    newmem->thread_map = (1<<os_threads_getpid());
    if(thread->heap == 0)
    {
        thread->heap = newmem;
        return &newmem->buffer;
    }
    for(temp = thread->heap;temp->next; temp = temp->next);
    temp->next = newmem;
    return &newmem->buffer;
}