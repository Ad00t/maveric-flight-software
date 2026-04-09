/*************************************************************************
//
//								FLASH_AT25DF641.C
//
//	This file contains code for use with the ATMEL AT25DF641 CHIP.
//
//
***************************************************************************
*/

#include "flash.h" // Contains public flash function prototypes.
#include "logger.h"
#include <stdint.h>

//========================================
//  			Specifications
//========================================
#define FLASH_PAGE_SIZE 256 // Size (bytes) of a page in flash
#define FLASH_BLOCK_SIZE 4096 // Size (bytes) of a block in flash
#define FLASH_SECTOR_SIZE 65536 // Size (bytes) of a sector in flash
#define MAX_FLASH_ADDR 0x7FFFFF // The last address for the flash chip
#define FLASH_MAX_BLOCKS 2048 // The total number of blocks for this chip
#define FLASH_MAX_SECTORS 128 // The total number of sectors for this chip

//========================================
//  			Commands
//========================================
// Read
#define FLASH_READ 0x03

// Program/Erase Commands
#define FLASH_ERASE_BLOCK 0x20
#define FLASH_ERASE_CHIP 0x60
#define FLASH_WRITE 0x02

// Protection
#define FLASH_WRITE_ENABLE 0x06
#define FLASH_WRITE_DISABLE 0x04
#define FLASH_PROTECT_SECTOR 0x36
#define FLASH_UNPROTECT_SECTOR 0x39
#define FLASH_READ_SECTOR_PROTECTION 0x3C

// Status Register
#define FLASH_READ_STATUS_REGISTER 0x05
#define FLASH_WRITE_STATUS_REGISTER_BYTE1 0x01
#define FLASH_WRITE_STATUS_REGISTER_BYTE2 0x31

// Miscellaneous
#define FLASH_RESET 0xF0
#define FLASH_VERSION 0x9F

//========================================
//  			Globals
//========================================
uint16_t FLASH_SWAP_BLOCK =
	FLASH_MAX_BLOCKS - 1; // The block # to use in swapping.  May be rotated to avoid excessive wear

#module

//========================================
//  			Local Defines
//========================================
#define FLASH_ENABLE_PIN FLASH_CHIP_SELECT // Flash enable Pin
#define FLASH_WRITE_PROTECT_PIN FLASH_WRITE_PROTECT // Write Protect Pin

uint16_t getPageSize() {
	return FLASH_PAGE_SIZE;
}

/*=================== flashAddr ========================
  PURPOSE: Creates an address in flash from a certain number of chars
  UNMODIFIED VARIABLES: tempad
  MODIFIED VARIABLES: addr
  GLOBALS: none
  RETURN VALUE: none (returned in *addr)
  NOTES:
-----------------------------------------------------------
*/
void flashAddr(uint32_t tempad, uint8_t* addr) {
	uint16_t len;
	uint16_t n;
	uint8_t* buf2;

	buf2 = &tempad;
	len = 3;

	for (n = 0; n < len; n++) {
		addr[n] = buf2[len - n - 1];
	}
}

void SPIWrRd(uint8_t* Write, uint16_t writeL, uint8_t* Read, uint16_t readL) {
	uint16_t n;

	// Set SPI to optimal flash settings
	spi_set_mode(FLASH_SPI_MODE);

	for (n = 0; n < readL; n++) {
		if (n < writeL) {
			Read[n] = spi_read(Write[n]);
		} else {
			Read[n] = spi_read(0xff);
		}

		while (SPITBF == 0b1) {
			;
		}
	}
}

// Writes first, then reads.  No transformations needed upon return to parent function
// This is for any command that reads data after writing to flash (status, reading)
void SPIWrThenRd(uint8_t* Write, uint16_t writeL, uint8_t* Read,
				 uint16_t readL) {
	uint16_t n;
	uint8_t discard;

	// Set SPI to optimal flash settings
	spi_set_mode(FLASH_SPI_MODE);

	// Write to flash
	for (n = 0; n < writeL; n++) {
		discard = spi_read(Write[n]);
		while (SPITBF == 0b1) {
			;
		}
	}

	// Read from flash
	for (n = 0; n < readL; n++) {
		Read[n] = spi_read(0xff);
		while (SPITBF == 0b1) {
			;
		}
	}
}

