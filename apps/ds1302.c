#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "gpio.h"

int CLOCKPIN = 0;
int DATPIN = 0;
int ENPIN = 0;

int clkfd;
int datfd;
int rstfd;

int gpio_from_env(char *env)
{
    int index;
    char *name = getenv(env);
    if(!name)
    {
        return -1;
    }
    index = atoi(name);
    if(index < 0)
    {
        return -1;
    }
    return gpio_open(index);
}

int open_gpios()
{

    clkfd = gpio_from_env("PIN_CLK");
    datfd = gpio_from_env("PIN_DAT");
    rstfd = gpio_from_env("PIN_RST");
    if(clkfd < 0 || datfd < 0 || rstfd < 0)
    {
        return -1;
    }
    gpio_set_mode(clkfd,GPIO_MODE_OUTPUT);
    gpio_set_mode(datfd,GPIO_MODE_INPUT);
    gpio_set_mode(rstfd,GPIO_MODE_OUTPUT);
    gpio_setOutput(rstfd,0);
    return 0;
}

void start_transaction()
{
    gpio_setOutput(clkfd,0);
    gpio_setOutput(rstfd,1);
    usleep(4);
}

void write_byte(uint8_t byte)
{
    int i;
    gpio_set_mode(datfd,GPIO_MODE_OUTPUT);
    usleep(1);
    for(i=0;i<8;i++)
    {
        gpio_setOutput(datfd,byte&1);
        usleep(1);
        gpio_setOutput(clkfd,1);
        usleep(1);
        if(i==7)
        {
            gpio_set_mode(datfd, GPIO_MODE_INPUT);
        }
        gpio_setOutput(clkfd,0);
        byte>>=1;
    }
}

uint8_t read_byte()
{
    int i;
    uint8_t byte;
    gpio_set_mode(datfd,GPIO_MODE_INPUT);
    for(i=0;i<8;i++)
    {
        byte >>= 1;
        byte |= gpio_getInput(datfd)?0x80:0x00;
        gpio_setOutput(clkfd,1);
        usleep(1);
        gpio_setOutput(clkfd,0);
        usleep(1);
    }
    return byte;
}

int main(int argc, char **argv)
{
    if(open_gpios() < 0)
    {
        fprintf(stderr,"failed to open GPIOs\n");
        return 1;
    }
    return 0;
}