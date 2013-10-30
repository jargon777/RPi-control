/* General Proceedures for MAIN
 * General functions, macros etc.
 * Theory for thermistors: http://www.daycounter.com/Calculators/Steinhart-Hart-Thermistor-Calculator.phtml and http://www.maximintegrated.com/app-notes/index.mvp/id/1753 and http://www.paulschow.com/2013/08/monitoring-temperatures-using-raspberry.html
 * Theory for capacitance: https://en.wikipedia.org/wiki/RC_time_constant
 * Theory for GPS Error: http://www.trakgps.com/en/index.php/information/gps-articles-information/65-gps-accuracy
 */

//INCLUDES
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
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

#define GPSBAUD 19200

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

struct GPSdata {
	short longitude_d;
	float longitude_m;
	char longitude_dir;
	short latitude_d;
	float latitude_m;
	char latitude_dir;
	char fix_quality;
	char num_sats;
	float HDOP_acc;
	float altitude_raw;
	char altitude_rawUNIT;
	float geoidal_sep; 
	char geoidal_sepUNIT;
	float actual_alt;
};

void RPi_ADCread_sensors(struct Thermistor *thermistor1, struct Thermistor *thermistor2, struct Humistor *humistor1) {	
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

unsigned char RPi_USBGPSset_time(char USBdev){
	char path[30];
	char gpsbuffer[1024] = {0};
	char cmd[30];
	char time[9];
	char date[9];
	char *toks = NULL;
	
	snprintf(path, 30, "/dev/ttyUSB%d", USBdev);
	FILE *fstream = fopen(path, "r");
	while(!(fgets(gpsbuffer, 1024, fstream) == NULL)) {
		printf(".");
		if (gpsbuffer[0] == '$') {
			if (memcmp(gpsbuffer+1, "GPRMC", 5) == 0) break;
		}
	}
	toks = strtok(gpsbuffer, ","); //tokenize the string. First token is the GPS flag
	toks = strtok(NULL, ","); //next token is Zulu time. Process it!
	if (toks != NULL) {
		time[0] = toks[0];
		time[1] = toks[1];
		time[2] = ':';
		time[3] = toks[2];
		time[4] = toks[3];
		time[5] = ':';
		time[6] = toks[4];
		time[7] = toks[5];
		time[8] = '\0';
	}
	else return 0;
	
	toks = strtok(NULL, ","); //next token is vakuduty
	toks = strtok(NULL, ","); //next token is latitude may be of use later for first fix
	toks = strtok(NULL, ","); //next token is N/S
	toks = strtok(NULL, ","); //next token is longitude
	toks = strtok(NULL, ","); //next token is E/W
	toks = strtok(NULL, ","); //next token is knots
	toks = strtok(NULL, ","); //next token is course
	toks = strtok(NULL, ","); //next token is date
	if (toks != NULL) {
		date[0] = '2';
		date[1] = '0';
		date[2] = toks[4];
		date[3] = toks[5];
		date[4] = toks[2];
		date[5] = toks[3];
		date[6] = toks[0];
		date[7] = toks[1];
		date[8] = '\0';
	}
	else return 0;
	printf("\n==============SET==============\nDATE:\n");
	sprintf(cmd, "date +%%Y%%m%%d -s \"%s\"", date);
	system(cmd);
	printf("TIME:\n");
	sprintf(cmd, "date +%%H:%%M:%%S -s \"%s\"", time);
	system(cmd);
	printf("===============================\n");
	fclose(fstream);
	return 1;
}

void RPi_USBGPSread(char USBdev, struct GPSdata *GPS_dev) {
	char path[30];
	char gpsbuffer[1024] = {0};
	char *toks = NULL;
	
	snprintf(path, 30, "/dev/ttyUSB%d", USBdev);
	FILE *fstream = fopen(path, "r");
	
	if (fstream == NULL) {
		ge_warn(1, "GPIO.h", "Unable to open GPS port for reading! Retrying...");
		return;
	}
	
	while(!(fgets(gpsbuffer, 1024, fstream) == NULL)) {
		if (gpsbuffer[0] == '$') {
			if (memcmp(gpsbuffer+1, "GPGGA", 5) == 0) break;
		}
	}
	toks = strtok(gpsbuffer, ","); //tokenize the string. First token is the GPS flag
	toks = strtok(NULL, ","); //next token is time
	toks = strtok(NULL, ","); //next token is latitude
	if (toks != NULL) {
		(*GPS_dev).latitude_d = (int)(atoi(toks) / 100); //truncate the minutes off the degrees
		(*GPS_dev).latitude_m = (atof(toks) - (*GPS_dev).latitude_d * 100); //remove the degrees off the minutes
	}
	toks = strtok(NULL, ","); //next token is latitude direction char
	if (toks != NULL) (*GPS_dev).latitude_dir = toks[0];
	toks = strtok(NULL, ","); //next token is longitude
	if (toks != NULL) {
		(*GPS_dev).longitude_d = (int)(atoi(toks) / 100); //truncate the minutes off the degrees
		(*GPS_dev).longitude_m = (atof(toks) - (*GPS_dev).longitude_d * 100); //remove the degrees off the minutes
	}
	toks = strtok(NULL, ","); //next token is longitude direction char
	if (toks != NULL) (*GPS_dev).longitude_dir = toks[0];
	toks = strtok(NULL, ","); //next token is quality
	if (toks != NULL) (*GPS_dev).fix_quality = atoi(toks);
	toks = strtok(NULL, ","); //next token is number of satalites
	if (toks != NULL) (*GPS_dev).num_sats = atoi(toks);
	toks = strtok(NULL, ","); //next token is HDOP
	if (toks != NULL) (*GPS_dev).HDOP_acc = atoi(toks);
	toks = strtok(NULL, ","); //next token is Raw Altitude
	if (toks != NULL) (*GPS_dev).altitude_raw = atof(toks);
	toks = strtok(NULL, ","); //next token is Raw Altitude unit
	if (toks != NULL) (*GPS_dev).altitude_rawUNIT = toks[0];
	toks = strtok(NULL, ","); //next token is Geoidal Separation
	if (toks != NULL) (*GPS_dev).geoidal_sep = atof(toks);
	toks = strtok(NULL, ","); //next token is Geoidal separation unit
	if (toks != NULL) (*GPS_dev).geoidal_sepUNIT = toks[0];
	
	(*GPS_dev).actual_alt = ((*GPS_dev).altitude_raw + (*GPS_dev).geoidal_sep);
	printf("|| GPS1: LT:%dd%.2f'%c LG:%dd%.2f'%c AT:%.2f%c SAT:%d QUA:%d HDOP:%.2f\n", (*GPS_dev).latitude_d, (*GPS_dev).latitude_m, 
	(*GPS_dev).latitude_dir, (*GPS_dev).longitude_d, (*GPS_dev).longitude_m, (*GPS_dev).longitude_dir, (*GPS_dev).actual_alt, 
	(*GPS_dev).altitude_rawUNIT, (*GPS_dev).num_sats, (*GPS_dev).fix_quality, (*GPS_dev).HDOP_acc);
	fclose(fstream);
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

//Things that the RPi must load at the start
void RPi_construct(char USBdev) {
	char cmd[30];
	char status;
	char upath[30];
	FILE *fstream;
	printf("Intializing... \n");
	
	printf("USB on ttyUSB%d... ", USBdev);
	snprintf(upath, 30, "/dev/ttyUSB%d", USBdev);
	do {
		fstream = fopen(upath, "r");
		if (fstream == NULL) printf("Failed to access GPS on %s! Check GPS! \n", upath);
		else break;
		sleep(2);
	} while (fstream == NULL);
	sprintf(cmd, "stty -F /dev/ttyUSB%d %d sane \n", USBdev, GPSBAUD); //ensure that the gps is setup correctly.
	system(cmd);
	printf("Baud: %d... Output type: sane... Success!\n", GPSBAUD);
	
	printf("Waiting for GPS lock.");
	do {
		 status = (RPi_USBGPSset_time(USBdev));
	} while (status == 0);
	printf("Date and Time updated!\n\n");
	
	printf("MCP3008... ");
	MCP3008_OPEN(THERMISTOR1_CH,MCP3008_SPD);
	printf("Success!\n");
	
	printf("GPIO%d... ", THERMISTORS_GPIOPIN);
	GPIOexport(THERMISTORS_GPIOPIN); //enable GPIO pin for turning the sensors on and off
	GPIOdirection(THERMISTORS_GPIOPIN, OUT);
	printf("Success!\n");
	
	printf("GPIO%d... ", RESET_PIN);
	GPIOexport(RESET_PIN);
	GPIOdirection(RESET_PIN, OUT);
	printf("Success!\n");
	
	printf("\nSetup complete!\n\n");
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
