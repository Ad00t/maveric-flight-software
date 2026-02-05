///////////////////////////////////////////////////////////////////////////
////                         loader_pcd.c                              ////
////                                                                   ////
////  Totally rewritten.

#include "pcd_bootloader.h"

#ifdef _bootloader

#warning Bootloader size LOADER_SIZE between LOADER_ADDR and LOADER_END
#define BUFFER_LEN_LOD 64

/*  //Unused
#define ACKLOD 0x06
#define XON    0x11
#define XOFF   0x13
*/

//#bit SPITBF = 0x0240.1

#SEPARATE
unsigned int8 atoi_b16(unsigned char *s);
#SEPARATE
void bootload(void);
#SEPARATE
void bootflashAddr(unsigned long tempad, unsigned char *addr);
#SEPARATE
void bootSPIWrThenRd(unsigned char *Write, unsigned int writeL, unsigned char *Read, unsigned int readL);
#SEPARATE
void mike_delay_ms(int ms);
#SEPARATE
void bootFlash(unsigned long addr, unsigned int len, unsigned char * buf);


#org default

#org LOADER_ADDR+10, LOADER_END auto=0 default

void real_load_program ()
{
   unsigned int1  done=FALSE;
   unsigned int8  checksum, line_type;
   unsigned int16 l_addr,h_addr=0;
   unsigned int32 addr;
   #if getenv("FLASH_ERASE_SIZE")>2
      unsigned int32 next_addr;
   #endif
   unsigned int8  dataidx, i;
   unsigned int8 count=0;
   unsigned int8  data[32];
   unsigned int16  buffidx;
   unsigned char buffer[BUFFER_LEN_LOD];
   unsigned int32 address_erase;
   unsigned int8 Condition1;
   unsigned int8 Condition2;
   unsigned int8 RetryCounter=0;
   
#warning getenv("FLASH_ERASE_SIZE")

	// LEDs
//   output_low(PIN_G0);
//   output_low(PIN_A15);

	// Disable interrupts
	setup_wdt(WDT_OFF);
	disable_interrupts(INTR_GLOBAL);

	// Clear the buffer
	memset(buffer,'0',sizeof(buffer));

//   output_high(PIN_G1);
//   fputc('S',COM_B);

   // Modified to save the bootloader from itself!!!
   for(address_erase=0;address_erase<(LOADER_ADDR)-(getenv("FLASH_ERASE_SIZE")/2);address_erase+=(getenv("FLASH_ERASE_SIZE")/2))
   {
	// Flash erase size is 2048 bytes (0x800)
	// So this steps from 0 to 173054 bytes  (0x0 to 0x2A3FE) in chunks of 0x400
	// 

     erase_program_memory(address_erase);
   }
//   fputc('E',COM_B);
//   output_low(PIN_G1);

	address_erase=0;	// Re-using address_erase as our reading pointer now

   while (!done)  // Loop until the entire program is downloaded
   {
      buffidx = 0;  
	  // Fill up the buffer
RETRY:	 
	  RetryCounter++;
	  bootFlash(HEXFILE_ADDR+address_erase,BUFFER_LEN_LOD-1,buffer);	// SEEMS BROKEN!
	 
	  // Process the buffer
	  // We want the buffer index to be 1 spot past the line return
      do {
//			output_toggle(PIN_G1);
//			mike_delay_ms(50);
//			bootFlash(HEXFILE_ADDR+address_erase,1,&buffer[buffidx]);
			address_erase++;		// Increment read pointer
			Condition1 = (buffer[buffidx] != '\r');
			buffidx++;
			Condition2 = (buffidx <= BUFFER_LEN_LOD);			
      } while ( Condition1 && Condition2 );
		
      // Only process data blocks that start with ':'
      if (buffer[0] == ':') {
//	    fputc('L',COM_B);

        count = atoi_b16 (&buffer[1]);  // Get the number of bytes from the buffer

         // Get the lower 16 bits of address
         l_addr = make16(atoi_b16(&buffer[3]),atoi_b16(&buffer[5]));

         line_type = atoi_b16 (&buffer[7]);

         addr = make32(h_addr,l_addr);

         addr /= 2;

         // If the line type is 1, then data is done being sent
         if (line_type == 1) {
            done = TRUE;
         }else if ((addr < LOADER_ADDR )){ //|| addr < LOADER_END) && addr < 0x300000)
            checksum = 0;  // Sum the bytes to find the check sum value
            for (i=1; i<(buffidx-3); i+=2)
               checksum += atoi_b16 (&buffer[i]);
            checksum = 0xFF - checksum + 1;

			// Test the checksum of the line
            if (checksum != atoi_b16 (&buffer[buffidx-3]))
			{
				// If it fails, let's try again (up to 5 times)  
				if(RetryCounter<=5) goto RETRY;
//	           output_high(PIN_A14);
//				   fputc('X',COM_B);
			}
            else   {
				// If the checksum succeeds, reset the RetryCounter
				RetryCounter = 0;
               if (line_type == 0) {
                  // Loops through all of the data and stores it in data
                  // The last 2 bytes are the check sum, hence buffidx-3

                  for (i = 9,dataidx=0; i < buffidx-3; i += 2)
                     data[dataidx++]=atoi_b16(&buffer[i]);
                  
                  #if getenv("FLASH_ERASE_SIZE") > getenv("FLASH_WRITE_SIZE")
                     #if defined(__PCM__)
                        if ((addr!=next_addr)&&(addr&(getenv("FLASH_ERASE_SIZE")-1)!=0))
                     #else
                        if ((addr!=next_addr)&&(addr&(getenv("FLASH_ERASE_SIZE")/2-1)!=0))
                     #endif
                     next_addr = addr + 1;
                  #endif

                  write_program_memory(addr, data, count);
//				  output_toggle(PIN_G1);
//				  fputc('W',COM_B);

                }
           
               else if (line_type == 4)
                  h_addr = make16(atoi_b16(&buffer[9]), atoi_b16(&buffer[11]));
             }//else checksum check

         }// else if address check
      else{
 //  		  output_high(PIN_A14);	
		}//else
      }// if line starts with colon check
      else
      {
		RetryCounter = 0;		// Reset the retry counter
//	    fputc('N',COM_B);
      }
   }// while
   
   reset_cpu();
}


