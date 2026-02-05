/***************************************************************************

									debug.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide multiple-level debug-to-screen functionality, and
		 various other useful debugging functions.

Changelog:

Date 	 |  Au.  |	Notes
04-24-10	mra		Created.		

****************************************************************************
*/

// Dependencies
#include "debug.h"

#module

/*void sendDBGALL(int port,unsigned char * msg)
{
	sendDBGALL(port,msg,0);
}
*/

void sendDBGALL(int port,unsigned char * msg,int lvl=0)
{
	#ifdef DEBUG
	if(DEBUG_LEVEL >= lvl)
	{
		// Will's Magic Debug Port:  Port 99 - Send it to the other PPM's user port
		// This is only really applicable from the UPPER_PPM to the LOWER_PPM since
		// the upper PPM will not have a physical user Serial Port.
		if(port==99)
		{
			// Temporary buffer
			unsigned char temp[256];

			// Get the message length
			int msglength = strlen(msg);

			// If message is too short, give up.  If message is greater than 252, when it is slid
			// to the left it will become larger than 255 bytes.
			if(msglength<1 || msglength>252) return;

			// Copy data to temp buffer and move over by 3 bytes.
			memcpy(&temp[3],msg,msglength);

			// Assign command and arguments
			temp[0] = 158;				// Command to forward to a port
			temp[1] = 1;				// Port to display (USER_PORT on lower PPM)
			temp[2] = msglength+0;		// Length of message part of packet 
			//temp[msglength+3] = '(';	// Message appended to back of DBG message
			//temp[msglength+4] = 'U';
			//temp[msglength+5] = 'P';
			//temp[msglength+6] = ')';

			// Send it to the other PPM (Length is data length plus three for above cmd and arguments)
			sendPPM(OTHER_PPM_PORT,temp,msglength+3);
		}
		else sendMSGALL(port,msg);
	}
	#endif
}

void byteMeHeader()
{
//	sendDBGALL(USER_PORT,"\r\nResult: MSB ");
}

void byteMeFooter()
{
//	sendDBGALL(USER_PORT," LSB");
}

