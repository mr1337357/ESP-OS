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

//stolen from goggle gemini
static const uint8_t ASCII_TO_7SEG[128] = {
    // 0x00 - 0x1F: Non-printable control characters (All OFF)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // 0x20 - 0x2F: Punctuation & Symbols
    0x00, // ' ' (Space)
    0x86, // '!' (B, C, DP)
    0x22, // '"' (B, F)
    0x00, // '#' (Unrepresentable)
    0x00, // '$' (Unrepresentable)
    0x00, // '%' (Unrepresentable)
    0x00, // '&' (Unrepresentable)
    0x02, // ''' (B)
    0x39, // '(' (A, D, E, F)
    0x0F, // ')' (A, B, C, D)
    0x00, // '*' (Unrepresentable)
    0x00, // '+' (Unrepresentable)
    0x04, // ',' (C)
    0x40, // '-' (G)
    0x80, // '.' (DP)
    0x52, // '/' (B, E, G)

    // 0x30 - 0x39: Digits 0-9
    0x3F, // '0'
    0x06, // '1'
    0x5B, // '2'
    0x4F, // '3'
    0x66, // '4'
    0x6D, // '5'
    0x7D, // '6'
    0x07, // '7'
    0x7F, // '8'
    0x6F, // '9'

    // 0x3A - 0x40: More Symbols
    0x00, // ':'
    0x00, // ';'
    0x00, // '<'
    0x48, // '=' (D, G)
    0x00, // '>'
    0x53, // '?' (A, B, E, G)
    0x00, // '@'

    // 0x41 - 0x5A: Uppercase A-Z (Approximated where necessary)
    0x77, // 'A'
    0x7C, // 'B' (rendered as 'b')
    0x39, // 'C'
    0x5E, // 'D' (rendered as 'd')
    0x79, // 'E'
    0x71, // 'F'
    0x3D, // 'G'
    0x76, // 'H'
    0x06, // 'I'
    0x1E, // 'J'
    0x76, // 'K' (rendered as 'H')
    0x38, // 'L'
    0x15, // 'M' (approximate)
    0x37, // 'N' (approximate)
    0x3F, // 'O'
    0x73, // 'P'
    0x67, // 'Q' (rendered as 'q')
    0x50, // 'R' (rendered as 'r')
    0x6D, // 'S'
    0x78, // 'T' (rendered as 't')
    0x3E, // 'U'
    0x3E, // 'V' (rendered as 'U')
    0x2A, // 'W' (approximate)
    0x76, // 'X' (rendered as 'H')
    0x6E, // 'Y' (rendered as 'y')
    0x5B, // 'Z'

    // 0x5B - 0x60: Brackets & Misc
    0x39, // '['
    0x64, // '\'
    0x0F, // ']'
    0x23, // '^'
    0x08, // '_'
    0x20, // '`'

    // 0x61 - 0x7A: Lowercase a-z
    0x5F, // 'a'
    0x7C, // 'b'
    0x58, // 'c'
    0x5E, // 'd'
    0x7B, // 'e'
    0x71, // 'f'
    0x6F, // 'g'
    0x74, // 'h'
    0x04, // 'i'
    0x0E, // 'j'
    0x75, // 'k'
    0x30, // 'l'
    0x15, // 'm'
    0x54, // 'n'
    0x5C, // 'o'
    0x73, // 'p'
    0x67, // 'q'
    0x50, // 'r'
    0x6D, // 's'
    0x78, // 't'
    0x1C, // 'u'
    0x1C, // 'v'
    0x2A, // 'w'
    0x76, // 'x'
    0x6E, // 'y'
    0x5B, // 'z'

    // 0x7B - 0x7F: Braces, Pipe, Tilde, DEL
    0x46, // '{'
    0x30, // '|'
    0x70, // '}'
    0x01, // '~'
    0x00  // DEL
};

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
        segments = ASCII_TO_7SEG[string[i]&0x7F];
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

    if(argc > 2)
    {
        tm1650_show_string(fd,argv[2]);
    }
    else
    {
        tm1650_show_string(fd,"----");
    }
    close(fd);
    return 0;
}