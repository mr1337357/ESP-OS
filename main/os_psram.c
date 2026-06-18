#include <stdio.h>

#include "esp_psram.h"

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