/* SPI HEADER
 * Based off of: http://www.raspberry-projects.com/pi/programming-in-c/spi/using-the-spi-interface
 * This file contains a number of functions to interact with the SPI bus and any device attached.
 * The RPi has only two SPI channels.
 * Contains the following functions:
 *  SPIc_open: Opens the SPI port
 *  SPIc_close: Closes the SPI port
 *  SPIc_rdwr: simultaneously sends and recieves data from the SPI device.
 * 
 * Version 0.1
 */

#ifndef SPI_CONTROL
#define SPI_CONTROL

int spi_cs0_fd;
int spi_cs1_fd;
unsigned char spi_mode;
unsigned char spi_bitsPerWord;
unsigned int spi_speed;

/*************************************************/
/****** OPEN SPI PORT FUNCTION *******************/
/*************************************************/
// for device, 0 = cs0 and 1 = cs1
int SPIc_open (unsigned char device, unsigned int speed);

/*************************************************/
/****** CLOSE SPI PORT FUNCTION ******************/
/*************************************************/	
int SPIc_close (unsigned char device);

/*************************************************/
/****** SPI WRITE AND READ DATA ******************/
/*************************************************/
// data = byes to write. Contents overwritten with bytes read
int SPIc_rdwr (int device, unsigned char *data, int length);

#endif /*SPI_CONTROL*/