void flashStatus(uint8_t* status) {
	uint8_t WriteBuf[2];

	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	delay_ms(1); // Hold here to make sure PIC completes sending last byte
	WriteBuf[0] = FLASH_READ_STATUS_REGISTER; // This is status command
	SPIWrRd(WriteBuf, 2, status, 3); // Read contents into buf
	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
	delay_ms(1);
}

void flashVersion(uint8_t* manufact_id, uint8_t* device_id) {
	uint8_t WriteBuf[2];
	uint8_t ReadBuf[3];

	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	delay_ms(1); // Hold here to make sure PIC completes sending last byte
	WriteBuf[0] = FLASH_VERSION; // This is version command
	SPIWrRd(WriteBuf, 2, ReadBuf, 3); // Read contents into buf
	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
	delay_ms(1);
	*manufact_id = ReadBuf[1];
	*device_id = ReadBuf[2];
}

// Checks the status bit from the flash and returns when complete or after MAX_SEC seconds,
// whichever is first.
void waitForFlash(void) {
	auto uint16_t n;
	auto uint8_t stat[3];
	const uint16_t MAX_SEC = 60; // Sets the maximum time to wait

	n = 0;
	flashStatus(stat); // Checks the flash status bits
	while ((stat[1] & 0x01) && (n < (MAX_SEC * 1000))) // check flash status bit and timeout
	{
		restart_wdt();
		flashStatus(stat); // Checks the flash status bits
		delay_ms(1); // with a 10 ms delay, MAX_SEC seconds is MAX_SEC*100 cycles
		n++;
	}
}

void flashWriteEnable() {
	uint8_t WriteBuf[2];
	uint8_t ReadBuf[2];

	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	//    delay_ms(1);
	WriteBuf[0] = FLASH_WRITE_ENABLE; // This is write enable command
	SPIWrRd(WriteBuf, 1, ReadBuf, 1);
	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
	//	  delay_ms(1);              				 // Hold here to make sure PIC completes the
	//transfer
}

// Returns 0 if unprotected.   1 if protected.
uint8_t flashReadSectorProtection(uint32_t address) {
	uint8_t WriteBuf[4];
	uint8_t ReadBuf[6];

	/********* Read PROTECTION *******/
	flashAddr(address, WriteBuf + 1); // Compute and Save the address to send
	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	delay_ms(1); // Hold here to make sure PIC completes sending last byte
	WriteBuf[0] = FLASH_READ_SECTOR_PROTECTION; // Read sector protection opcode
	SPIWrRd(WriteBuf, 4, ReadBuf, 6); // Send it off
	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
	delay_ms(1); // Hold here to make sure PIC completes the transfer

	// sprintf(dbgbuf,"\r\n%i %i %i %i %i
	// %i",ReadBuf[0],ReadBuf[1],ReadBuf[2],ReadBuf[3],ReadBuf[4],ReadBuf[5]);
	// // sendMSGALL(USER_PORT,dbgbuf);
	return ReadBuf[5];
}

// Disables sector protection on the ATMEL chip.  Will only work if Writing has been Enabled (0x06)
// previously The address it takes is BEFORE reversal
void flashSectorProtectDisable(uint32_t address) {
	uint8_t WriteBuf[4];
	uint8_t ReadBuf[4];

	flashWriteEnable(); // Enable writing to flash first

	// Disable protectection for that sector
	flashAddr(address, WriteBuf + 1); // Compute and Save the address to send
	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	delay_ms(1); // Hold here to make sure PIC completes sending last byte
	WriteBuf[0] =
		FLASH_UNPROTECT_SECTOR; // Write NO Protection to the Section (address is bytes 1 through 3)
	SPIWrRd(WriteBuf, 4, ReadBuf, 4); // Send it off
	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
	delay_ms(1); // Hold here to make sure PIC completes the transfer
}

void flashSectorProtectDisable(uint16_t sector) {
	uint32_t start, end;

	SectorToFlashAddr(sector, &start, &end);
	flashSectorProtectDisable(start);
	return;
}

