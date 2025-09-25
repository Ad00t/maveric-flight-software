/***************************************************************************

									imi100.c

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Provide hardware interface to IMI-100 ADACS unit.

Changelog:

Date 	 |  Au.  |	Notes
07-15-10   Will 	Initial revision.		

****************************************************************************

*/

#include "imi100.h"		// Public header

#module					// !!! Important: This command makes everything below scoped only to this file.

//========================================
//    		Note on Timing
//========================================

// According to the manual, the IMI-100 does the following tasks in order every 250ms:
// 	1. Sensor Aquisition
//  2. Attitude Determination
//  3. Control Law Computation
//  4. Wheels and Coils Commanded (at least 94ms)


//========================================
//  		Defines
//========================================

// Below are constants and scaling factors specified for the IMI-100
#define DEG_PER_SEC_PER_BIT 	6.0				// See IMI-200 Documentation (1 rpm/lsb, sec 24.3.3)
#define MAX_SPEED_IN_BITS 		10000			// See IMI-200 Documentation (10,000 rpm max)
#define TLM_PACKET_LENGTH 		149				// See IMI-100 Documentation (same in 200)
#define AM2_PER_BIT 			0.000297		// See IMI-100 Documentation (same in 200)
#define MNM_PER_BIT				0.005 			// See IMI-100 Documentation (same in 200)
#define RADS_PER_SEC_PER_BIT    0.104719755 	// Conversion for the IMI200: 6 degs per sec per bit mutliplied by pi/180 to get rads per second per bit


// i2c configuration
// Pumpkin Documentation INCORRECT!
// See Datasheet for TI PCA9534A.  Must include the LSB of 0 to indicate writing!
#define IO_SMB_ADDR 			0x7E		// Address of '9534A I/O I2C expander port

//========================================
//  		Driver Functions
//========================================

unsigned int makeIMICRC(unsigned char * packet, int length);
ReturnErr_t decodeTelemetry(unsigned char * tlmbuf, imi_s * telemetry);

//========================================
//  		Local Globals
//========================================

//unsigned char packet[40];

//========================================
// 		Development Functions
//========================================

/*=================================================================
  PURPOSE: Initialize IMI-100 Interface
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/

void imi100_init(int port, short on)
{
	unsigned char config[2];
	unsigned char output[2];

	// Turn IMI-100 on and enable MAX232 chip
	if(on)
	{
		// Manual override (bypasses I2C) via discrete I/O. 
		// Open-drain output, 0V out (low)  
		//ODCE   = 0x0000+BIT8;
		//PORTE &=       ~BIT8;
		//TRISE &=       ~BIT8;

		//output_low(IMI_PWR);

		// Turn on the MAX232 converter:
		config[0] = 0x03;
		config[1] = 0xF0;

		output[0] = 0x01;
		output[1] = 0x0E;
	
		/* From Pumpkin Code:

			// Configure '9534A port expander via two I2C commands.
			// U4 on ADACS I/F needs:
			//  -EN           0   // for output from U1
			//   FORCEON      1   // override req'd -- dunno why
			//  -FORCEOFF     1   // no need to override
			//   ON           1   // want stuff ON
			//  i.e. 0xbbbb1110 as outputs.
			csk_i2c0_write(IO_SMB_ADDR, config, 2); 
			csk_i2c0_write(IO_SMB_ADDR, output, 2); 
		*/

	}//if
	// Turn off IMI-100 and disable MAX232 chip
	else
	{
		config[0] = 0x03;
		config[1] = 0xF0;

		output[0] = 0x01;
		output[1] = 0x05;
    
		// Manual override (bypasses I2C) via discrete I/O.
		// Open-drain output, high out   
		//ODCE   = 0x0000+BIT8;
		//PORTE |=        BIT8;
		//TRISE &=       ~BIT8;

		//output_high(IMI_PWR);
		
		/* From Pumpkin Code:

			// Configure '9534A port expander via two I2C commands.
			// U4 on ADACS I/F needs:
			//  -EN           1   // for output from U1
			//   FORCEON      0   // no need to override
			//  -FORCEOFF     1   // no need to override
			//   ON           0   // want stuff ON
			//  i.e. 0xbbbb0101 as outputs.
			//  Due to 2N2222 schematic capture error, Q1 is mis-pinned out, so ON does nothing.
			csk_i2c0_write(IO_SMB_ADDR, config, 2); 
			csk_i2c0_write(IO_SMB_ADDR, output, 2); 
		*/
	}

	// Enable the RS232 Converter:


	i2c_start();
	i2c_write(IO_SMB_ADDR);
	i2c_write(config[0]);
	i2c_write(config[1]);
	i2c_stop();

	i2c_start();
	i2c_write(IO_SMB_ADDR);
	i2c_write(output[0]);
	i2c_write(output[1]);
	i2c_stop();

	return;
}

