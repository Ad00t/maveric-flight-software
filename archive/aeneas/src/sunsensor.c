/***************************************************************************

									sunsensor.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra), Will Bezouska

Purpose: Functions for talking to the SS-411 Digital Sun Sensor

Changelog:

Date 	 |  Au.  |	Notes
07-28-10	mra		Created.


****************************************************************************
*/

#include "sunsensor.h"

#module

//========================================
// 			Development Functions
//========================================

#ifdef DEBUG
void testCRC();
#endif // DEBUG

//=================================
//  	SLIP Framing Defines
//==================================
#define FEND 	0xc0
#define FESC 	0xdb
#define TFEND 	0xdc
#define TFESC 	0xdd

//=================================
// 		NSP Packet Defines
//===============================
#define POLY 						0x8408		// Used for CRC creation
#define DESTINATION_ADDRESS 		0x39 		// Address of Sun Sensor device.  Determined by wiring of connection.
#define SOURCE_ADDRESS 				0x11		// Address of this microcontroller, convetion used by SS manufacturer
#define MAX_NSP_PACKET_LENGTH 		260			// Max NSP packet size
#define MIN_NSP_PACKET_LENGTH		5			// Min NSP packet size
#define MAX_NSP_DATA_FIELD_LENGTH 	255			// Max Data Field size with NSP packet (5 byte header).
#define POLL_BIT 					7			// Bit to set when sending packet to Sun Sensor	
#define ACK_BIT 					5			// Bit to check for ACKs and NACKS.

//=================================
// 		NSP Modlue Commands
//===============================
#define PING			0x00
#define INIT_COMMAND	0x01
#define PEEK			0x02
#define POKE			0x03
#define TELEMETRY		0x04
#define CRC_COMMAND		0x06
#define APP_TELEMETRY 	0x07
#define APP_COMMAND 	0x08

//=================================
// 		Telemetry Data Lengths 
//=================================

// Note: Not including channel byte!
#define CHANNEL_0_LEN 1
#define CHANNEL_1_LEN 10
#define CHANNEL_2_LEN 12
#define CHANNEL_3_LEN 2
#define CHANNEL_4_LEN 512
#define CHANNEL_5_LEN 512
#define CHANNEL_6_LEN 14
#define CHANNEL_7_LEN 1
#define CHANNEL_8_LEN 4
#define CHANNEL_9_LEN 8
// Channels 10-12 are for CAN functionality (e.g. not used)
#define CHANNEL_13_LEN 6
#define CHANNEL_14_LEN 4

//=================================
// 		getSunSensorData() States
//=================================

#define SYNC						0
#define SYNC_REPLY					1
#define START_GO_CODE 				2
#define START_GO_CODE_REPLY 		3
#define START_FRAME_CYCLE 			4
#define START_FRAME_CYCLE_REPLY 	5
#define GO_CODE_CMDUEST				6
#define GO_CODE_REPLY				7
#define CHANNEL_CMDUEST				8
#define CHANNEL_CMDUEST_REPLY		9
#define DONE						10

//=================================
// 		Timers used during getSunSensorData()
//=================================

// This instructs the function to delay for a certina
// amount of time before requesting a status update
// from the sun sensor.  This allows the sun image
// aquisition time to happen.  A delay of zero
// will not break the system, but will cause many
// uneccessary status update requests.
#define GO_CODE_DELAY						100 //ms

// Once we do start asking for status updates,
// this interval timer prevents us from sending
// those updates continuously.  It forces us
// to request status updates no less than a 
// certain number of milliseconds apart.
#define GO_CODE_INTERVAL					10 //ms

//========================================
// 			Driver Functions
//========================================

void SLIPsend(int port, unsigned char * msg, int len);		
ReturnErr_t packetizeToNSP(unsigned char * message, unsigned int msglen, unsigned char * packet, unsigned int maxPacketLength, unsigned int * actualPacketLength, unsigned char messageControlField);
unsigned int makeSSCRC(unsigned char * packet, int len);
ReturnErr_t sendCommand(int port, unsigned char cmd, unsigned char * message, unsigned int msglen);
ReturnErr_t SLIPrecv(int port, unsigned char * msg, int maxLen, int * len);
ReturnErr_t verifyNSPPacket(int dataLength, unsigned char * packet, int packetLength);

//=======================================
//  Local Globals
//=======================================
//unsigned char packet[MAX_NSP_PACKET_LENGTH];
//unsigned char data[MAX_NSP_DATA_FIELD_LENGTH];	// Char array to store results

//========================================
// 			Driver Functions
//========================================

// Note: The below functions are necessary to use hardware.


/*=================================================================
  PURPOSE: Send out packet on port using SLIP framing.  Fist the FEND
	byte is sent, then each byte of the message is sent, and then the final
	FEND byte is sent.  All FEND bytes found in the message are automatically
	prepended with FESC characters.
	
	Replacements:
		FEND-->FESC TFEND
		FESC-->FESC TFESC

  UNMODIFIED:  	port - serial port to send message on
				msg - pointer to message
				len - length of character array
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Each message is encapsulated in a SLIP Frame.  The properties of the SLIP
	frame are as follows: 
	A special end character FEND (0xc0) marks both the beginning and end of 
	each NSP message.  Wherever FEND would occur within the message, it is 
	replaced by two bytes: FESC TFEND (0xdb 0xdc).  Wherever FESC would occur
	in the message, it is replaced by FESC TFESC (0xdb 0xdd).
	SLIP framing is used because not all of the link types supported by the
	makers of the sunsensor feature out-of-band message completion signals.
	(Crom SS Documentation)
-----------------------------------------------------------
*/

void SLIPsend(int port, unsigned char * msg, int len)
{
	int i;

	// Start of frame
	sendByte(port,FEND);	
	
	// Send each unsigned char or replace it if it's a special unsigned char
	for(i=0;i<len;i++)
	{
		if(msg[i]==FEND)
		{
			sendByte(port,FESC);
			sendByte(port,TFEND);	
		}
		else if(msg[i]==FESC)
		{
			sendByte(port,FESC);
			sendByte(port,TFESC);	
		}
		else
		{
			sendByte(port,msg[i]);
		}//else
	}//for
	sendByte(port,FEND);			// End of frame

}//SLIPsend()

/*=================================================================
  PURPOSE: Packetize a message and return its length
  UNMODIFIED:  	message - packet contents to be sent
				len - length of the message portion to be sent
  MODIFIED VARIABLES: packet - pointer to completed packet.  Must be
					at least 5 elements larger than the message array.
  GLOBALS: None
  RETURN VALUE: Length of completed packet, including CRC.
  NOTES: Untested.
-----------------------------------------------------------
*/

