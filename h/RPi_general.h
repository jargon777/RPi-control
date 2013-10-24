/* General Proceedures for MAIN
 * General functions, macros etc.
 * Theory for thermistors: http://www.daycounter.com/Calculators/Steinhart-Hart-Thermistor-Calculator.phtml and http://www.maximintegrated.com/app-notes/index.mvp/id/1753 and http://www.paulschow.com/2013/08/monitoring-temperatures-using-raspberry.html
 * Theory for capacitance: https://en.wikipedia.org/wiki/RC_time_constant
 */

//INCLUDES
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "GPIO.h"
#include "MCP3008.h"

//DEFINES
#define THERMISTORS_GPIOPIN 25
#define HUMISTOR_GPIOPIN 24

#define THERMISTOR1_CH 0
#define THERMISTOR1_R1 5000
#define THERMISTOR2_CH 1
#define THERMISTOR2_R1 200

#define MCP3008_SPD 1

//MACROS
#define THERMISTORS_ON GPIOwrite(THERMISTORS_GPIOPIN, ON);
#define THERMISTORS_OFF GPIOwrite(THERMISTORS_GPIOPIN, OFF);
#define HUMISTOR_ON GPIOwrite(HUMISTOR_GPIOPIN, ON);
#define HUMISTOR_OFF GPIOwrite(HUMISTOR_GPIOPIN, OFF);

//structs
struct Thermistor {
	float temperature;
	float resistance;
	float voltage;
	float ratio;
	unsigned short ADC;
};

struct Humistor {
	struct timespec start_meas;
	struct timespec end_meas;
	float voltage;
	float capacitance;
	float humidex;
};

//Things that the RPi must load at the start
void RPi_construct() {
	printf("Intializing... ");
	
	MCP3008_OPEN(THERMISTOR1_CH,MCP3008_SPD);
	printf("MCP3008... ");
	
	GPIOexport(THERMISTORS_GPIOPIN); //enable GPIO pin for turning the sensors on and off
	GPIOdirection(THERMISTORS_GPIOPIN, OUT);
	printf("GPIO%d... ", THERMISTORS_GPIOPIN);
	GPIOexport(HUMISTOR_GPIOPIN);
	GPIOdirection(HUMISTOR_GPIOPIN, OUT);
	printf("GPIO%d... ", HUMISTOR_GPIOPIN);
	
	
	printf("Done!\n");
}

//Things that the RPi must unload at the end
void RPi_destruct() {
	printf("Halting... ");
	
	MCP3008_CLOSE(THERMISTOR1_CH);
	printf("MCP3008... ");
	
	GPIOunexport(THERMISTORS_GPIOPIN);
	printf("GPIO%d... ", THERMISTORS_GPIOPIN);
	GPIOunexport(HUMISTOR_GPIOPIN);
	printf("GPIO%d... ", HUMISTOR_GPIOPIN);
	
	printf("Done!\n");
}

void RPi_ADCread_tmphumid(struct Thermistor *thermistor1, struct Thermistor *thermistor2, struct Humistor *humistor1) {
	
	//get the data
	THERMISTORS_ON
	(*thermistor1).ADC = MCP3008_SingleEndedRead(THERMISTOR1_CH);
	(*thermistor2).ADC = MCP3008_SingleEndedRead(THERMISTOR2_CH);
	THERMISTORS_OFF
	
	//process the data: thermistor 1
	(*thermistor1).voltage = ((*thermistor1).ADC*3.3)/1024;
	(*thermistor1).resistance = ((1024.0/(*thermistor1).ADC - 1.0)*THERMISTOR1_R1);
	(*thermistor1).ratio = 10000 / (*thermistor1).resistance;
	printf("|| THERM1: %fV %fR %f %d\n", (*thermistor1).voltage, (*thermistor1).resistance, 1/(*thermistor1).ratio, (*thermistor1).ADC);
	(*thermistor1).temperature = ( 298.15 * 4300 / log( (*thermistor1).ratio ) ) / ( 4300 / log( (*thermistor1).ratio ) - 298.15 ) - 273.15;
	
	//process the data: thermistor 2
	(*thermistor2).voltage = ((*thermistor2).ADC*3.3)/1024;
	(*thermistor2).resistance = ((1024.0/(*thermistor2).ADC - 1.0)*THERMISTOR2_R1);
	(*thermistor2).ratio = 100 / (*thermistor2).resistance;
	printf("|| THERM2: %fV %fR %f %d\n", (*thermistor2).voltage, (*thermistor2).resistance, 1/(*thermistor2).ratio, (*thermistor2).ADC);
	(*thermistor2).temperature = ( 298.15 * 3200 / log( (*thermistor2).ratio ) ) / ( 3200 / log( (*thermistor2).ratio ) - 298.15 ) - 273.15;
	
	//read and process humidity sensor. To measure capacitance, must time.
	clock_gettime(CLOCK_MONOTONIC, &((*humistor1).start_meas)); //record the current time.
	HUMISTOR_ON
	sleep(2);
	HUMISTOR_OFF
	
	/*
	(*humistor1).start_meas = (*humistor1).end_meas;
	clock_gettime(CLOCK_MONOTONIC, &((*humistor1).end_meas));
	printf("%u.%u diff: %u\n", (unsigned int)(*humistor1).end_meas.tv_sec, (unsigned int)(*humistor1).end_meas.tv_nsec, ((unsigned int)(*humistor1).end_meas.tv_nsec-(unsigned int)(*humistor1).start_meas.tv_nsec));
	*/
}

void RPi_generaltest() {
	THERMISTORS_ON
	sleep(1);
	THERMISTORS_OFF
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