unsigned int8 atoi_b16(unsigned char *s) {  // Convert two hex characters to a int8
   unsigned int8 result = 0;
   unsigned int8 i;

   for (i=0; i<2; i++,s++)  {
      if (*s >= 'A')
         result = 16*result + (*s) - 'A' + 10;
      else
         result = 16*result + (*s) - '0';
   }

   return(result);
}

void mike_delay_ms(int ms)
{
	int i,j;
	for(i=0;i<ms;i++)
	{
		for(j=0;j<1000;j++)	{};
	}
	return;
}

void bootflashAddr(unsigned long tempad, unsigned char *addr) {
	int len;
    int n;
	unsigned char * buf2;

	buf2 = &tempad;
	len = 3;

    for (n = 0; n < len; n++)
        addr[n] = buf2[len - n - 1];
}


void bootFlash(unsigned long addr, unsigned int len, unsigned char * buf)
{
	unsigned char WriteBuf[4];     // This is a holder for the opCode and address bytes
	   
	bootflashAddr(addr, WriteBuf + 1);   // Compute and Save the address to send.  3 bytes
 	WriteBuf[0] = FLASH_READ; 			 	 // This is read command

//	HexPrint(USER_PORT,WriteBuf,4);	// Debug

	output_low(FLASH_CHIP_SELECT); 				 // enable flash - lower chip select
//	mike_delay_ms(1);                    // Hold here to make sure PIC completes sending last byte	
    bootSPIWrThenRd(WriteBuf, 4, buf, len); // Go read the flash into ReadBuf
	output_high(FLASH_CHIP_SELECT);                // disable flash - raise chip select	
}

// Writes first, then reads.  No transformations needed upon return to parent function
// This is for any command that reads data after writing to flash (status, reading)
void bootSPIWrThenRd(unsigned char *Write, unsigned int writeL, unsigned char *Read, unsigned int readL) 
{
	unsigned int n;
	unsigned char discard;

	#if defined(UPPER_PPM)
    	#define FLASH_REGISTER_SETTING		0x033B			// 0000 0011 0011 1111
	#else
		#define FLASH_REGISTER_SETTING		0x027B			// 0000 0011 011 111 11
	#endif


	// Set SPI to optimal settings for flash by bitbanging registers
	bit_clear(SPISTAT,15);		// Clear the SPI Enable bit, thus allowing rewrites to the control register.
	SPICON1=FLASH_REGISTER_SETTING;				// Write the control register with new settings
	bit_clear(SPISTAT,6);		// Clear the SPIROV bit.
	bit_set(SPISTAT,15);		// Set the SPI Enable bit.	
	mike_delay_ms(1);           // Hold here to make sure PIC completes register work	

	#undef FLASH_REGISTER_SETTING


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

#org default

#ORG LOADER_ADDR, LOADER_ADDR+9
void bootload(void)
{
      real_load_program();
}

#endif //bootloader

#org default
