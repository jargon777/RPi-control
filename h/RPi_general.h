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
#define REF_VOLTAGE 3.3
#define THERMISTORS_GPIOPIN 25
#define RESET_PIN 24

#define THERMISTOR1_CH 0
#define THERMISTOR1_R1 10000
#define THERMISTOR2_CH 1
#define THERMISTOR2_R1 200

#define HUMISTOR_CH 2
#define HUMISTOR_R1 6200000
#define HUMISTOR_SLPSEC 0
#define HUMISTOR_SLPNSC 2000000
#define HUMISTOR_CRCNSC 0.935946 //calibrated to a 270pf capacitor

#define MCP3008_SPD 1

//MACROS
#define SENSORS_ON GPIOwrite(THERMISTORS_GPIOPIN, ON);
#define SENSORS_OFF GPIOwrite(THERMISTORS_GPIOPIN, OFF);
#define RESET_CAPACITORS GPIOwrite(RESET_PIN, ON); usleep(100000); GPIOwrite(RESET_PIN, OFF); usleep(100000);

//structs
struct Thermistor {
	float temperature;
	float resistance;
	float voltage;
	float ratio;
	unsigned short ADC;
};

struct Humistor {
	float voltage_initial;
	float voltage;
	float capacitance_uf;
	float capacitance_pf;
	float humidex;
	unsigned short ADC;
	unsigned short ADC_initial;
};

//Things that the RPi must load at the start
void RPi_construct() {
	printf("Intializing... ");
	
	MCP3008_OPEN(THERMISTOR1_CH,MCP3008_SPD);
	printf("MCP3008... ");
	
	GPIOexport(THERMISTORS_GPIOPIN); //enable GPIO pin for turning the sensors on and off
	GPIOdirection(THERMISTORS_GPIOPIN, OUT);
	printf("GPIO%d... ", THERMISTORS_GPIOPIN);
	
	GPIOexport(RESET_PIN);
	GPIOdirection(RESET_PIN, OUT);
	printf("GPIO%d... ", RESET_PIN);
	
	
	printf("Done!\n");
}

//Things that the RPi must unload at the end
void RPi_destruct() {
	printf("Halting... ");
	
	MCP3008_CLOSE(THERMISTOR1_CH);
	printf("MCP3008... ");
	
	GPIOunexport(THERMISTORS_GPIOPIN);
	printf("GPIO%d... ", THERMISTORS_GPIOPIN);
	
	GPIOunexport(RESET_PIN);
	printf("GPIO%d... ", RESET_PIN);
	
	printf("Done!\n");
}

void RPi_ADCread_tmphumid(struct Thermistor *thermistor1, struct Thermistor *thermistor2, struct Humistor *humistor1) {	
	/************************************************************/
	/******** READ DATA FROM SENSORS ****************************/
	/************************************************************/
	SENSORS_ON
	//get measurements of temperature	
	(*thermistor1).ADC = MCP3008_SingleEndedRead(THERMISTOR1_CH);
	(*thermistor2).ADC = MCP3008_SingleEndedRead(THERMISTOR2_CH);
	SENSORS_OFF
	
	/************************************************************/
	/************** PROCESS DATA FROM THERMISTORS ***************/
	/************************************************************/
	(*thermistor1).voltage = ((*thermistor1).ADC*REF_VOLTAGE)/1024;
	(*thermistor1).resistance = ((1024.0/(*thermistor1).ADC - 1.0)*THERMISTOR1_R1);
	(*thermistor1).ratio = 10000 / (*thermistor1).resistance;
	printf("|| THERM1: %.2fV %.2fR %.2f %d\n", (*thermistor1).voltage, (*thermistor1).resistance, 1/(*thermistor1).ratio, (*thermistor1).ADC);
	(*thermistor1).temperature = ( 298.15 * 4300 / log( (*thermistor1).ratio ) ) / ( 4300 / log( (*thermistor1).ratio ) - 298.15 ) - 273.15;
	
	(*thermistor2).voltage = ((*thermistor2).ADC*3.3)/1024;
	(*thermistor2).resistance = ((1024.0/(*thermistor2).ADC - 1.0)*THERMISTOR2_R1);
	(*thermistor2).ratio = 100 / (*thermistor2).resistance;
	printf("|| THERM2: %.2fV %.2fR %.2f %d\n", (*thermistor2).voltage, (*thermistor2).resistance, 1/(*thermistor2).ratio, (*thermistor2).ADC);
	(*thermistor2).temperature = ( 298.15 * 3200 / log( (*thermistor2).ratio ) ) / ( 3200 / log( (*thermistor2).ratio ) - 298.15 ) - 273.15;
	
}

