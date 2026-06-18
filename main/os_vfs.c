#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdkconfig.h"

#include "os_vfs.h"

static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

void os_vfs_init()
{
    const esp_vfs_fat_mount_config_t spi_mount_config = {
        .max_files = 4, // Number of files that can be open at a time
        .format_if_mount_failed = true, // If true, try to format the partition if mount fails
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE, // Size of allocation unit, cluster size.
        .use_one_fat = false, // Use only one FAT table (reduce memory usage), but decrease reliability of file system in case of power failure.
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl("/internal", "storage", &spi_mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        printf("Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
#if defined(CONFIG_USE_SDCARD)
    esp_vfs_fat_sdmmc_mount_config_t sd_mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_host_t host = SDSPI_HOST_DEFAULT(); //this probably is wrong
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    sdmmc_card_t *card;
    slot_config.gpio_cs = CONFIG_SD_CSPIN;
    slot_config.host_id = host.slot;

    err = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, &sd_mount_config, &card);
    if(err != ESP_OK)
    {
        printf("Failed to mount SDCARD (%s)", esp_err_to_name(err));
    }
#endif
}