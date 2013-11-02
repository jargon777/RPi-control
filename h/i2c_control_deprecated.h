/* I2C HEADER
 * Based off of: http://www.raspberry-projects.com/pi/programming-in-c/spi/using-the-spi-interface
 * This file contains a number of functions to interact with the I2C bus and any device attached.
 * Contains the following functions:
 * 
 * Version 0.1
 */
 
#ifndef I2C_CONTROL
#define I2C_CONTROL

#include <fcntl.h>
#include <unistd.h>
#include "sys/ioctl.h"
#include <linux/i2c-dev.h>
#include "RPi_generror.h"

int I2Cfile;
char *I2Cfile_loc = "/dev/i2c-0";

int I2Cbus_open() {
	if ((I2Cfile = open(I2Cfile_loc, O_RDWR)) < 0) {
		ge_warn(1, "i2c_control.h", "Could not open I2C bus. Maybe bus already open?");
		return 0;
	}
	return 1;
}

void I2Cbus_close() {	
	if (close(I2Cfile) < 0) ge_halt(2, "i2c_control.h", "Could not close I2C bus!");
}

int I2Cbus_communicate(unsigned char chip_address) {
	unsigned char cmd = (((0b1111111 & chip_address) << 1) | 1); //address and bitshift 1 to add the "read/write" reqest.
	if (ioctl(I2Cfile, I2C_SLAVE_FORCE, cmd) < 0) { 
		char message[50];
		sprintf(message, "Could not open I2C on %x for reading. Bad address?", chip_address);
		ge_warn (1, "i2c_control.h", message);
		return 0;
	}
	return 1;
}

void I2Cbus_read(unsigned char *read_buffer, unsigned char bytes) {
	if (read(I2Cfile, read_buffer, bytes) != bytes) ge_warn (1, "i2c_control.h", "Failed to read from I2C device.");
}

unsigned char I2Cbus_write(unsigned char *arguments, unsigned char bytes) {	
	if (write(I2Cfile, arguments, bytes) != bytes) {
		ge_warn (1, "i2c_control.h", "Failed to write to I2C device.");
		return 0;
	}
	return 1;
}

#endif /*I2C Control*/
