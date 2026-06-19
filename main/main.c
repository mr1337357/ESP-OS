#include <stdio.h>
#include <dirent.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "os_hal.h"
#include "os_vfs.h"

void list_dir(char *dirname)
{
    struct dirent *f;
    DIR *d = opendir("/sdcard");
    if(!d)
    {
        return;
    }
    
}

void app_main(void)
{
    esp_err_t ret;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("test\n");

    os_hal_spi_init();
    os_vfs_init();
    DIR *sdcard = opendir("/sdcard");
    if(!sdcard)
    {
        printf("no sdcard\n");
    }
    else
    {
        struct dirent *f;
        f = readdir(sdcard);
        while(f)
        {
            printf("  %s ",f->d_name);
            printf("%d\n",f->d_type);
            f = readdir(sdcard);
        }
    }
}