void byteMe(unsigned char c)
{
	int cnt,max;

	max = (sizeof(c)*8)-1;
	cnt= max;
	byteMeHeader();

	while(cnt>=0)
	{
		if(bit_test(c,cnt)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
		cnt--;
	}

	byteMeFooter();
}


void byteMe(int c)
{
	unsigned char a;
	
	a=make8(c,1);		// Takes the upper (leftmost) byte of 0xAABB
	byteMe((unsigned char)a);	
	a=make8(c,0);		// Takes the lower (rightmost) byte of 0xAABB
	byteMe((unsigned char)a);	
}

void byteMe(long c)
{
	unsigned char a;

	a=make8(c,0);		// Takes the lower (rightmost) byte of 0xAABB
	byteMe((unsigned char)a);	
	a=make8(c,1);		// Takes the upper (leftmost) byte of 0xAABB
	byteMe((unsigned char)a);	
	a=make8(c,2);		// Takes the upper (leftmost) byte of 0xAABB
	byteMe((unsigned char)a);	
	a=make8(c,3);		// Takes the upper (leftmost) byte of 0xAABB
	byteMe((unsigned char)a);	

}

void byteMe(float c)
{
	sendDBGALL(USER_PORT,"floats not supported in byteMe.");
}

void byteMe(float64 c)
{
	sendDBGALL(USER_PORT,"floats not supported in byteMe.");
}

void PrintDbgErr(int port, ReturnErr_t err)
{
	switch(err)
	{
		case SUCCESS: sendDBGALL(port,"SUCCESS"); break;
		case BUSY: sendDBGALL(port,"BUSY"); break;
		case FAILURE: sendDBGALL(port,"FAILURE"); break;
		case UNREACHABLE: sendDBGALL(port,"UNREACHABLE"); break;
		case OUT_OF_BOUNDS: sendDBGALL(port,"OUT_OF_BOUNDS"); break;
		case TIMEOUT: sendDBGALL(port,"TIMEOUT"); break;
		case INVALID_ARG: sendDBGALL(port,"INVALID_ARG"); break;
		case SUBFUNCTION_ERR: sendDBGALL(port,"SUBFUNCTION_ERR"); break;
		case FLASH_NOT_EMPTY: sendDBGALL(port,"FLASH_NOT_EMPTY"); break;
		case FLASH_BUSY: sendDBGALL(port,"FLASH_BUSY"); break;
		case FLASH_PROTECTED: sendDBGALL(port,"FLASH_PROTECTED"); break;
		case STACK_TOO_BIG_TO_PUSH: sendDBGALL(port,"STACK_TOO_BIG_TO_PUSH"); break;
		case STACK_INVALID_ITEM: sendDBGALL(port,"STACK_INVALID_ITEM"); break;
		case STACK_WARN_OVERWRITE: sendDBGALL(port,"STACK_WARN_OVERWRITE"); break;
		default: sendDBGALL(port,"Unknown Error"); break;
	}
}

void Tick(short reset=0)
{
	static unsigned char a='a';

	if(reset) a='a';

	sendByte(USER_PORT,a);
	a++;
	delay_ms(200);

}

// Custom Float Printing because sprintf %f crashes often.
void printFloat(unsigned char *string, float64 num, int precision=6) {
	// 8 bit enough for v
	int e = 0, v;

	// Position in the string buffer to write to
	if (num < 0) {
		*string++ = '-';
		num = -num;
	}

	// Find E value
	while (num >= 10) {
		e++; num = num/10;
	}
	while (num < 1 && num > 0) {
		e--; num = num*10;
	}

	// Now num should be in [1, 10)
	v = (int) num;
	num = num - v;
	*string++ = v+'0';
	*string++ = '.';

	// Digits after the dot
	while (precision-- > 0) {
		num = num * 10;
		v = (int) num;
		*string++ = v +'0';
		num = num-v;
	}

	if(e!=0) sprintf(string, "e%d", e);
	else *string='\0';
}


// Overloaded function for normal floats
void printFloat(unsigned char *string, float num, int precision=6) {
	printFloat(string,(float64)num,precision);
}

// Overloaded for no default
/*void printFloat(unsigned char *string, float num) {
	printFloat(string,(float64)num,6);
}
*/


// Measures time between calls.
// To use, start with MeasureTime(MEASURE_TIME_INIT);
// Then it will give you times everytime you call MeasureTime(MEASURE_TIME_SHOW).
void MeasureTime(unsigned char reset)
{
	int ticktime;
	const int overhead = 0;	// Set overhead of function here
	static unsigned int64 ms;

	if(reset==MEASURE_TIME_INIT)
	{
		ticktime=get_timer1();
		ms=MSEC;
		return;
	}
	else
	{
		ticktime = get_timer1() - ticktime - overhead;
		ms=MSEC-ms; 
		if(ms>0){
		sprintf(dbgbuf," %04u ms",ms);
		locateString(USER_PORT,35,40,dbgbuf);
		}
		else
		{
		sprintf(dbgbuf," %04u tks",ticktime);
		locateString(USER_PORT,36,40,dbgbuf);
		//sendDBGALL(USER_PORT,dbgbuf);
		}
		return;		
	}
}


/*
Displays to the screen a reading of all the output registers

Registers start are 06C0 and go to 06DE
Each is an int16.
Of each 8 bit portion in the int16, the lower 6 bits are all that count.

*/
void ShowOutputRegisters()
{
	int reg;
	int i;

	int *p;
	unsigned char *p_p;
	int8 n;

	p=getenv("SFR:RPOR0");// should be 0x06C0 -- Starting Register
	p_p=&p;

	sendDBGALL(USER_PORT,"\r\n\nOutput Register Listing");
	sendDBGALL(USER_PORT,  "\r\n----------------------------------------");
//	sendDBGALL(USER_PORT,  "\r\n");
//	HexPrint(USER_PORT,&p,2);

for(reg=0;reg<=15;reg++)
	{
		sprintf(dbgbuf,"\r\nRPOR%d(%2x%2x):\t",reg,*(p_p+1),p);
		sendDBGALL(USER_PORT,dbgbuf);
		
		if(bit_test(*p,15)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
	
		if(bit_test(*p,14)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
	
		sendDBGALL(USER_PORT,  "|");
	
		for(i=13;i>=8;i--)
		{
			if(bit_test(*p,i)) sendDBGALL(USER_PORT,"1");
			else sendDBGALL(USER_PORT,"0");
		}

		// Calculate the number and show it.
		n = make8(*p,1);
		sprintf(dbgbuf," (%i)",n);
		sendDBGALL(USER_PORT,dbgbuf);
	
		sendDBGALL(USER_PORT,  "\t");
	
		if(bit_test(*p,7)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
	
		if(bit_test(*p,6)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
	
		sendDBGALL(USER_PORT,  "|");
	
		for(i=5;i>=0;i--)
		{
			if(bit_test(*p,i)) sendDBGALL(USER_PORT,"1");
			else sendDBGALL(USER_PORT,"0");
		}

		// Calculate the number and show it.
		n = *p;
		sprintf(dbgbuf," (%i)",n);
		sendDBGALL(USER_PORT,dbgbuf);

		// Increment p's address
		p = p+1;
		p_p=&p;

	}

}




/*
Displays to the screen a reading of all the input registers
Registers are RPINRx (0 through 29)
Registers start are 0680 and go to 06BA
Each is an int16.
Of each 8 bit portion in the int16, the lower 6 bits are all that count.

*/
void ShowInputRegisters()
{
	int reg;
	int i;

	int *p;
	unsigned char *p_p;
	int8 n;

	p=getenv("SFR:RPINR0");// Starting Register
	p_p=&p;

	sendDBGALL(USER_PORT,"\r\n\nInput Register Listing");
	sendDBGALL(USER_PORT,  "\r\n----------------------------------------");
//	sendDBGALL(USER_PORT,  "\r\n");
//	HexPrint(USER_PORT,&p,2);

for(reg=0;reg<=29;reg++)
	{
		sprintf(dbgbuf,"\r\nRPINR%d(%2x%2x):\t",reg,*(p_p+1),p);
		sendDBGALL(USER_PORT,dbgbuf);
		
		// Get the first 2 bits
		if(bit_test(*p,15)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
	
		if(bit_test(*p,14)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
	
		sendDBGALL(USER_PORT,  "|");
	
		// Get the next 6
		for(i=13;i>=8;i--)
		{
			if(bit_test(*p,i)) sendDBGALL(USER_PORT,"1");
			else sendDBGALL(USER_PORT,"0");
		}

		// Calculate the number and show it.
		n = make8(*p,1);
		sprintf(dbgbuf," (%i)",n);
		sendDBGALL(USER_PORT,dbgbuf);
	
		sendDBGALL(USER_PORT,  "\t");
	
		// Get the first 2 bits
		if(bit_test(*p,7)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
	
		if(bit_test(*p,6)) sendDBGALL(USER_PORT,"1");
		else sendDBGALL(USER_PORT,"0");
	
		sendDBGALL(USER_PORT,  "|");
		
		//Get the next 6
		for(i=5;i>=0;i--)
		{
			if(bit_test(*p,i)) sendDBGALL(USER_PORT,"1");
			else sendDBGALL(USER_PORT,"0");
		}

		// Calculate the number and show it.
		n = *p;
		sprintf(dbgbuf," (%i)",n);
		sendDBGALL(USER_PORT,dbgbuf);

		// Increment p's address
		p = p+1;
		p_p=&p;

	}
}
