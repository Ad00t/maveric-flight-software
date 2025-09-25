/***************************************************************************

									Neon.h

Company: USC/ISI
License: Proprietary
Author(s): Omair A. Rahman (OAR)

Purpose: Header File for a Set of Functions for a Neon1 connected to a PIC.

Changelog:

Date 	 |  Au.  |	Notes
06-29-10	OAR		Created.

****************************************************************************
*/

#ifndef __BEACON__
#define __NEON__
#define __BEACON__


//==============================================
//  				Dependencies
//==============================================
#include "common.h"							// Quotes indicate to search only the same directory as this file. ANgle brackets indicate to search the MPLAB-specified search path
#include "pins.h"
#include "ports.h"					
#include "debug.h"
#include <stdlib.h>			


#warning Neon header included.

// 250 byte string to test out the full potential of the beacon
//const unsigned char* neonTestString = "Testing the NEON Beacon. We are filling up the entire packet of 250 bytes which is the maximum allowed amount. The NEON has 5 bytes of overhead, for a total transmission out the UART of 250 bytes. ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#define BEACON_ON 	1
#define BEACON_OFF	0

//==============================================
//  				Functions
//==============================================
int8 BeaconSendChar(unsigned char CharToSend);
int8 BeaconSendMessage(unsigned char *Message, int MsgLength);
int8 BeaconSendData(unsigned char *Message, int MsgLength);
void configureBeacon();										// Turns beacon on and configures it.
void BeaconPower(unsigned char OnOff);								// Turns the Beacon on or off
void BeaconReset();											// Pulses the reset pin on the beacon
void Vanity(unsigned char * msg, int len);							// Allows for any beacon message to be scheduled.  :)
#endif
