/***************************************************************************
				CRC Library

Author: Michael Aherne & Fred Tubb & Adhit Siripurapu
Company: USC ISI / iControl
License: Proprietary
This software is not to be used without the express written permission from 
the author(s) or the company(ies) specified above.

Purpose: To calculate and check CRC values. Provides an implementation of the
         AX.25 CRC protocol which produces a 16 bit CRC-CCITT value.

Usage: See the descriptions of each function below.
*******************************************************************************
*/

#ifndef __CRCNEW_H___
#define __CRCNEW_H___

//========================================
//  			Dependencies
//========================================

#include <stdint.h>

//========================================
//  			Definitions
//========================================
#define CRC_CONTINUE_PREVIOUS 	0		// For use with getCRCCont()
#define CRC_RESTART 			1		// For use with getCRCCont()
#define CRC32_SIZE              4

//========================================
//  			Globals
//========================================



//========================================
//  			Prototypes
//========================================
uint16_t compute_crc16(uint8_t* buf, uint8_t len);
uint16_t compute_crc16_cont(uint8_t restart, uint8_t* buf, uint8_t len);
int1 check_crc16(uint8_t* buf, int len, uint16_t crc);

uint32_t compute_crc32(uint8_t* data, int length);

//==============================================================================
//  						Function Descriptions
/*	

----------------------------------------------------------------------- getCRC()
	Function: compute_crc16(unsigned char *buf, int len, unsigned char *value);
	Purpose: Returns a CRC for a given length of bytes.
	Notes: None


-------------------------------------------------------------------- getCRCCont()
	Function: void compute_crc16_cont(unsigned char restart, unsigned char *buf, int len, unsigned char *value);

	Purpose: Returns a CRC for a given length of bytes,
			 with the option of continuing from a previous set of bytes.
	
	Notes: 
		These definitions can be used:
			CRC_CONTINUE_PREVIOUS
			CRC_RESTART

		Example usage:	The following methods are equivalent.

		Method 1:
		unsigned char crc[2];
		getCRC("Hello World",11,crc);			// Gets a crc for the string "Hello World"

		     is equivalent to 

		Method 2:		
		unsigned char crc[2];
		getCRCCont(CRC_RESTART, "Hello ", 6, crc);				// Gets a crc for the string "Hello "
		getCRCCont(CRC_CONTINUE_PREVIOUS, "Hello ", 6, crc);	// *Continues* a crc for the string "World"

			  Both methods produce equivalent CRCs.


---------------------------------------------------------------------- checkCRC()

	Function: unsigned char checkCRC(unsigned char *buf,int len,unsigned char * bufcrc)
	Purpose: Checks a given message for the correct crc
	Returns:  CRCs Match -- 1
			  CRCs Do Not Match -- 0
	Notes: None






====================================================================================
*/

#endif // __CRC_H__


//EOF
