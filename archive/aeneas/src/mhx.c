/***************************************************************************

									mhx.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide mhx functions

Changelog:

Date 	 |  Au.  |	Notes
07-19-10	mra		Created.
07-27-10	mra		Added more functions from main. MHXConnected() and MHXStatus().

****************************************************************************
*/

#include <mhx.h>

#MODULE  // Limit the scope of globals declared here



ReturnErr_t mhx_mode_switch(unsigned char mode)
{
	switch(mode)
	{
		case MHX_COMMAND_MODE:
			restart_wdt();
			delay_ms(4000);	
			sendMSG(MHX_PORT,"+",1);
			delay_ms(100);
			sendMSG(MHX_PORT,"+",1);
			delay_ms(100);
			sendMSG(MHX_PORT,"+",1);
			delay_ms(100);
			restart_wdt();
			delay_ms(4000);	
			restart_wdt();
			break;

		case MHX_DATA_MODE:
			restart_wdt();	
			sendMSG(MHX_PORT,"ATA\r",4);
			delay_ms(100);
			break;

		default: break;			

	}
	return SUCCESS;

}


// Configures the MHX register 
void configureMHX(unsigned char sreg, unsigned char newval, unsigned char writeToFlash)
{
	unsigned char cmd[40];

	// Report the command
	//sprintf(cmd,"ATS%i=%i\r",sreg,newval);
	//sendMSGALL(MHX_PORT,cmd);
	//delay_ms(100);		

	// Get Into Command Mode
	mhx_mode_switch(MHX_COMMAND_MODE);

	// Rewrite the register
	sendMSG(MHX_PORT,cmd,strlen(cmd));
	restart_wdt();
	delay_ms(100);	

	// Write to MHX's Flash to permanantly store change (optional)
	if(writeToFlash)
	{
	sendMSG(MHX_PORT,"AT&W\r",5);
	delay_ms(100);	
	}

	// Back to Data Mode
	mhx_mode_switch(MHX_DATA_MODE);
	
}// ConfigureMHX



//-----------------------------------
//        MHX Functions
//-----------------------------------
/*========================== MHXcmd ========================
  PURPOSE:  Sends commands to MHX based on input command string.
  UNMODIFIED VARIABLES:
  MODIFIED VARIABLES: 
  GLOBALS:
  RETURN VALUE:
  NOTES:    Having the entire MHX library of commands may not be necessary
			if we can deliver messages to it via a macro.
			Although it would be nice to have it be available both in code
			and over the COM.

			A setup-from-scratch macro should be created, either using these
			functions or just passing messages to the comm port.
----------------------------------------------------------*/
unsigned char MHXcmd(unsigned char *cmd, int port)
{	
	unsigned char stat;

	//Declare
	//int pkt,sz;
	
	// Initializations
	//pkt = 27;
	//pkt = (int)cmd[1]*256 + (int)cmd[2];
	//sz = (int)cmd[3]*256 + (int)cmd[4];;

	switch(cmd[0])
	{
		case 1: // Turn MHX Off
			sendDBGALL(port,"MHX OFF");
			MHXOff();
			break;
		case 2: // Turn MHX On
			sendDBGALL(port,"MHX ON");
			MHXOn();
			break;
		case 3: // Enable output
			sendDBGALL(port,"MHX OUTPUT ENABLED");
			output_low(MHX_OUTPUT_ENABLE);
			break;			
		case 4: // disable output
			sendDBGALL(port,"MHX OUTPUT DISABLED");
			output_high(MHX_OUTPUT_ENABLE);
			break;
		case 5: // Read MHX status
			MHXStatus(&stat);
			HexPrint(USER_PORT,&stat,1);
			break;
		case 6: // Read MHX Pins
			ReadMHXPins(port);
			break;
		case 7: // Configures the MHX
			//Example: r 150 7 (sreg) (newval) (writetoflash)
			//Example: r 150 7 102 0 1 // Writes S102 register with 0, writes to flash
			configureMHX(cmd[1], cmd[2], cmd[3]); 
			break;
		case 8: // Reserved
			break;

		case 9: // Reserved
			break;

		case 10: // Reserved
			break;

		default:
			break;
	}

return 0;

}


int MHXConnected()
{
	// Note: AENEAS PEC Board
	if(!input_state(MHX_CARRIER_DETECT) && input_state(MHX_PWR)) return 1;
	else return 0; 

}

// Loads a unsigned char with status data for the MHX
void MHXStatus(unsigned char * status)
{
	// First set all bits to zero
	*status = 0x00;

	// Set each bit of status according to the pins
	if(input_state(MHX_PWR)) bit_set(*status,0);
	if(input_state(MHX_OUTPUT_ENABLE)) bit_set(*status,1);
	if(input_state(MHX_DATA_TERMINAL_READY)) bit_set(*status,2);
	if(input_state(MHX_DATA_SEND_READY)) bit_set(*status,3);
	if(input_state(MHX_CMDUEST_TO_SEND)) bit_set(*status,4);
	if(input_state(MHX_CLEAR_TO_SEND)) bit_set(*status,5);
	if(input_state(MHX_CARRIER_DETECT)) bit_set(*status,6);

//	unsigned char tmp[5];
//	sprintf(tmp,"0x%2x",*status); 
//	sendDBGALL(USER_PORT,tmp);

	return;
}

void MHXOff()								// Kills power to MHX
{
	//output_high(MHX_OUTPUT_ENABLE);
	output_low(MHX_PWR);					// PEC requires 
}


void MHXOn()								// Provides power to MHX
{
	//output_low(MHX_OUTPUT_ENABLE);
	output_high(MHX_PWR);					
}

void ReadMHXPins(int port)
{
	if(port==USER_PORT){
		clrscrn(port);
		locxy(port,1,1);
	}
	if(input_state(MHX_PWR)) sendDBGALL(port,"\r\nPWR HIGH"); else sendDBGALL(port,"\r\nPWR LOW");
	if(input_state(MHX_OUTPUT_ENABLE)) sendDBGALL(port,"\r\nOUTPUT ENABLE HIGH"); else sendDBGALL(port,"\r\nOUTPUT ENABLE LOW");
	if(input_state(MHX_DATA_TERMINAL_READY)) sendDBGALL(port,"\r\nDTR HIGH"); else sendDBGALL(port,"\r\nDTR LOW");
	if(input_state(MHX_DATA_SEND_READY)) sendDBGALL(port,"\r\nDSR HIGH"); else sendDBGALL(port,"\r\nDSR LOW");
	if(input_state(MHX_CMDUEST_TO_SEND)) sendDBGALL(port,"\r\nRTS HIGH"); else sendDBGALL(port,"\r\nRTS LOW");
	if(input_state(MHX_CLEAR_TO_SEND)) sendDBGALL(port,"\r\nCTS HIGH"); else sendDBGALL(port,"\r\nCTS LOW");
	if(input_state(MHX_CARRIER_DETECT)) sendDBGALL(port,"\r\nDCD HIGH"); else sendDBGALL(port,"\r\nDCD LOW");
}

ReturnErr_t mhx_functional_check(unsigned int max_time_seconds)
{
	// Declare
	unsigned char stat;
	unsigned long timeout;
	ReturnErr_t response;
	
	// Initialize
	timeout = SEC_TIMER + max_time_seconds;
	response = SUCCESS;
	
	do{
		MHXStatus(&stat);
		//HexPrint(USER_PORT,stat,1);	
		if(SEC_TIMER>timeout)
		{
			response = FAILURE;
			break;
		}
	}while(stat!=0x41);
	
	return response;

}
