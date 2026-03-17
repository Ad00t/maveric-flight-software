#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

typedef enum  {
	SUCCESS 			= 			0,		// No error
	BUSY,									// Function is not finished.
	FAILURE 			= 			2,		// Generic error
	UNREACHABLE,							// "Unreachable" code was reached.
	OUT_OF_BOUNDS,							// Array overflow or pointer overflow detected.
	TIMEOUT,								// A time overflow has occured

	INVALID_ARG 		= 			10,	    // Invalid Arguments passed to the function
	SUBFUNCTION_ERR,					    // An error occured in a function called below the current function
	
	FLASH_NOT_EMPTY     = 			20,	
	FLASH_BUSY,
	FLASH_PROTECTED,

	UNKNOWN				=			90      // The error state is unknown
} status_e;

#define KNRM  "\033[0m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"

uint8_t bcdtohex(uint8_t bcd);
uint8_t hextobcd(uint8_t hex);

#endif // !__COMMON_H__

