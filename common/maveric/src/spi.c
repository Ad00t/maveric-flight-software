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
	}
}
