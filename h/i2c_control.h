/* I2C HEADER
 * Based off of: http://bunniestudios.com/blog/images/infocast_i2c.c
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
#include <linux/i2c.h>
#include "RPi_generror.h"

int I2Cfile;
char *I2Cfile_loc = "/dev/i2c-1";

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

int I2Cregister_write(unsigned char address, unsigned char reg, unsigned char reg_write) {
	unsigned char Odata[2];
	struct i2c_rdwr_ioctl_data data_packet;
	struct i2c_msg data_message[1];
	
	data_message[0].addr = address;
	data_message[0].flags = 0;
	data_message[0].len = sizeof(Odata);
	data_message[0].buf = Odata;
	
	Odata[0] = reg; //what register to write to
	Odata[1] = reg_write; //what to write to the register
	
	data_packet.msgs = data_message; //store message for sending
	data_packet.nmsgs = 1;
	
	if (ioctl(I2Cfile, I2C_RDWR, &data_packet) < 0) { //send message
		ge_warn (1, "i2c_control.h", "Failed to write to I2C device.");
		return 0;
	}	
	return 1;
}

int I2Cregister_read(unsigned char address, unsigned char reg, unsigned char *Idata_array, unsigned char bytes_expected) {
	unsigned char Odata = 0;
	struct i2c_rdwr_ioctl_data data_packet;
	struct i2c_msg data_message[2];
	
	data_message[0].addr = address;
	data_message[0].flags = 0;
	data_message[0].len = sizeof(Odata);
	data_message[0].buf = &Odata;
	
	data_message[1].addr = address;
	data_message[1].flags = I2C_M_RD;
	data_message[1].len = bytes_expected;
	data_message[1].buf = Idata_array;
	
	data_packet.msgs = data_message;
	data_packet.nmsgs = 2;
	
	Odata = reg; //must write the register we want to read from.
	
	if (ioctl(I2Cfile, I2C_RDWR, &data_packet) < 0) { //send message
		ge_warn (1, "i2c_control.h", "Failed to read to I2C device.");
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