/*=================================================================
  PURPOSE: Create CRC of packet except last two bytes.
  UNMODIFIED VARIABLES: unsigned char * packet - Contains raw bytes to CRC.
						int length - Length of full packet.
  MODIFIED VARIABLES: None
  GLOBALS: None
  RETURN VALUE: 16-bit CRC.
  NOTES: Untested.
-----------------------------------------------------------
*/

unsigned int makeIMICRC(unsigned char * packet, int length)
{
	int i = 0;    	//Counter
	unsigned int sum = 0;  	// 16bit sum

   	// Loop through each character in the packet
   	// except for the last 2 (which are the CRC)
	for(i=0;i<length-2;i++)
   	{
   		sum += (int) packet[i];
   	}

    // Return CRC
 	return sum;
}

/*=================================================================
  PURPOSE: Get the new telemetry from the UART
  UNMODIFIED VARIABLES: port - UART port that IMI-100 is attached to
  MODIFIED VARIABLES: tlm - A telemetry struct for storing decoded data.
  GLOBALS: None
  RETURN VALUE: Error codes
  NOTES: Should be called repeatedly.
-----------------------------------------------------------
*/

ReturnErr_t getIMITelemetry(int port, imi_s * telemetry)
{
	static unsigned char telem_buffer[TLM_PACKET_LENGTH];	// A local global variable to keep track of read bytes.
	static short syncFound = 0;			// A local global boolean flag indicating if a sync has been found
	static int writePosition = 0;		// A local global variable to keep track of write position in buffer
	unsigned char singleByte;

	// Test Packets
//	unsigned char test1[TLM_PACKET_LENGTH]={0x90,0xEB,0xAF,0x71,0xB6,0x7A,0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x66,0x00,0x8A,0x00,0x7B,0x00,0x58,0x00,0x45,0x00,0x32,0x00,0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF8,0x00,0xF8,0x00,0xF8,0xFC,0x00,0xFE,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x1C,0x0E};
//	unsigned char test2[TLM_PACKET_LENGTH]={0x90,0xEB,0xB0,0x71,0xB6,0x7A,0x1E,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x6F,0x00,0x92,0x00,0x81,0x00,0x55,0x00,0x4A,0x00,0x39,0x00,0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xE0,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0xE0,0xFC,0x00,0xE0,0x00,0x00,0xFC,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xB5,0x7A,0x1E,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x0F,0x0E};
//	unsigned char test3[TLM_PACKET_LENGTH]={0x90,0xEB,0xB1,0x71,0xB6,0x7A,0x1E,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x6A,0x00,0x8C,0x00,0x7D,0x00,0x56,0x00,0x47,0x00,0x35,0x00,0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0xDA,0x45,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1D,0x0D};
//	unsigned char test4[TLM_PACKET_LENGTH]={0x90,0xEB,0xB2,0x71,0xB6,0x7A,0x1E,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x65,0x00,0x88,0x00,0x7A,0x00,0x57,0x00,0x44,0x00,0x32,0x00,0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0xF8,0x00,0xFC,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x60,0xB1,0xB4,0xB3,0xDA,0xF1,0x40,0x87,0x10};
//	unsigned char test5[TLM_PACKET_LENGTH]={0x90,0xEB,0xB3,0x72,0xB6,0x7A,0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x66,0x00,0x89,0x00,0x7B,0x00,0x58,0x00,0x44,0x00,0x32,0x00,0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0xF8,0x00,0x00,0xF0,0xFC,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x60,0x0D};

	// Continue to get bytes until there are no bytes available
	while(getByte(port,&singleByte))
	{

	// Testing
//	int i;
//	for(i=0;i<TLM_PACKET_LENGTH;i++)
//	{
//		singleByte = test1[i];
  
	//	HexPrint(USER_PORT,&singleByte,1);

		if(syncFound)
		{
			// Write the byte to the packet and Increment write pointer.
			telem_buffer[writePosition++] = singleByte;

			// If the write pointer is equal to the packet length, we've received a full packet
			if(writePosition == TLM_PACKET_LENGTH)
			{
				// Reset the static parts of the function
				writePosition = 0;
				syncFound = 0;

			//	HexPrint(USER_PORT,telem_buffer,TLM_PACKET_LENGTH);

				// Decode telemetry or return an error
				if(decodeTelemetry(telem_buffer,telemetry) != SUCCESS) 	return SUBFUNCTION_ERR;
				else return SUCCESS;

			}//if
		}//if
		else
		{
			// If we find the first byte of the sync
			if(singleByte == 0x90 && writePosition == 0)
			{
				// Write the byte to the packet and increment the write pointer
				telem_buffer[writePosition++] = singleByte;
			}
			// Else if we find the second sync byte and it is indeed in the second byte position
			else if(singleByte == 0xEB && writePosition == 1)
			{
				// Set the sync found flag
				syncFound = 1;

				// Write the byte to the packet and increment the write pointer
				telem_buffer[writePosition++]	= singleByte;
			}
			else
			{
				// Reset values
				writePosition = 0;
				syncFound = 0;
			}
		}//else
	}//while

	return BUSY;
}
	
