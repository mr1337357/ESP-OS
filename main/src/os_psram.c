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
    os_thread *thread = os_threads_get_current_thread();
    newmem = heap_caps_malloc(size + 8, MALLOC_CAP_SPIRAM);
    newmem->next = 0;
    newmem->thread_map = (1<<os_threads_getpid());
    //printf("newmem %p\n",newmem);
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
    os_thread *thread = os_threads_get_current_thread();
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

void os_psram_free(void *address)
{
    alloc_chain_t *temp;
    alloc_chain_t *temp2;
    os_thread *thread = os_threads_get_current_thread();
    temp = thread->code;
    if(temp && temp->buffer == address)
    {
        thread->code = temp->next;
        free(temp);
    }
    for(temp = thread->code; temp; temp = temp->next)
    {   
        if(temp->next)
        {
            if(temp->next->buffer == address)
            {
                temp2 = temp->next;
                temp->next = temp->next->next;
                free(temp2);
                return;
            }
        }
    }
}

void os_psram_thread_free(int pid)
{
    os_thread *thread = os_threads_get_thread(pid);
    alloc_chain_t *temp;
    alloc_chain_t *temp_next;
    /*
    for(temp = thread->code;temp;temp = temp->next)
    {
        if((uint32_t)temp < 0x3C000000)
        {
            //hiding a bug for now
            break;
        }
        temp->thread_map &= ~(1 << pid);
        if(temp->thread_map == 0)
        {
            free(temp); //use-after-free :o
        }
    }*/
    for(temp = thread->heap;temp;temp = temp_next)
    {
        //printf("(%d)freeing(%p,%lX)\n", pid, temp,temp->thread_map);
        temp_next = temp->next;
        temp->thread_map &= ~(1 << pid);
        if(temp->thread_map == 0)
        {
            //printf("freed\n");
            free(temp);
        }
    }
}

void os_psram_duplicate_thread(int oldpid, int newpid)
{
    os_thread *oldthread = os_threads_get_thread(oldpid);
    alloc_chain_t *temp;
    if(!oldthread)
    {
        return;
    }
    for(temp = oldthread->code; temp; temp = temp->next)
    {
        temp->thread_map |= (1 << newpid);
    }

    for(temp = oldthread->heap;temp;temp = temp->next)
    {
        temp->thread_map |= (1 << newpid);
    }
}
