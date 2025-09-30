/***************************************************************************

									lower_tx.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: A stack monitor that delivers anything placed on the monitored stack
	to a stack on the other processor, with a prepended command.

Changelog:

Date 	 |  Au.  |	Notes
09-02-10	mra		Created.

****************************************************************************
*/

#ifndef __LOWER_TX__
#define __LOWER_TX__

//==============================================
//  				Dependencies
//==============================================
#include "common.h"							// Quotes indicate to search only the same directory as this file. ANgle brackets indicate to search the MPLAB-specified search path
#include "mystack.h"

extern ReturnErr_t sendPPM(int port, unsigned char * data, unsigned char len);

//==============================================
//  				typedefs
//==============================================
typedef struct
{
	stack_t * S;				// The stack to service
	short Packetize;			// Flag on whether to packetize 
	int PktType;				// The type of packet (if packetizing)
	unsigned char reserved1;				// The reserved bytes to add
	unsigned char reserved2;				// The reserved bytes to add
} tx_properties_t;

//==============================================
//  				Globals
//==============================================
#define PacketizeAndPushCommand		133		// This MUST match the packetize and push command in ProcessCmd	
#define PushCommand					134		// Ditto
#define PACKETIZING_ON				1		// To be passed in the init function
#define PACKETIZING_OFF				0		// To be passed in the init function


//==============================================
//  				Functions
//==============================================
// Initializes the properties
void initTX_Properties(	tx_properties_t * T, 
					   	stack_t * S,
						short Packetize,			// Flag on whether to packetize 
						int PktType,				// The type of packet (if packetizing)
						unsigned char reserved1,				// The reserved bytes to add
						unsigned char reserved2);

void CheckAndSend(tx_properties_t * T);				 // Call repeatedly to check the stack and send anything on it to other PPM

#endif