void flashWriteDisable() {
	uint8_t WriteBuf[2];
	uint8_t ReadBuf[2];

	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	delay_ms(1);
	WriteBuf[0] = FLASH_WRITE_DISABLE; // This is write disable command
	SPIWrRd(WriteBuf, 1, ReadBuf, 1);
	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
}

// Converts 3 chars to a long representing a flash address
uint32_t CharsToFlashAddr(uint8_t addbyte1, uint8_t addbyte2,
							   uint8_t addbyte3) {
	uint32_t retval;

	retval = (long)addbyte1 * 65536 + (long)addbyte2 * 256 + (long)addbyte3;

	return retval;
}

// Converts the last 3 bytes of a long to individual chars.
// Example: Long is 00000000 00000010 00000001 1000000
// Char 0 is 2nd byte:  00000010 --> 0x02
// Char 1 is 3rd byte:  00000001 --> 0x01
// Char 2 is last byte: 10000000 --> 0x80
void LongTo3Chars(uint32_t address, uint8_t* threebytearray) {
	threebytearray[0] = (uint8_t)(address >> 16 & 0x000000FF);
	threebytearray[1] = (uint8_t)(address >> 8 & 0x000000FF);
	threebytearray[2] = (uint8_t)(address & 0x000000FF);
}

// Convert Flash Addresses to Sector Number
uint16_t FlashAddrToSector(uint32_t addr) {
	uint16_t temp;

	temp = (addr >> 16); // Equivalent to dividing by 65536 (size of sector)
	return temp;
}

// Convert Flash Addresses to Block Number
uint16_t FlashAddrToBlock(uint32_t addr) {
	uint16_t temp;

	temp = (addr >> 12); // Equivalent to dividing by 4096 (size of block)
	return temp;
}

// Convert Flash Addresses to Page Number
uint16_t FlashAddrToPage(uint32_t addr) {
	uint16_t temp;

	addr = (addr >> 8); // Equivalent to dividing by 256 (size of page)
	temp = addr;
	return temp;
}

void SectorToFlashAddr(uint16_t sector, uint32_t* begin, uint32_t* end) {
	uint32_t temp;

	temp = (uint32_t)sector;
	*begin = (temp << 16);
	*end = *begin + 0x00FFFF;
}

void BlockToFlashAddr(uint16_t block, uint32_t* begin, uint32_t* end) {
	uint32_t temp;

	temp = (uint32_t)block;
	temp = (temp << 12); // DO NOT CHANGE! It looks like an extra line compared to the others.  But
						 // for some reason it only works this way.
	*begin = temp;
	*end = *begin + 0x000FFF;
}

void PageToFlashAddr(uint16_t page, uint32_t* begin, uint32_t* end) {
	uint32_t temp;

	temp = (uint32_t)page;
	*begin = (temp << 8);
	*end = *begin + 0x0000FF;
}

// Reads a length of flash from addr into buf
void flashRead(uint32_t addr, uint16_t len, uint8_t* buf) {
	uint8_t WriteBuf[4]; // This is a holder for the opCode and address bytes

	flashAddr(addr, WriteBuf + 1); // Compute and Save the address to send.  3 bytes
	WriteBuf[0] = FLASH_READ; // This is read command

	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	// delay_ms(1);                    // Hold here to make sure PIC completes sending last byte
	SPIWrThenRd(WriteBuf, 4, buf, len); // Go read the flash into ReadBuf
	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
}

