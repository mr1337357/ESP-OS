#include <sys/types.h>

#include "esp_vfs.h"
#include "driver/gpio.h"

#define HANDLE_IS_BASE 0
#define HANDLE_IS_GPIO 1
#define HANDLE_IS_SPI  2
#define HANDLE_IS_I2C  3

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

int path_is_valid(const char *path)
{
    char temp_path[256];
    int i;
    char *slash;
    strcpy(temp_path,path);
    slash = strrchr(temp_path,'/');
    if(slash && slash != temp_path)
    {
        slash[0] = 0;
    }
    if(!strcmp(temp_path,"/"))
    {
        return 0;
    }
    if(temp_path[0] == '/')
    {
        for(i=0;i<NUM_BASEDIRS;i++)
        {
            if(!strcmp(&temp_path[1],basedirs[i]))
            {
                return i+1;
            }
        }
    }
    return -1;
}

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
    char *filename = strrchr(name,'/');
    if(!filename || filename == name)
    {
        return -1;
    }
    filename++;
    if(strncmp(filename,"gpio",4))
    {
        return -1;
    }
    sscanf(&filename[4],"%d",&gpionum);
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
    int flag;
    flag = path_is_valid(path);
    switch(flag)
    {
        case HANDLE_IS_BASE:
            break;
        case HANDLE_IS_GPIO:
            fd = os_devfs_gpio_open(path, flags, mode);
            break;
        case HANDLE_IS_SPI:
            fd = -1;
            break;
        case HANDLE_IS_I2C:
            fd = -1;
            break;
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
    int flag;
    expanded_dirp *pdir;
    char *filename;
    flag = path_is_valid(name);
    if(flag < 0)
    {
        return 0;
    }
    filename = strrchr(name,'/');
    if(filename && filename != name)
    {
        return 0;
    }
    pdir = malloc(sizeof(expanded_dirp));
    pdir->index = 0;
    pdir->flags = flag;
    return (DIR *)pdir;
}

int os_devfs_readdir_base(struct dirent *de, expanded_dirp *pdir)
{
    if(pdir->index < 0 || pdir->index >= NUM_BASEDIRS)
    {
        return 0;
    }
    de->d_ino = 0;
    de->d_type = DT_DIR;
    strcpy(de->d_name, basedirs[pdir->index]);
    pdir->index++;
    return 1;
}

int os_devfs_readdir_gpio(struct dirent *de, expanded_dirp *pdir)
{
    if(pdir->index < 0 || pdir->index >= CONFIG_NUM_GPIO)
    {
        return 0;
    }
    de->d_ino = 0;
    de->d_type = DT_REG;
    sprintf(de->d_name,"gpio%d",pdir->index);
    pdir->index++;
    return 1;
}

int os_devfs_readdir_spi(struct dirent *de, expanded_dirp *pdir)
{
    if(pdir->index < 0 || pdir->index >= (CONFIG_SPI_0_NUMCS + CONFIG_SPI_1_NUMCS))
    {
        return 0;
    }
    de->d_ino = 0;
    de->d_type = DT_REG;
    if(pdir->index < CONFIG_SPI_0_NUMCS)
    {
        sprintf(de->d_name,"spi0.%d",pdir->index);
    }
    else
    {
        sprintf(de->d_name,"spi1.%d",pdir->index-CONFIG_SPI_0_NUMCS);
    }
    pdir->index++;
    return 1;
}

struct dirent *os_devfs_readdir(void *ctx, DIR *pdir)
{
    int rv = 0;
    expanded_dirp *epdir;
    static struct dirent dent;
    epdir = (expanded_dirp *)pdir;
    switch(epdir->flags)
    {
        case HANDLE_IS_BASE:
            rv = os_devfs_readdir_base(&dent,epdir);
            break;
        case HANDLE_IS_GPIO:
            rv = os_devfs_readdir_gpio(&dent,epdir);
            break;
        case HANDLE_IS_SPI:
            rv = os_devfs_readdir_spi(&dent,epdir);
            break;
        case HANDLE_IS_I2C:
            rv = 0;
            break;
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