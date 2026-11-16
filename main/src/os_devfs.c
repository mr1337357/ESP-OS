#include <sys/types.h>

#include "esp_vfs.h"
#include "driver/gpio.h"

#define CONFIG_NUM_GPIOCHIP ((CONFIG_NUM_GPIO+31)>>5)

#define HANDLE_IS_NONE 0
#define HANDLE_IS_GPIO 1
#define HANDLE_IS_I2C  2
#define HANDLE_IS_SPI  3

#define NUM_BASEDIRS 3

char *basedirs[NUM_BASEDIRS] = 
{
    "gpio",
    "spi",
    "i2c",
};

typedef struct 
{
    int inuse;
    int type;
    int index;
} fd_ctx;

fd_ctx fd_context[20];

typedef struct 
{
    DIR dirp;
    uint32_t flags;
    int index;
}   expanded_dirp;

int os_devfs_fd_nextcontext()
{
    int i;
    for(i=0;i<20;i++)
    {
        if(!fd_context[i].inuse)
        {
            return i;
        }
    }
    return -1;
}

int os_devfs_gpio_open(const char *name, int flags, int mode)
{
    int gpionum = 0;
    int fd;

    sscanf(&name[9],"%d",&gpionum);
    if(gpionum < 0 || gpionum >= CONFIG_NUM_GPIOCHIP)
    {
        return -1;
    }
    fd = os_devfs_fd_nextcontext();
    if(fd < 0)
    {
        return -1;
    }
    fd_context[fd].inuse = 1;
    fd_context[fd].type = HANDLE_IS_GPIO;
    fd_context[fd].index = gpionum;
    return fd;
}

ssize_t os_devfs_gpio_write(int fd, const void *data, size_t size)
{
    gpio_set_direction(fd_context[fd].index, GPIO_MODE_OUTPUT);
    if(fd_context[fd].index < 0 || fd_context[fd].index >= CONFIG_NUM_GPIO)
    {
        return -1;
    }
    if(size)
    {
        if((*(uint8_t *)data) == '0')
        {
            gpio_set_level(fd_context[fd].index, 0);
        }
        else
        {
            gpio_set_level(fd_context[fd].index, 1);
        }
        return 1;
    }
    return 0;
}

ssize_t os_devfs_write(void* p, int fd, const void *data, size_t size)
{
    if(fd < 0 || fd >= 20)
    {
        return -1;
    }
    if(fd_context[fd].inuse == 0)
    {
        return -1;
    }
    switch(fd_context[fd].type)
    {
        case HANDLE_IS_GPIO:
            return os_devfs_gpio_write(fd, data, size);
            break;
    }
    return 0;
}

ssize_t os_devfs_gpio_read(int fd, void *dst, size_t size)
{
    gpio_set_direction(fd_context[fd].index, GPIO_MODE_INPUT);
    if(fd_context[fd].index < 0 || fd_context[fd].index >= CONFIG_NUM_GPIO)
    {
        return -1;
    }
    if(size)
    {
        if(gpio_get_level(fd_context[fd].index))
        {
            *(uint8_t *)dst = '1';
        }
        else
        {
            *(uint8_t *)dst = '0';
        }
        return 1;
    }
    return 0;
}

ssize_t os_devfs_read(void* ctx, int fd, void * dst, size_t size)
{
    if(fd < 0 || fd >= 20)
    {
        return -1;
    }
    if(fd_context[fd].inuse == 0)
    {
        return -1;
    }
    switch(fd_context[fd].type)
    {
        case HANDLE_IS_GPIO:
            return os_devfs_gpio_read(fd, dst, size);
            break;
    }
    return 0;
}

int os_devfs_open(void* ctx, const char * path, int flags, int mode)
{
    int fd = -1;

    if(!strncmp(path,"/gpiochip",strlen("/gpiochip")))
    {
        fd = os_devfs_gpio_open(path, flags, mode);
    }
    
    return fd;
}

int os_devfs_close(void* ctx, int fd)
{
    if(fd<0 || fd >= 20)
    {
        return -1;
    }
    fd_context[fd].inuse = 0;
    return 0;
}

DIR *os_devfs_opendir(void* ctx, const char* name)
{
    expanded_dirp *pdir;
    char *filename;
    if(strcmp(name,"/"))
    {
        return 0;
    }
    pdir = malloc(sizeof(expanded_dirp));
    pdir->index = 0;
    pdir->flags = HANDLE_IS_GPIO;
    return (DIR *)pdir;
}


struct dirent *os_devfs_readdir(void *ctx, DIR *pdir)
{
    int rv = 0;
    expanded_dirp *epdir;
    static struct dirent dent;
    epdir = (expanded_dirp *)pdir;

    dent.d_ino = 0;
    dent.d_type = DT_REG;
    if(epdir->flags == HANDLE_IS_GPIO)
    {
        if(epdir->index >= CONFIG_NUM_GPIOCHIP)
        {
            epdir->index = 0;
            epdir->flags = HANDLE_IS_I2C;
        }
        else
        {
            sprintf(dent.d_name,"gpiochip%d",epdir->index);
            epdir->index++;
            rv = 1;
        }
    }
    if(epdir->flags == HANDLE_IS_I2C)
    {
        if(epdir->index == CONFIG_NUM_I2C)
        {
            epdir->index = 0;
            epdir->flags = HANDLE_IS_SPI;
        }
        else
        {
            sprintf(dent.d_name,"i2c-%d",epdir->index);
            epdir->index++;
            rv = 1;
        }
    }
    if(epdir->flags == HANDLE_IS_SPI)
    {
        if(epdir->index == CONFIG_SPI_0_NUMCS + CONFIG_SPI_1_NUMCS)
        {
            epdir->index = 0;
            epdir->flags = HANDLE_IS_NONE;
        }
        else
        {
            if(epdir->index < CONFIG_SPI_0_NUMCS)
            {
                sprintf(dent.d_name,"spidev0.%d",epdir->index);
            }
            else
            {
                sprintf(dent.d_name,"spidev1.%d",epdir->index-CONFIG_SPI_0_NUMCS);
            }
            epdir->index++;
            rv = 1;
        }
    }
    if(rv > 0)
    {
        return &dent;
    }
    return 0;
}

int  os_devfs_closedir(void *ctx, DIR *pdir)
{
    free(pdir);
    return 0;
}

int os_devfs_ioctl(void* ctx, int fd, int cmd, va_list args)
{
    return -1;
}

static const esp_vfs_dir_ops_t os_devfs_dir = 
{
    .opendir_p = os_devfs_opendir,
    .readdir_p = os_devfs_readdir,
    .closedir_p = os_devfs_closedir,
};

static const esp_vfs_fs_ops_t os_devfs = 
{
    .write_p = &os_devfs_write,
    .read_p = &os_devfs_read,
    .open_p = &os_devfs_open,
    .close_p = &os_devfs_close,
    .dir = &os_devfs_dir,
    //.opendir_p = &os_devfs_opendir,
    .ioctl_p = &os_devfs_ioctl,
};

void os_devfs_init(char *where)
{
    ESP_ERROR_CHECK(esp_vfs_register_fs(where, &os_devfs, ESP_VFS_FLAG_STATIC | ESP_VFS_FLAG_CONTEXT_PTR, NULL));
}