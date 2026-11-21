#include <fcntl.h>
#include <stdio.h>

#include "i2c.h"

int main(int argc, char **argv)
{
    int display;
    display = open("/dev/i2c-0",O_RDWR);
    if(display < 0)
    {
        printf("failed to open display\n");
        return 0;
    }
    return 0;
}