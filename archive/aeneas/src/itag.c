/***************************************************************************

									itag.c

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Provide hardware interface to icontrol iTag

Changelog:

Date 	 |  Au.  |	Notes
10-26-10   Will 	Initial revision.		

****************************************************************************
*/


#include <itag.h>		// Public header

#module					// !!! Important: This command makes everything below scoped only to this file.

//======================================
//				Defines
//======================================

//========================================
// 			Driver Functions
//========================================

// Note: The below functions are necessary to use hardware.

/*=================================================================
  PURPOSE: Initialize the Itag (put it into binary mode)
  UNMODIFIED VARIABLES: port - the Serial port to use.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

// Start the itag streaming binary data.
void itag_init(unsigned char port)
{
	// Put the iTag into data mode
	sendByte(port,'p'); 

	// Put itag into binary viewing mode
	sendByte(port,'b'); 

	return;
}


/*=================================================================
  PURPOSE: Get the iTag to acknowledge a command
  UNMODIFIED VARIABLES: port - the Serial port to use., 
					timeout_seconds -- seconds to keep trying
  MODIFIED VARIABLES: None.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/
ReturnErr_t itag_functional_check(unsigned char port, unsigned int timeout_seconds)
{
	unsigned long timeout;
	unsigned char response[GENERAL_BUFFER_SIZE];
	unsigned int i=0;
	unsigned int j;
	unsigned char checkword[10];
	
	timeout = SEC_TIMER + timeout_seconds;		// set the timeout
	memset(response,0x00,GENERAL_BUFFER_SIZE);	// clear the response
	sprintf(checkword,"CHECKOUT");

	// It might be asleep, so wake it up with a bunch of carriage returns
	//sendMSGALL(port,"\r\r\r\n\n\n");

	// Send a command
	sendMSGALL(port,"r 39 1 0 \"CHECKOUT\"\r\n");

	// Read results -- we're looking for the word CHECKOUT
	do{
		// If we get a character...
		if(getByte(port,&response[i]))
		{
			i++;								// increment write index
			//HexPrint(USER_PORT,response,i);	// DEBUG
			if(i>=GENERAL_BUFFER_SIZE) i =0;	// wraparound check
			
			// Search 
			j=0;
			for(j=0;j<GENERAL_BUFFER_SIZE;j++)
			{
				if(response[j]=='C') // if we match first character
				{
					// and if the rest of the word is there
					if(memcmp(&response[j], checkword, strlen(checkword)) == 0){
						// SUCCESS!  Let's take a second to clear out the rest
						// of the buffer before returning
						while(timeout>SEC_TIMER && getByte(port,&response[0]));
						{ 
							delay_ms(1);
							// do nothing
						}
						return SUCCESS;
					}

					// NOTE: This won't handle the case where the word is wrapped 
					// across the buffer boundary.
				}
			}
		}
	}while(timeout>SEC_TIMER);

	return FAILURE;
}

/*=================================================================
  PURPOSE: Start the itag up prior to a pass over an iGate
  UNMODIFIED VARIABLES: port - the Serial port to use.
  MODIFIED VARIABLES: None.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

// Start itag beaconing
void itag_on(unsigned char port)
{
	// For now, this is the same as init:
	sendByte(port,'r'); //delay_ms(10);
	sendByte(port,' '); //delay_ms(10);
	sendByte(port,'1'); //delay_ms(10);
	sendByte(port,' '); //delay_ms(10);
	sendByte(port,'2'); //delay_ms(10);
	sendByte(port,'\r'); //delay_ms(10);

	sendDBGALL(USER_PORT,"\r\nStarted itag data stream.");

	return;
}

/*=================================================================
  PURPOSE: Stop the itag following a pass over an iGate
  UNMODIFIED VARIABLES: port - the Serial port to use.
  MODIFIED VARIABLES: None.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

// Stop itag beaconing
void itag_off(unsigned char port)
{
	sendByte(port,'r'); //delay_ms(10);
	sendByte(port,' '); //delay_ms(10);
	sendByte(port,'1'); //delay_ms(10);
	sendByte(port,' '); //delay_ms(10);
	sendByte(port,'0'); //delay_ms(10);
	sendByte(port,'\r'); //delay_ms(10);

	sendDBGALL(USER_PORT,"\r\nStopped itag data stream.");
}

/*=================================================================
  PURPOSE: Request one packet of telemetry from iTag
  UNMODIFIED VARIABLES: port - the Serial port to use.
  MODIFIED VARIABLES: None.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/
void itag_once(unsigned char port)
{
	sendDBGALL(USER_PORT,"\r\nRequested one data packet from itag (not implemented)");

	return;
}

/*=================================================================
  PURPOSE: Request a No Op packet from the iTag to act as a ping
  UNMODIFIED VARIABLES: port - the Serial port to use.
  MODIFIED VARIABLES: None.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/
void itag_ping(unsigned char port)
{
	sendMSGALL(port,"r 39 1 0 \"hello\" \r");

	sendDBGALL(USER_PORT,"\r\nPinged iTag.");

	return;
}

/*=================================================================
  PURPOSE: Send a binary data string to the ground via database.
  UNMODIFIED VARIABLES: port - the Serial port to use.
			data - the data string to send
			length - the length of the string
			timestamp - a 7 byte unsigned char array with the current time
  MODIFIED VARIABLES: None.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/
void sendVIAiTag(unsigned char port, unsigned char * data, unsigned char length, unsigned char * timestamp)
{
	// Calculate total packet length (11 for pktID (2), time (7), TCW (1), Mode (1))
	unsigned char packetLength = length + 11;
	unsigned char header[6] = {0xff,0x5e,64,101,0,0};

	// Verify length is not too much
	if(packetLength > 128)
	{	
		sendDBGALL(USER_PORT,"\r\nERROR - Trying to send iTag packet > 128 bytes");
		return;
	}

	// Send Sync:
	sendByte(port,header[0]);					//HexPrint(USER_PORT,&header[0],1);
	sendByte(port,header[1]);					//HexPrint(USER_PORT,&header[1],1);

	// Send Header Info:
	sendByte(port,packetLength);				//HexPrint(USER_PORT,&packetLength,1);
	sendByte(port,header[2]);					//HexPrint(USER_PORT,&header[2],1);
	sendByte(port,header[3]);					//HexPrint(USER_PORT,&header[3],1);
	
	// Send Time:
	sendMSG(port,timestamp,7);					//HexPrint(USER_PORT,timestamp,7);

	// Send TCW and Mode (zeros are fine):
	sendByte(port,header[4]);					//HexPrint(USER_PORT,&header[4],1);
	sendByte(port,header[5]);					//HexPrint(USER_PORT,&header[5],1);

	// Send Data
	sendMSG(port,data,length);					//HexPrint(USER_PORT,data,length);	

	sendDBGALL(USER_PORT,"\r\nSent data to itag.");
	sprintf(dbgbuf,"\r\nPacket (dlength = %u, plength = %u):",length,packetLength); 
	sendDBGALL(USER_PORT,dbgbuf);

	return;
}	
#endif

//========================================
// 		Development Functions
//========================================

// Note: The below functions are only necessary for testing and development.  They
// can be removed during actual production

#ifdef DEBUG

#endif // DEBUG