/*=================================================================
  PURPOSE: Decode telemetry from a 149 byte packet to a struct
  UNMODIFIED VARIABLES: tlmbuf - A 149-byte packet to be decoded
  MODIFIED VARIABLES: tlm - A telemetry struct for storing decoded data.
  GLOBALS: None
  RETURN VALUE: FAILURE if CRC does not match expected.
  NOTES: Untested.
-----------------------------------------------------------
*/

ReturnErr_t decodeTelemetry(unsigned char * tlmbuf, imi_s * telemetry)
{
	unsigned int expectedCRC;	// What we calculate the CRC _should_ be.
	unsigned int recvdCRC;		// What we received from the IMI-100
	imi_s	tempTlm;				// Just used to find size of struct member
	int i = 0;					// Counter variable to copy over the data to the struct

	// Make CRC
	expectedCRC = makeIMICRC(tlmbuf,TLM_PACKET_LENGTH);

	// Decode Recevied CRC
	//memcpy(recvdCRC,tlmbuf[TLM_PACKET_LENGTH-2],2);
	recvdCRC = (int)tlmbuf[TLM_PACKET_LENGTH-2] + ((int)tlmbuf[TLM_PACKET_LENGTH-1])*256;
	
	// Debug
	//sprintf(dbgbuf,"\r\nR = %lX, E = %lX",recvdCRC,expectedCRC);
	//sendDBGALL(USER_PORT,dbgbuf);

	// Check it against end of packet:
	if(expectedCRC != recvdCRC) return FAILURE;
	
	// Copy packet to struct.  Note: We need to do it element by 
	// element because the compiler does not put structs in
	// sequential bytes.  It inserts spaces for odd bytes.
	memcpy(&(telemetry->sync),&tlmbuf[i],sizeof(tempTlm.sync));
	i+=sizeof(tempTlm.sync);

	// Skip 6 bytes (Not interested in it).
	i+=6;

	memcpy(&(telemetry->gus_cmdValidCntr),&tlmbuf[i],sizeof(tempTlm.gus_cmdValidCntr));
	i+=sizeof(tempTlm.gus_cmdValidCntr);

	memcpy(&(telemetry->gus_cmdInValidCntr),&tlmbuf[i],sizeof(tempTlm.gus_cmdInValidCntr));
	i+=sizeof(tempTlm.gus_cmdInValidCntr);

	memcpy(&(telemetry->gus_cmdInvalidChksumCntr),&tlmbuf[i],sizeof(tempTlm.gus_cmdInvalidChksumCntr));
	i+=sizeof(tempTlm.gus_cmdInvalidChksumCntr);

	memcpy(&(telemetry->guc_LastCommand),&tlmbuf[i],sizeof(tempTlm.guc_LastCommand));
	i+=sizeof(tempTlm.guc_LastCommand);

	memcpy(&(telemetry->guc_acsMode),&tlmbuf[i],sizeof(tempTlm.guc_acsMode));
	i+=sizeof(tempTlm.guc_acsMode);

	// Skip 37 bytes (Not interested in it).
	i+=37;

	// Note: Array
	memcpy(telemetry->gs_rwsSpeedCmd,&tlmbuf[i],sizeof(tempTlm.gs_rwsSpeedCmd));
	i+=sizeof(tempTlm.gs_rwsSpeedCmd);

	// Note: Array
	memcpy(telemetry->gs_rwsSpeedTach,&tlmbuf[i],sizeof(tempTlm.gs_rwsSpeedTach));
	i+=sizeof(tempTlm.gs_rwsSpeedTach);

	// Note: Array
	memcpy(telemetry->gf_rwaTorqueCmd,&tlmbuf[i],sizeof(tempTlm.gf_rwaTorqueCmd));
	i+=sizeof(tempTlm.gf_rwaTorqueCmd);

	// Note: Array
	memcpy(telemetry->gc_rwaTorqueCmd,&tlmbuf[i],sizeof(tempTlm.gc_rwaTorqueCmd));
	i+=sizeof(tempTlm.gc_rwaTorqueCmd);

	// Note: Array
	memcpy(telemetry->gf_torqueCoilCmd,&tlmbuf[i],sizeof(tempTlm.gf_torqueCoilCmd));
	i+=sizeof(tempTlm.gf_torqueCoilCmd);

	// Note: Array
	memcpy(telemetry->gc_torqueCoilCmd,&tlmbuf[i],sizeof(tempTlm.gc_torqueCoilCmd));
	i+=sizeof(tempTlm.gc_torqueCoilCmd);

	// Skip 6 bytes (Not interested in it).
	i+=40;

	// Note: Array
	memcpy(telemetry->g_RotatingVariable_A,&tlmbuf[i],sizeof(tempTlm.g_RotatingVariable_A));
	i+=sizeof(tempTlm.g_RotatingVariable_A);

	// Note: Array
	memcpy(telemetry->g_RotatingVariable_B,&tlmbuf[i],sizeof(tempTlm.g_RotatingVariable_B));
	i+=sizeof(tempTlm.g_RotatingVariable_B);

	// Note: Array
	memcpy(telemetry->g_RotatingVariable_C,&tlmbuf[i],sizeof(tempTlm.g_RotatingVariable_C));
	i+=sizeof(tempTlm.g_RotatingVariable_C);

	memcpy(&(telemetry->crc),&tlmbuf[i],sizeof(tempTlm.crc));
	i+=sizeof(tempTlm.crc);
/*
	sprintf(dbgbuf,"\r\n Copied over %u bytes. Should be %u",i,TLM_PACKET_LENGTH);
	sendDBGALL(USER_PORT,dbgbuf);

	// Print Struct Bytes
	sendDBGALL(USER_PORT,"\r\n Telemetry Struct Bytes:\r\n");
	HexPrint(USER_PORT,telemetry,sizeof(&telemetry));*/
/*
	// Print Packet Bytes
	sendDBGALL(USER_PORT,"\r\n Telemetry Packet Bytes:\r\n");
	HexPrint(USER_PORT,tlmbuf,TLM_PACKET_LENGTH);

	sendDBGALL(USER_PORT,"Raw Bytes Per section:");
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,&(telemetry->sync),							sizeof(tempTlm.sync));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,&(telemetry->gus_cmdValidCntr),				sizeof(tempTlm.gus_cmdValidCntr));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,&(telemetry->gus_cmdInValidCntr),				sizeof(tempTlm.gus_cmdInValidCntr));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,&(telemetry->gus_cmdInvalidChksumCntr),		sizeof(tempTlm.gus_cmdInvalidChksumCntr));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,&(telemetry->guc_LastCommand),				sizeof(tempTlm.guc_LastCommand));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,&(telemetry->guc_acsMode),					sizeof(tempTlm.guc_acsMode));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,telemetry->gs_rwsSpeedCmd,					sizeof(tempTlm.gs_rwsSpeedCmd));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,telemetry->gs_rwsSpeedTach,					sizeof(tempTlm.gs_rwsSpeedTach));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,telemetry->gf_rwaTorqueCmd,					sizeof(tempTlm.gf_rwaTorqueCmd));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,telemetry->gc_rwaTorqueCmd,					sizeof(tempTlm.gc_rwaTorqueCmd));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,telemetry->gf_torqueCoilCmd,					sizeof(tempTlm.gf_torqueCoilCmd));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,telemetry->g_RotatingVariable_A,				sizeof(tempTlm.g_RotatingVariable_A));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,telemetry->g_RotatingVariable_B,				sizeof(tempTlm.g_RotatingVariable_B));
	sendDBGALL(USER_PORT,"\r\n");
	HexPrint(USER_PORT,telemetry->g_RotatingVariable_C,				sizeof(tempTlm.g_RotatingVariable_C));
	sendDBGALL(USER_PORT,"\r\n");

	unsigned char dummy;
	while(!getByte(USER_PORT,&dummy)){}
*/
	return SUCCESS;
}

