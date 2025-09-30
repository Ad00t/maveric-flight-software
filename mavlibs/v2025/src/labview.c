// labview.c
// Contains helper routines for doing HITL testing with the 
// LabView simulation.

// Author: Nathaniel White

#include "labview.h"

void get8bytes(int port, unsigned char *dest);


void dotest() {
	float check[217];
	unsigned char breakchar;
//	float64 superfloat;
	int i=0;
	int32 num;
	int32 sync;
	unsigned char * p_sync;
	sendDBGALL(USER_PORT,"HITLLOOP\r\n");
	resetPort(0);
	resetPort(1);
	// Sync by finding 0x00000003
	sync=0x000000D9;
	p_sync = &num;
	while(1) {
		//while(!getByte(HITLPORT,&breakchar))
			restart_wdt();
		// check if we're done on userport and hitlport
		//if(breakchar == 0xff) break;
		breakchar = 0;
		getByte(USER_PORT,&breakchar);
		if(breakchar == '~') break;
		
		// Move all bytes up 1	
		memcpy(&p_sync[3],&p_sync[2],1);
		memcpy(&p_sync[2],&p_sync[1],1);
		memcpy(&p_sync[1],&p_sync[0],1);
		if(getByte(HITLPORT,&p_sync[0])) 
		{	

		}
		if(num==sync)
		{
			clrscrn(USER_PORT);
			for(i=0;i<num;i++) 
			{
				//if(num!=3) break;
				// For the first 20, display them to screen
				if(i<=20){
					sprintf(dbgbuf,"%d of %Ld. ",i+1,num);
					locateString(USER_PORT,i+1,10,dbgbuf);
					get4bytes(HITLPORT,&check[i]);
					printFloat(dbgbuf,check[i],6);
					locateString(USER_PORT,i+1,30,dbgbuf);
				}
				else
					get4bytes(HITLPORT,&check[i]); // just read it all into the same spot
			}//for
		}//if

	}
}

void dotest2() {
//	float check[25];
	unsigned char breakchar;
	unsigned long timeout;
	int counter=0;
//	int i=0;
//	int32 num;
	sendDBGALL(USER_PORT,"\r\nMike's HITLLOOP");
	sendDBGALL(USER_PORT,"\r\nChecks Port HITLPORT for Chars from Labview");
	sendDBGALL(USER_PORT,"\r\nAny key on USER_PORT will cancel test.");
	resetPort(0);
	resetPort(1);
	timeout=SEC_TIMER+3;
	while(1) {
		if(getByte(HITLPORT,&breakchar)){
			HexPrint(USER_PORT,&breakchar,1);
			counter++;
		}
		else
			restart_wdt();
		if(SEC_TIMER>timeout || counter==28)
		{
			sprintf(dbgbuf,"\r\n%d chars rcvd.",counter);
			sendMSGALL(USER_PORT,dbgbuf);
			timeout=SEC_TIMER+1;
			sendDBGALL(USER_PORT,"\r\n--------------------------------\r\n");
			counter=0;
		}
		if(getByte(USER_PORT,&breakchar))break; // break on any key
	}
}


void loadLabViewFloats(int32 sync, float * check) {
	int i=0;
	static int32 num;
	unsigned char * p_sync;
	unsigned char c;
	p_sync = &num;
				
	if(getByte(HITLPORT,&c)) 
	{	
		//sendByte(USER_PORT,'>');
		// Check for the sync (which is also the number of floats)
		memmove(&p_sync[1],&p_sync[0],3);  // Move all bytes up 1
		p_sync[0]=c;						// Assign newest byte
		if(num==sync)
		{
			// It matches, now assign all the floats.
			for(i=0;i<num;i++) 
			{
				get4bytes(HITLPORT,&check[i]); 
//				sendDBGALL(USER_PORT,"\r\nGot ");
//				printFloat(dbgbuf,check[i],6);
//				sendDBGALL(USER_PORT,dbgbuf);				
			}//for
			NewLabData=1;
//			sendByte(USER_PORT,'!');
		}//if
	}
}

void sendLabViewFloats(int32 num,float * data)
{
	int32 i;
	long sync = 0x12345678;

	// First, send the sync
	send4bytes(HITLPORT,&sync);

	// Then, send the total size
	send4bytes(HITLPORT,&num);
	
	// Then send all the elements.
	for(i=0;i<num;i++)
	{
		send4bytes(HITLPORT,&data[i]);	// send the floats out over the wire.
		//sendByte(USER_PORT,'>');
	}
}

void get4bytes(int port, unsigned char *dest) {
	unsigned char bytec = 4;
	unsigned char inchar;
	dest += 3;
	while(bytec>0) {
		while(!getByte(port,&inchar)){};
			*dest = inchar;
			dest--;
			bytec--;
		}
}

void send4bytes(int port, unsigned char *src) {
	unsigned char bytec = 4;
	unsigned char outchar;
	src += 3;
	while(bytec>0) {
		outchar = *src;
		sendByte(HITLPORT,outchar);
		src--;
		bytec--;
	}
}

void get8bytes(int port, unsigned char *dest) {
	unsigned char bytec = 8;
	unsigned char inchar;
	dest += 7;
	while(bytec>0) {
		while(!getByte(port,&inchar));
			*dest = inchar;
			dest--;
			bytec--;
		}
}	



