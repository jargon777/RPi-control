/* General Proceedures for MAIN
 * General functions, macros etc.
 * Theory for thermistors: http://www.daycounter.com/Calculators/Steinhart-Hart-Thermistor-Calculator.phtml and http://www.maximintegrated.com/app-notes/index.mvp/id/1753 and http://www.paulschow.com/2013/08/monitoring-temperatures-using-raspberry.html
 * Theory for capacitance: https://en.wikipedia.org/wiki/RC_time_constant
 * Theory for GPS Error: http://www.trakgps.com/en/index.php/information/gps-articles-information/65-gps-accuracy
 */

//INCLUDE BLOCK 
#ifndef RPi_GENERALHEADER
#define RPi_GENERALHEADER

//INCLUDES
#include "RPi_USBGPS.h"

//DEFINES
#define REF_VOLTAGE 2.6
#define NAU7802_REFVOLTS 3.3
#define THERMISTORS_GPIOPIN 25
#define RESET_PIN 24

#define THERMISTOR1_CH 0
#define THERMISTOR1_R1 10000
#define THERMISTOR2_CH 1
#define THERMISTOR2_R1 200
#define MQ7_1CH 2
#define MQ7_1R1 33000
#define MQ7_1Ro 10000

#define GPSBAUD 4800

#define HUMISTOR_CH 2
#define HUMISTOR_R1 6200000
#define HUMISTOR_SLPSEC 0
#define HUMISTOR_SLPNSC 2000000
#define HUMISTOR_CRCNSC 0.935946 //calibrated to a 270pf capacitor

#define MCP3008_SPD 10 //

//MACROS
#define SENSORS_ON GPIOwrite(THERMISTORS_GPIOPIN, ON);
#define SENSORS_OFF GPIOwrite(THERMISTORS_GPIOPIN, OFF);
#define RESET_CAPACITORS GPIOwrite(RESET_PIN, ON); usleep(100000); GPIOwrite(RESET_PIN, OFF); usleep(100000);

//structs
struct Thermistor {
	float temperature;
	float resistance;
	float voltage;
	float ratio;
	unsigned int ADC;
};

struct GasSensor {
	float PPM;
	float resistance;
	float voltage;
	float ratio;
	unsigned int ADC;
};

struct Humistor {
	float voltage_initial;
	float voltage;
	float capacitance_uf;
	float capacitance_pf;
	float humidex;
	unsigned short ADC;
	unsigned short ADC_initial;
};

//FUNCTIONS
void RPi_ADCread_sensors(struct Thermistor *thermistor1, struct Thermistor *thermistor2, struct Thermistor *thermistor3,
							struct GasSensor *MQ7_1, struct Humistor *humistor1);
void RPi_writeFileHeader ();
void RPi_writeToFile (struct Thermistor *thermistor1, struct Thermistor *thermistor2, struct Thermistor *thermistor3,
							struct GasSensor *MQ7_1, struct Humistor *humistor1, struct GPSdata *GPS1);

//keyboard hit detection function from http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
int kbhit();

//Things that the RPi must load at the start
void RPi_construct(char USBdev);

//Things that the RPi must unload at the end
void RPi_destruct();

void RPi_generaltest();

#endif //end of include block
