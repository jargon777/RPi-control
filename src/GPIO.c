/* GPIO Control <SOURCE>
 * based on http://elinux.org/RPi_Low-level_peripherals#C_2
 * This header file contains commands to interact with the GPIO. The functions in this file
 * do the same basic tasks that a user can do from the command prompt to control the GPIO.
 * Contains the following functions:
 *  	GPIOexport: sets up the GPIO pin so that it can be used. 
 *  	GPIOunexport: closes the GPIO pin after use is done.
 *  	GPIOdirection: sets whether the pin is an in or out pin.
 *  	GPIOread: reads the status of the pin.
 *  	GPIOwrite: sends a high or low signal out the GPIO.
 * 
 * Version 0.1
 * GCC:
 */

#include "RPi_generror.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void GPIOexport(unsigned char GPIOpin) {
	char buffer[MAX_BUFFER];
	ssize_t bytes_written;
	int afile;
	
	afile = open("/sys/class/gpio/export", O_WRONLY);
	if (afile < 0) ge_halt(1, "GPIO.h", "Unable to open export for writing!");
	
	bytes_written = snprintf(buffer, MAX_BUFFER, "%d", GPIOpin);
	write(afile, buffer, bytes_written);
	close(afile);
}

void GPIOunexport(unsigned char GPIOpin) {
	char buffer[MAX_BUFFER];
	ssize_t bytes_written;
	int afile;
	
	afile = open("/sys/class/gpio/unexport", O_WRONLY);
	if (afile < 0) ge_halt(1, "GPIO.h", "Unable to open unexport for writing!");
	
	bytes_written = snprintf(buffer, MAX_BUFFER, "%d", GPIOpin);
	write(afile, buffer, bytes_written);
	close(afile);
}

void GPIOdirection(unsigned char GPIOpin, unsigned char dir) {
	static const char s_direction_str[] = "in\0out";
	char path[DIRECTION_MAX];
	int afile;
	
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", GPIOpin);
	afile = open(path, O_WRONLY);
	if (afile < 0) ge_halt(1, "GPIO.h", "Unable to open GPIO direction for writing!");
	if (write(afile, &s_direction_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3) < 0) ge_halt(1, "GPIO.h", "Unable to set GPIO direction!");
	close(afile);
}

void GPIOread(unsigned char GPIOpin) {
	char path[VALUE_MAX];
	char value_str[3];
	int afile;
	
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", GPIOpin);
	afile = open(path, O_RDONLY);
	if (afile < 0) ge_halt(1, "GPIO.h", "Unable to open GPIO for reading!");
	if (read(afile, value_str, 3) < 0) ge_halt(1, "GPIO.h", "Unable to read value!!");
	close(afile);
}

void GPIOwrite(unsigned char GPIOpin, unsigned char value) {
	unsigned char attempts = 0;
	static const char s_values_str[] = "01";
	char path[VALUE_MAX];
	int afile;
	
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", GPIOpin);
	afile = open(path, O_WRONLY);
	if (afile < 0) {
		ge_warn(1, "GPIO.h", "Unable to open GPIO for writing! Retrying...");
		usleep(50000); //temporarily suspend to hopefully mitigate error
		printf("Retrying on pin %u... \n", GPIOpin);
		for (attempts = 0; attempts <= 2; attempts++) {
			printf("%u... \n", attempts);
			snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", GPIOpin);
			afile = open(path, O_WRONLY);
			if (!(afile < 0)) break;
			else if (attempts >= 2) {	
				printf("Attempting to re-export pin %u...\n", GPIOpin);
				GPIOunexport(GPIOpin);
				usleep(50000); //temporarily suspend to hopefully mitigate error
				GPIOexport(GPIOpin);
				snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", GPIOpin);
				afile = open(path, O_WRONLY);
				if (!(afile < 0)) break;
				ge_halt(1, "GPIO.h", "Failed to set GPIO for writing! Halting.");
			}
			usleep(50000); //temporarily suspend to hopefully mitigate error
		}
		
	}
	if (write(afile, &s_values_str[OFF == value ? 0 : 1], 1) < 0) ge_halt(1, "GPIO.h", "Unable to write GPIO!");
	close(afile);
}