ReturnErr_t packetizeToNSP(unsigned char * message, unsigned int msglen, unsigned char * packet, 
	unsigned int maxPacketLength, unsigned int * actualPacketLength, unsigned char messageControlField)
{
	// For use below when making crc
	unsigned int crc;

	//Verify arguments:
	if(msglen + 5 > maxPacketLength) return OUT_OF_BOUNDS;

	// Static items defined at compile time
	packet[0] = DESTINATION_ADDRESS;
	packet[1] = SOURCE_ADDRESS;
	
	// Message Control Field.  Depends on command.
	packet[2] = messageControlField;
	
	// Copy message contents into packet (starting with 4th byte of packet)
	memcpy(&packet[3],message,msglen);
	
	// Make two byte CRC.  Length of completed packet (except CRC) is 
	// the lengthe of the message plus the three bytes for the 
	// two address fields and the message control field.
	crc = makeSSCRC(packet,msglen+3);
	
	// Add CRC to the end of the packet
	memcpy(&packet[msglen+3],&crc,2);

	// Set length of packet
	*actualPacketLength = msglen + 5;

	// Return full length of packet (including CRC)
	return SUCCESS;
}

/*=================================================================
  PURPOSE: Create a CRC and add to packet.
  UNMODIFIED:  	packet - pointer to completed packet
				len - length of the packet, minus the 2 byte crc.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Taken verbatim from SS Documentation.  Can also be found
	at: http://www.utias-sfl.net/docs/ssp2.1e.pdf
-----------------------------------------------------------
*/
	
unsigned int makeSSCRC(unsigned char * packet, int len)
{
	unsigned char ch;
	unsigned int crc = 0xffff;
	int i=0,j=0;
	while(len-- > 0)
	{
		ch = packet[j++];
		for(i=0;i<8;i++)
		{
			crc = (crc>>1)^( ((ch^crc)&0x01) ? POLY:0);
			ch >>= 1;
		}
	}

	return crc;
}

/*=================================================================
  PURPOSE: Send a command
  UNMODIFIED: cmd - the command number to execute
		message - the optional message to go along with it
		len - length of this message
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t sendCommand(int port, unsigned char cmd, unsigned char * message, unsigned int msglen)
{
	// Create packet holder with maximum size.  We'll only use what we need.
	unsigned char packet[MAX_NSP_PACKET_LENGTH];

	// Length of completed packet.
	int actuallen = 0;
	
	// Set the "Poll" bit (e.g. a reply is requested)
	bit_set(cmd,POLL_BIT);

	// Packetize
	if(packetizeToNSP(message,msglen,packet,MAX_NSP_PACKET_LENGTH,&actuallen,cmd) != SUCCESS) return SUBFUNCTION_ERR;

	// Send out
	SLIPsend(port,packet,actuallen);

	// Debug
	// Note: for some reason, this must be here to work!!! WHAT IS GOING ON!?
	//sendDBGALL(USER_PORT,"\r\nSLIP Sent.");
	//sendDBGALL(USER_PORT,"\r\nSO:");
	//sendDBGALL(USER_PORT,"<");
	//HexPrint(USER_PORT,packet,actuallen);
	//MeasureTime(1);

	return SUCCESS;
}

/*=================================================================
  PURPOSE: Initialize sun sensor.
  UNMODIFIED: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t sunSensor_init(int port)
{
	unsigned char startAddress[4] = {0x00,0x10,0x00,0x00};

	// Send command along with 4 byte startAddress
	if(sendCommand(port,INIT_COMMAND,startAddress,4) != SUCCESS) return FAILURE;

	// Wait for reply
	if(blockForSunSensorACK(port) != SUCCESS) return FAILURE;

	return SUCCESS;
}

/*=================================================================
  PURPOSE: Request a reading from a Sun Sensor channel.
  UNMODIFIED: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Call continuously until response is not BUSY.
-----------------------------------------------------------
*/

ReturnErr_t requestSunSensorTelemetry(int port, unsigned char channel)
{
	switch(channel)
	{
		// Normal Channels
		case 0:
		case 1:
		case 2:
		case 3:
		case 6:		
		case 7:
		case 8:
		case 9:	
		case 13:
		case 14:
			return sendCommand(port,APP_TELEMETRY,&channel,1);
			break;	

		// Image Requests are handled by a different function.
		// Channels 10-12 are for CAN functionality (not used).
		default:
			return INVALID_ARG;	
			break;
	}
	
	return UNREACHABLE;
}

/*=================================================================
  PURPOSE: Start a frame cycle
  UNMODIFIED: port - which UART the sun sensor is connected to
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: Error code.
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t startSunSensorFrameCycle(int port)
{
	// Sets the Go Code (command 0x00) to default operation (0x4f) as noted in SS documentation.
	unsigned char cmd[2] = {0x00,0x4f};

	// Send the command and return status.
	return sendCommand(port,APP_COMMAND,cmd,2);
}

/*=================================================================
  PURPOSE: Recieve SLIP-framed response from sun sensor
  UNMODIFIED: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Will reply with a human readable message containing
	information on the device.
-----------------------------------------------------------
*/

ReturnErr_t SLIPrecv(int port, unsigned char * msg, int maxLen, int * len)
{
	static unsigned char buffer[MAX_NSP_PACKET_LENGTH];	// A local global variable to keep track of read bytes.
	static short syncFound = 0;					// A local global boolean flag indicating if a sync has been found
	static short escaped = 0;					// A local global boolean flag indicating the last character was an escape.
	static int writePosition = 0;				// A local global variable to keep track of write position in buffer
	unsigned char singleByte;							// The byte read each time we call getByte(),.

	// Continue to get bytes until there are no bytes available
	while(getByte(port,&singleByte))
	{
		// If we found the first sync byte
		if(syncFound)
		{
			// First, verify we won't overun any buffers
			if(writePosition == maxLen)
			{
				// Reset writePosition to default, indicating starting fresh.
				writePosition = 0;
	
				// Reset the sync found flag
				syncFound = 0;	
		
				// Reset the escape flag
				escaped = 0;
	
				//sendDBGALL(USER_PORT,"\r\nFAILURE - SLIP Buffer overrun!");
	
				return FAILURE;
			}//if

			// If we are not escaped
			if(!escaped)
			{
				// Check byte for END or ESC bytes
				if(singleByte==FEND)
				{
					// Copy to packet buffer
					memcpy(msg,buffer,writePosition);

					// Update modified argument for length
					*len = writePosition;
	
					// Reset writePosition to default, indicating starting fresh.
					writePosition = 0;
			
					// Reset the escape flag
					escaped = 0;
	
					return SUCCESS;
				}
				else if(singleByte==FESC)
				{
					// Note that we are escaped
					escaped = 1;
				}
				else
				{
					// Copy it to the buffer
					buffer[writePosition++] = singleByte;
				}//else
			}//if

			// We are escaped:
			else
			{
				// If the byte is a TFESC, then the original was FESC
				if(singleByte==TFESC)
				{
					buffer[writePosition++] = FESC;
				}
				// If the byte is a TFEMD, then the original was FEND
				else if(singleByte == TFEND)
				{
					buffer[writePosition++] = FEND;
				}
				// Otherwise, there was an error, so quit with it.
				else
				{
					// Reset writePosition to default, indicating starting fresh.
					writePosition = 0;
		
					// Reset the sync found flag
					syncFound = 0;	
			
					// Reset the escape flag
					escaped = 0;

					//sendDBGALL(USER_PORT,"\r\nFAILURE - SLIP Escape unsigned char error!");

					return FAILURE;
				}
			
				// Reset escaped state
				escaped=0;

			}//else
		}//else-if	

		// Else, no sync has been found yet, so keep looking!
		else
		{
			// Check byte for END or ESC bytes
			if(singleByte==FEND)
			{
				syncFound = 1;
			}
		}//else
	}//while

	// Signal to keep looking
	return BUSY;
}


