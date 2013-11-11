/* General Proceedures for MAIN
 * General functions, macros etc.
 * Theory for thermistors: http://www.daycounter.com/Calculators/Steinhart-Hart-Thermistor-Calculator.phtml and http://www.maximintegrated.com/app-notes/index.mvp/id/1753 and http://www.paulschow.com/2013/08/monitoring-temperatures-using-raspberry.html
 * Theory for capacitance: https://en.wikipedia.org/wiki/RC_time_constant
 * Theory for GPS Error: http://www.trakgps.com/en/index.php/information/gps-articles-information/65-gps-accuracy
 */

//INCLUDE BLOCK 
#ifndef RPi_GENERALHEADER
#define RPi_GENERALHEADER

//INCLUDES
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "RPi_generror.h"
#include "RPi_USBGPS.h"
#include "GPIO.h"
#include "MCP3008.h"
#include "NAU7802.h"

//DEFINES
#define REF_VOLTAGE 2.6
#define NAU7802_REFVOLTS 3.3
#define THERMISTORS_GPIOPIN 25
#define RESET_PIN 24

#define THERMISTOR1_CH 0
#define THERMISTOR1_R1 10000
#define THERMISTOR2_CH 1
#define THERMISTOR2_R1 200
#define MQ7_1CH 2
#define MQ7_1R1 33000
#define MQ7_1Ro 10000

#define GPSBAUD 4800

#define HUMISTOR_CH 2
#define HUMISTOR_R1 6200000
#define HUMISTOR_SLPSEC 0
#define HUMISTOR_SLPNSC 2000000
#define HUMISTOR_CRCNSC 0.935946 //calibrated to a 270pf capacitor

#define MCP3008_SPD 10 //

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
	unsigned int ADC;
};

struct GasSensor {
	float PPM;
	float resistance;
	float voltage;
	float ratio;
	unsigned int ADC;
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

void RPi_ADCread_sensors(struct Thermistor *thermistor1, struct Thermistor *thermistor2, struct Thermistor *thermistor3,
							struct GasSensor *MQ7_1, struct Humistor *humistor1) {	
	int Ref_Voltage;
	/************************************************************/
	/******** READ DATA FROM SENSORS ****************************/
	/************************************************************/
	SENSORS_ON
	//get measurements of temperature	
	(*thermistor1).ADC = MCP3008_SingleEndedRead(THERMISTOR1_CH);
	(*thermistor2).ADC = MCP3008_SingleEndedRead(THERMISTOR2_CH);
	(*MQ7_1).ADC = MCP3008_SingleEndedRead(MQ7_1CH);
	//(*thermistor3).ADC = NAU7802_ReadADCTMP();
	//Ref_Voltage = NAU7802_ReadADC();
	SENSORS_OFF
	
	/************************************************************/
	/************** PROCESS DATA FROM THERMISTORS ***************/
	/************************************************************/
	(*thermistor1).voltage = ((*thermistor1).ADC*REF_VOLTAGE)/1024;
	(*thermistor1).resistance = ((1024.0/(*thermistor1).ADC - 1.0)*THERMISTOR1_R1);
	(*thermistor1).ratio = 10000 / (*thermistor1).resistance;
	printf("|| THERM1: %.2fV %.2fR %.2f %d\n", (*thermistor1).voltage, (*thermistor1).resistance, 1/(*thermistor1).ratio, (*thermistor1).ADC);
	(*thermistor1).temperature = ( 298.15 * 4300 / log( (*thermistor1).ratio ) ) / ( 4300 / log( (*thermistor1).ratio ) - 298.15 ) - 273.15;
	
	(*thermistor2).voltage = ((*thermistor2).ADC*REF_VOLTAGE)/1024;
	(*thermistor2).resistance = ((1024.0/(*thermistor2).ADC - 1.0)*THERMISTOR2_R1);
	(*thermistor2).ratio = 100 / (*thermistor2).resistance;
	printf("|| THERM2: %.2fV %.2fR %.2f %d\n", (*thermistor2).voltage, (*thermistor2).resistance, 1/(*thermistor2).ratio, (*thermistor2).ADC);
	(*thermistor2).temperature = ( 298.15 * 3200 / log( (*thermistor2).ratio ) ) / ( 3200 / log( (*thermistor2).ratio ) - 298.15 ) - 273.15;
	
	(*thermistor3).voltage = ((*thermistor3).ADC*NAU7802_REFVOLTS)/16777.216;
	(*thermistor3).resistance = 0;
	(*thermistor3).ratio = 1;
	printf("|| THERM2: %.2fV %.2fR %.2f %d\n", (*thermistor3).voltage, (*thermistor3).resistance, 1/(*thermistor3).ratio, (*thermistor3).ADC);
	(*thermistor3).temperature = 0; //(((*thermistor3).voltage - 109 )/ 0.360) + 25;
	
	(*MQ7_1).voltage = ((*MQ7_1).ADC*REF_VOLTAGE)/1024;
	(*MQ7_1).resistance = ((1024.0/(*MQ7_1).ADC - 1.0)*MQ7_1R1);
	(*MQ7_1).ratio = (*MQ7_1).resistance / MQ7_1Ro;
	printf("|| MQ7: %.2fV %.2fR %.2f %d\n", (*MQ7_1).voltage, (*MQ7_1).resistance, 1/(*MQ7_1).ratio, (*MQ7_1).ADC);
	//equation estimated to be LOG(PPM) = -1.43 * log(RS/RO) + 1.94
	//gives y = 10^(-1.43 * log(RS/RO) + 1.94)
	(*MQ7_1).PPM = pow(10, (-1.43 * log10f((*MQ7_1).ratio) + 1.94));
}

void RPi_generaltest() {
	SENSORS_ON
	SENSORS_OFF
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
	int status;
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
	
	printf("I2C Bus... ");
	NAU7802_OPEN();
	printf("Success!\n");
	
	printf("NAU6702... ");
	printf("Calibration returned code 0x%02x... OK!\n", NAU7802_PowerUp());
	
	printf("GPIO%d... ", THERMISTORS_GPIOPIN);
	GPIOexport(THERMISTORS_GPIOPIN); //enable GPIO pin for turning the sensors on and off
	GPIOdirection(THERMISTORS_GPIOPIN, OUT);
	printf("Success!\n");
	
	/*
	printf("GPIO%d... ", RESET_PIN);
	GPIOexport(RESET_PIN);
	GPIOdirection(RESET_PIN, OUT);
	printf("Success!\n");
	*/
	printf("\nSetup complete!\n\n");
}

//Things that the RPi must unload at the end
void RPi_destruct() {
	printf("Halting... ");
	
	printf("MCP3008... ");
	MCP3008_CLOSE(THERMISTOR1_CH);
	
	printf("NAU6702... ");
	printf("Returned code 0x%02x... OK!\n", NAU7802_PowerDown());
	
	printf("I2C Bus... ");
	NAU7802_CLOSE();
	
	printf("GPIO%d... ", THERMISTORS_GPIOPIN);
	GPIOunexport(THERMISTORS_GPIOPIN);
	/*
	printf("GPIO%d... ", RESET_PIN);
	GPIOunexport(RESET_PIN);
	*/
	printf("Done!\n");
}

#endif //end of include block
