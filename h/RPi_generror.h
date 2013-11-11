/* RPI General Error Handling
 * Handles General Errors
 */
 
#ifndef RPi_GENERROR
#define RPi_GENERROR
 
void ge_halt (unsigned char error_code, char *file_name, char *error_text);
void ge_warn (unsigned char warn_code, char *file_name, char *warn_text);
 
#endif /* RPi_GENERROR */