/*=================================================================
  PURPOSE: Get the latest data packet from sun sensor
  UNMODIFIED VARIABLES: 
		port - UART device to use;
		maxDataLength - Maximum size of data storage
  MODIFIED VARIABLES: data - A pointer to a unsigned char array where the data should go
		dataLength - Actual data length of packet.
  GLOBALS: None
  RETURN VALUE: 
		Error condition 
			- BUSY if still looking for packet
			- FAILURE if encountered error looking for packet (CRC, etc). Continue looking.
			- SUCCESS good packet found.
  NOTES: Untested.
-----------------------------------------------------------
*/

ReturnErr_t getSunSensorReply(int port, unsigned char * data, int maxDataLength, int * dataLengthPtr)
{
	ReturnErr_t response;
	unsigned char packet[MAX_NSP_PACKET_LENGTH];	// Complete NSP Packet
	int packetLength = 0;				// Packet length modified by SLPrecv()
	int dataLength = 0;					// Data field length.
	short packetFound = 0;				// Assume packet has not been found.
//	short packetVerified = 0;			// Flag indicating packet is verified

	// Verify that the array can handle it
	if(maxDataLength<MAX_NSP_DATA_FIELD_LENGTH) return INVALID_ARG;

	// Read all frames from UART until none are left.
	do
	{
		// Check for packet through end of UART buffer
		response = SLIPrecv(port,packet,MAX_NSP_PACKET_LENGTH,&packetLength);

		// If we found a frame
		if(response == SUCCESS)
		{	
			//sendDBGALL(USER_PORT,"\r\nSLIP Frame Found:");
			//sendDBGALL(USER_PORT,"\r\nSI:");
			//HexPrint(USER_PORT,packet,packetLength);

			// Set the data length after subtracting 5 byte header.
			dataLength = packetLength - 5;

			// If we made it through the above:
			if(verifyNSPPacket(dataLength,packet,packetLength) == SUCCESS)
			{
				// Copy over data section of NSP. Note: 
				//  	&packet[3] - Because packet points to the full
				//     	NSP packet and we only want the data we ignore the first
				//     	3 bytes which containts addresses and messange control.
				memcpy(data,&packet[3],dataLength);

				// Write data length to ptr from calling function
				*dataLengthPtr = dataLength;
	
				// Signal that we should return success at the end.
				packetFound = 1;

				//sendDBGALL(USER_PORT,"\r\nNSP Data Found:");
				//sendDBGALL(USER_PORT,"\r\nNI:");
				//HexPrint(USER_PORT,data,dataLength);
				//sendDBGALL(USER_PORT,">");
				//MeasureTime(0);
			}
		}

	}while(response == SUCCESS);

	//sendDBGALL(USER_PORT,"\r\nNo packets left on UART.");

	if(packetFound) return SUCCESS;
	else return response;
}

/*=================================================================
  PURPOSE: Verify an NSP packet
  UNMODIFIED VARIABLES: 
		dataLength - Data field size of NSP packet
		packet - Array containing bytes of NSP packet
		packetLength - Array of the length.
  MODIFIED VARIABLES: None/.
  GLOBALS: None
  RETURN VALUE: 
		Error condition 
			- FAILURE packet is no good.
			- SUCCESS packet is good.
  NOTES: Untested.
-----------------------------------------------------------
*/

ReturnErr_t verifyNSPPacket(int dataLength, unsigned char * packet, int packetLength)
{
		// Verify packet length
		if(packetLength < MIN_NSP_PACKET_LENGTH || packetLength > MAX_NSP_PACKET_LENGTH)
		{
			//sendDBGALL(USER_PORT,"\r\nFAILURE - Incorrect packet length: ");
			//HexPrint(USER_PORT,packet,packetLength);
			return FAILURE;
		}

		// Verify Length
		if(dataLength > MAX_NSP_DATA_FIELD_LENGTH)
		{
			//sendDBGALL(USER_PORT,"\r\nFAILURE - Incorrect data length: ");
			//HexPrint(USER_PORT,packet,packetLength);
			return FAILURE;
		}

		// Verify the ACK bit is set
		if(!bit_test(packet[2],ACK_BIT))
		{
			//sendDBGALL(USER_PORT,"\r\nFAILURE - ACK bit not set.");
			//byteMe(packet[2]);
			return FAILURE;
		}

		// Verify CRC
		if(makeSSCRC(packet,packetLength) != 0x00)
		{
			//sendDBGALL(USER_PORT,"\r\nFAILURE - CRC error: ");
			//HexPrint(USER_PORT,packet,packetLength);
			return FAILURE;
		}

		return SUCCESS;
}

/*=================================================================
  PURPOSE: Decode telemetry data based on which channel was read.
  UNMODIFIED VARIABLES: 
		data - raw data returned by Sun Sensor
		len - the length of the raw data.
  MODIFIED VARIABLES: 
		SunSensorData - struct of all sun sensor data
  GLOBALS: None.
  RETURN VALUE: Error code.
  NOTES: The first byte of the data is the channel number
-----------------------------------------------------------
*/