void RPi_deprecated_humistor(struct Humistor *humistor1) {
	struct timespec sleep_time;
		sleep_time.tv_sec = HUMISTOR_SLPSEC;
		sleep_time.tv_nsec = HUMISTOR_SLPNSC * HUMISTOR_CRCNSC;  //APPLY A CORRECTION FACTOR TO COMPENSATE FOR RASPI TIME LOSSES
	printf("%lu", sleep_time.tv_nsec);
	/*
struct Humistor {
	struct timespec start_meas;
	struct timespec end_meas;
	unsigned long elapsed_us;
	float voltage_initial;
	float voltage;
	float capacitance_uf;
	float humidex;
	unsigned short ADC;
	unsigned short ADC_initial;
	unsigned long parses;
};
	unsigned long t_esecs;
	unsigned long t_ensec;
	struct timespec sleep_time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 1000000;
	(*humistor1).parses = 0; //set the parses to zero
	double capacitance_alt;
	double capacitance_alt2;
	
	clock_gettime(CLOCK_MONOTONIC, &((*humistor1).start_meas)); //record the current time, for humistor
	(*humistor1).ADC_initial = MCP3008_SingleEndedRead(HUMISTOR_CH); //store the initial voltage at humistor (should be zero)
	do {
		(*humistor1).ADC = MCP3008_SingleEndedRead(HUMISTOR_CH);
		(*humistor1).voltage = ((*humistor1).ADC*REF_VOLTAGE)/1024;
		clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, NULL);
		//printf("%f \n", (*humistor1).voltage);
		(*humistor1).parses++;
	} while ((*humistor1).voltage < (3.3*(1 - exp(-1))));
	clock_gettime(CLOCK_MONOTONIC, &((*humistor1).end_meas)); //record the current time.
	
	************************************************************
	************ PROCESS DATA FROM HUMISTOR ********************
	************************************************************
	(*humistor1).voltage_initial = ((*humistor1).ADC_initial*REF_VOLTAGE)/1024;
	t_esecs = ((*humistor1).end_meas.tv_sec - (*humistor1).start_meas.tv_sec);
	t_ensec = ((*humistor1).end_meas.tv_nsec - (*humistor1).start_meas.tv_nsec);
	(*humistor1).elapsed_us = ((t_esecs * 1000000) + (t_ensec / 1000) + 0.5);
	(*humistor1).capacitance_uf = (((*humistor1).elapsed_us / (float)HUMISTOR_R1) - //must scale capacitance to compensate for sampling losses
		(0)); // quick sampling consumes power, use of high ohm resistor makes this matter
	printf("%ld \n", (*humistor1).parses);
	capacitance_alt = (*humistor1).capacitance_uf * exp(-(((*humistor1).parses) / (1 + (*humistor1).parses + ((sleep_time.tv_nsec) / ((*humistor1).parses)))));
	capacitance_alt2 = (*humistor1).capacitance_uf - (*humistor1).capacitance_uf * (((*humistor1).parses) / ((*humistor1).parses + (1000000000/sleep_time.tv_nsec) * pow((sleep_time.tv_nsec / (*humistor1).parses), 0.36737)));
	printf("|| HUMIS1: %.2fVi %fVf %.2fVt %ldus %.2fuf %.2lfuf1 %.2lfuf2 %dADC\n", (*humistor1).voltage_initial, (*humistor1).voltage, 
		(3.3*(1 - exp(-1))), (*humistor1).elapsed_us, (*humistor1).capacitance_uf, capacitance_alt, capacitance_alt2, (*humistor1).ADC);
	
	/
	(*humistor1).start_meas = (*humistor1).end_meas;
	clock_gettime(CLOCK_MONOTONIC, &((*humistor1).end_meas));
	printf("%u.%u diff: %u\n", (unsigned int)(*humistor1).end_meas.tv_sec, (unsigned int)(*humistor1).end_meas.tv_nsec, ((unsigned int)(*humistor1).end_meas.tv_nsec-(unsigned int)(*humistor1).start_meas.tv_nsec));
	*/	
	
	/* MORE DEPRECATED CODE:
	(*humistor1).ADC_initial = MCP3008_SingleEndedRead(HUMISTOR_CH); //store the initial voltage at humistor (should be zero)
	clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, NULL);
	(*humistor1).ADC = MCP3008_SingleEndedRead(HUMISTOR_CH);
	
	//calcs the data
	(*humistor1).voltage_initial = ((*humistor1).ADC_initial*REF_VOLTAGE)/1024;
	(*humistor1).voltage = ((*humistor1).ADC*REF_VOLTAGE)/1024;
	//(*humistor1).capacitance_uf = (sleep_time.tv_sec * 1000000 + sleep_time.tv_nsec/1000) / 
	//	(-log((REF_VOLTAGE - ((*humistor1).voltage)) / REF_VOLTAGE) * (float)HUMISTOR_R1);
	(*humistor1).capacitance_pf = (sleep_time.tv_nsec*1000) / 
		(-log((REF_VOLTAGE - ((*humistor1).voltage)) / REF_VOLTAGE) * (float)HUMISTOR_R1);
	(*humistor1).humidex = ((*humistor1).capacitance_pf - 298) / 0.6;
	printf("|| HUMIS1: %.2fVi %.2fVf %.1fpf %dADCi %dADC\n", (*humistor1).voltage_initial, (*humistor1).voltage, 
		(*humistor1).capacitance_pf, (*humistor1).ADC_initial, (*humistor1).ADC); 
	*/
}

void RPi_generaltest() {
	
	sleep(1);
	
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
