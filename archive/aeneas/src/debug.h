/***************************************************************************

									debug.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide debugging functions

To use: Include debug.c anywhere you need debugging functions.

		Functions do the following:
		- Print to a port using sendDBGALL() 
			- Note that this function can take the debug level as a parameter.
		- Insert a delay using dbgdelay()
		
		To remove all debugging, comment out the #define DEBUG, and all functions 
		become empty calls, which the compiler should ignore(?).

Changelog:

Date 	 |  Au.  |	Notes
03-08-10	mra		Created.

****************************************************************************
*/

#ifndef __DEBUG_H_FILE__
#define __DEBUG_H_FILE__

//==============================================
//  	Dependencies
//==============================================
#include "config.h"
#include <common.h>				// Require for #device statement
#include <errors.h>				// For error definitions
#include <ports.h>				// Required for sending debug messages out ports
#include <hyperterm.h>			// Required for MeasureTime()

//extern unsigned int64 MSEC;
extern ReturnErr_t sendPPM(int port, unsigned char * data, unsigned char len);


//==============================================
//  	Definitions
//==============================================
#define DBG_BUF_SIZE 			128				// Debugging buffer size used in unsigned char dbgbuf[]
#define DEFAULT_DEBUG_LEVEL		0				// Default level for debug messages (depreciated)

#define MEASURE_TIME_INIT		1
#define MEASURE_TIME_SHOW		0

//==============================================
//  	Macros
//==============================================
#ifdef DEBUG
	#define dbgdelay(x) delay_ms(x)
#else
	#define dbgdelay(x) ((void)0)
#endif

//==============================================
//  	Globals
//==============================================
#ifdef DEBUG
	unsigned char dbgbuf[DBG_BUF_SIZE];						// general purpose debug buffer
	long dbg_long;											// general purpose debug long
	float dbg_float;
#endif						
unsigned char DEBUG_LEVEL = 100;								// Sets the level of detail in debug messages (higher = more detail) 
short MeasureFlag = 0;

//==============================================
//  	Functions
//==============================================
void sendDBGALL(int port,unsigned char * msg,int lvl=0);	 // Use to send debugging messages to a port
void byteMe(unsigned char c);							 // Use to display the bits in a byte 
void byteMe(int i);							 	 // Use to display the bits in an int
void byteMe(long l);							 // Use to display the bits in a long
void byteMe(float f);							 // Use to display the bits in a float
void PrintDbgErr(int port, ReturnErr_t err);	 // Will send a description of an error to the screen
void Tick(short reset=0);
void printFloat(unsigned char *string, float64 num, int precision=6);	// Default value added
void printFloat(unsigned char *string, float num, int precision=6);	// Overloaded function for regular floats
void MeasureTime(unsigned char reset);								// Measures time between calls to itself.  Places measurement on USER_PORT.
void ShowOutputRegisters();						// Shows the PIC's RPORx registers on the USER_PORT
void ShowInputRegisters();						// Shows the PIC's RPINRx registers on the USER_PORT
#endif // __DEBUG_H_FILE__
