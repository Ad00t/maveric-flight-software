/***************************************************************************

									errors.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide typedefs and structures for error handling

Changelog:

Date 	 |  Au.  |	Notes
03-08-10	mra		Created.

****************************************************************************
*/

#ifndef __ERRORS__
#define __ERRORS__


#define MAX_RAM_ADDRESS 0x47FE

#include "common.h"							// Quotes indicate to search only the same directory as this file. ANgle brackets indicate to search the MPLAB-specified search path
#include <stdlib.h>			


// Error returns
typedef enum  {
	SUCCESS 			= 			0,		// No error
	BUSY,									// Function is not finished.
	FAILURE 			= 			2,		// Generic error
	UNREACHABLE,							// "Unreachable" code was reached.
	OUT_OF_BOUNDS,							// Array overflow or pointer overflow detected.
	TIMEOUT,								// A time overflow has occured

	INVALID_ARG 		= 			100,	// Invalid Arguments passed to the function
	SUBFUNCTION_ERR,					    // An error occured in a function called below the current function
	
	FLASH_NOT_EMPTY     = 			200,	
	FLASH_BUSY,
	FLASH_PROTECTED,

	STACK_TOO_BIG_TO_PUSH =         300,
	STACK_INVALID_ITEM,
	STACK_WARN_OVERWRITE,

	UNKNOWN				=			900		// The error state is unknown
	
} ReturnErr_t;


//==============================================
//  	Functions
//==============================================


#endif