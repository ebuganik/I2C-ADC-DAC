#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>

// I2C address of ADS7828
#define ADS7828_I2C_ADDR	0x48

// ADS7828 operating mode - command byte
// Single-ended input, Channel 0, VREF ON, ADC ON
#define COMMAND                 0x8F


int main()
{
	int fd;
	unsigned char command = COMMAND;
	unsigned char rx_buffer[2];

	struct i2c_msg iomsgs[] = {
		[0] = {
		.addr = ADS7828_I2C_ADDR,
		.flags = 0,
		.buf = &command,
		.len = 1
	},
		[1] = {
		.addr = ADS7828_I2C_ADDR,
		.flags = I2C_M_RD,
		.buf = rx_buffer,
		.len = 2
	}
	};

	struct i2c_rdwr_ioctl_data msgset = {
	.msgs = iomsgs,
	.nmsgs = 2
	};

	// Try to open I2C device
	fd = open("/dev/i2c-1", O_RDWR);
	
	// Check for any errors
	if (fd < 0)
	{
		printf("Error while trying to open i2c device.\n");
		return -1;
	}

	// Loop forever printing ADC values every second
	for (;;)
	{
		// TODO: Initiate a combined I2C transaction to obtain adc values
		ioctl(fd, I2C_RDWR, &msgset);

		// Print the obtained ADC values
		int adc_raw_value = (rx_buffer[0] << 8) | (rx_buffer[1]);
		float output_voltage_vref = (adc_raw_value * 2.51)/4095;
		printf("%.2f [V]\n", output_voltage_vref);
		sleep(1);
	}
	
	close(fd);
	
	return 0;

}
