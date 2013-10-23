/* RPI General Error Handling
 * Handles General Errors
 */
 
 #ifndef RPi_GENERROR
 #define RPi_GENERROR
 
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
		printf("*** SPI-RELATED ERROR in %s, the program must stop. ***\n", file_name);
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
 }
 
 #endif /* RPi_GENERROR */
