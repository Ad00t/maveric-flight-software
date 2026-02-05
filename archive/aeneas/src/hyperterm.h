/***************************************************************************

									hyperterm.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: Contains functions that apply to a PC Hyperterminal.

Usage: See function descriptions.  
Examples of use are:
	- Clear the screen
	- Clear a line
	- Clear a certain number of characters
	- Place the cursor at a specific spot
	- Place a string at a specific spot
	- Format a message as alphanumeric characters representing hex (Eg: 0A 5B 03)


Changelog:

Date 	 |  Au.  |	Notes
07-27-10	mra		Created title block.

****************************************************************************
*/

#ifndef __HYPERTERM_H__
#define __HYPERTERM_H__

//======================
//  	Dependencies
//======================
#include "common.h"
#include "pins.h"
#include "ports.h"
#include <string.h>

//==========================
// 		Useful Definitions
//==========================
#define SCREEN_WIDTH	80		// Modify these if using a different display
#define SCREEN_HEIGHT	24
#define CENTERED (0x80)			// used in PlaceString to center the display in either row or column


//======================
//  	Functions
//======================
void clrscrn(int port);									// Clears a screen on a port
void clearLine(int port, int x, int y);					// Clears a line specified by x,y
void clearChars(int port, int x, int y, int blanks);	// Clears a certain number of characters from screen location x,y
void locxy(int port, unsigned char row, unsigned char col);				// Places the cursor at x,y
void placeString(int port, unsigned char row, unsigned char col, unsigned char *buf);		// Same as locateString
void locateString(int port, unsigned char row, unsigned char col, unsigned char *buf); // Places a string (null-terminated?) at x,y
void HexPrint(int port, unsigned char * msg, int len);			// Prints out the bytes of anything passed to it in hex form. (Consider moving to debug.c)

#endif
