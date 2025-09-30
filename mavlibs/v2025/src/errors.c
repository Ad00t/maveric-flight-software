/***************************************************************************

									errors.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide generic error-checking functions

Changelog:

Date 	 |  Au.  |	Notes
03-8-10		mra		Created.


****************************************************************************
*/

#include "errors.h"

/***** Overloaded functions to check pointers  ********/
ReturnErr_t checkPointer(unsigned char * ptr)
{
	if(ptr>MAX_RAM_ADDRESS) return FAILURE;
	else if (ptr == NULL) return FAILURE;  
	else return SUCCESS;
}

ReturnErr_t checkPointer(int * ptr)
{
	if(ptr>MAX_RAM_ADDRESS) return FAILURE;  
	else if (ptr == NULL) return FAILURE;  
	else return SUCCESS;
}

ReturnErr_t checkPointer(float * ptr)
{
	if(ptr>MAX_RAM_ADDRESS) return FAILURE;  
	else if (ptr == NULL) return FAILURE;  
	else return SUCCESS;
}
/*********************************************************/
