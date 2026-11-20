#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/i2c_master.h"

#include "os_hal.h"

void os_hal_sd_init()
{
    static bool sd_is_init = false;
    if(sd_is_init)
    {
        return;
    }
    os_hal_spi_init();

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

void os_hal_spi_init()
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

i2c_master_bus_handle_t i2c_handle_0;
i2c_master_dev_handle_t i2c_dev_handle_0;
i2c_master_bus_handle_t i2c_handle_1;
i2c_master_dev_handle_t i2c_dev_handle_1;

void os_hal_i2c_init()
{
    static bool i2c_is_init = false;
    if(i2c_is_init)
    {
        return;
    }
#if CONFIG_NUM_I2C == 0
    return;
#elif CONFIG_NUM_I2C >= 1
    i2c_master_bus_config_t i2c_mst_config0 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = CONFIG_I2C_0_SCL,
        .sda_io_num = CONFIG_I2C_0_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_device_config_t dev_cfg0 =
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 400000,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config0, &i2c_handle_0));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_handle_0, &dev_cfg0, &i2c_dev_handle_0));
#if CONFIG_NUM_I2C >= 2
    i2c_master_bus_config_t i2c_mst_config1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_1,
        .scl_io_num = CONFIG_I2C_1_SCL,
        .sda_io_num = CONFIG_I2C_1_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_device_config_t dev_cfg1 =
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 400000,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config1, &i2c_handle_1));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_handle_1, &dev_cfg1, &i2c_dev_handle_1));
#endif
#endif
}
