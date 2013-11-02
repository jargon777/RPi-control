/* Screen Control Header
 * Screen functions, macros etc.
 * Applies openVG, uses an API developed by Anthony Starks.
 * API available from: https://github.com/ajstarks/openvg
 * oglint.c has been modified as there as an issue with the resolution it was returning.
 */
#include <stdlib.h>
#include "RPi_generror.h"
#include <stdio.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

#define SCALE_X 32
#define SCALE_Y 32

struct ScreenData {
	int width;
	int height;
	int graph_display_Ox;
	int graph_display_Oy;
};

void RPi_GenerateScreen(struct ScreenData *Screen_Main) {
	Start((*Screen_Main).width, (*Screen_Main).height);                   // Start the picture 720*480 but actual is 656x416 32 pixel padding?
	Background(0, 0, 0);                    // Black background
	
	//graph display
	(*Screen_Main).graph_display_Ox = SCALE_X + 20 ;
	(*Screen_Main).graph_display_Oy = (*Screen_Main).graph_display_Ox;
	Fill(255, 255, 255, 1);
	//cheap chinese displays cannot show lines. Odd. Anything smaller than width 3 just chnages colour.
	Rect((*Screen_Main).graph_display_Ox, (*Screen_Main).graph_display_Oy, 3, ((*Screen_Main).height - (*Screen_Main).graph_display_Oy - SCALE_Y - 2));
	Rect((*Screen_Main).graph_display_Ox, (*Screen_Main).graph_display_Oy, ((*Screen_Main).width - (*Screen_Main).graph_display_Ox - SCALE_X - 5), 3);
	End();
}
void RPi_Graphics_Init(int *width, int *height) {
	init(width, height); // Graphics initialization	
}

void RPi_Graphics_Finish() {
	finish();
	
}