ReturnErr_t decodeSunSensorTelemetry(unsigned char * data, int len, sunsensor_s * SunSensorData)
{
	// Note: Channels 4,5 are image buffers, handled in different function.
	
	// Note: Channels 10-12 are for CAN functionality and not used.

	// Note: unsigned char * data includes a one byte channel number at position data[0].
	//  We use this to switch and determine where the data will be stored, but
	//  once we have done that, the number is no longer needed and discarded.
	//  That is why all the memcpy()'s use "len-1" as the data length.

	// If there is no data, give up.  Note: the first byte is the channel number!
	if(len<2) return INVALID_ARG;

	//sendDBGALL(USER_PORT,"\r\nChannel data:");
	//HexPrint(USER_PORT,&data[1],len-1);

	// Switch on the first bytem, the channel number.
	switch(data[0])
	{
		case 0:
		{
			if(len-1 != CHANNEL_0_LEN) return INVALID_ARG;
			memcpy(&(SunSensorData->GoCode),&data[1],len-1);
			break;
		}
		case 1:
		{
			if(len-1 != CHANNEL_1_LEN) return INVALID_ARG;
			memcpy(&(SunSensorData->MinExposure),&data[1],len-1);
			break;
		}
		case 2:
		{
			if(len-1 != CHANNEL_2_LEN) return INVALID_ARG;
			memcpy(&(SunSensorData->MinBrightness),&data[1],len-1);
			break;
		}
		case 3:
		{
			if(len-1 != CHANNEL_3_LEN) return INVALID_ARG;
			memcpy(&(SunSensorData->Temperature),&data[1],len-1);
			break;
		}
		case 4:
		{
			return INVALID_ARG;	// See note above.
		}
		case 5:
		{
			return INVALID_ARG; // See note above.
		}
		case 6:
		{
			if(len-1 != CHANNEL_6_LEN) return INVALID_ARG;
			memcpy(SunSensorData->Vector,&data[1],len-1);
			break;
		}
		case 7:
		{
			if(len-1 != CHANNEL_7_LEN) return INVALID_ARG;
			memcpy(&(SunSensorData->Iterations),&data[1],len-1);
			break;
		}
		case 8:
		{
			if(len-1 != CHANNEL_8_LEN) return INVALID_ARG;
			memcpy(&(SunSensorData->ProcessingTime),&data[1],len-1);
			break;
		}
		case 9:
		{
			if(len-1 != CHANNEL_9_LEN) return INVALID_ARG;
			memcpy(SunSensorData->Vector_int,&data[1],len-1);
			break;
		}
		case 10:
		{
			return INVALID_ARG;	// See note above.
		}
		case 11:
		{
			return INVALID_ARG; // See note above.
		}
		case 12:
		{
			return INVALID_ARG;	// See note above.
		}
		case 13:
		{
			if(len-1 != CHANNEL_13_LEN) return INVALID_ARG;
			memcpy(SunSensorData->Diagnostic,&data[1],len-1);
			break;
		}
		case 14:
		{
			if(len-1 != CHANNEL_14_LEN) return INVALID_ARG;
			memcpy(&(SunSensorData->CelsiusTemperature),&data[1],len-1);
			break;
		}
		default:
		{
			return UNREACHABLE;
		}
	}

	return SUCCESS;
}

/*=================================================================
  PURPOSE: Get an image from the Sun Sensor and place into struct
  UNMODIFIED VARIABLES: operating - True for Operating image or False for Offline image.
  MODIFIED VARIABLES: 
		SunSensorData - Struct holding all the aquired data.
  GLOBALS: None.
  RETURN VALUE: Error code.
  NOTES: Because of size limitations on NSP packets, we must
	use three seperate requests to aquire parts of the image.
	As each part is received, it is placed into the structure
	at a specific point depending on what part of the iamge it is.
-----------------------------------------------------------
*/

/*
ReturnErr_t getSunSensorImage(int port, sunsensor_s * SunSensorData, short operating)
{
	//unsigned char data[MAX_NSP_DATA_FIELD_LENGTH];	// Char array to store results
	int length;								// Length of response from Sun Sensor
	unsigned char imageRequestData[3];				// Used for image aquisition (channels 4 and 5) only
	unsigned char pixelIndex[3] = {0,100,200};		// The pixels Index for each request
	unsigned char pixelCount[3] = {100,100,56};		// The pixel Count for each request
	unsigned char i = 0;								// Counter Variable

	// Set the correct Telemetry Channel
	if(operating) 	imageRequestData[0] = 4;
	else 			imageRequestData[0] = 5;

	// Start a frame cycle and wait until it is done.
	if(startSunSensorFrameCycle(SUN_PORT) != SUCCESS) return SUBFUNCTION_ERR;
	if(blockForSunSensorACK(SUN_PORT) != SUCCESS) return SUBFUNCTION_ERR;
	if(blockForSunSensorGoCode(SUN_PORT) != SUCCESS)  return SUBFUNCTION_ERR;

	for(i=0;i<3;i++)
	{
		// Finish up the Telemetry request data
		imageRequestData[1] = pixelIndex[i];
		imageRequestData[2] = pixelCount[i];
	
		// Request first part of the image.
		if(sendCommand(port,APP_TELEMETRY,imageRequestData,3) != SUCCESS) return SUBFUNCTION_ERR;

		// Read reply
		if(blockForSunSensorReply(port,data,&length) != SUCCESS) return SUBFUNCTION_ERR;

		// Copy the data bytes, bypassing the first three bytes.  Start copying
		// at a location that is twice the pixelIndex (since each pixel is 2 bytes) from
		// the start of the image.  Note: MUST cast the pixelIndex to int, otherwise the
		// result of multiplication is larger than the size of an unsigned char and rolls over!
		if(operating) {memcpy(&(SunSensorData->OperatingImage[((int)pixelIndex[i])*2]),&data[3],length-3);}
		else {memcpy(&(SunSensorData->OfflineImage[((int)pixelIndex[i])*2]),&data[3],length-3);}
	}

	return SUCCESS;
}
*/

/*=================================================================
  PURPOSE: Get all requested data from the sun sensor
  UNMODIFIED VARIABLES: 
		port - The UART port the sun sensor is connected to
		requestedChannels - An array of channel numbers to 
			read off of sun sensor
		requestedChannelsLength - The number of channels
		reset - a flag, if true, that resets the functions state
  MODIFIED VARIABLES: 
		Data - The sun sensor struct
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: This function should be called repeatedly
-----------------------------------------------------------
*/

