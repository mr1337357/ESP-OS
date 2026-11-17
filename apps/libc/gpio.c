#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "gpio.h"

int gpio_open(int pin)
{
    int fd = -1;
    char filename[sizeof("/dev/gpioXXX")];
    if(pin > 0 && pin < 999)
    {
        sprintf(filename,"/dev/gpio%d",pin);
        fd = open(filename,O_RDWR);
    }
    return fd;
}

int gpio_set_mode(int fd, gpio_mode_t mode)
{
    return ioctl(fd,GPIO_IOCTL_SET_MODE,&mode);
}

int gpio_set_pupd(int fd, gpio_pull_mode_t pupd)
{
    return ioctl(fd,GPIO_IOCTL_SET_PUPD,&pupd);
}

int gpio_setOutput(int fd, int value)
{
    int val = value?'1':'0';
    return write(fd, &val, 1);
}

int gpio_getInput(int fd)
{
    int val;
    read(fd,&val,1);
    if(val == '1')
    {
        return 1;
    }
    return 0;
}

void gpio_close(int fd)
{
    close(fd);
}