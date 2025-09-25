/***************************************************************************

									stensat.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide beacon functions

Changelog:

Date 	 |  Au.  |	Notes
08-17-10	mra		Created.
10-31-11	mra		Updated to provide common functions as the Neon driver, 
					so they can be swapped easily.

****************************************************************************
*/

#include <stensat.h>

#MODULE  // Limit the scope of globals declared here


// Converts a single unsigned char to 2 Hex chars
// Eg: c=155, hex[0]='9' and hex[1]='B'
void C2H(unsigned char c,unsigned char * hex)
{
	// Do this as a giant lookup table for speed
	const unsigned char hexchart[256][3] = {
	"00",	"01",	"02",	"03",	"04",	"05",	"06",	"07",	"08",	"09",	"0A",	"0B",	"0C",	"0D",	"0E",	"0F",	"10",	"11",	"12",	"13",	"14",	"15",	"16",	"17",	"18",	"19",	"1A",	"1B",
	"1C",	"1D",	"1E",	"1F",	"20",	"21",	"22",	"23",	"24",	"25",	"26",	"27",	"28",	"29",	"2A",	"2B",	"2C",	"2D",	"2E",	"2F",	"30",	"31",	"32",	"33",	"34",	"35",	"36",	"37",
	"38",	"39",	"3A",	"3B",	"3C",	"3D",	"3E",	"3F",	"40",	"41",	"42",	"43",	"44",	"45",	"46",	"47",	"48",	"49",	"4A",	"4B",	"4C",	"4D",	"4E",	"4F",	"50",	"51",	"52",	"53",
	"54",	"55",	"56",	"57",	"58",	"59",	"5A",	"5B",	"5C",	"5D",	"5E",	"5F",	"60",	"61",	"62",	"63",	"64",	"65",	"66",	"67",	"68",	"69",	"6A",	"6B",	"6C",	"6D",	"6E",	"6F",
	"70",	"71",	"72",	"73",	"74",	"75",	"76",	"77",	"78",	"79",	"7A",	"7B",	"7C",	"7D",	"7E",	"7F",	"80",	"81",	"82",	"83",	"84",	"85",	"86",	"87",	"88",	"89",	"8A",	"8B",
	"8C",	"8D",	"8E",	"8F",	"90",	"91",	"92",	"93",	"94",	"95",	"96",	"97",	"98",	"99",	"9A",	"9B",	"9C",	"9D",	"9E",	"9F",	"A0",	"A1",	"A2",	"A3",	"A4",	"A5",	"A6",	"A7",
	"A8",	"A9",	"AA",	"AB",	"AC",	"AD",	"AE",	"AF",	"B0",	"B1",	"B2",	"B3",	"B4",	"B5",	"B6",	"B7",	"B8",	"B9",	"BA",	"BB",	"BC",	"BD",	"BE",	"BF",	"C0",	"C1",	"C2",	"C3",
	"C4",	"C5",	"C6",	"C7",	"C8",	"C9",	"CA",	"CB",	"CC",	"CD",	"CE",	"CF",	"D0",	"D1",	"D2",	"D3",	"D4",	"D5",	"D6",	"D7",	"D8",	"D9",	"DA",	"DB",	"DC",	"DD",	"DE",	"DF",
	"E0",	"E1",	"E2",	"E3",	"E4",	"E5",	"E6",	"E7",	"E8",	"E9",	"EA",	"EB",	"EC",	"ED",	"EE",	"EF",	"F0",	"F1",	"F2",	"F3",	"F4",	"F5",	"F6",	"F7",	"F8",	"F9",	"FA",	"FB",
	"FC",	"FD",	"FE",	"FF"
	};
	hex[0] = hexchart[c][0];
	hex[1] = hexchart[c][1];

	return;
}