// Returns True (non-zero) or False (zero) if the area in flash denoted by the inputs is empty.
uint8_t CheckFlashEmpty(uint32_t address, uint16_t len) {
	uint8_t ReadBuf[FLASH_PAGE_SIZE + 1]; // Flash can only write a page at a time.  So this
												// should be all we need to read.
	uint16_t i;
	uint16_t sz;

	// Validate
	if (address + len > MAX_FLASH_ADDR)
		return -1;

	// Loop while there's still more to read
	while (len > 0) {
		// If we want to read more than a page
		if (len >= FLASH_PAGE_SIZE) {
			// Read just a page
			flashRead(address, FLASH_PAGE_SIZE, ReadBuf); // Reads a page from flash into the buffer
			sz = FLASH_PAGE_SIZE;
			address += FLASH_PAGE_SIZE;
			len -= FLASH_PAGE_SIZE;
		} else // if we want to read less than a page
		{
			// Read less than a page
			flashRead(address, len, ReadBuf); // Reads a length from flash into the buffer
			sz = len;
			address += len;
			len -= len;
		}

		// This loop checks if each byte is full of 1's (erased).  If it makes it through the loop,
		// the area in flash is erased.  If anything is not 0xFF, it returns false.
		for (i = 0; i < sz; i++) {
			if (ReadBuf[i] != 0xFF)
				return 0;
		}
	} // while
	return 1;
}

void flashEraseBlockByNumber(uint16_t block) {
	uint32_t addr_begin, addr_end;

	// Convert block to address.
	BlockToFlashAddr(block, &addr_begin, &addr_end);

	// Erase the block.
	flashEraseBlockByAddr(addr_begin); // use of overloaded function
}

void flashEraseBetweenBlocks(uint16_t start_block,
							 uint16_t end_block) // Erases blocks, inclusive
{
	uint16_t i;
	for (i = start_block; i <= end_block; i++) {
		flashEraseBlockByNumber(i);
	}
}

// Erases a block (16 pages) of flash
void flashEraseBlockByAddr(uint32_t addr) {
	uint8_t WriteBuf[4]; // max write is FLASH_PAGE_SIZE, need up to 6 bytes for flash command
							   // and address
	uint8_t ReadBuf[4];

	flashWriteEnable(); // Enable writing to flash

	flashAddr(addr, WriteBuf + 1); // Compute and Save the address to send.  3 bytes

	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	delay_ms(1); // Hold here to make sure PIC completes sending last byte

	WriteBuf[0] = FLASH_ERASE_BLOCK; // This is the block erase command
	SPIWrRd(WriteBuf, 4, ReadBuf, 4); // Send the command

	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
	waitForFlash(); // Make sure flash is ready before releasing
}

void flashGlobalUnprotect() {
	uint8_t WriteBuf[4]; // max write is FLASH_PAGE_SIZE, need up to 6 bytes for flash command
							   // and address
	uint8_t ReadBuf[4];
	const uint8_t opcode = FLASH_WRITE_STATUS_REGISTER_BYTE1; // Write the status byte1
	const uint8_t status_register = 0; // This bit combination unlocks all sectors and keeps the protection bit unlocked

	flashWriteEnable(); // Enable writing to flash

	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	delay_ms(1); // Hold here to make sure PIC completes sending last byte

	WriteBuf[0] = opcode; //
	WriteBuf[1] = status_register; //
	SPIWrRd(WriteBuf, 2, ReadBuf, 4); // Send the command

	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
	waitForFlash(); // Make sure flash is ready before releasing
}

// Erases the whole chip
void flashEraseChip() {
	uint8_t WriteBuf[4]; // max write is FLASH_PAGE_SIZE, need up to 6 bytes for flash command
							   // and address
	uint8_t ReadBuf[4];

	// flashAddr(addr, WriteBuf + 1);   	  // Compute and Save the address to send.  3 bytes

	flashWriteEnable(); // Enable writing to flash

	output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
	delay_ms(1); // Hold here to make sure PIC completes sending last byte

	WriteBuf[0] = FLASH_ERASE_CHIP; // Erase whole chip command
	SPIWrRd(WriteBuf, 1, ReadBuf, 4); // Send the command

	output_high(FLASH_ENABLE_PIN); // disable flash - raise chip select
	waitForFlash(); // Make sure flash is ready before releasing
}

