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
 
int main() {
	int i = 20;
	int test;
	
	printf("Opening MCP3008 Device 0... ");
	MCP3008_OPEN(0,1);
	printf("Done!\n");
	
	do {
		test = MCP3008_SingleEndedRead(0);
		printf("OUTPUT: %d\n", test);
		sleep (1);
		i--;
	} while (1);
	 
	printf("Closing MCP3008 Device 0... ");
	MCP3008_CLOSE(0);
	printf("Done!\n");
	
	printf("Success!\n");
	exit(0);
}
 
