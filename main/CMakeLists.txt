idf_component_register(SRCS "src/os_elf.c" "src/os_psram.c" "src/os_thread.c" "src/os_vfs.c" "src/os_psram.c" "src/os_hal.c" "src/os_syscall.c" "src/main.c"
                       INCLUDE_DIRS "inc"
                       REQUIRES esp_psram fatfs esp_driver_sdspi esp_mm
                       WHOLE_ARCHIVE)

set(image ../FATFS)

fatfs_create_spiflash_image(storage ${image} FLASH_IN_PROJECT PRESERVE_TIME)
