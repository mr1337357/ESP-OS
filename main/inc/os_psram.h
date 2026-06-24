#ifndef __OS_PSRAM_H__
#define __OS_PSRAM_H__

void os_psram_init();

void *os_psram_code_malloc(int size);

void *os_psram_heap_malloc(int size);

void os_psram_free(void *address);

void os_psram_thread_free(int pid);

void os_psram_duplicate_thread(int oldpid, int newpid);

#endif