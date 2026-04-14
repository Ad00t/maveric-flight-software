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

#ifndef __SPI_H__
#define __SPI_H__


//========================================
//  			Dependencies
//========================================

//================================
//  	Public Defines
//================================

// "Why are there two settings for identical processors and flash chips?" you might ask.
// Good question.  I don't know the answer, but it has something to do with how
// the lines are pulled on each motherboard.  The LOWER ppm has a pullup on the SCLK,
// but the UPPER does not.  Anyway this seems to work.
#if defined(UPPER_PPM)
#define FLASH_REGISTER_SETTING		0x033B			// 0000 0011 001 110 11     (2:1 secondary scaler)
#else
//		#define FLASH_REGISTER_SETTING		0x027F			// 0000 0010 011 111 11
//		#define FLASH_REGISTER_SETTING		0x027E			// 0000 0010 011 111 10		(4:1 prescaler)
#define FLASH_REGISTER_SETTING		0x027B			// 0000 0010 011 110 11		(2:1 secondary scaler)
#endif

/* Description
Unused - 000 - Unused
DISSCK - 0 - Internal Spi clock enabled

DISSDO - 0 - SDOx pin is controlled by the module
MODE16 - 0 - Communication is byte-wide (8 bits)
SAMPLE - 1 - Input data sampled at end of data output time
CLOCK -  1 - Serial output data changes on transition from active clock state to Idle clock state

SSEN  -  0 - SSx pin not used by module; pin controlled by port function
POLARI - 0 - Idle state for clock is a low level; active state is a high level
MSTEN  - 1 - Master mode

S SCALE- 111 - 1:1
P SCALE- 11  - 1:1      01 - 
*/


#define GYRO_REGISTER_SETTING		0x00FD			// 

#define MAG_REGISTER_SETTING		0x03BD			// 0000 0011 101 111 01
// Restrictions on MAG:
/*
    - The serial clock signal is provided by the customer-supplied master device and should be 1 MHz or less
    - Data is shifted out and presented to the MicroMag2 on the MOSI pin on the falling edge of SCLK.
    - The clock polarity used with the MicroMag must be normally LOW (cpol = 0).
    - 
*/



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
void spi_set_io_lock();
void spi_clear_io_lock();
void spi_set_mode(uint8_t mode);

#endif //_SPI_
