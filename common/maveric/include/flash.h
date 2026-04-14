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
#include "common.h"
#include "spi.h"						// SPI functionality					
#include <stdint.h>


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
*/	void flashRead(uint32_t addr, uint16_t len, uint8_t * buf);
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
*/	void flashWrite(uint32_t addr, uint16_t len, uint8_t * buf);
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
*/	status_e flashWriteSafe(uint32_t addr, 
				 			   uint16_t len, 
							   uint8_t * buf,
						 	   uint32_t lower_bound,
							   uint32_t upper_bound);		
//------------------------------------------------------------------------

	// Write Enable/Disable
	void flashWriteEnable();
	void flashWriteDisable();
	
	// Sector Protection
	uint8_t flashReadSectorProtection(uint32_t address);
	void flashSectorProtectDisableAddr(uint32_t address);
	void flashSectorProtectDisableSector(uint16_t sector);
	void flashGlobalUnprotect();

	// Erasing...
	void flashEraseBlockByNumber(uint16_t block);
	void flashEraseBlockByAddr(uint32_t addr);
	void flashErasePage(uint16_t page);		// Safely erases a page by copying everything to/from the SWAP block
	void flashEraseChip();						// Wipe the whole thing
	void flashEraseBetweenBlocks(uint16_t start_block, uint16_t end_block);		// Erases blocks, inclusive

	// Copying...
	void flashCopyBlockExceptPage(uint16_t destination_block, uint16_t source_block, uint16_t page_to_except);
	void flashCopyBlock(uint16_t destination_block, uint16_t source_block);
	void flashCopyPage(uint16_t destination_page, uint16_t source_page);
	status_e flashCopy(uint32_t source_addr, uint32_t dest_addr, uint32_t len);	// Copies data from 1 address in flash to another.  Can return failure if len is too large, or if area to be written is not empty.

	// Status
	void flashStatus(uint8_t * status);							// Reads the status bytes (2) from flash.
	void statusReport(uint16_t port, uint8_t statusbyte1, uint8_t statusbyte2);
	void flashVersion(uint8_t * manufact_id, uint8_t * device_id);	// Reads the flash version (manufacturer and device ids)

	// Diagnostic
	void flashReadToPort(uint16_t port, uint32_t addr, uint32_t len);		// Read a specified length of flash to a port
	void flashPrintOccupiedBlocks(uint16_t port=0,
							  	  uint8_t ascii=1);			// Sends a listing of occupied blocks (in either ascii or binary) to a specified port

//========================================
//  		Helper Functions
//========================================
	// Manipulating Flash Addresses
	void IncrementAddrCircular(uint32_t * addr,
						 	   uint32_t lower_limit, 
							   uint32_t upper_limit,
							   uint32_t increment);						// Increments an addr in a circular buffer fashion.  
																				// This means that upper_limit + 1 will equal lower_limit.

	void DecrementAddrCircular(uint32_t * addr,
						 	   uint32_t lower_limit, 
							   uint32_t upper_limit,
							   uint32_t decrement);						// Decrements an addr in a circular buffer fashion.  
																				// This means that lower_limit - 1 will equal upper_limit.
	void flashAddr(uint32_t temp, uint8_t *addr);								// Creates an address from characters
	uint32_t CharsToFlashAddr(uint8_t addbyte1, uint8_t addbyte2, uint8_t addbyte3);// Creates an address from 3 characters

	// Conversion from Flash Address to equivalent characters
	void LongTo3Chars(uint32_t address, uint8_t * threebytearray);	// Creates 3 chars from an address

	// Conversions from a Flash Address to it's Sector/Block/Page
	uint16_t FlashAddrToSector(uint32_t addr);							// Address --> Sector Number
	uint16_t FlashAddrToBlock(uint32_t addr);							// Address --> Block Number
	uint16_t FlashAddrToPage(uint32_t addr);							// Address --> Page Number

	// Conversion from a Sector/Block/Page to the corresponding Flash Addresses (start and end)
	void SectorToFlashAddr(uint16_t sector, 								
						   uint32_t * begin, 
						   uint32_t * end);								
	void BlockToFlashAddr(uint16_t block, 
					 	  uint32_t * begin, 
						  uint32_t * end);
	void PageToFlashAddr(uint16_t page, 
						 uint32_t * begin, 
						 uint32_t * end);

	// Finding empty parts of flash
	uint8_t CheckFlashEmpty(uint32_t address,uint16_t len);
	status_e flashFindEmptyPage(uint16_t startpage,
								   uint16_t endpage,
								   uint16_t * returnpage);					// Checks for the first empty page between two page boundaries, inclusive

	// Actually performs the reading and writing to SPI.  This should be a private-only function eventually...
	void SPIWrRd(uint8_t *Write, uint16_t writeL, uint8_t *Read, uint16_t readL);
	void SPIWrThenRd(uint8_t *Write, uint16_t writeL, uint8_t *Read, uint16_t readL);

#endif 
