/***************************************************************************

									mhx.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide MHX functions

Changelog:

Date 	 |  Au.  |	Notes
07-19-10	mra		Created.
07-27-10	mra		Added MHXStatus().

****************************************************************************
*/

#ifndef __MHX__
#define __MHX__

//==============================================
//  				Dependencies
//==============================================
#include "common.h"							// Quotes indicate to search only the same directory as this file. ANgle brackets indicate to search the MPLAB-specified search path
#include "pins.h"
#include "errors.h"
#include "ports.h"					
#include "debug.h"
#include <stdlib.h>			


//==============================================
//  				Defines
//==============================================
// For use with mhx_mode_switch()
#define MHX_COMMAND_MODE	0
#define MHX_DATA_MODE		1


//==============================================
//  				Globals
//==============================================
unsigned long SyncTime;							// Holds the timeout value for syncs
int SyncRetryTime = 10;							// Holds the retry time for syncs


//==============================================
//  				Functions
//==============================================
int MHXConnected();							// Returns 1 if MHX Data Carrier Detect is active. 0 if not.
void configureMHX(unsigned char sreg, unsigned char newval, unsigned char writeToFlash);	// Configures the MHX registers
unsigned char MHXcmd(unsigned char *cmd, int port);   		// All sorts of MHX commands and testing.  TBD.
void MHXStatus(unsigned char * status);
void MHXOff();								// Kills power to MHX
void MHXOn();								// Provides power to MHX
void ReadMHXPins(int port);					// Display the pin status on the chosen port
ReturnErr_t mhx_functional_check(unsigned int max_time_seconds);		// Checks if the MHX looks connected
ReturnErr_t mhx_mode_switch(unsigned char mode); // Get to/from Command Mode, with a return value!
#endif
