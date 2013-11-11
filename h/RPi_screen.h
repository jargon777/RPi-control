/* Screen Control Header
 * Screen functions, macros etc.
 * Applies openVG, uses an API developed by Anthony Starks.
 * API available from: https://github.com/ajstarks/openvg
 * oglint.c has been modified as there as an issue with the resolution it was returning.
 */
#include "RPi_general.h"
#include "VG/openvg.h"
#include "VG/vgu.h"

#define SCALE_X 32
#define SCALE_Y 32
#define BARSCALE_X 150

struct ScreenData {
	int width;
	int height;
	int graph_display_Ox;
	int graph_display_Oy;
	VGfloat Data1_Yposi[200];
	VGfloat Data3_Yposi[200];
};

void RPi_GenerateScreen(struct ScreenData *Screen_Main, int records, struct Thermistor *thermistor1, 
		struct Thermistor *thermistor2, struct GasSensor *MQ7_1);
void RPi_Graphics_Init(int *width, int *height);
void RPi_Graphics_Finish();