void flashWrite(uint32_t addr, uint16_t len, uint8_t* buf) {
	uint8_t WriteBuf[FLASH_PAGE_SIZE + 6];

	if (len > FLASH_PAGE_SIZE)
		return; // bail if size is too big.  TODO:  Convert this to returnerr_t failure.

	WriteBuf[0] = FLASH_WRITE; // This is the writing command
	flashAddr(addr, WriteBuf + 1); // Next 3 bytes are the address
	memcpy(WriteBuf + 4, buf, len); // Copy data into the writing buffer

	flashWriteEnable(); // Enable writing to flash

	output_low(FLASH_ENABLE_PIN); // enable flash input - lower chip select
	delay_ms(1); // Hold here to make sure PIC completes sending last byte
	SPIWrThenRd(WriteBuf, len + 4, NULL, 0); // Send it off
	output_high(FLASH_ENABLE_PIN); // complete the command - raise chip select
	delay_ms(1);
}

void flashCopyPage(uint16_t destination_page, uint16_t source_page) {
	uint8_t ReadBuf[FLASH_PAGE_SIZE + 1];
	// Compute the address of the page.  Read it.  Compute address of destination.  Write it.

	uint32_t start_addr, end_addr;

	PageToFlashAddr(source_page, &start_addr, &end_addr); // Compute address of page.
	flashRead(start_addr, FLASH_PAGE_SIZE, ReadBuf); // Read it.
	delay_ms(1); // Hold here to make sure PIC completes copying
	PageToFlashAddr(destination_page, &start_addr, &end_addr); // Compuate address of destination
	flashWrite(start_addr, FLASH_PAGE_SIZE, ReadBuf); // Write it.
}

void flashCopyBlockExceptPage(uint16_t destination_block, uint16_t source_block,
							  uint16_t page_to_except) {
	// Loop over pages in source block, copying each to destination except for the page_to_except

	uint16_t i;
	uint16_t source_start_page, source_end_page;
	uint16_t dest_start_page, dest_end_page;
	uint32_t start_addr, end_addr;

	// First, what pages are we dealing with on the source?
	BlockToFlashAddr(source_block, &start_addr,
					 &end_addr); // Convert block # to start/end addresses
	source_start_page = FlashAddrToPage(start_addr); // Store Start Page
	source_end_page = FlashAddrToPage(end_addr); // Store End Page

	// Next, what pages are we dealing with on the destination?
	BlockToFlashAddr(destination_block, &start_addr,
					 &end_addr); // Convert block # to start/end addresses
	dest_start_page = FlashAddrToPage(start_addr); // Store Start Page
	dest_end_page = FlashAddrToPage(end_addr); // Store End Page

	// Finally, loop over the pages, copy all except the chosen one
	for (i = 0; i < 16; i++) {
		if (source_start_page + i != page_to_except)
			flashCopyPage(dest_start_page + i, source_start_page + i);
	}
}