/*=================================================================
  PURPOSE: Set reaction wheel torque
  UNMODIFIED VARIABLES: 3 element array of reaction wheel torques in IMI-100
		coordinate system. See IMI-100 documentation. Units are mNm.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: Error codes
  NOTES: Untested.
-----------------------------------------------------------
*/

ReturnErr_t setIMIReactionWheelTorque_mNm(int port, float X, float Y, float Z)
{
	unsigned int crc;

	// Note: Only use a unsigned char
	signed char rw_torque[3];

	// Convert to 8-bit signed chars via truncation:
	rw_torque[0] = X/MNM_PER_BIT;
	rw_torque[1] = Y/MNM_PER_BIT;
	rw_torque[2] = Z/MNM_PER_BIT;

	// Counter Variables
	int i=0;

	// Note: No need to check inputs because any unsigned char value (-127 to 127) is acceptable
	// for the IMI-100.  Any float provided to this function will be truncated into 
	// a signed char.  So we are okay.

	// Create Packet
	unsigned char packet[40];

	// Assemble Packet:
	packet[0] = 0x90;				// Sync Byte 1
	packet[1] = 0xEB;				// Sync Byte 2
	packet[2] = 0x01;				// Command ID
	packet[3] = rw_torque[0];		// X Toqeue
	packet[4] = 0x00;				// Empty
	packet[5] = rw_torque[1];		// Y Toqeue
	packet[6] = 0x00;				// Empty
	packet[7] = rw_torque[2];		// Z Toqeue
	packet[8] = 0x00;				// Empty
	packet[9] = 0x01;				// Change X
	packet[10] = 0x01;				// Change Y
	packet[11] = 0x01;				// Change Z

	// Fill Empty byte slots (12-37)
	for(i=12;i<38;i++) 
	{
		packet[i] = 0x00;
	}

	// Create the CRC and place in last two bytes of packet
	crc = makeIMICRC(packet,40);
	memcpy(&packet[38],&crc,2);

	// Send out the packet
	sendMSG(port ,packet,40);
	//HexPrint(USER_PORT,packet,40);

	return SUCCESS;

}

