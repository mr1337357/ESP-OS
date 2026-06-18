#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "os_hal.h"
#include "os_vfs.h"

void app_main(void)
{
    esp_err_t ret;
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    printf("test\n");

    os_hal_spi_init();
    os_vfs_init();
}