ReturnErr_t getSunSensorData(int port, unsigned char * requestedChannels, 
		unsigned char requestedChannelsLength, sunsensor_s * Data, short reset, short sync)
{
	// Static Variables
	static int state = 0;					// State of the function
	static unsigned char chanIndex = 0;				// Current channel being requested
	static int64 goCodeStartTimer = 0;		// Timer to wait for first Go Code
	static int64 goCodeIntervalTimer = 0;	// Timer to wait between Go Codes
	
	// Local, non-static variables
	int length;								// Length of response
	ReturnErr_t response;					// Response of various functions
	unsigned char syncMsg[3] = {5,0,0};				// Default sync
	unsigned char data[MAX_NSP_DATA_FIELD_LENGTH];	// Char array to store results

	// When called with a reset, start everything fresh.
	if(reset)
	{
		//state = 0;
		state = SYNC;
		chanIndex = 0;

		// Restart Timers:
		goCodeStartTimer = MSEC + GO_CODE_DELAY;
		goCodeIntervalTimer = MSEC + GO_CODE_INTERVAL;
	}

	// Determine which sync code we are looking for
	if(sync) syncMsg[1] = 1;
	else syncMsg[1] = 0;

	//sprintf(dbgbuf,"\r\n%u",state);
	//sendDBGALL(USER_PORT,dbgbuf);	

	// Switch on the static state
	switch(state)
	{
		case SYNC:
		{	
			//MeasureTime(1);

			//sendDBGALL(USER_PORT,"\r\nSending Sync Code Request.");
			//sendDBGALL(USER_PORT,"\r\nS");

			// Send out a request for a part of an image to use as a sync code
			if(sendCommand(port,APP_TELEMETRY,syncMsg,3) != SUCCESS)
			{
				//sprintf(dbgbuf,"\r\nFailed - State: %u",state);
				//sendDBGALL(USER_PORT,dbgbuf);	
				state = DONE;
				return SUBFUNCTION_ERR;
			}

			state = SYNC_REPLY;
		}
		case SYNC_REPLY:
		{
			// Check for reply
			response = getSunSensorReply(port,data,MAX_NSP_DATA_FIELD_LENGTH,&length);

			if(response != SUCCESS) 
			{
				// If some type of failure
				if(response != BUSY) 
				{
					//sprintf(dbgbuf,"\r\nFailure in state: %u",state);
					//sendDBGALL(USER_PORT,dbgbuf);	
				}
				return BUSY;
			}

			// Verify Length so we don't mess up buffers
			if(length!=3) return BUSY;

			// Verify this is the sync we are looking for:
			if(	data[0] != syncMsg[0] ||
				data[1] != syncMsg[1] ||
				data[2] != syncMsg[2] )
			{
				//sendDBGALL(USER_PORT,"\r\nSync code incorrect. Ignoring.");	
				//sendDBGALL(USER_PORT,"\r\nI");	
				return BUSY;
			}

			// Move to the next state
			state = START_FRAME_CYCLE;
		}	
/*	case START_GO_CODE:
		{
			sendDBGALL(USER_PORT,"\r\nStarting Sun Sensor Reading Cycle.");

			// Request Go Code to determine state
			if(requestSunSensorTelemetry(SUN_PORT,0) != SUCCESS)
			{
				sprintf(dbgbuf,"\r\nFailed - State: %u",state);
				sendDBGALL(USER_PORT,dbgbuf);	
				state = DONE;
				return SUBFUNCTION_ERR;
			}

			state = START_GO_CODE_REPLY;
		}
		case START_GO_CODE_REPLY:
		{
			// Check for reply
			response = getSunSensorReply(port,data,MAX_NSP_DATA_FIELD_LENGTH,&length);

			if(response != SUCCESS) 
			{
				// If some type of failure
				if(response != BUSY) 
				{
					sprintf(dbgbuf,"\r\nFailure in state: %u",state);
					sendDBGALL(USER_PORT,dbgbuf);	
				}
				return BUSY;
			}
		
			// Verify Length
			if(length<2)
			{
				sprintf(dbgbuf,"\r\nFailed - Bad Length -  State: %u",state);
				sendDBGALL(USER_PORT,dbgbuf);	
				HexPrint(USER_PORT,data,length);	
				state = DONE;
				return SUBFUNCTION_ERR;
			}

			// Check the value of the Go Code
			if(data[1] != 0x00)
			{
				sprintf(dbgbuf,"\r\nFailed - Bad Go Code - State: %u",state);
				sendDBGALL(USER_PORT,dbgbuf);	
				HexPrint(USER_PORT,data,length);	
				state = DONE;
				return FAILURE;
			}

			state = START_FRAME_CYCLE;
		}
*/		case START_FRAME_CYCLE:
		{
			//sendDBGALL(USER_PORT,"\r\nStarting a New Frame Cycle");
	
			// Start a frame cycle
			if(startSunSensorFrameCycle(port) != SUCCESS)
			{
				//sprintf(dbgbuf,"\r\nFailed - State: %u",state);
				//sendDBGALL(USER_PORT,dbgbuf);	
				state = DONE;
				return SUBFUNCTION_ERR;
			}

			// Update state
			state = START_FRAME_CYCLE_REPLY;
		}
		case START_FRAME_CYCLE_REPLY:
		{
			// Check for reply
			response = getSunSensorReply(port,data,MAX_NSP_DATA_FIELD_LENGTH,&length);

			if(response != SUCCESS) 
			{
				// If some type of failure
				if(response != BUSY) 
				{
					//sprintf(dbgbuf,"\r\nFailure in state: %u",state);
					//sendDBGALL(USER_PORT,dbgbuf);	
				}
				return BUSY;
			}

			// Update state
			state = GO_CODE_CMDUEST;
		}
		case GO_CODE_CMDUEST:
		{	
			// Check Start Timer:
			if(MSEC < goCodeStartTimer)
			{
				//sprintf(dbgbuf,"\r\nWaiting for delay to start Go Code Checking");
				//sendDBGALL(USER_PORT,dbgbuf);	
				return BUSY;	
			}

			// Check Interval Timer:
			if(MSEC < goCodeIntervalTimer)
			{
				//sprintf(dbgbuf,"\r\nWaiting for Go Code interval.");
				//sendDBGALL(USER_PORT,dbgbuf);	
				return BUSY;	
			}

			//sendDBGALL(USER_PORT,"\r\nRequesting new Go Code.");
	
			// Request Go Code to determine state
			if(requestSunSensorTelemetry(SUN_PORT,0) != SUCCESS)
			{
				//sprintf(dbgbuf,"\r\nFailed - State: %u",state);
				//sendDBGALL(USER_PORT,dbgbuf);	
				state = DONE;
				return SUBFUNCTION_ERR;
			}

			state = GO_CODE_REPLY;
		}
		case GO_CODE_REPLY:
		{
			// Check for reply
			response = getSunSensorReply(port,data,MAX_NSP_DATA_FIELD_LENGTH,&length);

			if(response != SUCCESS) 
			{
				// If some type of failure
				if(response != BUSY) 
				{
					//sprintf(dbgbuf,"\r\nFailure in state: %u",state);
					//sendDBGALL(USER_PORT,dbgbuf);	
				}
				return BUSY;
			}
		
			// Verify Length
			if(length<2)
			{
				//sprintf(dbgbuf,"\r\nFailed - Bad Go Code Length - State: %u",state);
				//sendDBGALL(USER_PORT,dbgbuf);
				HexPrint(USER_PORT,data,length);	
				state = DONE;
				return SUBFUNCTION_ERR;
			}

			// Debug
			//sendDBGALL(USER_PORT,"\r\nGo Code:");
			//byteMe(data[1]);

			// Check the value of the Go Code
			if(data[1] != 0x00)
			{
				// Restart Interval Timer:
				goCodeIntervalTimer = MSEC + GO_CODE_INTERVAL;

				// Return to previous state
				state = GO_CODE_CMDUEST;

				return BUSY;
			}

			state = CHANNEL_CMDUEST;
		}
		case CHANNEL_CMDUEST:
		{
			//sendDBGALL(USER_PORT,"\r\nRequesting Channel.");
	
			// Request a channel
			if(requestSunSensorTelemetry(port,requestedChannels[chanIndex]) != SUCCESS)
			{
				//sprintf(dbgbuf,"\r\nFailed - State: %u",state);
				//sendDBGALL(USER_PORT,dbgbuf);	
				state = DONE;
			  	return SUBFUNCTION_ERR;
			}

			//sprintf(dbgbuf,"\r\nRequested: Index: %u Channel: %u",chanIndex,requestedChannels[chanIndex]);
			//sendDBGALL(USER_PORT,dbgbuf);

			state = CHANNEL_CMDUEST_REPLY;
		}
		case CHANNEL_CMDUEST_REPLY:
		{
			// Check for reply for channel
			response = getSunSensorReply(port,data,MAX_NSP_DATA_FIELD_LENGTH,&length);

			if(response != SUCCESS) 
			{
				// If some type of failure
				if(response != BUSY) 
				{
					//sprintf(dbgbuf,"\r\nFailure in state: %u",state);
					//sendDBGALL(USER_PORT,dbgbuf);	
				}
				return BUSY;
			}

			// Decode telemetry
		 	decodeSunSensorTelemetry(data,length,Data);

			//sprintf(dbgbuf,"\r\nReceived: Index: %u Channel: %u",chanIndex,requestedChannels[chanIndex]);
			//sendDBGALL(USER_PORT,dbgbuf);
			//HexPrint(USER_PORT,data,length);

			// Move to next channel
			chanIndex++;

			// If we are at the end of the list
			if(chanIndex >= requestedChannelsLength)
			{
				//sprintf(dbgbuf,"\r\nSuccess - State: %u",state);
				//sendDBGALL(USER_PORT,dbgbuf);	
				//MeasureTime(0);

				//sendDBGALL(USER_PORT,"+");	
				state = DONE;
			 	return SUCCESS;
			}

			state = CHANNEL_CMDUEST;

			return BUSY;
		}
		case DONE:
		{
			// This is the retirement state.  If we succeed or fail anywhere above, we should end
			// up here.  Then, each time the user calls this function following a success or
			// failure, they will get a failure returned until the time they call the
			// function with the reset flag set to true.
			//sendDBGALL(USER_PORT,"\r\nFAILURE - getSensorData() called when not BUSY.");
			//sendDBGALL(USER_PORT,"D");
			return FAILURE;
		}
		default:
		{
			//sendDBGALL(USER_PORT,"\r\nFAILURE - Invalid state in getSensorData()");
			return UNREACHABLE;
		}
	}//switch
}//getSunSensorData()

