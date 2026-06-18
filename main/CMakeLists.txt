idf_component_register(SRCS "os_vfs.c" "os_psram.c" "os_hal.c" "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_psram fatfs esp_driver_sdspi
                       WHOLE_ARCHIVE)