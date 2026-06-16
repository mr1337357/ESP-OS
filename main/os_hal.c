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

#if CONFIG_NUM_SPI >= 1
spi_device_handle_t spi0_devices[CONFIG_SPI_0_NUMCS];
const uint8_t spi0_cspins[CONFIG_SPI_0_NUMCS] = 
{
#if CONFIG_SPI_0_NUMCS >= 1
    CONFIG_SPI_0_CS_0,
#endif
#if CONFIG_SPI_0_NUMCS >= 2
    CONFIG_SPI_0_CS_1,
#endif
#if CONFIG_SPI_0_NUMCS >= 3
    CONFIG_SPI_0_CS_2,
#endif
};
#endif

#if CONFIG_NUM_SPI >= 2
spi_device_handle_t spi0_devices[CONFIG_SPI_1_NUMCS];
const uint8_t spi0_cspins[CONFIG_SPI_1_NUMCS] = 
{
#if CONFIG_SPI_1_NUMCS >= 1
    CONFIG_SPI_1_CS_0,
#endif
#if CONFIG_SPI_1_NUMCS >= 2
    CONFIG_SPI_1_CS_1,
#endif
#if CONFIG_SPI_1_NUMCS >= 3
    CONFIG_SPI_1_CS_2,
#endif
};
#endif

spi_device_handle_t *spi_devices[CONFIG_NUM_SPI];

void spi_init()
{
    int i;
    esp_err_t ret;
    static bool spi_is_init = false;
    if(spi_is_init)
    {
        return;
    }
#if CONFIG_NUM_SPI == 0
    (void)i;
#endif
#if CONFIG_NUM_SPI >= 1
    {
        spi_bus_config_t bus_cfg = {
            .mosi_io_num = CONFIG_SPI_0_MOSI,
            .miso_io_num = CONFIG_SPI_0_MISO,
            .sclk_io_num = CONFIG_SPI_0_SCLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000,
        };
        ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        ESP_ERROR_CHECK(ret);
        spi_devices[0] = spi0_devices;
        for(i=0;i<CONFIG_SPI_0_NUMCS;i++)
        {
            spi_device_interface_config_t devcfg = {
                .clock_speed_hz = 10 * 1000 * 1000,     //Clock out at 10 MHz
                .mode = 0,                              //SPI mode 0
                .spics_io_num = spi0_cspins[i],             //CS pin
                .queue_size = 7,                        //We want to be able to queue 7 transactions at a time
            };
            ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi0_devices[i]);
            ESP_ERROR_CHECK(ret);
        }

    }
#endif
#if CONFIG_NUM_SPI >= 2
    {
        spi_bus_config_t bus_cfg = {
            .mosi_io_num = CONFIG_SPI_1_MOSI,
            .miso_io_num = CONFIG_SPI_1_MISO,
            .sclk_io_num = CONFIG_SPI_1_SCLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000,
        };
        ret = spi_bus_initialize(SPI3_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        ESP_ERROR_CHECK(ret);
        spi_devices[1] = spi1_devices;
        for(i=0;i<CONFIG_SPI_1_NUMCS;i++)
        {
            spi_device_interface_config_t devcfg = {
                .clock_speed_hz = 10 * 1000 * 1000,     //Clock out at 10 MHz
                .mode = 0,                              //SPI mode 0
                .spics_io_num = spi1_cspins[i],             //CS pin
                .queue_size = 7,                        //We want to be able to queue 7 transactions at a time
            };
            ret = spi_bus_add_device(SPI3_HOST, &devcfg, &spi1_devices[i]);
            ESP_ERROR_CHECK(ret);
        }
    }
#endif
    spi_is_init = true;
}