// Safe writing to flash.
//   -- waits for flash to be ready
//	 -- checks if area to write is empty.
//	 -- checks sector protection
//	 -- stays within specified upper and lower bounds
//   -- checks if we're outside the max address of the flash chip
//	 -- writes across page boundaries safely (without wrapping)
//	 -- doesn't return until writing is complete
status_e flashWriteSafe(uint32_t addr, uint16_t len, uint8_t* buf, uint32_t lower_bound, uint32_t upper_bound) {
	uint8_t cleared = 0;
	uint32_t ending_addr, page_boundary, throwaway;
	uint16_t startPage, endPage, currentPage;
	uint16_t length_to_write;

	waitForFlash(); // First wait that everything is ready

	// sprintf(dbgbuf,"\r\nReceived %u,%i,%u,%u",addr,len,lower_bound,upper_bound);
	// // sendMSGALL(USER_PORT,dbgbuf);

	//------Validation of inputs------

	// Compute upper address we're goign to write to
	ending_addr = addr + len;

    // sprintf(LOGBUF, "%u %u %u %u", ending_addr, lower_bound, upper_bound, MAX_FLASH_ADDR); log_error();

	// Check the specified memory bounds
	// Upper
	if (ending_addr > upper_bound)
		return OUT_OF_BOUNDS;
	if (addr > upper_bound)
		return OUT_OF_BOUNDS;
	// Lower
	if (ending_addr < lower_bound)
		return OUT_OF_BOUNDS;
	if (addr < lower_bound)
		return OUT_OF_BOUNDS;
	// Chip
	if (ending_addr > MAX_FLASH_ADDR)
		return OUT_OF_BOUNDS;
	if (addr > MAX_FLASH_ADDR)
		return OUT_OF_BOUNDS;

	// Check if sector protection is on, for both the lower and upper addresses
	if (flashReadSectorProtection(addr))
		return FLASH_PROTECTED;
	if (flashReadSectorProtection(ending_addr))
		return FLASH_PROTECTED;

	// Checks if area to write is empty
	cleared = CheckFlashEmpty(addr, len);
	if (!cleared)
		return FLASH_NOT_EMPTY;

	// Compute Range of Pages we are going to write
	startPage = FlashAddrToPage(addr);
	endPage = FlashAddrToPage(ending_addr);

	if (startPage == endPage) // If they're the same, just write it and get out of here!
	{
		flashWrite(addr, len, buf);
	} else // Otherwise we have to loop across the pages, splitting as we go
	{
		// Validate
		if (startPage > endPage)
			return SUBFUNCTION_ERR; // endPage should be bigger, otherwise bail

		// For each page, check if we're writing past it.  If yes, write just to the page boundary.
		// If no, write to the end and quit.
		for (currentPage = startPage; currentPage <= endPage; currentPage++) {
			PageToFlashAddr(currentPage, &throwaway,
							&page_boundary); // Get a new page boundary (upper) for the current page
			if (ending_addr > page_boundary) // Check if we're writing past that boundary
			{
				length_to_write =
					page_boundary - addr +
					1; // If we are, instead compute the length to write to get us to the boundary
				flashWrite(addr, length_to_write, buf); // write to flash to the page boundary
				addr += length_to_write; // update address index for what you just wrote
				buf += length_to_write; // update buffer index for what you just wrote
				waitForFlash(); // give the flash a sec to finish
			} else // last page
			{
				length_to_write =
					ending_addr - addr; // If not writing past that boundary, length is just from
										// where we are to the original ending addres
				flashWrite(addr, length_to_write, buf); // write to flash to the end
				if (currentPage != endPage)
					return FAILURE; // Validation Check: the only way we should be here is when
									// we're on the last page.  Check that.
			}
		} // for loop
	} // else (across multiple pages)

	return SUCCESS;
} // flashWriteSafe

void flashCopyBlock(uint16_t destination_block, uint16_t source_block) {
	// Loop over pages in source block, copying each to destination

	uint16_t i;
	uint16_t source_start_page, source_end_page;
	uint16_t dest_start_page, dest_end_page;
	uint32_t start_addr, end_addr;

	// First, what pages are we dealing with on the source?
	BlockToFlashAddr(source_block, &start_addr,
					 &end_addr); // Convert block # to start/end addresses
	source_start_page = FlashAddrToPage(start_addr); // Store Start Page
	source_end_page = FlashAddrToPage(end_addr); // Store End Page address

	// Next, what pages are we dealing with on the destination?
	BlockToFlashAddr(destination_block, &start_addr,
					 &end_addr); // Convert block # to start/end addresses
	dest_start_page = FlashAddrToPage(start_addr); // Store Start Page
	dest_end_page = FlashAddrToPage(end_addr); // Store End Page

	// Finally, loop over the pages in a block, copying all
	for (i = 0; i < 16; i++) {
		flashCopyPage(dest_start_page + i, source_start_page + i);
	}
}

// Reads from flash to the specified port, one page at a time
void flashReadToPort(uint16_t port, uint32_t addr, uint32_t len) {
	uint8_t ReadBuf[FLASH_PAGE_SIZE + 1];
	uint32_t end_addr;

	end_addr = addr + len;

	while (addr < end_addr) {
		flashRead(addr, FLASH_PAGE_SIZE, ReadBuf); // read a page into the buf
		// sendMSG(port, ReadBuf, FLASH_PAGE_SIZE); // send it off
		addr = addr + FLASH_PAGE_SIZE; // update address
		delay_ms(100); // give it some time to clock it out the port
	}
}

