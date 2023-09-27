#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>

// I2C address of DAC53401
#define DAC53401_I2C_ADDR	0x48
// Status register of DAC53401 to read DEVID
#define STATUS                  0xD0
// Margin high and low registers, necessary to use CWG mode
#define DAC_MARGIN_HIGH         0x25 
#define DAC_MARGIN_LOW          0x26
// General config register of DAC53401
#define GENERAL_CONFIG          0xD1
// Trigger register of DAC53401
#define TRIGGER                 0xD3 


int main()

{
        int fd;
        unsigned char regs[] = {STATUS, DAC_MARGIN_HIGH, DAC_MARGIN_LOW, GENERAL_CONFIG, TRIGGER, DAC_DATA};
        unsigned char write_mlow[] = {DAC_MARGIN_LOW, 0x00, 0x00};
        unsigned char write_mhigh[] = {DAC_MARGIN_HIGH, 0x0F, 0xFC};
        unsigned char write_gen[] = {GENERAL_CONFIG, 0x40, 0x00};  
        unsigned char write_trigger[] = {TRIGGER, 0x01, 0x10};
        unsigned char rx_buffer[3] = {};

        struct i2c_msg iomsgs[] = {
		
		[0] = {
		.addr = DAC53401_I2C_ADDR,	
		.flags = 0,	
		.buf = &regs[0],
		.len = 1	
	    },
		
		[1] = {
		.addr = DAC53401_I2C_ADDR,	
		.flags = I2C_M_RD,	
		.buf = rx_buffer,
		.len = 2
        },
               
		[2] = {
		.addr = DAC53401_I2C_ADDR,	
		.flags = 0,	
		.buf = write_gen,
		.len = 3
        },
                 [3] = {
		.addr = DAC53401_I2C_ADDR,	
		.flags = 0,	
		.buf = &regs[3],
		.len = 1
        },
               
		[4] = {
		.addr = DAC53401_I2C_ADDR,	
		.flags = I2C_M_RD,	
		.buf = rx_buffer,
		.len = 2
        },
                [5] = {
		.addr = DAC53401_I2C_ADDR,	
		.flags = 0,	
		.buf = write_trigger,
		.len = 3
        },
                 [6] = {
		.addr = DAC53401_I2C_ADDR,	
		.flags = 0,	
		.buf = write_mlow,
		.len = 3
        }, 	
		[7] = {
		.addr = DAC53401_I2C_ADDR,	
		.flags = 0,	
		.buf = write_mhigh,
		.len = 3
        }
                  
                
        };
   
       
	struct i2c_rdwr_ioctl_data msgset[] = {

		[0]={
			.msgs = &iomsgs[0],    // message set to get STATUS data
			.nmsgs = 2
		    },

                [1]={
			.msgs = &iomsgs[2],    // message set to write to GENERAL_CONFIG
			.nmsgs = 1
		    },

                [2]={
			.msgs = &iomsgs[3],    // message set to read GENERAL_CONFIG
			.nmsgs = 2
		    },
                 
                [3]={
			.msgs = &iomsgs[5],    // message set to write to TRIGGER
			.nmsgs = 1
		    },

                [4]={
			.msgs = &iomsgs[6],    // message set to write to MARGIN_LOW
			.nmsgs = 1
		    },

		[5]={
			.msgs = &iomsgs[7],    // message set to write to MARGIN_HIGH
			.nmsgs = 1
		    }
          
        };


        // Try to open I2C device
	fd = open("/dev/i2c-1", O_RDWR);
	
	// Check for any errors
	if (fd < 0)
	{
		printf("Error while trying to open i2c device.\n");
		return -1;
	}
	
	// read from status register
    ioctl(fd, I2C_RDWR, &msgset[0]);
    printf("Device ID is %02x %02x\n", rx_buffer[0], rx_buffer[1]);
	sleep(1);
	
	//write to gen conf
    ioctl(fd, I2C_RDWR, &msgset[1]);
	sleep(1);

	// read from GENERAL	
    ioctl(fd, I2C_RDWR, &msgset[2]);
	sleep(1);
    printf("Read from GENERAL_CONFIG %02x %02x\n", rx_buffer[0], rx_buffer[1]);

	// write to margin high and low
    ioctl(fd, I2C_RDWR, &msgset[4]);
	sleep(1);
	ioctl(fd, I2C_RDWR, &msgset[5]);
	sleep(1);
     
	//trigger
    ioctl(fd, I2C_RDWR, &msgset[3]);


	sleep(1);

    while(1) {}

    close(fd);
    return 0;

}