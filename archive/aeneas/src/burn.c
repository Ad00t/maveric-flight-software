/***************************************************************************

									burn.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide burn wire functions

Changelog:

Date 	 |  Au.  |	Notes
11-09-10	bez		Created.

****************************************************************************
*/

#include "burn.h"

#module

//==============================================
//  				Functions
//==============================================
void BurnBabyBurn(int driver)
{
	int i;		// Counter for timer

	switch(driver)
	{
		case 1:
		{
			sendDBGALL(USER_PORT,"\r\nBurning Payload Wire (A)");
			output_low(BurnA);  // Wire burning for side A
			break;
		}
		case 2:
		{
			sendDBGALL(USER_PORT,"\r\nBurning Solar Panels Wire (B)");
			output_low(BurnB);  // Wire burning for side B
			break;
		}
		default:
		{
			return;
		}
	}

	// Wait for 10 seconds
	for(i=0;i<BURN_SECONDS;i++)
	{
		restart_wdt();
		delay_ms(1000);
		sendDBGALL(USER_PORT,".");
	}
	sendDBGALL(USER_PORT,"BURNT!");
				
	// Turn both burn wires off.
	output_high(BurnA);  // Wire burning off (low)
	output_high(BurnB);  // Wire burning off (low)
	
	return;
}
