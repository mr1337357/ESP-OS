#include <sys/types.h>
#include <sys/ioctl.h>

#define IOCPARM_MASK    0x7fUL          /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000UL    /* no parameters */
#define IOC_OUT         0x40000000UL    /* copy out parameters */
#define IOC_IN          0x80000000UL    /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
                                        /* 0x20000000 distinguishes new &
                                           old ioctl's */
#define _IO(x,y)        ((long)(IOC_VOID|((x)<<8)|(y)))
#define _IOR(x,y,t)     ((long)(IOC_OUT|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y)))
#define _IOW(x,y,t)     ((long)(IOC_IN|((sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y)))

#define GPIO_IOCTL 'g'

#define GPIO_IOCTL_SET_MODE _IOW(GPIO_IOCTL, 1, unsigned int)
#define GPIO_IOCTL_GET_MODE _IOR(GPIO_IOCTL, 2, unsigned int)
#define GPIO_IOCTL_SET_PUPD _IOW(GPIO_IOCTL, 3, unsigned int)
#define GPIO_IOCTL_GET_PUPD _IOR(GPIO_IOCTL, 4, unsigned int)
#define GPIO_IOCTL_EDGEMODE _IOW(GPIO_IOCTL, 5, unsigned int)

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
    int edgeMode;
    SemaphoreHandle_t mtx;
} gpio_info;

typedef struct 
{
    int inuse;
    int type;
    int index;
    union 
    {
        gpio_info ginfo;
    };
    
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

ssize_t os_devfs_gpio_write(int fd, const void *data, size_t size)
{
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
    if(fd_context[fd].index < 0 || fd_context[fd].index >= CONFIG_NUM_GPIO)
    {
        return -1;
    }
    if(fd_context[fd].ginfo.edgeMode != GPIO_INTR_DISABLE)
    {
        xSemaphoreTake(fd_context[fd].ginfo.mtx,portMAX_DELAY);
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

int os_devfs_gpio_open(const char *name, int flags, int mode)
{
    int gpionum = 0;
    int fd;
    sscanf(&name[5],"%d",&gpionum);
    if(gpionum < 0 || gpionum >= CONFIG_NUM_GPIO)
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
    fd_context[fd].ginfo.mtx = xSemaphoreCreateMutex();
    return fd;
}

int os_devfs_open(void* ctx, const char * path, int flags, int mode)
{
    int fd = -1;
    if(!strncmp(path,"/gpio",strlen("/gpio")))
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
    if(fd_context[fd].type == HANDLE_IS_GPIO)
    {
        vSemaphoreDelete(fd_context[fd].ginfo.mtx);
    }
    return 0;
}

DIR *os_devfs_opendir(void* ctx, const char* name)
{
    expanded_dirp *pdir;
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
        if(epdir->index >= CONFIG_NUM_GPIO)
        {
            epdir->index = 0;
            epdir->flags = HANDLE_IS_I2C;
        }
        else
        {
            sprintf(dent.d_name,"gpio%d",epdir->index);
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

void gpio_isr_handler(void* arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int pin = (int)arg;
    int i;
    for(i=0;i<20;i++)
    {
        if(fd_context[i].inuse && fd_context[i].type == HANDLE_IS_GPIO && fd_context[i].index == pin)
        {
            xSemaphoreGiveFromISR(fd_context[i].ginfo.mtx,&xHigherPriorityTaskWoken);
        }
    }
}

int os_devfs_gpio_ioctl(int fd, int cmd, va_list args)
{
    unsigned int *arg;
    if(fd < 0 || fd >= 20)
    {
        return -1;
    }
    if(fd_context[fd].inuse == 0)
    {
        return -1;
    }
    arg = va_arg(args, unsigned int *);
    switch(cmd)
    {
        case GPIO_IOCTL_SET_MODE:
            gpio_set_direction(fd_context[fd].index,*arg);
            break;
        case GPIO_IOCTL_GET_MODE:
            //gpio_get_direction(fd_context[fd].index);
            return -1;
            break;
        case GPIO_IOCTL_SET_PUPD:
            gpio_set_pull_mode(fd_context[fd].index,*arg);
            break;
        case GPIO_IOCTL_EDGEMODE:
            fd_context[fd].ginfo.edgeMode = *arg;
            gpio_set_intr_type(fd_context[fd].index, fd_context[fd].ginfo.edgeMode);
            if(fd_context[fd].ginfo.edgeMode == GPIO_INTR_DISABLE)
            {
                gpio_isr_handler_remove(fd_context[fd].index);
            }
            else
            {
                gpio_isr_handler_add(fd_context[fd].index, gpio_isr_handler, (void *)fd_context[fd].index);
            }
            break;
        default:
            return -1;
            break;
        
    }
    return 0;
}

int os_devfs_ioctl(void* ctx, int fd, int cmd, va_list args)
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
            return os_devfs_gpio_ioctl(fd, cmd, args);
            break;
    }
    return 0;
    return -1;
}

static esp_err_t os_devfs_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                   esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    int i;
    for(i=0;i<nfds;i++)
    {
        if(FD_ISSET(i,readfds))
        {
            if(fd_context[i].inuse == 0)
            {
                return ESP_ERR_INVALID_STATE;
            }
            else
            {

            }
        }
    }
    return ESP_OK;
}

static esp_err_t os_devfs_end_select(void *end_select_args)
{
    return ESP_OK;
}



static const esp_vfs_select_ops_t os_devfs_select = 
{
    .start_select = os_devfs_start_select,
    .end_select = os_devfs_end_select,
};

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
    .select = &os_devfs_select,
};

void os_devfs_init(char *where)
{
    ESP_ERROR_CHECK(esp_vfs_register_fs(where, &os_devfs, ESP_VFS_FLAG_STATIC | ESP_VFS_FLAG_CONTEXT_PTR, NULL));
}