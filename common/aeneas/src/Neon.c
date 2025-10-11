/***************************************************************************

									Neon.c

Company: USC/ISI
License: Proprietary
Author(s): Omair A. Rahman (OAR)

Purpose: Set of Functions for a Neon1 connected to a PIC.

Changelog:

Date 	 |  Au.  |	Notes
06-29-10	OAR		Created.

****************************************************************************
*/

#include <Neon.h>						// Neon Header File

#warning Neon c file included.

#module

// Private Functions
void Calculate_Checksum(unsigned char *InputMsg, unsigned char *OutputChkSum, int MsgLength);
void Calculate_Checksum_Rolling(unsigned char byte, unsigned char *OutputChkSum);
void C2H(unsigned char c,unsigned char * hex);
void testC2H();

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

#ifdef DEBUG
void testC2H()
{
	int c;
	unsigned char hx[2];

	for(c=0;c<256;c++)
	{
		C2H(c,hx);
		sendMSG(USER_PORT,hx,2);
		sendByte(USER_PORT,' ');
	}
	return;
}
#endif

/*=================== configureNeon ========================
  PURPOSE: Configure the Neon beacon
  ARGUMENT: The port the Neon is on
  RETURN VALUE: no significance.
  NOTES: Not sure whether or not we need to do this
-----------------------------------------------------------*/
void configureBeacon() {
	sendByte(BEACON_PORT,'D');
}	


void BeaconPower(unsigned char OnOff)								// Turns the Beacon on or off
{
	if(OnOff == BEACON_ON)
	{
		output_high(BEACON_SHUTDOWN);	
	}
	else if (OnOff == BEACON_OFF)
	{
		output_low(BEACON_SHUTDOWN);
	}
}

/*=================== NeonBeaconSendChar ========================
  PURPOSE: Send a Character via a Neon beacon
  ARGUMENT: The character to be sent.
  RETURN VALUE: no significance.
  NOTES: This function may not be required, but is faster.
-----------------------------------------------------------*/
int8 BeaconSendChar(unsigned char CharToSend)
{
	sendByte(BEACON_PORT,'N');	
	sendByte(BEACON_PORT,'e');	
	sendByte(BEACON_PORT,0x01);	
	sendByte(BEACON_PORT,CharToSend);	
	sendByte(BEACON_PORT,CharToSend);	
	sendByte(BEACON_PORT,CharToSend);
	return 0;
}

/*=================== NeonBeaconSendMessage ========================
  PURPOSE: Send a String via a Neon beacon
  ARGUMENT: The message string to be sent, the length of the string.
  RETURN VALUE: no significance.
  NOTES: Message Length of 1 or greater.
-----------------------------------------------------------*/
int8 BeaconSendMessage(unsigned char *Message, int MsgLength)
{
	unsigned char CalculatedCheckSum[2];
	CalculatedCheckSum[0] = "r";
	CalculatedCheckSum[1] = "t";

	unsigned char len;
	len = (unsigned char)MsgLength;
	
	if(MsgLength>255 || MsgLength<1) return -1;
	
	Calculate_Checksum(Message, CalculatedCheckSum, MsgLength);
	sendMSG(BEACON_PORT, "Ne", 2);	
	sendByte(BEACON_PORT,len);	
	sendMSG(BEACON_PORT, Message, MsgLength);	
	sendByte(BEACON_PORT,CalculatedCheckSum[0]);	
	sendByte(BEACON_PORT,CalculatedCheckSum[1]);
	return 0;
}

/*=================== NeonBeaconSendMessage ========================
  PURPOSE: Send a String via a Neon beacon that has been converted into hex.
  ARGUMENT: The message string to be sent, the length of the string.
  RETURN VALUE: no significance.
  NOTES: Message Length of 1 or greater.
-----------------------------------------------------------*/
int8 BeaconSendData(unsigned char *Message, int MsgLength)
{
	int i;
	unsigned char temp[2];
	unsigned char CalculatedCheckSum[2] = {0,0};
	
	if(MsgLength*2>255) return -1;
	
	// Send Header
	sendMSG(BEACON_PORT, "Ne", 2);	
	sendByte(BEACON_PORT,MsgLength*2);	
	
	// Send Message
	for(i=0;i<MsgLength;i++){
		C2H(Message[i],temp);
		sendByte(BEACON_PORT,temp[0]);
		Calculate_Checksum_Rolling(temp[0],CalculatedCheckSum);
		sendByte(BEACON_PORT,temp[1]);
		Calculate_Checksum_Rolling(temp[1],CalculatedCheckSum);
	}
	
	// Send Checksum
	sendByte(BEACON_PORT,CalculatedCheckSum[0]);	
	sendByte(BEACON_PORT,CalculatedCheckSum[1]);
	return 0;
}

/*=================== Calculate_Checksum ========================
  PURPOSE: Calculate_Checksum for the Neon beacon frame
  ARGUMENT: The message string to be sent, the returned string with the ChkSum, the length of the input string.
  RETURN VALUE: none.
  NOTES: Message Length of 1 or greater.
-----------------------------------------------------------*/
void Calculate_Checksum(unsigned char *InputMsg, unsigned char *OutputChkSum, int MsgLength)
{
	int i;

	OutputChkSum[0] = 0;
	OutputChkSum[1] = 0;

	for (i=0; i<MsgLength; i++)
	{
		OutputChkSum[0] += *(InputMsg+i);
		OutputChkSum[1] += OutputChkSum[0];
	}
}

/*=================== Calculate_Checksum ========================
  PURPOSE: Calculate_Checksum for the Neon beacon frame
  ARGUMENT: The message string to be sent, the returned string with the ChkSum, the length of the input string.
  RETURN VALUE: none.
  NOTES: Message Length of 1 or greater.
-----------------------------------------------------------*/
void Calculate_Checksum_Rolling(unsigned char byte, unsigned char *OutputChkSum)
{
	OutputChkSum[0] += byte;
	OutputChkSum[1] += OutputChkSum[0];
}

