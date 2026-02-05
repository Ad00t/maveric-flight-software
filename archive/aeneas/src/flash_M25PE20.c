/*************************************************************************
//
//								FLASH_M25PE20.C
//		
//	This file contains code for use with the Numonyx M25PE20 CHIP.
//	
//
***************************************************************************
*/


#include "flash.h"						// Contains public flash function prototypes.

//========================================
//  			Sepcifications
//========================================
#define FLASH_PAGE_SIZE     		256						// Size (bytes) of a page in flash
#define FLASH_BLOCK_SIZE			4096					// Size (bytes) of a block in flash
#define FLASH_SECTOR_SIZE			65536					// Size (bytes) of a sector in flash
#define MAX_FLASH_ADDR				0x3FFFF					// The last address for the flash chip
#define FLASH_MAX_BLOCKS 			64						// The total number of blocks for this chip
#define FLASH_MAX_SECTORS   		4						// The total number of sectors for this chip

//========================================
//  			Commands
//========================================

// Read
#define FLASH_READ							0x03			// 
#define FLASH_READ_LOCK						0xE8			// 

// Program/Erase Commands
#define FLASH_PAGE_PROGRAM					0x02			// Does not pre-erase page
#define FLASH_PAGE_WRITE					0x0A			// Pre-erases the page, uses data buffer to be byte-addressable
#define FLASH_ERASE_BLOCK					0x20			// 
#define FLASH_ERASE_CHIP					0xC7			// 
#define FLASH_ERASE_PAGE					0xDB			// 

// Protection
#define FLASH_WRITE_ENABLE					0x06			// 
#define FLASH_WRITE_DISABLE					0x04			// 

// Status Register
#define FLASH_READ_STATUS_REGISTER			0x05			// 
#define FLASH_WRITE_STATUS_REGISTER			0x01			// 
#define FLASH_WRITE_LOCK_REGISTER			0xE5			// 

// Miscellaneous
#define FLASH_VERSION						0x9F			// TODO: 1 to 20 bytes returning -- check the function

// Power
#define FLASH_POWER_DOWN					0xB9			// 
#define FLASH_POWER_UP						0xAB			// 



//========================================
//  			Globals
//========================================
unsigned int FLASH_SWAP_BLOCK = FLASH_MAX_BLOCKS-1;				// The block # to use in swapping.  May be rotated to avoid excessive wear


#module

//========================================
//  			Local Defines
//========================================
#define FLASH_ENABLE_PIN 			FLASH_CHIP_SELECT					// Flash enable Pin
#define FLASH_WRITE_PROTECT_PIN		FLASH_WRITE_PROTECT					// Write Protect Pin


// Private Functions
void SPIWrRd(unsigned char *Write, unsigned int writeL, unsigned char *Read, unsigned int readL);


