#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

void spi_init();

void sd_init()
{
    static bool sd_is_init = false;
    if(sd_is_init)
    {
        return;
    }
    spi_init();

}

void spi_init()
{
    static bool spi_is_init = false;
    if(spi_is_init)
    {
        return;
    }
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.unaligned_multi_block_rw_max_chunk_size = 8;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = CONFIG_PIN_MOSI_0,
        .miso_io_num = CONFIG_PIN_MISO_0,
        .sclk_io_num = CONFIG_PIN_SCLK_0,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
}