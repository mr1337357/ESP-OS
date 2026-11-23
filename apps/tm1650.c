#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
//#include <linux/i2c.h>
//#include <linux/i2c-dev.h>
#include <i2c.h>

#define TM1650_DISPLAY_BASE 0x34 // Address of the left-most digit 
#define TM1650_DCTRL_BASE   0x24 // Address of the control register of the left-most digit

// Standard 7-segment font (0-9, A-F) mapped to bits: DP G F E D C B A
static const uint8_t HEX_FONT[16] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71  // F
};

//
int tm1650_send_cmd(int fd, uint8_t cmd, uint8_t data)
{
    struct i2c_msg msg =
    {
        .addr  = cmd,
        .flags = 0, // Write
        .len   = 1,
        .buf   = &data,
    };
    struct i2c_rdwr_ioctl_data packet =
    {
        .msgs  = &msg,
        .nmsgs = 1
    };

    if (ioctl(fd, I2C_RDWR, &packet) < 0)
    {
        perror("I2C_RDWR failed");
        return -1;
    }
    return 0;
}

// Configure brightness (0-7, where 0 is max brightness on TM1650) and display state
void tm1650_set_display(int fd, uint8_t brightness, int on)
{
    uint8_t ctrlbyte;
    int i;
    for(i=0;i<4;i++)
    {
        ctrlbyte = (brightness << 4) | (!!on);
        tm1650_send_cmd(fd, TM1650_DCTRL_BASE + i, ctrlbyte);
    }
}

void tm1650_show_string(int fd, char *string)
{
    uint8_t segments;
    int i;
    for(i=0;i<4;i++)
    {
        if(string[i] >= '0' && string[i] <= '9')
        {
            segments = HEX_FONT[string[i]-'0'];
        }
        //segments = 0x55;//HEX_FONT[string[i]];
        tm1650_send_cmd(fd, TM1650_DISPLAY_BASE + i, segments);
    }
}

int main(int argc, char **argv) {
    const char *i2c_dev = (argc > 1) ? argv[1] : "/dev/i2c-0";
    int fd = open(i2c_dev, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C bus");
        return 1;
    }

    // Turn display ON at medium brightness (level 3)
    tm1650_set_display(fd, 3, 1);

    tm1650_show_string(fd,"1234");
    sleep(2);

    close(fd);
    return 0;
}