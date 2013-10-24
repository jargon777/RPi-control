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
#include "MCP3008.h"
#include "RPi_general.h"
 
int main() {	
	RPi_construct(); //general function for starting tasks
	
	RPi_generaltest();
	
	RPi_destruct(); //general function for ending tasks
	exit(0);
}
 
