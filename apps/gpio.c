#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpio.h"

int main(int argc, char **argv)
{
    int pin;
    int fd;
    if(argc < 3)
    {
        printf("USAGE: %s n operation [value]\n",argv[0]);
        return 1;
    }
    pin = atoi(argv[1]);
    fd = gpio_open(pin);
    if(fd < 0)
    {
        printf("failed to open pin %d\n",pin);
        return 1;
    }
    if(!strcmp(argv[2],"read"))
    {
        printf("%c\n",gpio_getInput(fd)?'1':'0');
    }
    if(!strcmp(argv[2],"write"))
    {
        if(argc < 4)
        {
            printf("error! no value\n");
            return 1;
        }
        gpio_setOutput(fd,argv[3][0]=='0'?0:1);
    }
    if(!strcmp(argv[2],"dir"))
    {
        if(argc < 4)
        {
            printf("error! no value\n");
            return 1;
        }
        gpio_set_mode(fd,argv[3][0]=='0'?GPIO_MODE_INPUT:GPIO_MODE_OUTPUT);
    }
    return 0;
}