// Returns the first completely empty flash page found between
// the specified start and end pages, inclusive.
// Returns:  SUCCESS or FAILURE, and a modified returnpage
status_e flashFindEmptyPage(uint16_t startpage, uint16_t endpage,
							   uint16_t* returnpage) {
	// Declare locals
	uint16_t currentpage;
	uint8_t empty;
	uint32_t begin, end;

	// validate inputs
	if (endpage < startpage)
		return FAILURE;

	// initialize locals
	currentpage = startpage;
	empty = 0;
	// uint8_t c;

	for (currentpage = startpage; currentpage <= endpage; currentpage++) {
		// getByte(USER_PORT, &c);
		// if (c == 0x1B) // if user presses Esc
		// 	return FAILURE; // abort
		// sprintf(dbgbuf,"\r\nP:%u",currentpage);
		// sendDBGALL(USER_PORT,dbgbuf);
		PageToFlashAddr(currentpage, &begin, &end); // Find address of the page
		// sprintf(dbgbuf,"\tB:%u,E:%u",begin,end);
		// sendDBGALL(USER_PORT,dbgbuf);
		empty = CheckFlashEmpty(begin, FLASH_PAGE_SIZE); // Check the page for emptiness
		if (empty == 1) // if Empty...
		{
			*returnpage = currentpage; // Set the return page
			return SUCCESS;
		}
	}
	return FAILURE; // If nothing found, return FAILURE and no modifications to the returnpage
}

// Safely Erases a page from flash memory, using the swap block to copy things over
void flashErasePage(uint16_t page) {
	uint32_t addr, temp; // Addresses for deletion page
	uint32_t start_addr, end_addr; // Addresses for swap block
	uint8_t cleared;
	uint16_t block;

	// First, compute where we are...
	PageToFlashAddr(page, &addr, &temp);
	block = FlashAddrToBlock(addr);

	// Are we already empry?
	cleared = CheckFlashEmpty(addr, FLASH_PAGE_SIZE);
	if (!cleared) {
		// Ok. So the flash is not empty.  We need to do the following:
		//     1. Unprotect and Erase the swap block
		//     2. Copy everything *except* where we are deleting to the swap block. (15 pages)
		//	   3. Unprotect and Erase the original block.
		//	   4. Copy everything back from the swap block to the original block. (16 pages)
		//  Note that recursion is disallowed.  So all the above must be done without using the
		//  flash() function.

		BlockToFlashAddr(FLASH_SWAP_BLOCK, &start_addr, &end_addr); // Get address of swap
		flashSectorProtectDisable(start_addr); // unprotect swap
		flashEraseBlockByNumber(FLASH_SWAP_BLOCK); // erase swap
		flashCopyBlockExceptPage(FLASH_SWAP_BLOCK, block, page); // copy to swap
		flashSectorProtectDisable(addr); // unprotect original
		flashEraseBlockByNumber(block); // erase original
		flashCopyBlock(block, FLASH_SWAP_BLOCK); // copy to original
		// flashWriteEnable();											// re-enable writing
	}
	// Assume that worked correctly.
	return;
}

/*
void IncrementAddrCircular(uint32_t * addr,
							   uint32_t lower_limit,
							   uint32_t upper_limit,
							   uint32_t increment)
{
	// Bail if invalid input
	if (*addr<lower_limit || *addr>upper_limit) return;		// Checks for valid input
	if (*addr+increment<lower_limit) return; 			// Looks redundant, but checks if the
uint32_t rolls over the top (sorta)

	*addr += increment;

	if(*addr>upper_limit)
		*addr = lower_limit + (*addr - upper_limit - 1);

	return;

//  IncrementAddrCircular  increments an address in a circular buffer fashion
//	Example usage:
//	  	addr = 5
//		lower limit = 2
//		upper limit = 10
//		increment = 7
//	Note: 5+5=10, 5+6=2, 5+7=3
//
//	Here's how:
//	5 + 7 = 12
//	12 > 10, so addr becomes
//	lower_limit + (*addr - upper_limit - 1)
//	2 + (12 - 10 - 1) = 2 + 1 = 3
}
*/

