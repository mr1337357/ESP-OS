idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_psram fatfs esp_driver_sdspi
                       WHOLE_ARCHIVE)