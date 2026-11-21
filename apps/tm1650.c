#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
//#include <linux/i2c.h>
//#include <linux/i2c-dev.h>
#include <i2c.h>

// TM1650 8-bit commands shifted right by 1 to act as 7-bit I2C addresses
#define TM1650_CMD_CTRL   (0x48 >> 1) // 0x24: Display control
#define TM1650_DIGIT_BASE (0x68 >> 1) // 0x34: Digit 0 (0x34..0x37)

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

// Sends a single byte to one of the TM1650's pseudo-addresses
int tm1650_write_byte(int fd, uint8_t addr7, uint8_t data) {
    struct i2c_msg msg = {
        .addr  = addr7,
        .flags = 0, // Write
        .len   = 1,
        .buf   = &data
    };
    struct i2c_rdwr_ioctl_data packet = {
        .msgs  = &msg,
        .nmsgs = 1
    };

    if (ioctl(fd, I2C_RDWR, &packet) < 0) {
        perror("I2C_RDWR failed");
        return -1;
    }
    return 0;
}

// Configure brightness (0-7, where 0 is max brightness on TM1650) and display state
int tm1650_set_display(int fd, uint8_t brightness, int on) {
    // Control byte format: [0 | B2 B1 B0 | 0 | 0 | 0 | ON/OFF]
    // Note: 000 = level 8 (brightest), 001 = level 1 (dimmest), ..., 111 = level 7
    uint8_t ctrl = ((brightness & 0x07) << 4) | (on ? 0x01 : 0x00);
    return tm1650_write_byte(fd, TM1650_CMD_CTRL, ctrl);
}

// Write a 4-digit hex number with an optional decimal point mask (bits 0..3)
void tm1650_draw_hex(int fd, uint16_t value, uint8_t dot_mask) {
    for (int i = 0; i < 4; i++) {
        // Extract nibbles from left (digit 0) to right (digit 3)
        uint8_t nibble = (value >> ((3 - i) * 4)) & 0x0F;
        uint8_t segments = HEX_FONT[nibble];
        
        if (dot_mask & (1 << i)) {
            segments |= 0x80; // Set DP bit (bit 7)
        }
        tm1650_write_byte(fd, TM1650_DIGIT_BASE + i, segments);
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
    if (tm1650_set_display(fd, 3, 1) < 0) {
        close(fd);
        return 1;
    }

    // Draw "CAFE" with a decimal point after 'A' (digit index 1)
    tm1650_draw_hex(fd, 0xCAFE, 0b0010);
    sleep(2);

    // Simple counter loop
    for (uint16_t count = 0; count <= 0x0100; count++) {
        tm1650_draw_hex(fd, count, 0x00);
        usleep(50000); // 50ms
    }

    close(fd);
    return 0;
}