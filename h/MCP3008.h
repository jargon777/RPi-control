/* Interactor for the MCP3008.h
 * layer ontp of the SPI_control file to interact specifically with the MCP3008.
 * 
 * Version 0.1
 */
#ifndef MCP3008_h
#define MCP3008_h

#include "SPI_control.h"
#define MCP3008_OPEN(id, spd) SPIc_open(id, spd);
#define MCP3008_CLOSE(id) SPIc_close(id);

int MCP3008_SingleEndedRead (unsigned char SPIchannel) {
    int result = 0;
    unsigned char data[3];
    
    //Setup Transmission Request
	data[0] = 1;  //  Start Bit
	data[1] = 0b10000000 |( ((SPIchannel & 7) << 4)); // Single Ended, Channel
	data[2] = 0; // Meaningless Bit
 
	SPIc_rdwr(0, data, sizeof(data) ); //Send data, function will modify data with data received
 
	result = (data[1]<< 8) & 0b1100000000; //Last two bits of the second word contain data, clip off the junk
	result |=  (data[2] & 0xff); //append third word to result
	return result;
}

int MCP3008_DifferentialRead (unsigned char CHselect) {
	//Specify a number between 0-7, refer to datasheet.
    int result = 0;
    unsigned char data[3];
    
    //Setup Transmission Request
	data[0] = 1;  //  Start Bit
	data[1] = 0b00000000 |( ((CHselect) << 4)); // differentialed, Channel
	data[2] = 0; // Meaningless Bit
 
	SPIc_rdwr(0, data, sizeof(data) ); //Send data, function will modify data with data received
 
	result = (data[1]<< 8) & 0b1100000000; //Last two bits of the second word contain data, clip off the junk
	result |=  (data[2] & 0xff); //append third word to result
	return result;
}		

#endif //MCP3008_h
