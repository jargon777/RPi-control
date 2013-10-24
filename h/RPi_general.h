/* General Proceedures for MAIN
 * General functions, macros etc.
 */

//INCLUDES
#include <unistd.h>
#include "GPIO.h"

//DEFINES
#define SENSOR_GPIOPIN 25
#define THERMISTOR1_CH 0
#define MCP3008_SPD 1

//MACROS
#define SENSORS_ON GPIOwrite(SENSOR_GPIOPIN, ON);
#define SENSORS_OFF GPIOwrite(SENSOR_GPIOPIN, OFF);

//Things that the RPi must load at the start
void RPi_construct() {
	printf("Initializing Sensors... ");
	
	MCP3008_OPEN(THERMISTOR1_CH,MCP3008_SPD);
	
	GPIOexport(SENSOR_GPIOPIN); //enable GPIO pin for turning the sensors on and off
	GPIOdirection(SENSOR_GPIOPIN, OUT);
	
	printf("Done!\n");
}

//Things that the RPi must unload at the end
void RPi_destruct() {
	printf("Halting Sensors... ");
	
	MCP3008_CLOSE(THERMISTOR1_CH);
	
	GPIOunexport(SENSOR_GPIOPIN);
	
	printf("Done!\n");
}

float * RPi_readtemp() {
	static float data[1];
	do {
		test = MCP3008_SingleEndedRead(0);
		printf("OUTPUT: %d\n", test);
		sleep (1);
	} while (1);
	return data;
}

void RPi_generaltest() {
	SENSORS_ON
	sleep(5);
	SENSORS_OFF
}
