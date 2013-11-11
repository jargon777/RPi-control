/* GPIO Control
 * based on http://elinux.org/RPi_Low-level_peripherals#C_2
 * This header file contains commands to interact with the GPIO. The functions in this file
 * do the same basic tasks that a user can do from the command prompt to control the GPIO.
 * Contains the following functions:
 *  GPIOexport: sets up the GPIO pin so that it can be used. 
 *  GPIOunexport: closes the GPIO pin after use is done.
 *  GPIOdirection: sets whether the pin is an in or out pin.
 *  GPIOread: reads the status of the pin.
 *  GPIOwrite: sends a high or low signal out the GPIO.
 * 
 * Version 0.1
 */

#ifndef RPi_GPIO
#define RPi_GPIO

#define IN 0
#define OUT 1
#define ON 1
#define OFF 0
#define MAX_BUFFER 3
#define DIRECTION_MAX 35
#define VALUE_MAX 30

void GPIOexport(unsigned char GPIOpin);
void GPIOunexport(unsigned char GPIOpin);
void GPIOdirection(unsigned char GPIOpin, unsigned char dir);
void GPIOread(unsigned char GPIOpin);
void GPIOwrite(unsigned char GPIOpin, unsigned char value);

#endif //RPi_GPIO