/*=================================================================
  PURPOSE: Set magnetic coil dipoles
  UNMODIFIED VARIABLES: 3 element array of dipole strengths in IMI-100
		coordinate system. See IMI-100 documentation. Units are Am^2.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: Error codes
  NOTES: Untested.
-----------------------------------------------------------
*/

ReturnErr_t setIMICoilDipole_Am2(int port, float X, float Y, float Z)
{
	// Note: Only use a unsigned char
	signed char coil_dipole[3];
	unsigned int crc;

	// Convert to 8-bit signed chars via truncation:
	coil_dipole[0] = X/AM2_PER_BIT;
	coil_dipole[1] = Y/AM2_PER_BIT;
	coil_dipole[2] = Z/AM2_PER_BIT;

//	sprintf(dbgbuf,"\r\nCoils set to %d, %d, %d",coil_dipole[0],coil_dipole[1],coil_dipole[2]);
//	sendDBGALL(USER_PORT,dbgbuf);

	// Counter Variables
	int i=0;

	// Note: No need to check inputs because any unsigned char value (-127 to 127) is acceptable
	// for the IMI-100.  Any float provided to this function will be truncated into 
	// a signed char.  So we are okay.

	// Create Packet
	unsigned char packet[40];

	// Assemble Packet:
	packet[0] = 0x90;				// Sync Byte 1
	packet[1] = 0xEB;				// Sync Byte 2
	packet[2] = 0x04;				// Command ID
	packet[3] = coil_dipole[0];		// X Coil Am2
	packet[4] = 0x00;				// Empty
	packet[5] = coil_dipole[1];		// Y Coil Am2
	packet[6] = 0x00;				// Empty
	packet[7] = coil_dipole[2];		// Z Coil Am2
	packet[8] = 0x00;				// Empty
	packet[9] = 0x01;				// Change X
	packet[10] = 0x01;				// Change Y
	packet[11] = 0x01;				// Change Z

	// Fill Empty byte slots (12-37)
	for(i=12;i<38;i++) 
	{
		packet[i] = 0x00;
	}

	// Create the CRC and place in last two bytes of packet
	crc = makeIMICRC(packet,40);
	memcpy(&packet[38],&crc,2);

	// Send out the packet
	sendMSG(port,packet,40);
	//HexPrint(USER_PORT,packet,40);

	return SUCCESS;
}

/*=================================================================
  PURPOSE: Set reaction wheel speeds
  UNMODIFIED VARIABLES: 3 element array of reaction wheel speeds in IMI-100
		coordinate system. See IMI-100 documentation. Units are Degrees per Second.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: Error codes
  NOTES: Untested.
-----------------------------------------------------------
*/

ReturnErr_t setIMIReactionWheelSpeeds_DegPerSec(int port, float X, float Y, float Z)
{
	signed int rw_speeds[3];
	unsigned int crc;

	// Convert to 16-bit signed integers via truncation:
	rw_speeds[0] = (signed int) (X/DEG_PER_SEC_PER_BIT);
	rw_speeds[1] = (signed int) (Y/DEG_PER_SEC_PER_BIT);
	rw_speeds[2] = (signed int) (Z/DEG_PER_SEC_PER_BIT);

	sprintf(dbgbuf,"\r\nCommanded Speed Counts: X: %d, Y: %d, Z %d",rw_speeds[0],rw_speeds[1],rw_speeds[2]);
	sendDBGALL(USER_PORT,dbgbuf);

	// Counter Variables
	int i=0;

	// For all three reaction wheel speeds
	for(i=0;i<3;i++)
	{
		// If we are outside the maximum and minimum limits of the IMI-100
		if(rw_speeds[i] > MAX_SPEED_IN_BITS || rw_speeds[i] < -MAX_SPEED_IN_BITS)
		{
			sendDBGALL(USER_PORT,"\r\nInvalid speed command");
			return INVALID_ARG;
		}
	}

	// Create Packet
	unsigned char packet[40];

	// Assemble Packet:
	packet[0] = 0x90;				// Sync Byte 1
	packet[1] = 0xEB;				// Sync Byte 2
	packet[2] = 0x02;				// Command ID
	memcpy(&packet[3],&rw_speeds[0],2);		// X Wheel
	memcpy(&packet[5],&rw_speeds[1],2);		// Y Wheel
	memcpy(&packet[7],&rw_speeds[2],2);		// Z Wheel
	packet[9] = 0x01;				// Enable X
	packet[10] = 0x01;				// Enable Y
	packet[11] = 0x01;				// Enable Z

	// Fill Empty byte slots (12-37)
	for(i=12;i<38;i++) 
	{
		packet[i] = 0x00;
	}
		
	// Create the CRC and place in last two bytes of packet
	crc = makeIMICRC(packet,40);
	memcpy(&packet[38],&crc,2);

	// Send out the packet
	sendMSG(port,packet,40);
	//HexPrint(USER_PORT,packet,40);

	return SUCCESS;
}


