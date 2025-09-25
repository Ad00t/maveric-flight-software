/***************************************************************************

									stensat.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide STENSAT beacon functions

Changelog:

Date 	 |  Au.  |	Notes
08-17-10	mra		Created.
06-30-11    mra		Updated name to better indicate that this is a stensat-only driver.

****************************************************************************
*/

#ifndef __BEACON__
#define __STENSAT__
#define __BEACON__


//#warning Stensat header included.

//==============================================
//  				Dependencies
//==============================================
#include "common.h"							// Quotes indicate to search only the same directory as this file. ANgle brackets indicate to search the MPLAB-specified search path
#include "pins.h"
#include "errors.h"
#include "ports.h"					
#include "debug.h"
#include <stdlib.h>			


// Globals
//const int BEACON_STACK_ITEMS = 1;					// Settings for the Beacon Stack
//const int BEACON_STACK_ITEM_SIZE = 200;				// Settings for the Beacon Stack

//==============================================
//  				Global Defines
//==============================================
#define BEACON_ON 	1
#define BEACON_OFF	0

//==============================================
//  				Functions
//==============================================
void Vanity(unsigned char * msg, int len);							// Allows for any beacon message to be scheduled.  :)
ReturnErr_t configureBeacon();

void BeaconPower(unsigned char OnOff);
int8 BeaconSendChar(unsigned char CharToSend);								// Turns the Beacon on or off
int8 BeaconSendMessage(unsigned char *Message, int MsgLength);
int8 BeaconSendData(unsigned char *Message, int MsgLength);


#endif
