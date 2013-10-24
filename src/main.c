/*
 * main.c 
 * 
 * Program contains main methods for RasPi operations. At the moment this is nothing but a testing ground.
 * 
 * Depends on: libjpeg8-dev, and an openVG api included in this set: git://github.com/ajstarks/openvg
 * Compile: gcc -Wall -I../h -I../lib/openvg -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/usr/include -c "%f" ../lib/openvg/libshapes.o ../lib/openvg/oglinit.o -L/opt/vc/lib -lGLESv2 -ljpeg
 * Build: gcc -Wall -I../h -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I../lib/openvg -I/usr/include -o "%e" "%f" ../lib/openvg/libshapes.o ../lib/openvg/oglinit.o -L/opt/vc/lib -lGLESv2 -ljpeg
 * 
 */
#include <stdlib.h>
#include "RPi_general.h"
float temp_therm1;
float volt_therm1;
float resi_therm1;

int main() {	
	RPi_construct(); //general function for starting tasks
	do {
		RPi_ADCread_tmphumid(&temp_therm1, &volt_therm1, &resi_therm1);
		printf("DATA: %f\n", temp_therm1);
		sleep(2);
	} while (!kbhit());
	
	RPi_destruct(); //general function for ending tasks
	exit(0);
}
 
