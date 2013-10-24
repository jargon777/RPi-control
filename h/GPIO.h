/* GPIO Control
 * based on http://elinux.org/RPi_Low-level_peripherals#C_2
 */

#ifndef RPi_GPIO
#define RPi_GPIO

#include "RPi_generror.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IN 0
#define OUT 1
#define ON 1
#define OFF 0
#define MAX_BUFFER 3
#define DIRECTION_MAX 35
#define VALUE_MAX 30

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
	static const char s_values_str[] = "01";
	char path[VALUE_MAX];
	int afile;
	
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", GPIOpin);
	afile = open(path, O_WRONLY);
	if (afile < 0) ge_halt(1, "GPIO.h", "Unable to open GPIO for writing!");
	if (write(afile, &s_values_str[OFF == value ? 0 : 1], 1) < 0) ge_halt(1, "GPIO.h", "Unable to write GPIO!");
}

#endif //RPi_GPIO
