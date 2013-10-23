/* SPI HEADER
 * Based off of: http://www.raspberry-projects.com/pi/programming-in-c/spi/using-the-spi-interface
 */

#ifndef SPI_CONTROL
#define SPI_CONTROL

#include <fcntl.h>
#include <unistd.h>
#include "sys/ioctl.h"
#include "linux/spi/spidev.h"
#include "RPi_generror.h"

int spi_cs0_fd;
int spi_cs1_fd;
unsigned char spi_mode;
unsigned char spi_bitsPerWord;
unsigned int spi_speed;

/*************************************************/
/****** OPEN SPI PORT FUNCTION *******************/
/*************************************************/
// for device, 0 = cs0 and 1 = cs1

int SPIc_open (unsigned char device, unsigned int speed) {
	int status_value = -1;
	int *spi_cs_fd;
	
	//----- SET SPI MODE -----
    //SPI_MODE_0 (0,0) 	CPOL=0 (Clock Idle low level), CPHA=0 (SDO transmit/change edge active to idle)
    //SPI_MODE_1 (0,1) 	CPOL=0 (Clock Idle low level), CPHA=1 (SDO transmit/change edge idle to active)
    //SPI_MODE_2 (1,0) 	CPOL=1 (Clock Idle high level), CPHA=0 (SDO transmit/change edge active to idle)
    //SPI_MODE_3 (1,1) 	CPOL=1 (Clock Idle high level), CPHA=1 (SDO transmit/change edge idle to active)
    spi_mode = SPI_MODE_0;
    spi_bitsPerWord = 8;
    
    if (!speed) spi_speed = 500000;  //1MHZ = 1E6; speed can be 0.5, 1, 2, 4, 8, 16, 32
    else spi_speed = speed*1000000;
    
    if (!device) spi_cs_fd = &spi_cs0_fd;
	else spi_cs_fd = &spi_cs1_fd;
	
	if (!device) *spi_cs_fd = open("/dev/spidev0.0", O_RDWR);
	else *spi_cs_fd = open("/dev/spidev0.1", O_RDWR);
	if (*spi_cs_fd < 0)	ge_halt(1, "SPI_control.h", "Could not open SPi device");
	
	status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MODE, &spi_mode);
	if (status_value < 0) ge_halt(1, "SPI_control.h, ioctl.h", "Could not set SPI Mode (WR)");
	status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MODE, &spi_mode);
	if (status_value < 0) ge_halt(1, "SPI_control.h, ioctl.h", "Could not set SPI Mode (RD)");
	status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
	if (status_value < 0) ge_halt(1, "SPI_control.h, ioctl.h", "Could not set SPI bits per word (WR)");
	status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
	if (status_value < 0) ge_halt(1, "SPI_control.h, ioctl.h", "Could not set SPI bits per word (RD)");
	status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
	if (status_value < 0) ge_halt(1, "SPI_control.h, ioctl.h", "Could not set SPI speed (WR)");
	status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
	if (status_value < 0) ge_halt(1, "SPI_control.h, ioctl.h", "Could not set SPI speed (RD)");
	return(status_value);
}

/*************************************************/
/****** CLOSE SPI PORT FUNCTION ******************/
/*************************************************/	
int SPIc_close (unsigned char device) {
	int status_value = -1;
	int *spi_cs_fd;
	
	if(!device) spi_cs_fd = &spi_cs0_fd;
	else spi_cs_fd = &spi_cs1_fd;
	
	status_value = close(*spi_cs_fd);
	if (status_value < 0) ge_halt(2, "SPI_control.h", "Could not close SPI device!");
	
	return(status_value);
}

/*************************************************/
/****** SPI WRITE AND READ DATA ******************/
/*************************************************/
// data = byes to write. Contents overwritten with bytes read
int SPIc_rdwr (int device, unsigned char *data, int length) {
	struct spi_ioc_transfer spi[length];
	int i = 0;
	int status_value = -1;
	int *spi_cs_fd;
	
	if (!device) spi_cs_fd = &spi_cs0_fd;
	else spi_cs_fd = &spi_cs1_fd;
	
	//one spi transfer per byte
	for (i = 0; i < length; i++) {
		spi[i].tx_buf		= (unsigned long)(data + i); //transmit from "data"
		spi[i].rx_buf		= (unsigned long)(data + i); //recieve into "data"
		spi[i].len			= sizeof(*(data + i));
		spi[i].delay_usecs	= 0;
		spi[i].speed_hz		= spi_speed;
		spi[i].bits_per_word= spi_bitsPerWord;
		spi[i].cs_change	= 0;
	}
	
	status_value = ioctl(*spi_cs_fd, SPI_IOC_MESSAGE(length), &spi);
	if (status_value < 0) ge_halt(1, "SPI_control.h, ioctl.h", "Read/Write Data to SPI failed!");
	
	return(status_value);
}

#endif /*SPI_CONTROL*/
