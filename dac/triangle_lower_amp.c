#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <math.h>

// I2C address of DAC53401
#define DAC53401_I2C_ADDR       0x48

// Device ID status register
#define STATUS                  0xD0

// Addresses of the DAC53401 margin high and low registers
#define DAC_MARGIN_HIGH         0x25 
#define DAC_MARGIN_LOW          0x26

// Address of the DAC53401 general configuration register 
#define GENERAL_CONFIG          0xD1

// Address of the DAC53401 trigger register
#define TRIGGER                 0xD3 

/* The following arrays have more or less the same structure: first member is the address of the registry, 
   and the other two contain two eight bit values determined from the datasheet itself 
   The values themselves can be described with the following: 
        - write_mlow    - value which determines the lower bound of output signal  (0 V)
	- write_mhigh   - value which determines the higher bound of output signal (1.65 V)
	- write_gen     - value which enables the configuration of a triangular wave with the code step of 1 LSB and slew rate of 25.6 us x 1.5,
                        enables the extern reference from the supplied voltage of 3.3 V and the power up mode
        - write_trigger - enables CWG mode (mode where the DAC itself generates the wave on its output) by setting the START_FUNC_GEN bit,
			as well as writing to non-volatile memory by setting NVM_PROG
*/
int main()

{
        int fd;
        unsigned char regs[] = {STATUS, DAC_MARGIN_HIGH, DAC_MARGIN_LOW, GENERAL_CONFIG, TRIGGER, DAC_DATA};
        unsigned char write_mlow[] = {DAC_MARGIN_LOW, 0x00, 0x00}; 
        unsigned char write_mhigh[] = {DAC_MARGIN_HIGH, 0x08, 0x00}; 
        unsigned char write_gen[] = {GENERAL_CONFIG, 0x00, 0x40}; 
        unsigned char write_trigger[] = {TRIGGER, 0x01, 0x10}; 
        unsigned char rx_buffer[2];

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

	        [0] = {
			.msgs = &iomsgs[0],    // message set to read Device ID
			.nmsgs = 2
		    },

                [1] = {
			.msgs = &iomsgs[2],    // message set to write to GENERAL_CONFIG
			.nmsgs = 1
		    },

               [2] = {
			.msgs = &iomsgs[3],    // message set to read from GENERAL_CONFIG
			.nmsgs = 2
		    },
                 
                [3] = {
			.msgs = &iomsgs[5],    // message set to write to TRIGGER
			.nmsgs = 1
		    },

                [4] = {
			.msgs = &iomsgs[6],    // message set to write to MARGIN_LOW
			.nmsgs = 1
		    },

		[5] = {
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

	// Initiate a combined I2C transaction to obtain Device ID
        ioctl(fd, I2C_RDWR, &msgset[0]);
	// Print the obtained Device ID
        printf("Device ID is %02x %02x\n", rx_buffer[0], rx_buffer[1]);
	sleep(1);
	
	// Initiate a combined I2C transaction to write to GENERAL_CONFIG
      	ioctl(fd, I2C_RDWR, &msgset[1]);
	sleep(1);

	// Initiate a combined I2C transaction to read from GENERAL_CONFIG	
        ioctl(fd, I2C_RDWR, &msgset[2]);
        sleep(1);
        printf("Read from GENERAL_CONFIG %02x %02x\n", rx_buffer[0], rx_buffer[1]);

	// Initiate a combined I2C transaction to write to DAC_MARGIN_LOW and _HIGH
        ioctl(fd, I2C_RDWR, &msgset[4]);
	sleep(1);
	ioctl(fd, I2C_RDWR, &msgset[5]);
	sleep(1);
     
	// Initiate a combined I2C transaction to write to TRIGGER
        ioctl(fd, I2C_RDWR, &msgset[3]);
        sleep(1);

    while(1) {}

    close(fd);
    return 0;

}
