/*
 * main.c 
 * 
 * Program contains main methods for RasPi operations. At the moment this is nothing but a testing ground.
 * 
 * Depends on: libjpeg8-dev, and an openVG api included in this set: git://github.com/ajstarks/openvg
 * Compile: gcc -Wall -I../h -I../lib/openvg -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -c "%f" ../lib/openvg/libshapes.o ../lib/openvg/oglinit.o -L/opt/vc/lib -lGLESv2 -ljpeg
 * Build: gcc -Wall -I../h -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I../lib/openvg -o "%e" "%f" ../lib/openvg/libshapes.o ../lib/openvg/oglinit.o -L/opt/vc/lib -lGLESv2 -ljpeg
 * 
 */

// first OpenVG program 
// Anthony Starks (ajstarks@gmail.com)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

int main() {
    int width, height;
    char s[3];

    init(&width, &height);                  // Graphics initialization

    Start(width, height);                   // Start the picture
    Background(0, 0, 0);                    // Black background
    Fill(44, 77, 232, 1);                   // Big blue marble
    Circle(width / 2, 0, width);            // The "world"
    Fill(255, 255, 255, 1);                 // White text
    TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);  // Greetings 
    End();                                  // End the picture

    fgets(s, 2, stdin);                     // look at the pic, end with [RETURN]
    finish();                               // Graphics cleanup
    exit(0);
}