/*=================================================================
  PURPOSE: Wait for reply from sunsensor
  UNMODIFIED: port - which port to look for reply on
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t blockForSunSensorACK(int port)
{
	unsigned char data[MAX_NSP_DATA_FIELD_LENGTH];	// Char array to store results
	ReturnErr_t response = SUCCESS;					// Response of getSunSensorReply()
	unsigned int length = 0;
	unsigned long timeout;	// Timer variable

	timeout = SEC_TIMER + SS_WAIT_TIME;

	do
	{
		// Look for response.
		response = getSunSensorReply(port,data,MAX_NSP_DATA_FIELD_LENGTH,&length);

	}while(response != SUCCESS && SEC_TIMER<timeout);

	//HexPrint(USER_PORT,GLOBAL_hugebuf,260); 
	//GLOBAL_cnt = 0;
	//memset(GLOBAL_hugebuf,0x00,sizeof(GLOBAL_hugebuf));

	return response;
}

ReturnErr_t blockForSunSensorReply(int port, unsigned char * data, int * lengthPtr)
{
	ReturnErr_t response;					// Response of getSunSensorReply()
	int tries = 0;
	unsigned long timeout;	// Timer variable

	timeout = SEC_TIMER + SS_WAIT_TIME;

	do
	{
		// Look for response.
		response = getSunSensorReply(port,data,MAX_NSP_DATA_FIELD_LENGTH,lengthPtr);

		tries++;

	}while(response != SUCCESS && SEC_TIMER<timeout);

	//sprintf(dbgbuf,"\r\nSLIP called %u times. Buffer:",tries);
	//sendDBGALL(USER_PORT,dbgbuf);

	//HexPrint(USER_PORT,GLOBAL_hugebuf,260); 
	//GLOBAL_cnt = 0;
	//memset(GLOBAL_hugebuf,0x00,sizeof(GLOBAL_hugebuf));

	return response;
}

/*=================================================================
  PURPOSE: Wait for Sun Sensor Go Code to become zero
  UNMODIFIED: port - which port to look for reply on
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t blockForSunSensorGoCode(int port)
{
	unsigned char data[MAX_NSP_DATA_FIELD_LENGTH];	// Char array to store results
	int length = 0;
	unsigned long timeout;	// Timer variable

	timeout = SEC_TIMER + GOCODE_WAIT_TIME;

	// Wait for frame cycle to finish
	do{

		// Request Go Code to determine state
		if(requestSunSensorTelemetry(SUN_PORT,0) != SUCCESS) return  SUBFUNCTION_ERR;
	
		// Wait for reply
		if(blockForSunSensorReply(port,data,&length) != SUCCESS) return SUBFUNCTION_ERR;

		// Verify Length
		if(length<2) return SUBFUNCTION_ERR;

		//sendDBGALL(USER_PORT,"\r\nGo Code:");
		//byteMe(data[1]);

		// Put a little delay in there
		#ifdef DEBUG
		delay_ms(100);
		#endif
	}
	while(data[1] != 0x00 && SEC_TIMER<timeout);

	return SUCCESS;
}



/*=================================================================
  PURPOSE: Convert the sunsensor data into the vehicle's body frame
  UNMODIFIED: 
  MODIFIED VARIABLES: The Sun Vector in SunSensorData
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/
ReturnErr_t convertFrameSStoBody(sunsensor_s * SunSensorData)
{
	// The sunsensor must be mounted so that the connector lines up with
	// the NEGATIVE X BODY AXIS.  This makes the X axis work with no alteration.
	// The Sun Sensor's X axis must be flipped, so that torques about the Body Y are correct.

	SunSensorData->Vector[0] = (-1.0)*SunSensorData->Vector[0]; 	// Flip the Axis.

	return SUCCESS;
}


//========================================
// 		Development Functions
//========================================

// Note: The below functions are only necessary for testing and development.  They
// can be removed during actual production

#ifdef DEBUG	

/*=================================================================
  PURPOSE: Get all data from the Sun Sensor
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: 
		SunSensorData - Struct holding all the aquired data.
  GLOBALS: None.
  RETURN VALUE: Error code.
  NOTES: None.
-----------------------------------------------------------
*/
	
