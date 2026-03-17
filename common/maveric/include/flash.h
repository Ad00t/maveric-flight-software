/***************************************************************************
					Generic Flash API

Author: Michael Aherne
Company: USC ISI
License: Proprietary

Purpose: To provide standard functions for the flash chips
This file will hopefully be a common interface to the flash chip, with
the specifics defined in the relevant flash file.  For example:
- FLASH_AT25DF641.C
- FLASH_M25PE20.C

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
07-27-10	mra		Added changelog.
10-15-10	mra		Small mods to create a common interface for multiple flash chips.

*******************************************************************************
*/



#ifndef __FLASH_H__
#define __FLASH_H__
//========================================
//  			Dependencies
//========================================
#include <stdint.h>
#include "common.h"
#include "spi.h"						// SPI functionality					


//========================================
//  			Definitions
//========================================

// Under development: A handler for flash allocations that I can refer to
// for uniformity.
typedef struct 
{
	long location;
	long size;
	long wr_ptr;
	long rd_ptr;
} flash_alloc_t;

uint16_t getPageSize();

//========================================
//  		Standard Functions
//========================================

/*	Function: flashRead()

	Purpose: Reads a continuous section of flash, starting at the given
	*addr* and extending for the given *len*.  The contents read are
	stored in the provided *buf*.
	
	Notes: If the buffer is not large enough to store the characters, a
	trap conflict may ensue, or data corruption may occur.
*/	void flashRead(unsigned long addr, unsigned int len, unsigned char * buf);
//------------------------------------------------------------------------

	
/*	Function: flashWrite()

	Purpose: Writes the specified length of buffer to the address in 
	flash.
	
	Notes: 
	1. The maximum size that can be written is 256 bytes (1 page).
	2. If the sector has not been unprotected, function will fail.
	3. If there is already data where you are writing, it will 
	   be overwritten and corrupted.
	4. If you attempt to write across a page boundary, the writing will
	   wrap around and continue on the beginning of the page.

	See Also:
		flashWriteSafe()
*/	void flashWrite(unsigned long addr, unsigned int len, unsigned char * buf);
//------------------------------------------------------------------------



/*	Function: waitForFlash()

	Purpose: Waits for the BUSY status on flash to clear, or 
	120 seconds.  Whichever comes first.
	
	Notes: 
	TODO: Create a SUCCESS/FAILURE return.
*/	void waitForFlash(void);
//------------------------------------------------------------------------



/*	Function: flashWriteSafe()

	Purpose: Writes a specified length of buffer to flash in a safe way.
	Returns SUCCESS or an error according to errors.h.  There is no limit
	to the size of the buffer you are writing, so long as it fits within
	the specified upper and lower bounds.
	
	Notes: 
	1. This function does the following before writing:
		-- waits for flash to be ready
		-- checks if area to write is empty
		-- checks that sector protection is disabled (if applicable)
		-- checks that writing will occur within specified upper and lower bounds 
		-- checks that writing will occur below the max address of the chip

	2. When writing, it will write across page boundaries safely (without wrapping)
		
	3. The function doesn't return until writing is complete and the BUSY status is cleared.

	4. The function can still fail if the length specified exceeds the buffer,
	   resulting in an trap conflict.
*/	status_e flashWriteSafe(unsigned long addr, 
				 			   int len, 
							   unsigned char * buf,
						 	   unsigned long lower_bound,
							   unsigned long upper_bound);		
