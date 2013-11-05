RPi-control
===========
This program is designed to interface a Raspberry Pi with a number of sensors. Files used in this program are written in C.
Files can generally stand on their own, but all user-made files at least require the RPi_generror.h file to interact with error handling.
At the moment, this program is albe to read data from a few thermistors using an ADC.

main.c
-----------
Source file containing all main methods

GPIO.h
-----------
This header file contains commands to interact with the GPIO. The functions in this file do the same basic tasks that a user can do from the command prompt to control the GPIO.

SPI_control.h
-----------
Based off of: http://www.raspberry-projects.com/pi/programming-in-c/spi/using-the-spi-interface; this file contains a number of functions to interact with the SPI bus and any device attached.

SPI_control.h
-----------
Based off of: http://www.raspberry-projects.com/pi/programming-in-c/spi/using-the-spi-interface; this file contains a number of functions to interact with the I2C bus and any device attached.

RPi_general.h
-----------
Contains all user-defined functions to interact with sensors or other elements.
