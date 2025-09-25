// unsorted.h

// Unsorted test code.  Should be moved to appropriate libraries.



//====================================================================================================
//
//									FUNCTION TESTING CODE BELOW 
//
//====================================================================================================

void TestFlashAddrToPage(int port)
{
	unsigned int page;
	unsigned long addy;
		
	for(addy=0x700000;addy<MAX_FLASH_ADDR+0x200;addy+=256)
	{
		page = FlashAddrToPage(addy);
		sprintf(dbgbuf,"\r\n%Lx\t%ui",addy,page);
		sendMSGALL(port,dbgbuf);
	}	
}

// Test by adding various values
void TestIncrementAddrCircular(int port)
{
	unsigned long base = PLYMOUTH_ADDR+PLYMOUTH_ALLOC_SZ-16;
	unsigned long inc;
	unsigned long addy;
	unsigned long start = PLYMOUTH_ADDR;
	unsigned long end = PLYMOUTH_ADDR+PLYMOUTH_ALLOC_SZ-1;

	for(inc=0;inc<33;inc++)
	{
		addy = base;	
		IncrementAddrCircular(&addy,start,end,inc);
		sprintf(dbgbuf,"\r\n%Lx+%i=%Lx",base,inc,addy);
		sendMSGALL(port,dbgbuf);
	}
}




// Randomly Corrupt Data
// Link quality between 0 and 100
void RandomCorruptData(unsigned char * data,int link_quality)
{
	// When quality = 0, always true
	// When quality = 100, never true
	if(!(rand()<=link_quality*RAND_MAX/100))
	{
		*data = (unsigned char)rand();	// Corrupt the data
	}
}

/*
void Switcher(int s)
{
	switch(s)
	{
		case MHX: output_high(SWITCHER_PIN); return;			// Change me if switcher pins change
		case BEACON: output_low(SWITCHER_PIN); return;			// Change me if switcher pins change
	}
}
*/


// A test of the hardware handshaking for MHX.  Should be moved to test_mhx.c .
void HandshakeTest()
{
	int i;

	//ReadMHXPins(USER_PORT);
	sendDBGALL(USER_PORT,"\r\nBeginning Test.");		
	for(i=0;i<25000;i++)
	{
		//if(!input_state(MHX_CLEAR_TO_SEND) && !input_state(MHX_DATA_SEND_READY))
		if(!input_state(PIN_D15))
		//if(!input_state(MHX_CLEAR_TO_SEND))		
		{
			fputc('A',COM_A);
			//fputc('A',COM_A);
		}
		else
		{
			i--;
			//ReadMHXPins();
			sendDBGALL(USER_PORT,"\r\n--= X =-- \r\n");
		}
	}
	//ReadMHXPins();
	sendDBGALL(USER_PORT,"\r\nTest Finished.");		
	//output_high(MHX_REQUEST_TO_SEND);
	//setup_wdt(WDT_ON);	//re-enable WDT
	return;
}