ReturnErr_t collectAllDataSunSensor(int port, sunsensor_s * SunSensorData)
{		
	unsigned char data[MAX_NSP_DATA_FIELD_LENGTH];	// Char array to store results
	int length;								// Length of response
	int i=0;								// Counter

	// Start a frame cycle
	if(startSunSensorFrameCycle(SUN_PORT) != SUCCESS)  return SUBFUNCTION_ERR;

	// Get ACK
	if(blockForSunSensorACK(port) != SUCCESS) return SUBFUNCTION_ERR;

	// Get Go Code
	if(blockForSunSensorGoCode(port) != SUCCESS) return SUBFUNCTION_ERR;

	// Now collect data from registers
	for(i=0;i<SS_NUM_OF_CHANNELS;i++)
	{
		// Skip channels 4 and 5, image buffers handled differently
		// Skip channels 10-12, reserved for CAN functionality
		if(i==4 || i==5 || i==10 || i==11 || i==12)
		{}
		else
		{
			// Request the Channel
			if(requestSunSensorTelemetry(port,i) != SUCCESS) return SUBFUNCTION_ERR;
			
			// Receive data
			if(blockForSunSensorReply(port,data,&length) != SUCCESS) return SUBFUNCTION_ERR;

			// Decode it into the structure.
			decodeSunSensorTelemetry(data,length,SunSensorData);
		}//else
	}//for		

	return SUCCESS;
}

/*=================================================================
  PURPOSE: Set an image from the Sun Sensor and place into struct
  UNMODIFIED VARIABLES: operating - True for Operating image or False for Offline image.
  MODIFIED VARIABLES: 
		SunSensorData - Struct holding all the aquired data.
  GLOBALS: None.
  RETURN VALUE: Error code.
  NOTES: Because of size limitations on NSP packets, we must
	use three seperate requests to aquire parts of the image.
	As each part is received, it is placed into the structure
	at a specific point depending on what part of the iamge it is.

  !!!! UNTESTED !!!!!

-----------------------------------------------------------
*/
/*
ReturnErr_t setSunSensorImage(int port, sunsensor_s * SunSensorData, short operating)
{
	//unsigned char data[MAX_NSP_DATA_FIELD_LENGTH];	// Char array to send to Sun Sensor
	int totalLength;						// Total length of data field
	unsigned char imageRequestData[2];				// Used for image writig
	unsigned char pixelIndex[3] = {0,100,200};		// The pixels Index for each write 
	unsigned char pixelCount[3] = {100,100,56};		// The pixel Count for each write
	unsigned char i = 0;								// Counter Variable

	// Set the correct Telemetry Channel
	if(operating) 	imageRequestData[0] = 4;
	else 			imageRequestData[0] = 5;

	for(i=0;i<3;i++)
	{
		// Finish up the Telemetry request data
		imageRequestData[1] = pixelIndex[i];

		// Copy in the command part of the NSP data.
		memcpy(data,imageRequestData,2);

		// Copy in the raw bytes of the image to the NSP data.
		// The source pointer is the Image in the struct plus the offset for twice the 
		// pixel index since each pixel is two bytes.
		if(operating) 	memcpy(data,SunSensorData->OperatingImage + (pixelIndex[i]*2), pixelCount[i]*2);
		else 			memcpy(data,SunSensorData->OfflineImage + (pixelIndex[i]*2), pixelCount[i]*2);

		// Update total length:
		totalLength = 2 + pixelCount[i]*2;
	
		// Write part of the image.
		if(sendCommand(port,APP_COMMAND,data,totalLength) != SUCCESS) return SUBFUNCTION_ERR;

		if(blockForSunSensorACK(port) != SUCCESS) return SUBFUNCTION_ERR;
	}
	
	return SUCCESS;
}
*/
/*=================================================================
  PURPOSE: Print out the entire Sun Sensor struct
  UNMODIFIED VARIABLES: Data - The sun sensor struct
  MODIFIED VARIABLES: 
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

void printSunSensorStruct(sunsensor_s * Data)
{
	int i=0;

	for(i=0;i<SS_NUM_OF_CHANNELS;i++)
	{
		printSunSensorChannel(i,Data);
	}//for

	return;
}

/*=================================================================
  PURPOSE: Print out the entire Sun Sensor struct
  UNMODIFIED VARIABLES: Data - The sun sensor struct
  MODIFIED VARIABLES: 
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

void printSunSensorChannel(unsigned char channel, sunsensor_s * Data)
{
//	int i = 0;
//	sprintf(dbgbuf,"\r\n------ Channel %d: ------ ",channel);
	//sendDBGALL(USER_PORT,dbgbuf);

	switch(channel)
	{
		case 0:
		{		
			byteMe(Data->GoCode);
			break;
		}
		case 1:
		{
			sprintf(dbgbuf,"\r\nMinExposure: %Lu MaxExposure: %Lu TargetBrightness: %u",
				Data->MinExposure,Data->MaxExposure,Data->TargetBrightness);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}
		case 2:
		{
			sprintf(dbgbuf,"\r\nMinBrightness: %Lu MaxBrightness: %Lu IntegratedBrightness: %Lu ExposureLength: %Lu",
				Data->MinBrightness,Data->MaxBrightness,Data->IntegratedBrightness,Data->ExposureLength);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}
		case 3:
		{
			sprintf(dbgbuf,"\r\nTemperature: %Lu",
				Data->Temperature);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}
		case 4:
		{
			//sendDBGALL(USER_PORT,"\r\nOperatingImage: NOT IMPLEMENTED.\r\n");
/*	
			HexPrint(USER_PORT,Data->OperatingImage,512);
			//sendDBGALL(USER_PORT,"\r\n");	
			for(i=0;i<512;i=i+2)
			{
				if(i%25==0 && i>0) sendDBGALL(USER_PORT,"\r\n");
				sprintf(dbgbuf,"%Lu,",(int)(Data->OperatingImage[i]) + 256*((int)Data->OperatingImage[i+1]) );
				sendDBGALL(USER_PORT,dbgbuf);		
			}
*/
			break;
		}
		case 5:
		{
			//sendDBGALL(USER_PORT,"\r\nOfflineImage: NOT IMPLEMENTED.\r\n");
/*	
			HexPrint(USER_PORT,Data->OfflineImage,512);
			//sendDBGALL(USER_PORT,"\r\n");	
			for(i=0;i<512;i=i+2)
			{
				if(i%25==0 && i>0) sendDBGALL(USER_PORT,"\r\n");
				sprintf(dbgbuf,"%Lu,",(int)(Data->OfflineImage[i]) + 256*((int)Data->OperatingImage[i+1]) );
				sendDBGALL(USER_PORT,dbgbuf);		
			}
*/
			break;
		}
		case 6:
		{
			sendDBGALL(USER_PORT,"\r\nVector X: ");
			printFloat(dbgbuf,Data->Vector[0],2);
			sendDBGALL(USER_PORT,dbgbuf);

			sendDBGALL(USER_PORT," Y: ");
			printFloat(dbgbuf,Data->Vector[1],2);
			sendDBGALL(USER_PORT,dbgbuf);

			sendDBGALL(USER_PORT," Z: ");
			printFloat(dbgbuf,Data->Vector[2],2);
			sendDBGALL(USER_PORT,dbgbuf);

			sprintf(dbgbuf,"\r\nFitQuality %Ld GeometryQuality %Ld",
				Data->FitQuality,Data->GeometryQuality);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}
		case 7:
		{
			sprintf(dbgbuf,"\r\nIterations: %Lu",
				Data->Iterations);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}	
		case 8:
		{
			sprintf(dbgbuf,"\r\nProcessingTime: %Lu",
				Data->ProcessingTime);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}
		case 9:
		{
			sprintf(dbgbuf,"\r\nVector_Int X: %Ld Y: %Ld Z: %Ld FitQuality %Ld GeometryQuality %Ld",
				Data->Vector_int[0],Data->Vector_int[1],Data->Vector_int[2],Data->FitQuality,Data->GeometryQuality);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}
		case 13:
		{
			sprintf(dbgbuf,"\r\nDiagnostic: 1: %Lu 2: %Lu 3: %Lu",
				Data->Diagnostic[0],Data->Diagnostic[1],Data->Diagnostic[2]);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}	
		case 14:
		{
			sendDBGALL(USER_PORT,"\r\nCelsiusTemperature: ");
			printFloat(dbgbuf,Data->CelsiusTemperature,2);
			sendDBGALL(USER_PORT,dbgbuf);
			break;
		}	
		default:
		{
			break;
		}
	}//switch
}

