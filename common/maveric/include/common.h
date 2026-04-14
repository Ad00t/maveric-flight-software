#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

typedef enum  {
	FAILURE 			= 			0,		// Generic error
	SUCCESS 			= 			1,		// No error
	BUSY                =           2,		// Function is not finished.
	UNREACHABLE         =           3,		// "Unreachable" code was reached.
	OUT_OF_BOUNDS       =           4,		// Array overflow or pointer overflow detected.
	TIMEOUT             =           5,		// A time overflow has occured

	INVALID_ARG 		= 			10,	    // Invalid Arguments passed to the function
	SUBFUNCTION_ERR     =           11,	    // An error occured in a function called below the current function
	
	FLASH_NOT_EMPTY     = 			20,	
	FLASH_BUSY          =           21,
	FLASH_PROTECTED     =           22,

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

#define MS_PER_MIN      60*1000

uint8_t bcdtohex(uint8_t bcd);
uint8_t hextobcd(uint8_t hex);

uint8_t maxu8(uint8_t a, uint8_t b);
uint8_t minu8(uint8_t a, uint8_t b);

float rad2deg(float rad);
float deg2rad(float deg);

uint8_t ftoa(float x, char* str, uint8_t prec, char format);

#endif // !__COMMON_H__

