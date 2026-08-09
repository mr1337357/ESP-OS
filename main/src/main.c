#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "os_psram.h"
#include "os_hal.h"
#include "os_vfs.h"
#include "os_thread.h"

void list_dir(char *dirname, int indent)
{
    struct dirent *f;
    int i;
    DIR *d = opendir(dirname);
    if(!d)
    {
        return;
    }
    f = readdir(d);
    while(f)
    {
        for(i=0;i<indent;i++)
        {
            printf(" ");
        }
        printf("%s\n",f->d_name);
        if(f->d_type == DT_DIR)
        {
            char *name = malloc(256);
            strcpy(name,dirname);
            strcat(name,"/");
            strcat(name,f->d_name);
            list_dir(name,indent+2);
            free(name);
        }
        f = readdir(d);
    }
    closedir(d);
}

void init_launcher(void *arg)
{
    os_threads_exec("/sdcard/espos/bin/init.elf", 0, 0);
    printf("SD Init not found\r\n");
    os_threads_exec("/internal/espos/bin/init.elf", 0, 0);
    printf("PANIC: init not found\r\n");
}

void app_main(void)
{
    esp_err_t ret;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("os start\n");
    os_psram_init();
    os_hal_spi_init();
    os_vfs_init();
    os_threads_init();
    os_threads_create(init_launcher, 0);
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}