// Radians per second required by FSW.
// Conversion: 15 degs per sec per bit multiplied by pi/180 to get rads per second.  (Note the 15 deg/sec/bit)
// Frame Conversion: Flip X and Z axes.
ReturnErr_t	convertFrameIMIToBody(imi_s * Original, imi_converted_s * Converted)		// IMI frame and unit conversion
{
	Converted->rwa_SpeedRPSBodyFrame[0] = (float)-1.0*Original->gs_rwsSpeedTach[0]*RADS_PER_SEC_PER_BIT;
	Converted->rwa_SpeedRPSBodyFrame[1] = (float)Original->gs_rwsSpeedTach[1]*RADS_PER_SEC_PER_BIT;
	Converted->rwa_SpeedRPSBodyFrame[2] = (float)-1.0*Original->gs_rwsSpeedTach[2]*RADS_PER_SEC_PER_BIT;

	return SUCCESS;
}

// This converts from requested torques in the Body Frame to torques in the IMI frame.
ReturnErr_t	convertFrameBodyToIMI(imi_converted_s * Converted, 
									float X, float Y, float Z,					// Reaction Wheel Commands
									float MTRx, float MTRy, float MTRz)		// Torque Coils
{
	// Frame Conversion
		// Wheels
		Converted->rwa_TorqueCmdIMIFrame[0] = X;		// Body to Wheels is one negative, but wheels need to REACT, so a second negative cancels.
		Converted->rwa_TorqueCmdIMIFrame[1] = -1.0*Y;	// Body to wheels is nothing. Wheels need to REACT, so a negative is required.
		Converted->rwa_TorqueCmdIMIFrame[2] = Z;		// Body to Wheels is one negative, but wheels need to REACT, so a second negative cancels.
		// Coils
		Converted->gc_torqueCoilCmd[0] = MTRx;		// Body to Wheels is one negative, but wheels need to REACT, so a second negative cancels.
		Converted->gc_torqueCoilCmd[1] = -1.0*MTRy;	// Body to wheels is nothing. Wheels need to REACT, so a negative is required.
		Converted->gc_torqueCoilCmd[2] = MTRz;		// Body to Wheels is one negative, but wheels need to REACT, so a second negative cancels.

	// Units Conversion (from Nm to mNm)
		// Wheels
		Converted->rwa_TorqueCmdIMIFrame[0] = Converted->rwa_TorqueCmdIMIFrame[0]*1000.0;
		Converted->rwa_TorqueCmdIMIFrame[1] = Converted->rwa_TorqueCmdIMIFrame[1]*1000.0;
		Converted->rwa_TorqueCmdIMIFrame[2] = Converted->rwa_TorqueCmdIMIFrame[2]*1000.0;
		// Coils
		// Not needed.

	return SUCCESS;
}

/*=================================================================
  PURPOSE: Set the IMI-100 to Test Mode
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: Error codes
  NOTES: Can only set to test mode, not another mode (like QbX)
-----------------------------------------------------------
*/

ReturnErr_t setIMItoTestMode(int port)
{

	// Counter Variable
	int i;

	// Create Packet
	unsigned char packet[40];
	unsigned int crc;

	packet[0] = 0x90;				// Sync Byte 1
	packet[1] = 0xEB;				// Sync Byte 2
	packet[2] = 0x00;				// Command ID
	packet[3] = 0x00;				// Test Mode

	// Fill Empty byte slots (4-37)
	for(i=4;i<38;i++) 
	{
		packet[i] = 0x00;
	}

	// Create the CRC and place in last two bytes of packet
	crc = makeIMICRC(packet,40);
	memcpy(&packet[38],&crc,2);

	// Send out the packet
	sendMSG(port,packet,40);
	//HexPrint(USER_PORT,packet,40);

	return SUCCESS;
}

