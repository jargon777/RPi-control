/* Interactor for the NAU7802.h
 * layer ontp of the I2C_control file to interact specifically with the NAU7802.
 * 
 * Version 0.1
 */
 
#ifndef NAU7802_h
#define NAU7802_h

#include "i2c_control.h"
#include "RPi_generror.h"

#define NAU7802_ADDRESS 0x2a

#define NAU7802_OPEN() I2Cbus_open()
#define NAU7802_CLOSE() I2Cbus_close()

unsigned char NAU7802_PowerUp() {
	unsigned char data[1];
	I2Cregister_write(NAU7802_ADDRESS, 0x00, 0b00000001);
	usleep(200000);
	I2Cregister_write(NAU7802_ADDRESS, 0x00, 0b00000110);
	I2Cregister_write(NAU7802_ADDRESS, 0x02, 0b00000010);
	usleep(200000);
	I2Cregister_read(NAU7802_ADDRESS, 0x02, data, 1);
	return data[0];
}

unsigned char NAU7802_PowerDown() {
	unsigned char data[1];
	I2Cregister_write(NAU7802_ADDRESS, 0x00, 0x00);
	I2Cregister_read(NAU7802_ADDRESS, 0x00, data, 1);
	return data[0];
}

int NAU7802_ReadADC() {
	unsigned char data[3];
	int attempts;
	I2Cregister_write(NAU7802_ADDRESS, 0x11, 0b00000000); //set to voltages
	I2Cregister_write(NAU7802_ADDRESS, 0x00, 0b00010110);
	
	attempts = 0;
	do { //wait for data to be ready
		(I2Cregister_read(NAU7802_ADDRESS, 0x00, data, 1));
		usleep(500);
		data[0] = (data[0] & 0b00100000);
		if (attempts == 50) I2Cregister_write(NAU7802_ADDRESS, 0x00, 0b00010110);
		if (attempts >= 100) {
			ge_warn(1, "NAU7802.h", "Data could not be verified");
			return 0;
		}
		attempts++;
	} while (data[0] != 0b00100000);
	I2Cregister_read(NAU7802_ADDRESS, 0x00, data, 1);
	printf("0x%02x \n", data[0]);
	
	
	printf("0x%02x \n", data[0]);
	return 0;
}

int NAU7802_ReadADCTMP() {
	unsigned char data[3];
	int result = 0;
	int attempts;
	I2Cregister_write(NAU7802_ADDRESS, 0x11, 0b00000010); //set temp
	I2Cregister_write(NAU7802_ADDRESS, 0x00, 0b00010110);
	I2Cregister_read(NAU7802_ADDRESS, 0x11, data, 1);
	printf("0x%02x \n", data[0]);
	
	attempts = 0;
	do { //wait for data to be ready
		(I2Cregister_read(NAU7802_ADDRESS, 0x00, data, 1));
		data[0] = (data[0] & 0b00100000);
		if (attempts == 500) I2Cregister_write(NAU7802_ADDRESS, 0x00, 0b00010110);
		if (attempts >= 1000) {
			ge_warn(1, "NAU7802.h", "Data could not be verified");
			return 0;
		}
		attempts++;
		usleep(500);
	} while (data[0] != 0b00100000);
	I2Cregister_read(NAU7802_ADDRESS, 0x00, data, 1);
	printf("0x%02x \n", data[0]);
	
	I2Cregister_read(NAU7802_ADDRESS, 0x12, data, 3);
	result = (((data[0] << 14) | data[1] << 7) | data[2]);
	I2Cregister_write(NAU7802_ADDRESS, 0x00, 0b00000110);
	//printf("%d \n", result);
	return (result);
}


#endif