// Configures the Stensat beacon to default values.
// TODO: Upgrade to allow non-default values to be passed in to the function.
// Return SUCCESS or FAILURE based on the presence of ACKs, but will continue to configure even if
// an error is encountered.
ReturnErr_t configureBeacon()
{
	unsigned char ack;
	unsigned char * temp;
	unsigned long TIMEOUT;
	ReturnErr_t err;

	// Clear the acks and set the timeout and initializations
	ack = 'x'; 							// Explicitly set to something wrong to start
	temp = &ack;						// Pointer cause I was too lazy to rewrite the working code from Neon Beacon on which this is based
	while(getByte(BEACON_PORT,temp)){};  // Clear the buffer
	err = SUCCESS;						// Assume success

	// Set Call Sign
	TIMEOUT = SEC_TIMER + 2;			// Seconds timeout
	sendMSG(BEACON_PORT,"C",1);
	sendMSG(BEACON_PORT,globals.BeaconCallsign,6); //KE6YFA, or ???? for Rohan
	sendMSG(BEACON_PORT,"\r",1);
	while(!getByte(BEACON_PORT,temp) && TIMEOUT>SEC_TIMER){}; // Wait for ack or timeout
	if(ack!='A'){ err = FAILURE; sprintf(dbgbuf,"1.temp = %c",ack); sendDBGALL(USER_PORT,dbgbuf);}			// The beacon return "ACK" so we check for the first A.
	while(ack!=0x0d && TIMEOUT>SEC_TIMER){getByte(BEACON_PORT,temp);}; // Clear the ack for the rest of the timeout
	ack=0;
	// Set Destination Call Sign?
	//sendMSG(BeaconPort,"DCQ\r",4);
	//while(!getByte(BeaconPort,temp) && TIMEOUT>SEC_TIMER){}; // Wait for ack or timeout
	//while(getByte(BeaconPort,temp)){}; // Clear the ack

	// Set Via (relay) Call Sign
	//sendMSG(BeaconPort,"VTELEM\r",7);
	//while(!getByte(BeaconPort,temp) && TIMEOUT>SEC_TIMER){}; // Wait for ack or timeout
	//while(getByte(BeaconPort,temp)){}; // Clear the ack

	// Set RF Power Level
	TIMEOUT = SEC_TIMER + 2;			// Seconds timeout
	sendMSG(BEACON_PORT,"P9c\r",4);
	while(!getByte(BEACON_PORT,temp) && TIMEOUT>SEC_TIMER){}; // Wait for ack or timeout
	if(ack!='A'){ err = FAILURE; sprintf(dbgbuf,"2.temp = %c",ack); sendDBGALL(USER_PORT,dbgbuf);}			// The beacon return "ACK" so we check for the first A.
	while(ack!=0x0d && TIMEOUT>SEC_TIMER){getByte(BEACON_PORT,temp);}; // Clear the ack for the rest of the timeout
	ack=0;

	// Set BitRate Mode
	TIMEOUT = SEC_TIMER + 2;			// Seconds timeout
	sendMSG(BEACON_PORT,"M1200\r",6);
	while(!getByte(BEACON_PORT,temp) && TIMEOUT>SEC_TIMER){}; // Wait for ack or timeout
	if(ack!='A'){ err = FAILURE; sprintf(dbgbuf,"3.temp = %c",ack); sendDBGALL(USER_PORT,dbgbuf);}			// The beacon return "ACK" so we check for the first A.
	while(ack!=0x0d && TIMEOUT>SEC_TIMER){getByte(BEACON_PORT,temp);}; // Clear the ack for the rest of the timeout
	ack=0;

	return err;
}

// Turns beacon on and off
void BeaconPower(unsigned char OnOff)								// Turns the Beacon on or off
{
	// This function is common to all beacons, but the stensat does not have
	// an on-off setting, so this does nothing.


	if(OnOff == BEACON_ON)
	{
		
		//output_high(BEACON_SHUTDOWN);	
	}
	else if (OnOff == BEACON_OFF)
	{
		//output_low(BEACON_SHUTDOWN);
	}
}


/*=================== Stensat BeaconSendChar ========================
  PURPOSE: Send a Character via a Stensat beacon
  ARGUMENT: The character to be sent.
  RETURN VALUE: no significance.
  NOTES: This function may not be required, but is faster.
-----------------------------------------------------------*/
int8 BeaconSendChar(unsigned char CharToSend)
{
	// TODO: Check here for a valid (all-ascii) char.

	sendByte(BEACON_PORT,'S');	
	sendByte(BEACON_PORT,CharToSend);	
	sendByte(BEACON_PORT,'\r');	
	return 0;
}

/*=================== Stensat BeaconSendMessage ========================
  PURPOSE: Send a String via a Neon beacon
  ARGUMENT: The message string to be sent, the length of the string.
  RETURN VALUE: no significance.
  NOTES: Message Length of 1 or greater.
-----------------------------------------------------------*/
int8 BeaconSendMessage(unsigned char *Message, int MsgLength)
{

	unsigned char len;
	len = (unsigned char)MsgLength;
	
	if(MsgLength>255 || MsgLength<1) return -1;	// bails if too big or small

	// TODO: Check here for a valid (all-ascii) message.
	
	sendByte(BEACON_PORT,'S');	
	sendMSG(BEACON_PORT, Message, MsgLength);	
	sendByte(BEACON_PORT,'\r');	
	return 0;
}

/*=================== Stensat BeaconSendData ========================
  PURPOSE: Send a String via a Neon beacon that has been converted into hex.
  ARGUMENT: The message string to be sent, the length of the string.
  RETURN VALUE: no significance.
  NOTES: Message Length of 1 or greater.
-----------------------------------------------------------*/
int8 BeaconSendData(unsigned char *Message, int MsgLength)
{
	int i;
	unsigned char temp[2];
	
	if(MsgLength*2>255) return -1;
	
	// Send Header
	sendByte(BEACON_PORT,'S');	
	
	// Send Message
	for(i=0;i<MsgLength;i++){
		C2H(Message[i],temp);
		sendByte(BEACON_PORT,temp[0]);
		sendByte(BEACON_PORT,temp[1]);
	}
	
	// Send return
	sendByte(BEACON_PORT,'\r');	
	return 0;
}