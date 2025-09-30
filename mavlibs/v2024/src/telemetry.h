/*


		Telemetry.h
		
		Module to handle making packets of data to send to
		the ground.
		
		Somehow we have to figure out how we're doing the enum structure
		We need the following information (i think):
		1. Variable name
		2. Number
		3. Type (size in bytes?)
		4. Memory address
		
		I'm not sure how we will put all the stuff into a enum...
		
		We have a enumeration of telemetry values that we may want on the ground.
		First, initialize the buffer.
		Then, build up the buffer by adding values via their indices
		
		
*/

#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

// buffer space for where we build up the packet.
// not sure if we have this or it is external, but we'll have it for fun/testing
unsigned char telembuf[256];

// function to initialize the buffer
// Internal info: resets the write pointer.
void initTelem();

// function to add another struct member to the buffer.
// You must initialize the copy first
// returns 0 for success, non-zero for failure
// reasons for failure: overrunning the buffer?
int appendTelem(int index);



#endif