void DecrementAddrCircular(uint32_t* addr, uint32_t lower_limit,
						   uint32_t upper_limit, uint32_t decrement) {
	// Bail if invalid input
	if (*addr < lower_limit || *addr > upper_limit)
		return; // Checks for valid input
	if (decrement > addr)
		return; // Checks for a potential negative value

	*addr -= decrement;

	if (*addr < lower_limit)
		*addr = upper_limit - (lower_limit - *addr + 1);

	return;

	/*
		Example usage:
			addr = 10
			lower limit = 5
			upper limit = 20
			decrement = 7
		Note: 10-5=5
			  10-6=20
			  10-7=19

		For this example:
		10 - 7 = 3
		3 < 5, so addr becomes
		upper_limit - (lower_limit - *addr + 1);
		20 - (5 - 3 + 1) = 20 - (1) = 19
	*/
}

// Returns an interpretation of status bytes for the AT25DF641 Flash chip
void statusReport(uint16_t port, uint8_t statusbyte1, uint8_t statusbyte2) {
	const uint8_t STATUS_BIT = 0x01;
	const uint8_t ENABLE_BIT = 0x02;
	const uint8_t SPS_SOME = 0x04;
	const uint8_t SPS_ALL = 0x04 | 0x08;
	const uint8_t WPP_BIT = 0x10;
	const uint8_t ERROR_BIT = 0x20;
	const uint8_t SECT_PROT = 0x80;

	// Check each bit of the status and attach a description based on the bit
	if ((statusbyte1 & STATUS_BIT) == STATUS_BIT) {
		// sendMSGALL(port, "\r\nBit 0 = 1 : Device is busy.");
	} else {
		// sendMSGALL(port, "\r\nBit 0 = 0 : Device is ready.");
	}

	// Check each bit of the status and attach a description based on the bit
	if ((statusbyte1 & ENABLE_BIT) == ENABLE_BIT) {
		// sendMSGALL(port, "\r\nBit 1 = 1 : Device is write-enabled.");
	} else {
		// sendMSGALL(port, "\r\nBit 1 = 0 : Device is NOT write-enabled.");
	}

	// Check each bit of the status and attach a description based on the bit
	if ((statusbyte1 & SPS_ALL) == SPS_ALL) {
		// sendMSGALL(port, "\r\nBit2&3= 1 : All sectors software-protected.");
	} else if ((statusbyte1 & SPS_SOME) == SPS_SOME) {
		// sendMSGALL(port, "\r\nBit2|3= 1 : Some or all sectors NOT software-protected.");
	} else {
		// sendMSGALL(port, "\r\nBit2&3= 0 : All sectors NOT software-protected.");
	}

	// Check each bit of the status and attach a description based on the bit
	if ((statusbyte1 & WPP_BIT) == WPP_BIT) {
		// sendMSGALL(port, "\r\nBit 4 = 1 : WP is deasserted.");
	} else {
		// sendMSGALL(port, "\r\nBit 4 = 0 : WP is asserted.");
	}

	// Check each bit of the status and attach a description based on the bit
	if ((statusbyte1 & ERROR_BIT) == ERROR_BIT) {
		// sendMSGALL(port, "\r\nBit 5 = 1 : Last op error detected.");
	} else {
		// sendMSGALL(port, "\r\nBit 5 = 0 : Last op successful.");
	}

	// Check each bit of the status and attach a description based on the bit
	if ((statusbyte1 & SECT_PROT) == SECT_PROT) {
		// sendMSGALL(port, "\r\nBit 7 = 1 : Sector Protection Registers locked.");
	} else {
		// sendMSGALL(port, "\r\nBit 7 = 0 : Sector Protection Registers unlocked.");
	}
}

/*

---------------------
-  Unused functions -
---------------------

// Copies data from 1 place in flash to another
// Maximum write is 1 page, so we'll have to iterate over the pages.
status_e flashCopy(uint32_t source_addr, uint32_t dest_addr, uint16_t len)
{
	uint8_t CopyBuf[FLASH_PAGE_SIZE];	// Cannot be replaced with global ReadBuf because of
collisions in flashWriteSafe()

	// Enforce 1 page at a time
	if(len>FLASH_PAGE_SIZE) return FAILURE;

	//Read it
	flashRead(source_addr,len,CopyBuf);

	//Write it
	return flashWriteSafe(dest_addr,len,CopyBuf,dest_addr,dest_addr+len);
}
*/
