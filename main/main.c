#include <stdio.h>

#include "esp_psram.h"

void app_main(void)
{
    printf("test\n");
    esp_err_t mem_err = esp_psram_init();
    if(mem_err != ESP_OK)
    {
        printf("mem err %d\n",mem_err);
        return;
    }
    size_t size = esp_psram_get_size();
    printf("mem size %d\n" ,size);
}