//------------------------------------------------------------------------

	// Write Enable/Disable
	void flashWriteEnable();
	void flashWriteDisable();
	
	// Sector Protection
	unsigned char flashReadSectorProtection(unsigned long address);
	void flashSectorProtectDisable(unsigned long address);
	void flashSectorProtectDisable(unsigned int sector);
	void flashGlobalUnprotect();

	// Erasing...
	void flashEraseBlockByNumber(unsigned int block);
	void flashEraseBlockByAddr(unsigned long addr);
	void flashErasePage(unsigned int page);		// Safely erases a page by copying everything to/from the SWAP block
	void flashEraseChip();						// Wipe the whole thing
	void flashEraseBetweenBlocks(unsigned int start_block, unsigned int end_block);		// Erases blocks, inclusive

	// Copying...
	void flashCopyBlockExceptPage(unsigned int destination_block, unsigned int source_block, unsigned int page_to_except);
	void flashCopyBlock(unsigned int destination_block, unsigned int source_block);
	void flashCopyPage(unsigned int destination_page, unsigned int source_page);
	status_e flashCopy(unsigned long source_addr, unsigned long dest_addr, unsigned long len);	// Copies data from 1 address in flash to another.  Can return failure if len is too large, or if area to be written is not empty.

	// Status
	void flashStatus(unsigned char * status);							// Reads the status bytes (2) from flash.
	void statusReport(int port, unsigned char statusbyte1, unsigned char statusbyte2);
	void flashVersion(unsigned char * manufact_id, unsigned char * device_id);	// Reads the flash version (manufacturer and device ids)

	// Diagnostic
	void flashReadToPort(int port, unsigned long addr, unsigned long len);		// Read a specified length of flash to a port
	void flashPrintOccupiedBlocks(int port=0,
							  	  unsigned char ascii=1);			// Sends a listing of occupied blocks (in either ascii or binary) to a specified port

//========================================
//  		Helper Functions
//========================================
	// Manipulating Flash Addresses
	void IncrementAddrCircular(unsigned long * addr,
						 	   unsigned long lower_limit, 
							   unsigned long upper_limit,
							   unsigned long increment);						// Increments an addr in a circular buffer fashion.  
																				// This means that upper_limit + 1 will equal lower_limit.

	void DecrementAddrCircular(unsigned long * addr,
						 	   unsigned long lower_limit, 
							   unsigned long upper_limit,
							   unsigned long decrement);						// Decrements an addr in a circular buffer fashion.  
																				// This means that lower_limit - 1 will equal upper_limit.
	void flashAddr(unsigned long temp, unsigned char *addr);								// Creates an address from characters
	unsigned long CharsToFlashAddr(unsigned char addbyte1, unsigned char addbyte2, unsigned char addbyte3);// Creates an address from 3 characters

	// Conversion from Flash Address to equivalent characters
	void LongTo3Chars(unsigned long address, unsigned char * threebytearray);	// Creates 3 chars from an address

	// Conversions from a Flash Address to it's Sector/Block/Page
	unsigned int FlashAddrToSector(unsigned long addr);							// Address --> Sector Number
	unsigned int FlashAddrToBlock(unsigned long addr);							// Address --> Block Number
	unsigned int FlashAddrToPage(unsigned long addr);							// Address --> Page Number

	// Conversion from a Sector/Block/Page to the corresponding Flash Addresses (start and end)
	void SectorToFlashAddr(unsigned int sector, 								
						   unsigned long * begin, 
						   unsigned long * end);								
	void BlockToFlashAddr(unsigned int block, 
					 	  unsigned long * begin, 
						  unsigned long * end);
	void PageToFlashAddr(unsigned int page, 
						 unsigned long * begin, 
						 unsigned long * end);

	// Finding empty parts of flash
	unsigned char CheckFlashEmpty(unsigned long address,unsigned int len);
	status_e flashFindEmptyPage(unsigned int startpage,
								   unsigned int endpage,
								   unsigned int * returnpage);					// Checks for the first empty page between two page boundaries, inclusive

	// Actually performs the reading and writing to SPI.  This should be a private-only function eventually...
	void SPIWrRd(unsigned char *Write, unsigned int writeL, unsigned char *Read, unsigned int readL);
	void SPIWrThenRd(unsigned char *Write, unsigned int writeL, unsigned char *Read, unsigned int readL);

#endif 
