/* GPS Control
 * This header file contains commands to interact with the GPS on a USB port. 
 * The functions in this file do the same basic tasks that a user can do 
 * from the command prompt to control the GPIO.
 * Contains the following functions:
 *  GPIOexport: sets up the GPIO pin so that it can be used. 
 *  GPIOunexport: closes the GPIO pin after use is done.
 *  GPIOdirection: sets whether the pin is an in or out pin.
 *  GPIOread: reads the status of the pin.
 *  GPIOwrite: sends a high or low signal out the GPIO.
 * 
 * Version 0.1
 */
#ifndef RPi_USBGPS
#define RPi_USBGPS

#include <sys/time.h>

struct GPSdata {
	short longitude_d;
	float longitude_m;
	char longitude_dir;
	short latitude_d;
	float latitude_m;
	char latitude_dir;
	char fix_quality;
	char num_sats;
	float HDOP_acc;
	float altitude_raw;
	char altitude_rawUNIT;
	float geoidal_sep; 
	char geoidal_sepUNIT;
	float actual_alt;
	float speed;
	float speedAngleFromNorth;
	struct timeval time_var;
	char zulu_timeSTR[8];
};

int RPi_USBGPSset_time(char USBdev);
void RPi_USBGPSread(char USBdev, struct GPSdata *GPS_dev);

#endif //wrapper define