/*=================== flashAddr ========================
  PURPOSE: Creates an address in flash from a certain number of chars
  UNMODIFIED VARIABLES: tempad
  MODIFIED VARIABLES: addr
  GLOBALS: none
  RETURN VALUE: none (returned in *addr)
  NOTES:
-----------------------------------------------------------
*/
void flashAddr(unsigned long tempad, unsigned char *addr) {
	int len;
    int n;
	unsigned char * buf2;

	buf2 = &tempad;
	len = 3;

    for (n = 0; n < len; n++)
        addr[n] = buf2[len - n - 1];
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
	void SPIWrRd(unsigned char *Write, unsigned int writeL, unsigned char *Read, unsigned int readL) 
	{
		unsigned int n;

		for(n=0;n<readL;n++) 
		{
			if(n<writeL) 
			{			
				Read[n]=spi_read(Write[n]);
			}
			else 
			{
				Read[n]=spi_read(0xff);
			}
	
			while(SPITBF == 0b1) { ;}
		}
	}


	// Writes first, then reads.  No transformations needed upon return to parent function
	// This is for any command that reads data after writing to flash (status, reading)
	void SPIWrThenRd(unsigned char *Write, unsigned int writeL, unsigned char *Read, unsigned int readL) 
	{
		unsigned int n;
		unsigned char discard;

		// Write to flash
		for(n=0;n<writeL;n++) 
		{
			discard=spi_read(Write[n]);
			while(SPITBF == 0b1) { ;}
		}
	
		// Read from flash
		for(n=0;n<readL;n++) 
		{
			Read[n]=spi_read(0xff);
			while(SPITBF == 0b1) { ;}
		}
	}

	// Writes only.
	// This is for any command that only writes data.  No reads.
/*	void SPIWrOnly(unsigned char *Write, unsigned int writeL) 
	{
		unsigned int n;
		unsigned char discard;

		// Write to flash
		for(n=0;n<writeL;n++) 
		{
			discard=spi_read(Write[n]);
			while(SPITBF == 0b1) { ;}
		}
	}	
*/


	void flashStatus(unsigned char * status)
	{
	    unsigned char WriteBuf[2];
		
		output_low(FLASH_ENABLE_PIN); // enable flash - lower chip select
    	delay_ms(1);                          // Hold here to make sure PIC completes sending last byte
        WriteBuf[0] = FLASH_READ_STATUS_REGISTER; // This is status command
        SPIWrRd(WriteBuf, 2, status, 3); // Read contents into buf
		output_high(FLASH_ENABLE_PIN);   // disable flash - raise chip select
		delay_ms(1);	
	}

	void flashVersion(unsigned char * manufact_id, unsigned char * device_id)
	{
	    unsigned char WriteBuf[2];
		unsigned char ReadBuf[3];
		
		output_low(FLASH_ENABLE_PIN);     // enable flash - lower chip select
    	delay_ms(1);                      // Hold here to make sure PIC completes sending last byte
        WriteBuf[0] = FLASH_VERSION; 	  // This is version command
        SPIWrRd(WriteBuf, 2, ReadBuf, 3); // Read contents into buf
		output_high(FLASH_ENABLE_PIN);    // disable flash - raise chip select
		delay_ms(1);	
		*manufact_id=ReadBuf[1];
		*device_id=ReadBuf[2];
	}


// Checks the status bit from the flash and returns when complete or after MAX_SEC seconds, whichever is first.
void waitForFlash(void) {
    auto int n;
    auto unsigned char stat[3];
	const int MAX_SEC = 120;				// Sets the maximum time to wait

    n = 0; 									
    flashStatus(stat);						// Checks the flash status bits
	while ((stat[1] & 0x01) && (n < (MAX_SEC*100)))   // check flash status bit and timeout
    {
        restart_wdt();
		flashStatus(stat);					// Checks the flash status bits
	    delay_ms(10);						// with a 10 ms delay, MAX_SEC seconds is MAX_SEC*100 cycles
        n++;
    }
}


// Copies data from 1 place in flash to another
// Maximum write is 1 page, so we'll have to iterate over the pages.
ReturnErr_t flashCopy(unsigned long source_addr, unsigned long dest_addr, unsigned int len)
{
	unsigned char CopyBuf[FLASH_PAGE_SIZE];	// Cannot be replaced with global ReadBuf because of collisions in flashWriteSafe()	

	// Enforce 1 page at a time
	if(len>FLASH_PAGE_SIZE) return FAILURE;

	//Read it
	flashRead(source_addr,len,CopyBuf);

	//Write it
	return flashWriteSafe(dest_addr,len,CopyBuf,dest_addr,dest_addr+len);
}


	void flashWriteEnable()
	{
	      unsigned char WriteBuf[2];
		  unsigned char ReadBuf[2];
	
		  output_low(FLASH_ENABLE_PIN);                        // enable flash - lower chip select
	      delay_ms(1);
	      WriteBuf[0] = FLASH_WRITE_ENABLE;               		 // This is write enable command
	      SPIWrRd(WriteBuf, 1, ReadBuf, 1);   		 
	      output_high(FLASH_ENABLE_PIN);                       // disable flash - raise chip select
		  delay_ms(1);              				 // Hold here to make sure PIC completes the transfer
	}

	


	// Returns 0 if unprotected.   1 if protected.
	unsigned char flashReadSectorProtection(unsigned long address)
	{
		return 0;	// No protection on MP25E20
	}	

	
	// Disables sector protection on the ATMEL chip.  Will only work if Writing has been Enabled (0x06) previously
	// The address it takes is BEFORE reversal
	void flashSectorProtectDisable(unsigned long address)
	{
		return;	// No protection on MP25E20
	}

	void flashSectorProtectDisable(unsigned int sector)
	{
		return;	// No protection on MP25E20
	}	
	
	
	void flashWriteDisable()
	{
	      unsigned char WriteBuf[2];
		  unsigned char ReadBuf[2];
	
	      output_low(FLASH_ENABLE_PIN);                          // enable flash - lower chip select
	      delay_ms(1);
	      WriteBuf[0] = FLASH_WRITE_DISABLE;                		 // This is write disable command
	      SPIWrRd(WriteBuf, 1, ReadBuf, 1);   		 
	      output_high(FLASH_ENABLE_PIN);                         // disable flash - raise chip select
	}
	
	// Converts 3 chars to a long representing a flash address
	unsigned long CharsToFlashAddr(unsigned char addbyte1, unsigned char addbyte2, unsigned char addbyte3)
	{
		unsigned long retval;
	
		retval = (long)addbyte1 * 65536 + (long)addbyte2 *256 + (long)addbyte3;
	
		return retval;
	}


	// Converts the last 3 bytes of a long to individual chars.
	// Example: Long is 00000000 00000010 00000001 1000000
    // Char 0 is 2nd byte:  00000010 --> 0x02
	// Char 1 is 3rd byte:  00000001 --> 0x01
	// Char 2 is last byte: 10000000 --> 0x80
	void LongTo3Chars(unsigned long address, unsigned char * threebytearray)
	{
		threebytearray[0] = (unsigned char)(address>>16 & 0x000000FF);   
		threebytearray[1] = (unsigned char)(address>>8 & 0x000000FF);	 
		threebytearray[2] = (unsigned char)(address & 0x000000FF); 
	}	

	// Convert Flash Addresses to Sector Number
	unsigned int FlashAddrToSector(unsigned long addr)
	{
		unsigned int temp;
		
		temp = (addr>>16);  // Equivalent to dividing by 65536 (size of sector)
		return temp;
	}

	// Convert Flash Addresses to Block Number
	unsigned int FlashAddrToBlock(unsigned long addr)
	{
		unsigned int temp;
		
		temp = (addr>>12); // Equivalent to dividing by 4096 (size of block)
		return temp;
	}

	// Convert Flash Addresses to Page Number
	unsigned int FlashAddrToPage(unsigned long addr)
	{
		unsigned int temp;
		
		addr = (addr>>8); // Equivalent to dividing by 256 (size of page)
		temp = addr;
		return temp;
	}

	void SectorToFlashAddr(unsigned int sector, 
								unsigned long * begin, 
								unsigned long * end)
	{
		unsigned long temp;
		
		temp = (unsigned long)sector;
		*begin = (temp<<16);
		*end = *begin + 0x00FFFF;	
	}

	void BlockToFlashAddr(unsigned int block, 
								unsigned long * begin, 
								unsigned long * end)
	{
		unsigned long temp;
		
		temp = (unsigned long)block;
		temp = (temp<<12);				// DO NOT CHANGE! It looks like an extra line compared to the others.  But for some reason it only works this way.
		*begin = temp;
		*end = *begin + 0x000FFF;	
	}

	void PageToFlashAddr(unsigned int page, 
								unsigned long * begin, 
								unsigned long * end)
	{
		unsigned long temp;
		
		temp = (unsigned long)page;
		*begin = (temp<<8);
		*end = *begin + 0x0000FF;	
	}

	// Reads a length of flash from addr into buf
	void flashRead(unsigned long addr, unsigned int len, unsigned char * buf)
	{
		unsigned char WriteBuf[4];     // This is a holder for the opCode and address bytes
	   
	    flashAddr(addr, WriteBuf + 1);   // Compute and Save the address to send.  3 bytes
 	    WriteBuf[0] = FLASH_READ; 		 // This is read command
 		
	    output_low(FLASH_ENABLE_PIN); 			// enable flash - lower chip select
	    //delay_ms(1);                    // Hold here to make sure PIC completes sending last byte	
        SPIWrThenRd(WriteBuf, 4, buf, len); // Go read the flash into ReadBuf
	    output_high(FLASH_ENABLE_PIN);             // disable flash - raise chip select	
	}



	// Returns True (non-zero) or False (zero) if the area in flash denoted by the inputs is empty.
	unsigned char CheckFlashEmpty(unsigned long address,unsigned int len)
	{
		unsigned char ReadBuf[FLASH_PAGE_SIZE+1];	// Flash can only write a page at a time.  So this should be all we need to read.
		int i;
		int sz;		

		// Validate
		if(address+len>MAX_FLASH_ADDR) return -1;

		// Loop while there's still more to read
		while(len>0)
		{
			// If we want to read more than a page
			if(len>=FLASH_PAGE_SIZE)
			{
				// Read just a page
				flashRead(address,FLASH_PAGE_SIZE,ReadBuf); // Reads a page from flash into the buffer
				sz=FLASH_PAGE_SIZE;
				address+=FLASH_PAGE_SIZE;
				len-=FLASH_PAGE_SIZE;
			}
			else // if we want to read less than a page
			{
				// Read less than a page
				flashRead(address,len,ReadBuf); // Reads a length from flash into the buffer
				sz=len;
				address+=len;
				len-=len;
			}

			// This loop checks if each byte is full of 1's (erased).  If it makes it through the loop, 
			// the area in flash is erased.  If anything is not 0xFF, it returns false.	
			for(i=0;i<sz;i++)
			{
				//DEBUG:
				//HexPrint(USER_PORT,&ReadBuf[i],1);

				//--------
				if(ReadBuf[i] != 0xFF) 
					{
						//sprintf(dbgbuf,"%02x!=%02x ",ReadBuf[i],0xFF);
						//sendDBGALL(USER_PORT,dbgbuf);
						return 0;
					}
			}
		}//while
		return 1;
	}



	void flashEraseBlock(unsigned int block)
	{
		unsigned long addr_begin, addr_end;
		
		// Convert block to address.
		BlockToFlashAddr(block,&addr_begin,&addr_end);
		
		// Erase the block.
		flashEraseBlock(addr_begin);			// use of overloaded function

	}

	void flashEraseBetweenBlocks(unsigned int start_block, unsigned int end_block)		// Erases blocks, inclusive
	{
		unsigned int i;
		for (i=start_block;i<=end_block;i++){
			flashEraseBlock(i);
		}	
	}


	
	// Erases a block (16 pages) of flash
	void flashEraseBlock(unsigned long addr)
	{
		unsigned char WriteBuf[4];     				  // max write is FLASH_PAGE_SIZE, need up to 6 bytes for flash command and address
	    unsigned char ReadBuf[4];
	
		flashWriteEnable();					  // Enable writing to flash

	    flashAddr(addr, WriteBuf + 1);   	  // Compute and Save the address to send.  3 bytes

	    output_low(FLASH_ENABLE_PIN); 				  // enable flash - lower chip select
	    delay_ms(1);                          // Hold here to make sure PIC completes sending last byte

	    WriteBuf[0] = FLASH_ERASE_BLOCK;	  // This is the block erase command
	    SPIWrRd(WriteBuf, 4, ReadBuf, 4); 	  // Send the command

	    output_high(FLASH_ENABLE_PIN);        // disable flash - raise chip select	
		waitForFlash();						  // Make sure flash is ready before releasing
	}

	void flashGlobalUnprotect()
	{
		unsigned char WriteBuf[4];     				  // max write is FLASH_PAGE_SIZE, need up to 6 bytes for flash command and address
	    unsigned char ReadBuf[4];
		const unsigned char opcode=FLASH_WRITE_STATUS_REGISTER;	// Write the status register
		const unsigned char status_register = B8(00000000);	// This bit combination unlocks all sectors and keeps the protection bit unlocked
		
		flashWriteEnable();					  // Enable writing to flash

	    output_low(FLASH_ENABLE_PIN); 		  // enable flash - lower chip select
	    delay_ms(1);                          // Hold here to make sure PIC completes sending last byte

	    WriteBuf[0] = opcode;				  // 
		WriteBuf[1] = status_register;		  // 
	    SPIWrRd(WriteBuf, 2, ReadBuf, 4); 	  // Send the command

	    output_high(FLASH_ENABLE_PIN);        // disable flash - raise chip select	
		waitForFlash();						  // Make sure flash is ready before releasing

	}


	// Erases the whole chip
	void flashEraseChip()
	{
		unsigned char WriteBuf[4];     				  // max write is FLASH_PAGE_SIZE, need up to 6 bytes for flash command and address
	    unsigned char ReadBuf[4];
	
	    //flashAddr(addr, WriteBuf + 1);   	  // Compute and Save the address to send.  3 bytes

		flashWriteEnable();					  // Enable writing to flash

	    output_low(FLASH_ENABLE_PIN); 				  // enable flash - lower chip select
	    delay_ms(1);                          // Hold here to make sure PIC completes sending last byte

	    WriteBuf[0] = FLASH_ERASE_CHIP;		  // Erase whole chip command
	    SPIWrRd(WriteBuf, 1, ReadBuf, 4); 	  // Send the command

	    output_high(FLASH_ENABLE_PIN);        // disable flash - raise chip select	
		waitForFlash();						  // Make sure flash is ready before releasing
	}

	
	void flashWrite(unsigned long addr, unsigned int len, unsigned char * buf)
	{	
		unsigned char WriteBuf[FLASH_PAGE_SIZE+6];

		if(len>FLASH_PAGE_SIZE) return;		  // bail if size is too big.  TODO:  Convert this to returnerr_t failure.

		WriteBuf[0] = FLASH_PAGE_WRITE;		  // This is the "easy" writing command with pre-erase.
	    flashAddr(addr, WriteBuf + 1);   	  // Next 3 bytes are the address
	    memcpy(WriteBuf + 4, buf, len);       // Copy data into the writing buffer
        
		flashWriteEnable();					  // Enable writing to flash

	    output_low(FLASH_ENABLE_PIN); 		  // enable flash input - lower chip select
	    delay_ms(1);                          // Hold here to make sure PIC completes sending last byte
        SPIWrThenRd(WriteBuf, len + 4, NULL, 0);   		// Send it off
	    output_high(FLASH_ENABLE_PIN);                  // complete the command - raise chip select
		delay_ms(1);
	}


	void flashCopyPage(unsigned int destination_page, unsigned int source_page)
	{
		
		unsigned char ReadBuf[FLASH_PAGE_SIZE+1];
	// Compute the address of the page.  Read it.  Compute address of destination.  Write it.
	
		unsigned long start_addr,end_addr;

		PageToFlashAddr(source_page,&start_addr,&end_addr);			// Compute address of page.
		flashRead(start_addr,FLASH_PAGE_SIZE,ReadBuf);				// Read it.
		delay_ms(1);                          						// Hold here to make sure PIC completes copying
		PageToFlashAddr(destination_page,&start_addr,&end_addr);	// Compuate address of destination
		flashWrite(start_addr,FLASH_PAGE_SIZE,ReadBuf);				// Write it.
	}
	

	void flashCopyBlockExceptPage(unsigned int destination_block, unsigned int source_block, unsigned int page_to_except)
	{
		// Loop over pages in source block, copying each to destination except for the page_to_except
		
		int i;
		unsigned int source_start_page, source_end_page;
		unsigned int dest_start_page, dest_end_page;
		unsigned long start_addr, end_addr;


		// First, what pages are we dealing with on the source?
		BlockToFlashAddr(source_block, &start_addr, &end_addr);	// Convert block # to start/end addresses
		source_start_page = FlashAddrToPage(start_addr);		// Store Start Page
		source_end_page = FlashAddrToPage(end_addr);			// Store End Page
	
		// Next, what pages are we dealing with on the destination?
		BlockToFlashAddr(destination_block, &start_addr, &end_addr);	// Convert block # to start/end addresses
		dest_start_page = FlashAddrToPage(start_addr);			// Store Start Page
		dest_end_page = FlashAddrToPage(end_addr);				// Store End Page
	
		// Finally, loop over the pages, copy all except the chosen one
		for(i=0;i<16;i++)
		{
			if(source_start_page+i != page_to_except)
				flashCopyPage(dest_start_page+i,source_start_page+i);
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
ReturnErr_t flashWriteSafe(unsigned long addr, 
									  int len, 
									unsigned char * buf,
					 unsigned long lower_bound,
					 unsigned long upper_bound)
	{
		unsigned char cleared=0;
		unsigned long ending_addr, page_boundary, throwaway;
		unsigned int startPage, endPage, currentPage;
		unsigned int length_to_write;
	
		waitForFlash();			  // First wait that everything is ready

		//sprintf(dbgbuf,"\r\nReceived %u,%i,%u,%u",addr,len,lower_bound,upper_bound);
		//sendMSGALL(USER_PORT,dbgbuf);

		//------Validation of inputs------

		// Compute upper address we're goign to write to
		ending_addr = addr + len;

		// Check the specified memory bounds
			//Upper		
			if(ending_addr>upper_bound) return OUT_OF_BOUNDS;
			if(addr>upper_bound) return OUT_OF_BOUNDS;
			// Lower
			if(ending_addr<lower_bound) return OUT_OF_BOUNDS;
			if(addr<lower_bound) return OUT_OF_BOUNDS;
			// Chip
			if(ending_addr>MAX_FLASH_ADDR) return OUT_OF_BOUNDS;
			if(addr>MAX_FLASH_ADDR) return OUT_OF_BOUNDS;			

		// Check if sector protection is on, for both the lower and upper addresses
		if(flashReadSectorProtection(addr))   
			return FLASH_PROTECTED;
		if(flashReadSectorProtection(ending_addr))   
			return FLASH_PROTECTED;

		// Checks if area to write is empty
		cleared = CheckFlashEmpty(addr,len);  
		if(!cleared) 
			return FLASH_NOT_EMPTY;
	
		// Compute Range of Pages we are going to write		
		startPage = FlashAddrToPage(addr);
		endPage = FlashAddrToPage(ending_addr);

		if(startPage == endPage)	// If they're the same, just write it and get out of here!
		{
			flashWrite(addr,len,buf);
		}
		else						// Otherwise we have to loop across the pages, splitting as we go
		{
			
			// Validate
			if(startPage>endPage) return SUBFUNCTION_ERR;		// endPage should be bigger, otherwise bail
			
			// For each page, check if we're writing past it.  If yes, write just to the page boundary.
			// If no, write to the end and quit.
			for(currentPage=startPage;currentPage<=endPage;currentPage++)
			{
				PageToFlashAddr(currentPage,&throwaway,&page_boundary);	// Get a new page boundary (upper) for the current page
				if(ending_addr>page_boundary)						    // Check if we're writing past that boundary
				{
					length_to_write = page_boundary-addr+1;				// If we are, instead compute the length to write to get us to the boundary
					flashWrite(addr,length_to_write,buf);				// write to flash to the page boundary						
					addr += length_to_write;							// update address index for what you just wrote
					buf += length_to_write;								// update buffer index for what you just wrote
					waitForFlash();										// give the flash a sec to finish
				}
				else // last page
				{
					length_to_write = ending_addr-addr;					// If not writing past that boundary, length is just from where we are to the original ending addres			
					flashWrite(addr,length_to_write,buf);				// write to flash to the end
					if(currentPage != endPage) return FAILURE;			// Validation Check: the only way we should be here is when we're on the last page.  Check that.
				}				
			}//for loop
		}// else (across multiple pages)
		
		return SUCCESS;
		
	}//flashWriteSafe


	
	void flashCopyBlock(unsigned int destination_block, unsigned int source_block)
	{
		// Loop over pages in source block, copying each to destination 
		
		int i;
		unsigned int source_start_page, source_end_page;
		unsigned int dest_start_page, dest_end_page;
		unsigned long start_addr, end_addr;


		// First, what pages are we dealing with on the source?
		BlockToFlashAddr(source_block, &start_addr, &end_addr);	// Convert block # to start/end addresses
		source_start_page = FlashAddrToPage(start_addr);		// Store Start Page
		source_end_page = FlashAddrToPage(end_addr);			// Store End Page address
	
		// Next, what pages are we dealing with on the destination?
		BlockToFlashAddr(destination_block, &start_addr, &end_addr);	// Convert block # to start/end addresses
		dest_start_page = FlashAddrToPage(start_addr);			// Store Start Page
		dest_end_page = FlashAddrToPage(end_addr);				// Store End Page
	
		// Finally, loop over the pages in a block, copying all 
		for(i=0;i<16;i++)
		{
			flashCopyPage(dest_start_page+i,source_start_page+i);	
		}	
	}

// Reads from flash to the specified port, one page at a time
void flashReadToPort(int port, unsigned long addr, unsigned long len)
{
	unsigned char ReadBuf[FLASH_PAGE_SIZE+1];
	unsigned long end_addr;

	end_addr = addr + len;

	while(addr<end_addr)
	{
		flashRead(addr,FLASH_PAGE_SIZE,ReadBuf);	//read a page into the buf
		sendMSG(port,ReadBuf,FLASH_PAGE_SIZE);		//send it off
		addr = addr + FLASH_PAGE_SIZE;			//update address
		delay_ms(100);							// give it some time to clock it out the port
	}
}


// Returns the first completely empty flash page found between 
// the specified start and end pages, inclusive.  
// Returns:  SUCCESS or FAILURE, and a modified returnpage
ReturnErr_t flashFindEmptyPage(unsigned int startpage,
							   unsigned int endpage,
							   unsigned int * returnpage)
{
	// Declare locals
	unsigned int currentpage;
	unsigned char empty;
	unsigned long begin,end;

	//validate inputs
	if(endpage<startpage) return FAILURE;

	// initialize locals
	currentpage = startpage;
	empty = 0;
	unsigned char c;

	for(currentpage=startpage;currentpage<=endpage;currentpage++)
	{
		getByte(USER_PORT,&c);
		if(c==0x1B) // if user presses Esc
			return FAILURE;	//abort
		//sprintf(dbgbuf,"\r\nP:%u",currentpage);
		//sendDBGALL(USER_PORT,dbgbuf);
		PageToFlashAddr(currentpage,&begin,&end);   		// Find address of the page
		//sprintf(dbgbuf,"\tB:%u,E:%u",begin,end);
		//sendDBGALL(USER_PORT,dbgbuf);
		empty = CheckFlashEmpty(begin,FLASH_PAGE_SIZE);		// Check the page for emptiness
		if(empty==1)										// if Empty...
		{
			*returnpage=currentpage;						// Set the return page
			return SUCCESS;									
		}
	}
	return FAILURE;											// If nothing found, return FAILURE and no modifications to the returnpage
}


// Safely Erases a page from flash memory, using the swap block to copy things over
void flashErasePage(unsigned int page)
{
	unsigned long addr,temp;				// Addresses for deletion page
	unsigned char WriteBuf[4];     				 
    unsigned char ReadBuf[4];

	// First, compute where we are...
	PageToFlashAddr(page,&addr,&temp);

	// Next, erase the page.
	flashWriteEnable();					  // Enable writing to flash

    flashAddr(addr, WriteBuf + 1);   	  // Compute and Save the address to send.  3 bytes. 

    output_low(FLASH_ENABLE_PIN); 		  // enable flash - lower chip select
    delay_ms(1);                          // Hold here to make sure PIC completes sending last byte

    WriteBuf[0] = FLASH_ERASE_PAGE;	  	  // This is the block erase command
    SPIWrRd(WriteBuf, 4, ReadBuf, 4); 	  // Send the command

    output_high(FLASH_ENABLE_PIN);                  // disable flash - raise chip select	
	waitForFlash();						  // Make sure flash is ready before releasing

	return;
}


void IncrementAddrCircular(unsigned long * addr,
						 	   unsigned long lower_limit, 
							   unsigned long upper_limit,
							   unsigned long increment)
{
	// Bail if invalid input
	if (*addr<lower_limit || *addr>upper_limit) return;		// Checks for valid input
	if (*addr+increment<lower_limit) return; 			// Looks redundant, but checks if the unsigned long rolls over the top (sorta)

	*addr += increment;

	if(*addr>upper_limit)
		*addr = lower_limit + (*addr - upper_limit - 1);

	return;

/*  IncrementAddrCircular  increments an address in a circular buffer fashion
	Example usage:
	  	addr = 5
		lower limit = 2
		upper limit = 10
		increment = 7
	Note: 5+5=10, 5+6=2, 5+7=3

	Here's how:
	5 + 7 = 12
	12 > 10, so addr becomes 
	lower_limit + (*addr - upper_limit - 1)
	2 + (12 - 10 - 1) = 2 + 1 = 3
*/
}


void DecrementAddrCircular(unsigned long * addr,
						 	   unsigned long lower_limit, 
							   unsigned long upper_limit,
							   unsigned long decrement)
{
	// Bail if invalid input
	if (*addr<lower_limit || *addr>upper_limit) return;		// Checks for valid input
	if (decrement>addr) return; 	// Checks for a potential negative value

	*addr -= decrement;

	if(*addr<lower_limit)
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
	void statusReport(int port, unsigned char statusbyte1, unsigned char statusbyte2)
	{
		const unsigned char STATUS_BIT = 0x01;
		const unsigned char ENABLE_BIT = 0x02;
		const unsigned char SPS_SOME	  = 0x04;
		const unsigned char SPS_ALL    = 0x04 | 0x08;
		const unsigned char WPP_BIT	  = 0x10;
		const unsigned char ERROR_BIT  = 0x20;
		const unsigned char SECT_PROT  = 0x80;
		
		// Check each bit of the status and attach a description based on the bit
		if((statusbyte1 & STATUS_BIT) == STATUS_BIT)
		{
			sendMSGALL(port,"\r\nBit 0 = 1 : Device is busy.");
		}
		else
		{
			sendMSGALL(port,"\r\nBit 0 = 0 : Device is ready.");
		}			
		
		// Check each bit of the status and attach a description based on the bit
		if((statusbyte1 & ENABLE_BIT) == ENABLE_BIT)
		{
			sendMSGALL(port,"\r\nBit 1 = 1 : Device is write-enabled.");
		}
		else
		{
			sendMSGALL(port,"\r\nBit 1 = 0 : Device is NOT write-enabled.");
		}
	
		// Check each bit of the status and attach a description based on the bit
		if((statusbyte1 & SPS_ALL) == SPS_ALL)
		{
			sendMSGALL(port,"\r\nBit2&3= 1 : All sectors software-protected.");
		}
		else if ((statusbyte1 & SPS_SOME) == SPS_SOME)
		{
			sendMSGALL(port,"\r\nBit2|3= 1 : Some or all sectors NOT software-protected.");
		}
		else
		{
			sendMSGALL(port,"\r\nBit2&3= 0 : All sectors NOT software-protected.");
		}
	
		// Check each bit of the status and attach a description based on the bit
		if((statusbyte1 & WPP_BIT) == WPP_BIT)
		{
			sendMSGALL(port,"\r\nBit 4 = 1 : WP is deasserted.");
		}
		else
		{
			sendMSGALL(port,"\r\nBit 4 = 0 : WP is asserted.");
		}
	
		// Check each bit of the status and attach a description based on the bit
		if((statusbyte1 & ERROR_BIT) == ERROR_BIT)
		{
			sendMSGALL(port,"\r\nBit 5 = 1 : Last op error detected.");
		}
		else
		{
			sendMSGALL(port,"\r\nBit 5 = 0 : Last op successful.");
		}
	
		// Check each bit of the status and attach a description based on the bit
		if((statusbyte1 & SECT_PROT) == SECT_PROT)
		{
			sendMSGALL(port,"\r\nBit 7 = 1 : Sector Protection Registers locked.");
		}
		else
		{
			sendMSGALL(port,"\r\nBit 7 = 0 : Sector Protection Registers unlocked.");
		}		
	}
	