// This sets all the default values for the IMI data structure
void imiDataInit(imi_s * I)
{
	I->sync = 0xEB90;
	I->gus_cmdValidCntr = 0;
	I->gus_cmdInValidCntr = 0;
	I->gus_cmdInvalidChksumCntr = 0;
	I->guc_LastCommand = 0;
	I->guc_acsMode = 0;
	I->gs_rwsSpeedCmd[0] = 0;
	I->gs_rwsSpeedCmd[1] = 0;
	I->gs_rwsSpeedCmd[2] = 0;
	I->gs_rwsSpeedTach[0] = 0;
	I->gs_rwsSpeedTach[1] = 0;
	I->gs_rwsSpeedTach[2] = 0;
	I->gf_rwaTorqueCmd[0] = 0;
	I->gf_rwaTorqueCmd[1] = 0;
	I->gf_rwaTorqueCmd[2] = 0;
	I->gc_rwaTorqueCmd[0] = 0;
	I->gc_rwaTorqueCmd[1] = 0;
	I->gc_rwaTorqueCmd[2] = 0;
	I->gf_torqueCoilCmd[0] = 0;
	I->gf_torqueCoilCmd[1] = 0;
	I->gf_torqueCoilCmd[2] = 0;
	I->gc_torqueCoilCmd[0] = 0;
	I->gc_torqueCoilCmd[1] = 0;
	I->gc_torqueCoilCmd[2] = 0;
	I->g_RotatingVariable_A[0] = 0;
	I->g_RotatingVariable_A[1] = 0;
	I->g_RotatingVariable_A[2] = 0;
	I->g_RotatingVariable_A[3] = 0;
	I->g_RotatingVariable_B[0] = 0;
	I->g_RotatingVariable_B[1] = 0;
	I->g_RotatingVariable_B[2] = 0;
	I->g_RotatingVariable_B[3] = 0;
	I->g_RotatingVariable_C[0] = 0;
	I->g_RotatingVariable_C[1] = 0;
	I->g_RotatingVariable_C[2] = 0;
	I->g_RotatingVariable_C[3] = 0;
	I->crc = 0xFFFF;

}


//========================================
// 		Development Functions
//========================================

// Note: The below functions are only necessary for testing and development.  They
// can be removed during actual production

#ifdef DEBUG

/*=================================================================
  PURPOSE: Display in human-readble text the contents of an IMI telemetry struct
  UNMODIFIED VARIABLES: telemetry - an IMItlm structure.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. Does not include all fields, especially rotating variables.
-----------------------------------------------------------
*/