void sunAutomaticDataCollection(long iterations, short continuous, short human)
{
/*
	sunsensor_s Data;			// The data structure to use for each iteration
	unsigned char channels[4] = {		// Channels to collect
				SS_TELEMETRY,
				SS_TEMPERATURE,
				SS_SUN_VECTOR_FLOAT,
				SS_PROCESSING_TIME
				};
	unsigned char numChannels = 4;	// Channels to collect
	long i = 0, j = 0;		// Iteration counter
	long timeout;			// Timeout used to pause before data collection
	short userFlag = 1;		// Indicator that user wants to quit.  (False = quit)
	unsigned char temp;				// Temporary character holder to check user keystroke
	short reset = 1;		// Reset the first time through
	short sync = 0;			
	ReturnErr_t response;	// Error code returned by getSunSensorData()
	
	// Let user know how to escape!
	sendDBGALL(USER_PORT,"\r\nCollecting Sun Sensor data.  Press 'q' to quit.  Starting in 1 seconds ...");

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

		// Set the timeout timer
		timeout=SEC_TIMER+GOCODE_WAIT_TIME;

		// Reset the Function
		reset = 1;

		// Flip the sync bit
		sync = !sync;

		do{
			// Collect all data
			response = getSunSensorData(SUN_PORT,channels,numChannels,&Data,reset,sync);
			reset = 0;
		}while(response == BUSY && SEC_TIMER<timeout);
	
		//sendDBGALL(USER_PORT,"\r\nDone");

		if(response != SUCCESS)
		{
			sendDBGALL(USER_PORT,"\r\n\FAILURE - Bad response in sunAutoDataCollection().");
			return;
		}
		else
		{
			sendDBGALL(USER_PORT,"\r\n");
			for(j=0;j<numChannels;j++)
			{
				printSunSensorChannel(channels[j],&Data);
			}
		}

		delay_ms(2000);

		// Check if a character is available
		if(getByte(USER_PORT,&temp)) 
		{
			// If it is any form of 'q', signal that we want to quit.
			if(temp == 'q' || temp == 'Q') userFlag = 0;
		}
	}
	
	return;
*/
}

/*=================================================================
  PURPOSE: Ping the sun sensor
  UNMODIFIED: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Will reply with a human readable message containing
	information on the device.
-----------------------------------------------------------
*/

ReturnErr_t pingSunSensor(int port)
{

	unsigned char dummy;								// Dummy variable to create a pointer
	unsigned char data[MAX_NSP_DATA_FIELD_LENGTH];	// Char array to store results
	unsigned int length=0;								// Length of response
	int i=0;								// Counter variable used below to print characters

	// Send PING command with no data
	if(sendCommand(port,PING,&dummy,0) != SUCCESS) return SUBFUNCTION_ERR;

	// Get data
	if(blockForSunSensorReply(port,data,&length) != SUCCESS) return SUBFUNCTION_ERR;

	sprintf(dbgbuf,"\r\nPing Reply Length: %u",length);
	sendDBGALL(USER_PORT,dbgbuf);
	for(i=0;i<length;i++) sendByte(USER_PORT,data[i]);	
	
	return SUCCESS;
}


/*=================================================================
  PURPOSE: Test the CRC engine.
  UNMODIFIED: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Taken verbatim from SS Documentation.  Can also be found
	at: http://www.utias-sfl.net/docs/ssp2.1e.pdf
-----------------------------------------------------------
*/

void testCRC()
{
/*
	unsigned char test1[8] = {0x43, 0x43, 0x49, 0x54, 0x54, 0x2d, 0x31, 0x36};
	unsigned char test2[9] = {'1','2','3','4','5','6','7','8','9'};
	unsigned char test3[3] = {0x11,0x39,0x87};
	int crc;

	crc = makeSSCRC(test1,8);
	sprintf(dbgbuf,"\r\nTest CRC1: %LX (should be: 0x2364)",crc);
	sendDBGALL(USER_PORT,dbgbuf);

	crc = makeSSCRC(test2,9);
	sprintf(dbgbuf,"\r\nTest CRC2: %LX (should be: 0x6f91)",crc);
	sendDBGALL(USER_PORT,dbgbuf);

	crc = makeSSCRC(test3,3);
	sprintf(dbgbuf,"\r\nTest CRC2: %LX (should be: 0x6f91)",crc);
	sendDBGALL(USER_PORT,dbgbuf);
*/
}

#endif // DEBUG

 
			
	
