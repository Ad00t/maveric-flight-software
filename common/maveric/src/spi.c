/***************************************************************************

									spi.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: Functions for talking to SPI devices

Changelog:

Date 	 |  Au.  |	Notes
07-28-10	mra		Created.


****************************************************************************
*/

#include "spi.h"

#module

void spi_set_io_lock() {
	memset(OSCCON,0x46,1);	//sets the lower byte of OSCCON		
	memset(OSCCON,0x57,1);	//sets the lower byte of OSCCON		
	IOLOCK = 1;				// Sets the IOLOCK
}


void spi_clear_io_lock() {
	OSCCON = 0x47;
	OSCCON = 0x57;
	IOLOCK = 0;
}

void spi_set_mode(uint8_t mode) {

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



	switch(mode) {
		case FLASH_SPI_MODE:
			bit_clear(SPISTAT,15);		// Clear the SPI Enable bit, thus allowing rewrites to the control register.
			SPICON1=FLASH_REGISTER_SETTING;				// Write the control register with new settings
			bit_clear(SPISTAT,6);		// Clear the SPIROV bit.
			bit_set(SPISTAT,15);		// Set the SPI Enable bit.	
//			sendDBGALL(USER_PORT,"\r\nF Mode");
			break;
	
		case GYRO_SPI_MODE:
			bit_clear(SPISTAT,15);		// Clear the SPI Enable bit, thus allowing rewrites to the control register.
			SPICON1=GYRO_REGISTER_SETTING;		// Write the control register with new settings
			bit_clear(SPISTAT,6);		// Clear the SPIROV bit.
			bit_set(SPISTAT,15);		// Set the SPI Enable bit.	
//			sendDBGALL(USER_PORT,"\r\nG Mode");
			break;

		case MAG_SPI_MODE:
			bit_clear(SPISTAT,15);		// Clear the SPI Enable bit, thus allowing rewrites to the control register.
			SPICON1=MAG_REGISTER_SETTING;	// Write the control register with new settings
			bit_clear(SPISTAT,6);		// Clear the SPIROV bit.
			bit_set(SPISTAT,15);		// Set the SPI Enable bit.	
//			sendDBGALL(USER_PORT,"\r\nG Mode");
			break;

		default: 
            break;
	}


}
