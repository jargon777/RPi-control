/*
 * main_console.c
 *
 * Program contains main methods for RasPi operations. Program strips the screen-drawing methods and outputs data only to console.
 *
 * General Compile: gcc -c -Wall -I../h -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I../lib/openvg -I/usr/include %f
 * Build: gcc -Wall -I../h -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I../lib/openvg -I/usr/include -o "%e" "%f" RPi_screen.o RPi_general.o GPIO.o SPI_control.o RPi_USBGPS.o RPi_generror.o ../lib/openvg/libshapes.o ../lib/openvg/oglinit.o -L/opt/vc/lib -lGLESv2 -ljpeg
 *
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "RPi_general.h"
#include "RPi_USBGPS.h"

#define GPS_USBID 0
struct Thermistor thermistor1, thermistor2, thermistor3;
struct GasSensor MQ7_1;
struct GasSensor MQ2_1;
struct Humistor humistor1;
struct GPSdata GPS_1;
struct ScreenData Screen_Main;
int record_count;

unsigned long long getMemoryAvailable() {
	unsigned long long av_pages = sysconf(_SC_AVPHYS_PAGES);
	unsigned long long page_size = sysconf(_SC_PAGE_SIZE);
	return av_pages * page_size;
}

unsigned long long getTotalMemoryAvailable() {
	unsigned long long av_pages = sysconf(_SC_PHYS_PAGES);
	unsigned long long page_size = sysconf(_SC_PAGE_SIZE);
	return av_pages * page_size;
}

int main() {
	RPi_construct(GPS_USBID); //general function for starting tasks
	char gen[3];
	do {
		printf("\nRECORD: %d\n", record_count);
		/**********************************************************************************/
		/**************** READ THE DATA ***************************************************/
		/**********************************************************************************/
		RPi_USBGPSread(GPS_USBID, &GPS_1); //obtain the GPS fix
		RPi_ADCread_sensors(&thermistor1, &thermistor2, &thermistor3, &MQ7_1, &MQ2_1, &humistor1);

		/**********************************************************************************/
		/**************** OUTPUT THE DATA TO THE CONSOLE **********************************/
		/**********************************************************************************/
		printf("T1: %.2fC   T2: %.2fC   T3: %.2fC   MQ7: %.2fPPM   MQ2: %.2fPPM \nMEM:%lluKB of %lluKB \n", thermistor1.temperature, thermistor2.temperature, thermistor3.temperature, MQ7_1.PPM, MQ2_1.PPM, (getMemoryAvailable()/1024), (getTotalMemoryAvailable()/1024));


		/**********************************************************************************/
		/**************** FINAL STEPS *****************************************************/
		/**********************************************************************************/
		RPi_writeToFile(&thermistor1, &thermistor2, &thermistor3, &MQ7_1, &MQ2_1, &humistor1, &GPS_1);
		//sleep(2);
		record_count++;

	} while (!kbhit());


	//fgets(gen, 2, stdin);                     // end with [RETURN]
	exit(0);
}

