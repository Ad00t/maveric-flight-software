/*


		Telemetry.c
		
		Module to handle making packets of data to send to
		the ground
		
		
*/

#include "telemetry.h"

#module // keep scope in here

//write pointer
unsigned char* wPointer;

// number of bytes we have appended so far
int telemsize;

/* Here we reset our local vars so we can start appending */
void initTelem() {
	wPointer = telembuf;
	telemsize = 0;
}

/* 	This is the main function to use to build up a packet. 
	initTelem() MUST be called before starting a new packet */
int appendTelem(int index) {
	// lookup the value in the enum
	
	// check size
	
	// copy the values over
	
	// increment up the write pointer
	
	// return success!
	return 1;
	
}		


/* Possibly Auto-generated via XML?! */

/*
ReturnErr_t	findTelem(int index, int * dataPtr, int dataSize)
{
	switch(index)
	{
		case 0: 
			dataPtr = &global_location;
			dataSize = 0;
			break;
		.....


		default: return FAILURE;
	}

	return SUCCESS;
}
*/	
