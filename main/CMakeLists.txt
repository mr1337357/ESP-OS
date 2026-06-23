idf_component_register(SRCS "src/os_psram.c" "src/os_thread.c" "src/os_vfs.c" "src/os_psram.c" "src/os_hal.c" "src/main.c"
                       INCLUDE_DIRS "inc"
                       REQUIRES esp_psram fatfs esp_driver_sdspi
                       WHOLE_ARCHIVE)