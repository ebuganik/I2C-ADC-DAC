#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <math.h>

// I2C address of DAC53401
#define DAC53401_I2C_ADDR 0x48
// I2C address of ADS7828
#define ADS7828_I2C_ADDR 0x48
// Status register of DAC53401 to read DEVID
#define STATUS 0xD0
// General config register of DAC53401
#define GENERAL_CONFIG 0xD1
// Data register of DAC53401
#define DAC_DATA 0x21
// Trigger register of DAC53401
#define TRIGGER 0xD3
// ADS7828 operating mode - command byte
// Single-ended input, Channel 0, VREF ON, ADC ON
#define COMMAND 0x8F

int main()

{
    int fd;
    unsigned char regs[] = {
        STATUS,
        GENERAL_CONFIG,
        TRIGGER,
        DAC_DATA,
        COMMAND
    };
    unsigned char write_gen[] = {
        GENERAL_CONFIG,
        0x00,
        0x00
    };
    unsigned char write_trigger[] = {
        TRIGGER,
        0x00,
        0x10
    };
    unsigned char write_data[] = {
        DAC_DATA,
        dac_data[0],
        dac_data[1]
    };
    unsigned char rx_buffer[2] = {};
    unsigned char dac_data[2] = {};

    struct i2c_msg iomsgs[] = {

        [0] = {
            .addr = ADS7828_I2C_ADDR,
            .flags = 0,
            .buf = & regs[4],
            .len = 1
        },

        [1] = {
            .addr = ADS7828_I2C_ADDR,
            .flags = I2C_M_RD,
            .buf = rx_buffer,
            .len = 2
        },

        [2] = {
            .addr = DAC53401_I2C_ADDR,
            .flags = 0,
            .buf = & regs[0],
            .len = 1
        },

        [3] = {
            .addr = DAC53401_I2C_ADDR,
            .flags = I2C_M_RD,
            .buf = rx_buffer,
            .len = 2
        },

        [4] = {
            .addr = DAC53401_I2C_ADDR,
            .flags = 0,
            .buf = write_gen,
            .len = 3
        },

        [5] = {
            .addr = DAC53401_I2C_ADDR,
            .flags = 0,
            .buf = & regs[1],
            .len = 1
        },

        [6] = {
            .addr = DAC53401_I2C_ADDR,
            .flags = I2C_M_RD,
            .buf = rx_buffer,
            .len = 2
        },

        [7] = {
            .addr = DAC53401_I2C_ADDR,
            .flags = 0,
            .buf = write_trigger,
            .len = 3
        },

        [8] = {
            .addr = DAC53401_I2C_ADDR,
            .flags = 0,
            .buf = write_data,
            .len = 3
        }

    };

    struct i2c_rdwr_ioctl_data msgset[] = {

        [0] = {
            .msgs = & iomsgs[0], // message set to configure and read raw ADC data
            .nmsgs = 2
        },

        [1] = {
            .msgs = & iomsgs[2], // message set to read DAC Device ID
            .nmsgs = 2
        },

        [2] = {
            .msgs = & iomsgs[4], // message set to write to GENERAL_CONFIG register
            .nmsgs = 1
        },

        [3] = {
            .msgs = & iomsgs[5], // message set to read from GENERAL_CONFIG register
            .nmsgs = 2
        },

        [4] = {
            .msgs = & iomsgs[7], // message set to write to write to TRIGGER register
            .nmsgs = 1
        },

        [5] = {
            .msgs = & iomsgs[8], // message set to write to write to DAC_DATA register
            .nmsgs = 1
        }

    };

    // Try to open I2C device
    fd = open("/dev/i2c-1", O_RDWR);

    // Check for any errors
    if (fd < 0) {
        printf("Error while trying to open i2c device.\n");
        return -1;
    }

    // read Device ID from DAC STATUS register
    ioctl(fd, I2C_RDWR, & msgset[1]);
    printf("Device ID: %02x %02x\n", rx_buffer[0], rx_buffer[1]);
    sleep(1);

    //write to gen conf
    ioctl(fd, I2C_RDWR, & msgset[2]);
    sleep(1);

    // read from GENERAL	
    ioctl(fd, I2C_RDWR, & msgset[3]);
    sleep(1);
    printf("read from GENERAL %02x %02x\n", rx_buffer[0], rx_buffer[1]);

    // write to TRIGGER 
    ioctl(fd, I2C_RDWR, & msgset[4]);
    sleep(1);

    while (1) {
        // Initiate a combined I2C transaction to obtain adc values
        ioctl(fd, I2C_RDWR, & msgset[0]);
        unsigned short adc_raw_value = (rx_buffer[0] << 8) | (rx_buffer[1]);

        // ADC is 12 bit, with Vref = 2.51 V and DAC is 10 bit with Vref = 3.3 V
        // ADCVout = ADC_DATA * 2.51 / 4096, DACVout = DAC_DATA * 3.3 / 1024
        // 12 bit alignment
        unsigned short dac_raw_value = ((adc_raw_value / 4) * (2.51 / 3.3)) << 2;
        dac_data[0] = (unsigned char)((dac_raw_value >> 8) & 0xFF); // MSB
        dac_data[1] = (unsigned char)(dac_raw_value & 0xFF); // LSB

        // Write DAC raw data to DAC_DATA register
        ioctl(fd, I2C_RDWR, & msgset[5]);

    }

    close(fd);
    return 0;

}