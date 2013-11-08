/* RPI General Error Handling <SOURCE>
 * Handles General Errors 
 * 
 * ge_halt: Halts the program immediately, and displays an error to the console based on the code passed to it.
 * 		Type 0 Errors: General Errors
 * 		Type 1 Errors: Input/Output Errors
 * 
 * ge_warn: Warns the user that an error occured. Generally the code passing the error must then end whatever
 * 	task it was doing. Does not halt the program.
 * 		Type 0 Warnings: General Warnings
 * 		Type 1 Warnings: Input/Output Warnings
 * 
 * Version 0.1
 * GCC:
 */
 
#include <stdio.h>
#include <stdlib.h>
 
void ge_halt (unsigned char error_code, char *file_name, char *error_text) {
	if (!error_code) { //generic error
		printf("*********************************************************************\n");
		printf("*** GENERAL ERROR in %s, the program must stop. ***\n", file_name);
		printf("*********************************************************************\n");
		printf("*** %s\n", error_text);
		exit(1);
	}
	if (error_code==1) { //SPI Errors
		printf("********************************************************************\n");
		printf("*** I/O ERROR in %s, the program must stop. ***\n", file_name);
		printf("********************************************************************\n");
		printf("*** %s\n", error_text);
		exit(1);
	}
	else {
		printf("********************************************************************\n");
		printf("*** UNSPECIFIED ERROR in %s, the program must stop. ***\n", file_name);
		printf("********************************************************************\n");
		printf("*** %s\n", error_text);
		exit(1);		
	}
}
void ge_warn (unsigned char warn_code, char *file_name, char *warn_text){
	if (!warn_code) {
		printf("** GENERAL WARNING in %s: %s", file_name, warn_text);
	}
	if (warn_code==1) { //SPI Errors
		printf("*** I/O WARNING in %s, Program will proceed. ***\n", file_name);
		printf("*** %s\n", warn_text);
	}
}
 
#endif /* RPi_GENERROR */

