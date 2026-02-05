/***************************************************************************

								spi.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide functions for the PICFJ256GA110 to access its 3 SPI modules.

Changelog:

Date 	 |  Au.  |	Notes
08-01-11	mra		Created.	

TODO: 



The Plan:
	In order to use the PIC24's 3 SPI modules, a number of things must occur.

- Pins must be set up
- SPI data rates and modes must be set up
- SPI functions must be written for tx/rx

First, we have to map the pins of the SPI connection in the PIC's register.  This means
we need to set the RPINRx and RPORx (input and output) registers.  Inputs and outputs
are specificied differently, so we'll start with inputs.

The input registers on the PIC24FJ256GA110 are called RPINRx, and each one can be set
to an RPn pin from 0 to 45.  These pin numbers correspond to the actual chip's pin 
layout according to Table 1-4 in the datasheet (c.f. pages 20-21).

However, in order to set or clear a register, the IOLOCK bit needs to be cleared first.

However,


****************************************************************************
*/

#ifndef __SPI__
#define __SPI__


//========================================
//  			Dependencies
//========================================
#include "common.h"
#include "errors.h"
#include "debug.h"
//#include "math.h"	


//================================
//  	Public Defines
//================================

// These are used with SetSPIMode()
#define FLASH_SPI_MODE			1
#define	GYRO_SPI_MODE			2
#define	MAG_SPI_MODE			3


//================================
//  	Public Globals
//================================

// Status and Control registers
#word SPISTAT = getenv("SFR:SPI1STAT")	// Status Register
#bit SPITBF = SPISTAT.1					// SPIx Transmit Buffer Full Status bit (page 179 of datasheet for PICFJ256GA110)
#bit SPITRBF = SPISTAT.0				// SPIx Receive Buffer Full Status bit
#word SPICON1 = getenv("SFR:SPI1CON1")	// Control Register 1
#word SPICON2 = getenv("SFR:SPI1CON2")	// Control Register 2

/* SPITBF Notes from Datasheet
1 = Transmit not yet started, SPIxTXB is full
0 = Transmit started, SPIxTXB is empty
In Standard Buffer mode:
Automatically set in hardware when CPU writes SPIxBUF location, loading SPIxTXB. Automatically
cleared in hardware when SPIx module transfers data from SPIxTXB to SPIxSR.
In Enhanced Buffer mode:
Automatically set in hardware when CPU writes SPIxBUF location, loading the last available buffer location.
Automatically cleared in hardware when a buffer location is available for a CPU write.

SPIx Receive Buffer Full Status bit
1 = Receive complete, SPIxRXB is full
0 = Receive is not complete, SPIxRXB is empty
In Standard Buffer mode:
Automatically set in hardware when SPIx transfers data from SPIxSR to SPIxRXB. Automatically
cleared in hardware when core reads SPIxBUF location, reading SPIxRXB.

*/



//================================
//  	Public Functions
//================================
ReturnErr_t SetIOLOCK();
ReturnErr_t ClearIOLOCK();
ReturnErr_t SetSPIMode(unsigned char mode);

#endif //__ORBIT__