void printIMIData(imi_s * telemetry)
{
	sendDBGALL(USER_PORT,"\r\n======================================");
	sendDBGALL(USER_PORT,"\r\n      IMI-100 Telemetry Output");
	sendDBGALL(USER_PORT,"\r\n======================================");

	sprintf(dbgbuf,"\r\nSync Code: %lX",telemetry->sync);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nValid Command Counter: %u",telemetry->gus_cmdValidCntr);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nInvalid Checksum Counter: %u",telemetry->gus_cmdInvalidChksumCntr);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nLast Command: %u",telemetry->guc_LastCommand);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nACS Mode: %u",telemetry->guc_acsMode);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nCommanded Reaction Wheel Speeds (15 per sec per bit): X: %d Y: %d Z: %d (15 per sec per bit)",
		telemetry->gs_rwsSpeedCmd[0],telemetry->gs_rwsSpeedCmd[1],telemetry->gs_rwsSpeedCmd[2]);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nActual Reaction Wheel Speeds (0.05 rpm per bit): X: %d Y: %d Z: %d ",telemetry->gs_rwsSpeedTach[0],telemetry->gs_rwsSpeedTach[1],telemetry->gs_rwsSpeedTach[2]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT,"\r\nADACS-Computed Commanded Reaction Wheel Torque (mNm): X: ");
	printFloat(dbgbuf,telemetry->gf_rwaTorqueCmd[0]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Y: ");
	printFloat(dbgbuf,telemetry->gf_rwaTorqueCmd[1]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Z: ");
	printFloat(dbgbuf,telemetry->gf_rwaTorqueCmd[2]);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nCommanded Reaction Wheel Torque (0.005mNm per bit): X: %i Y: %i Z: %i",
		telemetry->gc_rwaTorqueCmd[0],telemetry->gc_rwaTorqueCmd[1],telemetry->gc_rwaTorqueCmd[2]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT,"\r\nADACS-Computed Commanded Coil Torque (Am2): X: ");
	printFloat(dbgbuf,telemetry->gf_torqueCoilCmd[0]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Y: ");
	printFloat(dbgbuf,telemetry->gf_torqueCoilCmd[1]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Z: ");
	printFloat(dbgbuf,telemetry->gf_torqueCoilCmd[2]);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nCommanded Coil Torque (.000297 Am2 per bit): X: %i Y: %i Z: %i",
		telemetry->gc_torqueCoilCmd[0],telemetry->gc_torqueCoilCmd[1],telemetry->gc_torqueCoilCmd[2]);
	sendDBGALL(USER_PORT,dbgbuf);
}


void printIMICnvData(imi_converted_s * data)
{
	sendDBGALL(USER_PORT,"\r\n======================================");
	sendDBGALL(USER_PORT,"\r\n      IMI-100 Converted Data");
	sendDBGALL(USER_PORT,"\r\n======================================");

	sprintf(dbgbuf,"\r\nReaction Wheel Speeds (Body Frame): X: ");
	sendDBGALL(USER_PORT,dbgbuf);
	printFloat(dbgbuf,data->rwa_SpeedRPSBodyFrame[0]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Y: ");
	printFloat(dbgbuf,data->rwa_SpeedRPSBodyFrame[1]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Z: ");
	printFloat(dbgbuf,data->rwa_SpeedRPSBodyFrame[2]);
	sendDBGALL(USER_PORT,dbgbuf);


	sprintf(dbgbuf,"\r\nCommanded Reaction Wheel Torque (Body Frame): X: ");
	sendDBGALL(USER_PORT,dbgbuf);
	printFloat(dbgbuf,data->rwa_TorqueCmdIMIFrame[0]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Y: ");
	printFloat(dbgbuf,data->rwa_TorqueCmdIMIFrame[1]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Z: ");
	printFloat(dbgbuf,data->rwa_TorqueCmdIMIFrame[2]);
	sendDBGALL(USER_PORT,dbgbuf);

	sprintf(dbgbuf,"\r\nCommanded Coil Torque (Body Frame): X: ");
	sendDBGALL(USER_PORT,dbgbuf);
	printFloat(dbgbuf,data->gc_torqueCoilCmd[0]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Y: ");
	printFloat(dbgbuf,data->gc_torqueCoilCmd[1]);
	sendDBGALL(USER_PORT,dbgbuf);

	sendDBGALL(USER_PORT," Z: ");
	printFloat(dbgbuf,data->gc_torqueCoilCmd[2]);
	sendDBGALL(USER_PORT,dbgbuf);

}


/*=================================================================
  PURPOSE: Collect Telemetry from IMI-100 continuously
  UNMODIFIED: 
		iterations - Number of times to collect before quitting
		continuous - If true, collect continously
		human - print in human readable form if true.  If false, use CSV.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

void imiAutomaticDataCollection(int port, long iterations, short continuous, short human)
{
	long i = 0;		// Iteration counter
	long timeout;			// Timeout used to pause before data collection
	short userFlag = 1;		// Indicator that user wants to quit.  (False = quit)
	unsigned char temp;				// Temporary character holder to check user keystroke
	imi_s IMIData;			// Struct holding results	

	// Let user know how to escape!
	sendDBGALL(USER_PORT,"\r\nCollecting IMI-100 data.  Press 'q' to quit.  Starting in 1 seconds ...");

	// Set the timeout timer
	timeout=SEC_TIMER+1;

	// Loop until current time passes timeout.
	while(SEC_TIMER<timeout)
	{
		// Do nothing.
	}

	// userFlag defaults to true and will remain true until user
	// hits a 'q' keystroke.  If the function call has continuous as
	// true, then it will ignore the i vs. iterations condition.  
	// if not, it will watch the counter and then quit when it supersedes
	// iterations.
	while(userFlag && (continuous || (i < iterations)))
	{
		// Iterate
		i++;
	
		if(blockForIMITelemetry(port,&IMIData,10) != SUCCESS)
		{
			sendDBGALL(USER_PORT,"Fail.");
			return;
		}

		printIMIData(&IMIData);

		// Check if a character is available
		if(getByte(USER_PORT,&temp)) 
		{
			// If it is any form of 'q', signal that we want to quit.
			if(temp == 'q' || temp == 'Q') userFlag = 0;
		}
				
		// Attempt 5Hz
		delay_ms(200);
	}
	
	return;
}

/*=================================================================
  PURPOSE: Convience function to wait for successul read of telemetry
		stream.  This function will block until telemetry is read
		or a timeout is reached. Once telemetry is received, it is
		printed to the screen.
  UNMODIFIED: 
		port - The UART port that the IMI-100 is attached to
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t blockForIMITelemetry(int port, imi_s * Data,unsigned int time_to_block)
{
	ReturnErr_t response;	
	unsigned long timeout;
	timeout = SEC_TIMER + time_to_block;
	do{
		response = getIMITelemetry(port,Data);
		if(SEC_TIMER>timeout)
		{
			response = FAILURE;
			break;
		}
	}while(response == BUSY);
	return response;
}

#endif // DEBUG
