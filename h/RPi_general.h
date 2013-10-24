/* General Proceedures for MAIN
 * General functions, macros etc.
 */

//INCLUDES
#include <unistd.h>
#include "GPIO.h"
#include "MCP3008.h"

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

void RPi_ADCread_tmphumid(float *temperature_estimate, float *voltage, float *resistance) {
	static int data;
	float ratio;
	//get the data
	SENSORS_ON
	sleep(1);
	data = MCP3008_SingleEndedRead(THERMISTOR1_CH);
	SENSORS_OFF
	
	*voltage = (data*3.3)/1024;
	*resistance = ((1024.0/data - 1.0)*5000);
	ratio = *resistance/10000;
	printf("FUNCTION: %fV %fR %f %d \n", *voltage, *resistance, ratio, data);
	*temperature_estimate = 0;
}

void RPi_generaltest() {
	SENSORS_ON
	sleep(1);
	SENSORS_OFF
	sleep(1);
}

//keyboard hit detection function from http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}
