/***************************************************************************

									eps.c

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Provide hardware interface to ClydeSpace EPS.

Changelog:

Date 	 |  Au.  |	Notes
08-30-10   Will 	Initial revision.		

****************************************************************************
*/

#include "eps.h"		// Public header

#module					// !!! Important: This command makes everything below scoped only to this file.

//========================================
//  		Defines
//========================================

#define EPS_DELAY			2				// Delay between ADC readings (in milliseconds)

#define ADC					0				// Read ADC Channel (0-38)
#define STATUS				1				// Request Status Bytes
#define PDM_Off				2				// Turns off the selected PDM for a short time (0-7)
#define VERSION				4				// Request Firmware Version
#define WATCHDOG			128				// Causes a soft reset of the micro


//========================================
//  		Driver Functions
//========================================

signed int bytes2intTEN(unsigned char * bytes);
signed int bytes2intSIXTEEN(unsigned char * bytes);
int epsChannelWrite(unsigned char addr, unsigned char channel, eps_s * Data, int value);
int epsChannelRead(unsigned char addr, unsigned char channel, eps_s * Data);
ReturnErr_t clearEPSData(eps_s * D);

//========================================
//  		Local Globals
//========================================

// None

//========================================
//  		Driver Functions
//========================================

/*=================================================================
  PURPOSE: Initialize EPS Interface.
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/

void eps_init()
{
	output_high(I2C_ON);

	return;
}






/*=================================================================
  PURPOSE: Convert two bytes into a 10-bit signed int.
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/

signed int bytes2intTEN(unsigned char * bytes)
{
	unsigned char mask = 0x03;		// Mask out the last two bits of the MSB.
	signed int a;

	// The Clyde Space sends the MSB first and the LSB second.
	a = (int) bytes[1]  +( ((int)bytes[0]&mask) <<8);
	return a;
}

/*=================================================================
  PURPOSE: Convert two bytes into a 16-bit signed int.
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/

signed int bytes2intSIXTEEN(unsigned char * bytes)
{
	//sprintf(dbgbuf,"\r\nByte 0: %u Byte 1: %u",bytes[0],bytes[1]);
	//sendDBGALL(USER_PORT,dbgbuf);

	// The Clyde Space sends the MSB first and the LSB second.
	return (int)bytes[1] + ( ((int)bytes[0])<<8);
}

/*=================================================================
  PURPOSE: Retrieve and output EPS revision number.
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/

int getEPSVersionNumber(unsigned char addr)
{
	unsigned char version_bytes[2];

	// Request
	sendEPSCommand(addr,VERSION,0);

	// Wait (1.2ms minimum)
	delay_ms(EPS_DELAY);

	// Read
	if(readEPSResponse(addr,version_bytes) != SUCCESS)
	{
		sendDBGALL(USER_PORT,"\r\nFAILURE - Cannot get EPS Version Number");
		return -1;
	}
	else
	{
		// Convert to a 16-bit integer
		return bytes2intSIXTEEN(version_bytes);
	}
}

/*=================================================================
  PURPOSE: Retrieve and output EPS status.
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/

int getEPSStatus(unsigned char addr)
{
	unsigned char status[2];

	// Request
	sendEPSCommand(addr, STATUS, 0);

	// Wait (1.2ms minimum)
	delay_ms(EPS_DELAY);

	// Read
	if(readEPSResponse(addr,status) != SUCCESS)
	{
		sendDBGALL(USER_PORT,"\r\nFAILURE - Cannot get EPS status");
		return -1;
	}
	else
	{
		// Print it out!
		sprintf(dbgbuf,"\r\nEPS Status Bitfields:");
		sendDBGALL(USER_PORT,dbgbuf);
		byteMe(status[0]);
		byteMe(status[1]);

		return bytes2intSIXTEEN(status);
	}
}

/*=================================================================
  PURPOSE: Retrieve and output EPS status.
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/

int getEPSHeaterStatus(unsigned char addr)
{
	unsigned char status[2];

	// Verify inputs
	if(addr != BAT_I2C_ADDR && addr != RBB_I2C_ADDR)
	{
		sendDBGALL(USER_PORT,"\r\nFAILURE - Attempting to get heater status for board w/o heater.");
		return -1;
	}

	// Request
	sendEPSCommand(addr,4,0);

	// Wait (1.2ms minimum)
	delay_ms(EPS_DELAY);

	// Read
	if(readEPSResponse(addr,status) != SUCCESS)
	{
		sendDBGALL(USER_PORT,"\r\nFAILURE - Cannot get EPS Version Number");
	}
	else
	{
		// Print it out!
		sprintf(dbgbuf,"\r\nEPS Heater Status Bitfields:");
		sendDBGALL(USER_PORT,dbgbuf);
		byteMe(status[0]);
		byteMe(status[1]);
	}
	
	return bytes2intSIXTEEN(status);
}




/*=================================================================
  PURPOSE: Request a channel to read
  UNMODIFIED VARIABLES: channel - The ADC channel to read.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Convience frontend for sendEPSCommand()
-----------------------------------------------------------
*/

void requestEPSChannel(unsigned char addr, unsigned char channel)
{
	// Verify inputs (there are only 39 channels)
	if(channel>38) return;

	sendEPSCommand(addr,0,channel);

	return;
}

/*=================================================================
  PURPOSE: Read off the response
  UNMODIFIED VARIABLES: channel - The ADC channel to read.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Convience frontend for sendEPSCommand()
-----------------------------------------------------------
*/

ReturnErr_t readEPSResponse(unsigned char addr, unsigned char * response)
{
	// Set the Read Bit
	unsigned char new_addr = (addr<<1)+1;

	//sendDBGALL(USER_PORT, "\r\nRequested response from address: ");
	//HexPrint(USER_PORT,&new_addr,1);

	// Get response
	i2c_start(I2C_STREAM,2);
	i2c_write(new_addr);
	response[0] = i2c_read();
	response[1] = i2c_read(0);
	i2c_stop();

	// Check response
	if(response[0] == 0xF0 && response[1] == 0x00)
	{
		sendDBGALL(USER_PORT,"\r\nFAILURE - No data found when querying EPS.  Try extending delay time.");
		return FAILURE;
	}
	else return SUCCESS;
}

/*=================================================================
  PURPOSE: Collect data from a single EPS channel.  Block as necessary.
  UNMODIFIED VARIABLES: channel - The ADC channel to read.
	addr - the EPS device to read from.
  MODIFIED VARIABLES: value - a pointer to the location to place the reading.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t blockForEPSChannel(unsigned char addr,unsigned char channel, int * value)
{
	unsigned char bytes[2];

	// Request
	requestEPSChannel(addr,channel);
	
	// Wait
	delay_ms(EPS_DELAY);

	// Read
	if(readEPSResponse(addr,bytes) != SUCCESS)
	{
		sendDBGALL(USER_PORT,"\r\nFAILURE - Cannot get EPS channel.");
		
		return FAILURE;
	}
	else
	{
		// Convert to a 16-bit integer
		*value = bytes2intTEN(bytes);

		return SUCCESS;
	}
}


/*=================================================================
  PURPOSE: Collect all data from EPS, waiting as necessary.
  UNMODIFIED VARIABLES: None.
  MODIFIED VARIABLES: Data - the EPS struct to be filled.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t blockForEPSTelemetry(eps_s * Data, unsigned int max_time_seconds)
{
	short reset = 1;
	ReturnErr_t response;
	unsigned long timeout;
	timeout = SEC_TIMER + max_time_seconds;
	
	do
	{
		response = getAllEPSDevices(Data, reset);
		reset = 0;
	}while(response == BUSY && timeout>SEC_TIMER);

	return response;
}


/*=================================================================
  PURPOSE: Collect all data from all EPS devices.
  UNMODIFIED VARIABLES: reset - signal that we should start from the begining
  MODIFIED VARIABLES: Data - strucutre holding all EPS data.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: This function is designed to be called continuously.
-----------------------------------------------------------
*/

ReturnErr_t getAllEPSDevices(eps_s * Data, short reset)
{
	static int state;
	const unsigned char addr[3] = {EPS_I2C_ADDR,BAT_I2C_ADDR,RBB_I2C_ADDR};
	const unsigned int num_channels[3] = {EPS_NUM_CHANNELS,BAT_NUM_CHANNELS,RBB_NUM_CHANNELS};
	ReturnErr_t response;

	// If we reset, start on first device.
	if(reset) state = 0;

	// Check if we are already finished
	if(state==3) return SUCCESS;

	//sprintf(dbgbuf,"\r\nRunning getEPSDeviceTelemetry() with dev addr = %x & state = %u",addr[state],state);
	//sendDBGALL(USER_PORT,dbgbuf);

	// Request or Get new telemetry (and pass along reset if applicable)
	response = getEPSDeviceTelemetry(addr[state],num_channels[state],Data,reset);
	
	// If we have finished a device
	if(response == SUCCESS)
	{
		// Move to next device
		state++;

		//sendDBGALL(USER_PORT,"\r\n===== NEW DEVICE =====");

		// If we are past the third device, we are done!
		if(state==3) return SUCCESS;

		// Otherwise, we reset for the new device.
		else return getEPSDeviceTelemetry(addr[state],num_channels[state],Data,1);
	}

	// Otherwise, return any non-SUCCESS responses.
	else return response;
}


/*=================================================================
  PURPOSE: Clear an EPS structure (set all zeros)
  UNMODIFIED VARIABLES: 
  MODIFIED VARIABLES: eps_s Data
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/
ReturnErr_t clearEPSData(eps_s * D)
{
	const unsigned char addr[3] = {EPS_I2C_ADDR,BAT_I2C_ADDR,RBB_I2C_ADDR};
	const unsigned int num_channels[3] = {EPS_NUM_CHANNELS,BAT_NUM_CHANNELS,RBB_NUM_CHANNELS};
	int i,j;

	// For each addr
	for(i=0;i<3;i++)
	{
		// And for each channel
		for(j=0;j<num_channels[i];j++)
		{
			//sprintf(dbgbuf,"\r\nClearing Address %02x, Channel %d",addr[i],j);
			epsChannelWrite(addr[i],j, D, 0);	// Set to zero.
		}
	}

	return SUCCESS;
}



/*=================================================================
  PURPOSE: Send a command to EPS
  UNMODIFIED VARIABLES: valuel - Command Value
	command - Command Type
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Untested. 
-----------------------------------------------------------
*/

void sendEPSCommand(unsigned char addr, unsigned char command, unsigned char value)
{
//	sprintf(dbgbuf,"\r\nAddress before anything: %x",addr);
//	sendDBGALL(USER_PORT,dbgbuf);

	addr = addr<<1;

//	sprintf(dbgbuf,"\r\nAddress after bitshift: %x",addr);
//	sendDBGALL(USER_PORT,dbgbuf);

	i2c_start();
	i2c_write(addr);
	i2c_write(command);
	i2c_write(value);

	return;
}


/*=================================================================
  PURPOSE: Collect all data from a single EPS device.
  UNMODIFIED VARIABLES: reset - signal that we should start from the begining
  MODIFIED VARIABLES: Data - strucutre holding all EPS data.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: This function is designed to be called continuously.
-----------------------------------------------------------
*/

ReturnErr_t getEPSDeviceTelemetry(unsigned char addr, unsigned char num_channels, eps_s * Data, short reset)
{
	static int state = 0;					// State: 0 - Requesting, 1 - Reading, 2 - All Done.
	static unsigned char channel = 0;		// Channel of ADC currently being queried.
	static int64 timeout;							// Time at which we can start reading from ADC sensor.
	unsigned char response[2];				// Two byte response from EPS board.

	if(reset)
	{
		state = 0;
		channel = 0;
	}	

	switch(state)
	{
		case 0:
		{
			//sprintf(dbgbuf,"\r\n Channel %u, Address %u",channel,addr);
			//sendDBGALL(USER_PORT,dbgbuf);

			// Zero-out the struct value.
			epsChannelWrite(addr,channel,Data,0);

			//sprintf(dbgbuf,"\r\n Channel %u, Address %u",channel,addr);
			//sendDBGALL(USER_PORT,dbgbuf);

			// Request
			requestEPSChannel(addr,channel);

			//sprintf(dbgbuf,"\r\n Channel %u, Address %u",channel,addr);
			//sendDBGALL(USER_PORT,dbgbuf);
	
			// Flip the flag so we look for a channel next time around
			state = 1;
	
			// Set delay timer
			timeout = MSEC + EPS_DELAY;
	
			return BUSY;
		}	
		case 1:
		{
			// Check delay timer
			if(timeout<MSEC)
			{	
				// Read response
				if(readEPSResponse(addr,response) != SUCCESS)
				{
					// Move back to begining, and repeat channel
					state = 0;

					// Let calling function know we had a problem
					return FAILURE;
				}

				// Decode response
				epsChannelWrite(addr,channel,Data,bytes2intTEN(response));
	
				// Move to next channel in ADC
				channel++;

				//sendDBGALL(USER_PORT,"\r\n**** NEW CHANNEL ****");

				// Check if we are done.  If so, indicate we are done!
				if(channel == num_channels) return SUCCESS;

				// Otherwise, move to the next channel.
				else state = 0;
			}
			else
			{
				//sprintf(dbgbuf,"\r\nAwaiting timer. Timeout: %Lu, MSEC: %Lu, Difference: %d (Should be positive)",timeout,MSEC,timeout-MSEC);
				//sendDBGALL(USER_PORT,dbgbuf);
				//sendDBGALL(USER_PORT,"*");
			}

			return BUSY;
		}
		default:
		{
			return UNREACHABLE;
		}

	}//switch
}

int epsChannelRead(unsigned char addr, unsigned char channel, eps_s * Data)
{
	switch(addr)
	{
		case EPS_I2C_ADDR:
		{
			switch(channel)
			{
				case 0: return Data->EPS_Voltage_PLUS[0]; 
				case 1: return Data->EPS_Current_PLUS[0];
				case 2: return Data->EPS_Temperature_PLUS[0];
				case 3: return Data->EPS_Voltage_MINUS[0];
				case 4: return Data->EPS_Current_MINUS[0];
				case 5: return Data->EPS_Temperature_MINUS[0];
		
				case 6: return Data->EPS_Voltage_PLUS[1];
				case 7: return Data->EPS_Current_PLUS[1];
				case 8: return Data->EPS_Temperature_PLUS[1];
				case 9: return Data->EPS_Voltage_MINUS[1];
				case 10: return Data->EPS_Current_MINUS[1];
				case 11: return Data->EPS_Temperature_MINUS[1];
		
				case 12: return Data->EPS_Voltage_PLUS[2];
				case 13: return Data->EPS_Current_PLUS[2];
				case 14: return Data->EPS_Temperature_PLUS[2];
				case 15: return Data->EPS_Voltage_MINUS[2];
				case 16: return Data->EPS_Current_MINUS[2];
				case 17: return Data->EPS_Temperature_MINUS[2];
		
				case 18: return Data->EPS_Voltage_PLUS[3];
				case 19: return Data->EPS_Current_PLUS[3];
				case 20: return Data->EPS_Temperature_PLUS[3];
				case 21: return Data->EPS_Voltage_MINUS[3];
				case 22: return Data->EPS_Current_MINUS[3];
				case 23: return Data->EPS_Temperature_MINUS[3];
		
				case 24: return Data->EPS_Voltage_PLUS[4];
				case 25: return Data->EPS_Current_PLUS[4];
				case 26: return Data->EPS_Temperature_PLUS[4];
				case 27: return Data->EPS_Voltage_MINUS[4];
				case 28: return Data->EPS_Current_MINUS[4];
				case 29: return Data->EPS_Temperature_MINUS[4];
		
				case 30: return Data->EPS_ThreePointThreeCurrentSense;
				case 31: return Data->EPS_FiveCurrentSense;
				case 32: return Data->EPS_BatteryBusCurrentSense;
		
				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); return 0;
			}//switch
			break;
		}
		case BAT_I2C_ADDR:
		{
			switch(channel)
			{
				case 0: return Data->BAT_BatteryCurrentDirection[0];
				case 1: return Data->BAT_BatteryCurrent[0];
				case 2: return Data->BAT_CellOneVoltage[0];
				case 3: return Data->BAT_BatteryVoltage[0];
				case 4: return Data->BAT_BatteryTemp[0];

				case 5: return Data->BAT_BatteryCurrentDirection[1];
				case 6: return Data->BAT_BatteryCurrent[1];
				case 7: return Data->BAT_CellOneVoltage[1];
				case 8: return Data->BAT_BatteryVoltage[1];
				case 9: return Data->BAT_BatteryTemp[1];

				case 10: return Data->BAT_BatteryCurrentDirection[2];
				case 11: return Data->BAT_BatteryCurrent[2];
				case 12: return Data->BAT_CellOneVoltage[2];
				case 13: return Data->BAT_BatteryVoltage[2];
				case 14: return Data->BAT_BatteryTemp[2];

				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); return 0;
			}//switch
			break;
		}
		case RBB_I2C_ADDR:
		{
			switch(channel)
			{
				case 0: return Data->RBB_BatteryVoltage;
				case 1: return Data->RBB_CellVoltage;
				case 2: return Data->RBB_Current;
				case 3: return Data->RBB_Temperature;

				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); return 0;
			}//switch
			break;
		}
		default: sendDBGALL(USER_PORT,"\r\nFAILURE - Attempt to decode for incorrect device."); return 0;
	}//switch
}

int epsChannelWrite(unsigned char addr, unsigned char channel, eps_s * Data, int value)
{
	switch(addr)
	{
		case EPS_I2C_ADDR:
		{
			switch(channel)
			{
				case 0: Data->EPS_Voltage_PLUS[0] = value; break; break;
				case 1: Data->EPS_Current_PLUS[0] = value; break; break;
				case 2: Data->EPS_Temperature_PLUS[0] = value; break;
				case 3: Data->EPS_Voltage_MINUS[0] = value; break;
				case 4: Data->EPS_Current_MINUS[0] = value; break;
				case 5: Data->EPS_Temperature_MINUS[0] = value; break;
		
				case 6: Data->EPS_Voltage_PLUS[1] = value; break;
				case 7: Data->EPS_Current_PLUS[1] = value; break;
				case 8: Data->EPS_Temperature_PLUS[1] = value; break;
				case 9: Data->EPS_Voltage_MINUS[1] = value; break;
				case 10: Data->EPS_Current_MINUS[1] = value; break;
				case 11: Data->EPS_Temperature_MINUS[1] = value; break;
		
				case 12: Data->EPS_Voltage_PLUS[2] = value; break;
				case 13: Data->EPS_Current_PLUS[2] = value; break;
				case 14: Data->EPS_Temperature_PLUS[2] = value; break;
				case 15: Data->EPS_Voltage_MINUS[2] = value; break;
				case 16: Data->EPS_Current_MINUS[2] = value; break;
				case 17: Data->EPS_Temperature_MINUS[2] = value; break;
		
				case 18: Data->EPS_Voltage_PLUS[3] = value; break;
				case 19: Data->EPS_Current_PLUS[3] = value; break;
				case 20: Data->EPS_Temperature_PLUS[3] = value; break;
				case 21: Data->EPS_Voltage_MINUS[3] = value; break;
				case 22: Data->EPS_Current_MINUS[3] = value; break;
				case 23: Data->EPS_Temperature_MINUS[3] = value; break;
		
				case 24: Data->EPS_Voltage_PLUS[4] = value; break;
				case 25: Data->EPS_Current_PLUS[4] = value; break;
				case 26: Data->EPS_Temperature_PLUS[4] = value; break;
				case 27: Data->EPS_Voltage_MINUS[4] = value; break;
				case 28: Data->EPS_Current_MINUS[4] = value; break;
				case 29: Data->EPS_Temperature_MINUS[4] = value; break;
		
				case 30: Data->EPS_ThreePointThreeCurrentSense = value; break;
				case 31: Data->EPS_FiveCurrentSense = value; break;
				case 32: Data->EPS_BatteryBusCurrentSense = value; break;
		
				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); break;
			}//switch
			break;
		}
		case BAT_I2C_ADDR:
		{
			switch(channel)
			{
				case 0: Data->BAT_BatteryCurrentDirection[0] = value; break;
				case 1: Data->BAT_BatteryCurrent[0] = value; break;
				case 2: Data->BAT_CellOneVoltage[0] = value; break;
				case 3: Data->BAT_BatteryVoltage[0] = value; break;
				case 4: Data->BAT_BatteryTemp[0] = value; break;

				case 5: Data->BAT_BatteryCurrentDirection[1] = value; break;
				case 6: Data->BAT_BatteryCurrent[1] = value; break;
				case 7: Data->BAT_CellOneVoltage[1] = value; break;
				case 8: Data->BAT_BatteryVoltage[1] = value; break;
				case 9: Data->BAT_BatteryTemp[1] = value; break;

				case 10: Data->BAT_BatteryCurrentDirection[2] = value; break;
				case 11: Data->BAT_BatteryCurrent[2] = value; break;
				case 12: Data->BAT_CellOneVoltage[2] = value; break;
				case 13: Data->BAT_BatteryVoltage[2] = value; break;
				case 14: Data->BAT_BatteryTemp[2] = value; break;

				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); break;
			}//switch
			break;
		}
		case RBB_I2C_ADDR:
		{
			switch(channel)
			{
				case 0: Data->RBB_BatteryVoltage = value; break;
				case 1: Data->RBB_CellVoltage = value; break;
				case 2: Data->RBB_Current = value; break;
				case 3: Data->RBB_Temperature = value; break;

				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); break;
			}//switch
			break;
		}
		default: sendDBGALL(USER_PORT,"\r\nFAILURE - Attempt to decode for incorrect device."); break;
	}//switch

	return 0;
}


/*=================================================================
  PURPOSE: 
  UNMODIFIED VARIABLES: 
  MODIFIED VARIABLES: 
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

void epsFlicker(short ThreePointThree, short Five, short bus)
{
	unsigned char data = 0;

	// Set the bits according to what user wants to flicker
	if(bus) 	bit_set(data,2);
	if(Five) 		bit_set(data,2);
	if(ThreePointThree) 	bit_set(data,2);

	// If user selected anything, send a command!
	if(data != 0) sendEPSCommand(EPS_I2C_ADDR,2,data);
	
	return;
}
	

/*=================================================================
  PURPOSE: Convert the structure into a unsigned char array to deliver to ground.
  UNMODIFIED VARIABLES: Data - the EPS struct containing the data to package
  MODIFIED VARIABLES: packetd_telem - unsigned char array with all data stored as int16.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

void epsTriggerWatchdog(unsigned char addr)
{
	sendEPSCommand(addr,128,0);
	return;
}

/*=================================================================
  PURPOSE: Convert the structure into a unsigned char array to deliver to ground.
  UNMODIFIED VARIABLES: Data - the EPS struct containing the data to package
  MODIFIED VARIABLES: packetd_telem - unsigned char array with all data stored as int16.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

void epsTurnHeaterOn(unsigned char addr)
{
	// Verify inputs
	if(addr != BAT_I2C_ADDR && addr != RBB_I2C_ADDR)
	{
		sendDBGALL(USER_PORT,"\r\nFAILURE - Attempting to turn on heater for board w/o heater.");
		return;
	}

	sendEPSCommand(addr,5,0);

	return;
}

/*=================================================================
  PURPOSE: Convert the structure into a unsigned char array to deliver to ground.
  UNMODIFIED VARIABLES: Data - the EPS struct containing the data to package
  MODIFIED VARIABLES: packetd_telem - unsigned char array with all data stored as int16.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/
void epsTurnHeaterOff(unsigned char addr)
{
	// Verify inputs
	if(addr != BAT_I2C_ADDR && addr != RBB_I2C_ADDR)
	{
		sendDBGALL(USER_PORT,"\r\nFAILURE - Attempting to turn off heater for board w/o heater.");
		return;
	}

	sendEPSCommand(addr,5,1);

	return;
}


ReturnErr_t eps_functional_check(eps_s * Data, unsigned int max_time_seconds)
{
	ReturnErr_t response;

	response = blockForEPSTelemetry(Data, max_time_seconds);	// <--- Fixed a stupid error with taking the address of a pointer!

	return response;
}



void eps_show_batteries(eps_s * Data)
{
	int value;
	float fvalue;
	#define BATTERY_ONE_CHANNEL		3
	#define BATTERY_TWO_CHANNEL		8
	#define BATTERY_THREE_CHANNEL	13
	#define RBB_CHANNEL				0

	sendDBGALL(USER_PORT,"\r\n Batt1: ");

	// Battery 1
	value = epsChannelRead(BAT_I2C_ADDR,BATTERY_ONE_CHANNEL,Data);
	fvalue = convertEPSChannel(BAT_I2C_ADDR,BATTERY_ONE_CHANNEL,value);
	printFloat(dbgbuf,fvalue,2);
	sendDBGALL(USER_PORT,dbgbuf);
	sendDBGALL(USER_PORT,"  Batt2: ");

	// Battery 2
	value = epsChannelRead(BAT_I2C_ADDR,BATTERY_TWO_CHANNEL,Data);
	fvalue = convertEPSChannel(BAT_I2C_ADDR,BATTERY_TWO_CHANNEL,value);
	printFloat(dbgbuf,fvalue,2);
	sendDBGALL(USER_PORT,dbgbuf);
	sendDBGALL(USER_PORT,"  Batt3: ");
 	
	// Battery 3
	value = epsChannelRead(BAT_I2C_ADDR,BATTERY_THREE_CHANNEL,Data);
	fvalue = convertEPSChannel(BAT_I2C_ADDR,BATTERY_THREE_CHANNEL,value);
	printFloat(dbgbuf,fvalue,2);
	sendDBGALL(USER_PORT,dbgbuf);
	sendDBGALL(USER_PORT,"  Rbb: ");

	// RBB 
	value = epsChannelRead(RBB_I2C_ADDR,RBB_CHANNEL	,Data);
	fvalue = convertEPSChannel(RBB_I2C_ADDR,RBB_CHANNEL	,value);
	printFloat(dbgbuf,fvalue,2);
	sendDBGALL(USER_PORT,dbgbuf);

	return;
}

//========================================
// 		Development Functions
//========================================


// Note: The below functions are only necessary for testing and development.  They
// can be removed during actual production

#ifdef DEBUG

/*=================================================================
  PURPOSE: Print out the EPS telemetry struct in human readable format.
  UNMODIFIED VARIABLES: Data - the EPS data struct to print
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: None.
-----------------------------------------------------------
*/

void printEPSTelemetry(eps_s * Data, unsigned char counts)
{
	int i=0, j=0;
	int value;
	float fvalue;
	unsigned char channel = 0;

 	disable_interrupts(INTR_GLOBAL);	// avoid sprintf with floats bug

	sendDBGALL(USER_PORT,"\r\n-------------------------- ClydeSpace EPS Telemetry ---------------------------");
	
						   //---------1---------2---------3---------4---------5---------6---------7---------8
	sendDBGALL(USER_PORT,"\r\n| BCR   | Volts + | Curr. + | Temp. + | Volts - | Curr. - | Temp. - |");
	sendDBGALL(USER_PORT,"\r\n-------------------------------------------------------------------------------");

	// Five BCRs
	for(i=0;i<5;i++)
	{
		switch(i)
		{
			case 0: sendDBGALL(USER_PORT,"\r\n| Xd    |"); break;
			case 1: sendDBGALL(USER_PORT,"\r\n| Yd    |"); break;
			case 2: sendDBGALL(USER_PORT,"\r\n| Xf    |"); break;
			case 3: sendDBGALL(USER_PORT,"\r\n| Yf    |"); break;
			case 4: sendDBGALL(USER_PORT,"\r\n| USB   |"); break;
			default: break;
		}

		// Six measurements per panel
		for(j=0;j<6;j++)
		{
			value = epsChannelRead(EPS_I2C_ADDR,channel,Data);
			fvalue = convertEPSChannel(EPS_I2C_ADDR,channel,value);
			if(counts){
				sprintf(dbgbuf," %5.2f   |",fvalue);	// Float value
			}
			else
			{
				sprintf(dbgbuf," %5u   |",value);	// Counts value
			}
			sendDBGALL(USER_PORT,dbgbuf);

			channel++;		// Next channel
		}//for
	}//for

	// Current Senses
	channel = 30;
	sendDBGALL(USER_PORT,"\r\n 3.3V Current Sense: ");
	value = epsChannelRead(EPS_I2C_ADDR,channel,Data);
	fvalue = convertEPSChannel(EPS_I2C_ADDR,channel,value);
	sprintf(dbgbuf," %5.2f   ",fvalue);	// Six Times
	sendDBGALL(USER_PORT,dbgbuf);


	sendDBGALL(USER_PORT,"\r\n-------------------------- ClydeSpace BAT Telemetry ---------------------------");

					   	   //---------1---------2---------3---------4---------5---------6---------7---------8
	sendDBGALL(USER_PORT,"\r\n| Batt | CurrDir     | Current | C1 Volt |  Volts  |  Temp   |");
	sendDBGALL(USER_PORT,"\r\n-------------------------------------------------------------------------------");

	channel = 0;

	// Three Batteries
	for(i=0;i<3;i++)
	{
		sprintf(dbgbuf,"\r\n| %1u    |",i);	// Once
		sendDBGALL(USER_PORT,dbgbuf);

		// Five measurements per panel
		for(j=0;j<5;j++)
		{
			value = epsChannelRead(BAT_I2C_ADDR,channel,Data);
			fvalue = convertEPSChannel(BAT_I2C_ADDR,channel,value);
			if(counts)
			{
				sprintf(dbgbuf," %5.2f |",fvalue);	// Floats
			}
			else
			{
				sprintf(dbgbuf," %7u |",value);		// Counts
			}

			if(channel==0 || channel==5 || channel==10)
			{
				if(fvalue>0.0) 	sprintf(dbgbuf," charging |");
				else sprintf(dbgbuf," discharging |");
			} 
			sendDBGALL(USER_PORT,dbgbuf);

			channel++;
		}//for
	}//for


	sendDBGALL(USER_PORT,"\r\n-------------------------- ClydeSpace RBB Telemetry ---------------------------");

					   	   //---------1---------2---------3---------4---------5---------6---------7---------8
	sendDBGALL(USER_PORT,"\r\n| Volts | C1 Volt |  Current  |  Temp   |");
	sendDBGALL(USER_PORT,"\r\n-------------------------------------------------------------------------------");

	channel = 0;

	// Just One Battery
	sendDBGALL(USER_PORT,"\r\n|");

	// Five measurements per panel
	for(j=0;j<4;j++)
	{
		value = epsChannelRead(RBB_I2C_ADDR,channel,Data);
		fvalue = convertEPSChannel(RBB_I2C_ADDR,channel,value);

		if(counts)
		{
			sprintf(dbgbuf," %5.2f |",fvalue);	// Floats
		}
		else
		{
			sprintf(dbgbuf," %7u |",value);		// Counts
		}
		sendDBGALL(USER_PORT,dbgbuf);

		channel++;
	}//for




 	enable_interrupts(INTR_GLOBAL);	// avoid sprintf with floats bug

	return;
}


/*=================================================================
  PURPOSE: Automatically aquire EPS data repeatedly
  UNMODIFIED VARIABLES: iterations - number of times to aquire data
		(if not continuous).
	continuous - TRUE if collect infinite data, FALSE if use iterations number.
	human - TRUE if display should be verbose, FALSE if just CSV.
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Waits 1 second between each full aquisition.
-----------------------------------------------------------
*/

void epsAutomaticDataCollection(long iterations, short continuous, short human)
{
	long i = 0;				// Iteration counter
	long timeout;			// Timeout used to pause before data collection
	short userFlag = 1;		// Indicator that user wants to quit.  (False = quit)
	unsigned char temp;				// Temporary unsigned character holder to check user keystroke
	unsigned char counts=0;	// Flag for displaying counts (1) or floats (0)
	eps_s EPSData;			// Struct holding results	

	clearEPSData(&EPSData);	// Clear out the structure

	// Let user know how to escape!
	sendDBGALL(USER_PORT,"\r\nCollecting EPS data.  Press 'q' to quit.  Starting in 1 seconds ...");

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
	
		if(blockForEPSTelemetry(&EPSData,10) != SUCCESS)
		{
			sendDBGALL(USER_PORT,"Fail.");
			return;
		}

		printEPSTelemetry(&EPSData,counts);

		// Check if a unsigned character is available
		if(getByte(USER_PORT,&temp)) 
		{
			// If it is any form of 'q', signal that we want to quit.
			if(temp == 'q' || temp == 'Q') userFlag = 0;

			// If it's a command to switch to/from counts, do that.
			if(temp == 'c' || temp == 'C') counts = !counts;
		}
				
		delay_ms(1000);
	}
	
	return;
}


float convertEPSChannel(unsigned char addr, unsigned char channel, int ADCreading)
{

	switch(addr)
	{
		case EPS_I2C_ADDR:
		{

			switch(channel)
			{
				// These conversions are taken from an email from Andrew at ClydeSpace dated 
				// November 18, 2011
				
				// VOLTAGES
				// Array 1
				case 0: 
				case 3:
				// Array 2
				case 6:
				case 9:
				// Array 3
				case 12:
				case 15:
				// Array 4
				case 18:
				case 21:
						return (-0.0340 *ADCreading) + 35.520; break;

				// CURRENTS
				// Array 1
				case 1: 
				case 4:
				// Array 2
				case 7:
				case 10:
				// Array 3
				case 13:
				case 16:
				// Array 4
				case 19:
				case 22:
						return (-0.5 *ADCreading) + 515.7; break;

				// TEMPERATURES
				// Array 1
				case 2: 
				case 5:
				// Array 2
				case 8:
				case 11:
				// Array 3
				case 14:
				case 17:
				// Array 4
				case 20:
				case 23:
						return (-0.163 *ADCreading) +110.338; break;				

				// Array 5 (smaller and different than others -- also not hooked up to anything other than USB)		
				case 24: return (-0.00832*ADCreading)+8.65507; break;		// Note this conversion is difference than the others
				case 25: return (-0.45903*ADCreading)+499.097; break;		// Note this conversion is difference than the others		
				case 26: return (-0.1619*ADCreading)+110.119; break;
				case 27: return (-0.00832*ADCreading)+8.65507; break;		// Note this conversion is difference than the others
				case 28: return (-0.45903*ADCreading)+499.097; break;		// Note this conversion is difference than the others
				case 29: return (-0.1619*ADCreading)+110.119; break;
		
				// CURRENT SENSE
				// 3.3V
				case 30: return (-4.039 * ADCreading) + 4155.271; break;		// Note this conversion is difference than the others

				case 31: return (-3.500 * ADCreading) + 3611.509; break;		// Note this conversion is difference than the others

				case 32: return (-3.153 * ADCreading) + 3250.815; break;		// Note this conversion is difference than the others
		

				// These conversions are taken from the Clyde Space DEPS user manual
				// C3-USM-5007-CS-DEPS
				// Issue: A    Date: 15th October 2008
/*				case 0: return (-0.034*ADCreading)+35.164; break;
				case 1: return (-0.49916*ADCreading)+517.174; break;
				case 2: return (-0.1619*ADCreading)+110.119; break;
				case 3: return (-0.034*ADCreading)+35.164; break;
				case 4: return (-0.49916*ADCreading)+517.174; break;
				case 5: return (-0.1619*ADCreading)+110.119; break;
		
				case 6: return (-0.034*ADCreading)+35.164; break;
				case 7: return (-0.49916*ADCreading)+517.174; break;
				case 8: return (-0.1619*ADCreading)+110.119; break;
				case 9: return (-0.034*ADCreading)+35.164; break;
				case 10: return (-0.49916*ADCreading)+517.174; break;
				case 11: return (-0.1619*ADCreading)+110.119; break;
		
				case 12: return (-0.034*ADCreading)+35.164; break;
				case 13: return (-0.49916*ADCreading)+517.174; break;
				case 14: return (-0.1619*ADCreading)+110.119; break;
				case 15: return (-0.034*ADCreading)+35.164; break;
				case 16: return (-0.49916*ADCreading)+517.174; break;
				case 17: return (-0.1619*ADCreading)+110.119; break;
		
				case 18: return (-0.034*ADCreading)+35.164; break;
				case 19: return (-0.49916*ADCreading)+517.174; break;
				case 20: return (-0.1619*ADCreading)+110.119; break;
				case 21: return (-0.034*ADCreading)+35.164; break;
				case 22: return (-0.49916*ADCreading)+517.174; break;
				case 23: return (-0.1619*ADCreading)+110.119; break;
		
				case 24: return (-0.00832*ADCreading)+8.65507; break;		// Note this conversion is difference than the others
				case 25: return (-0.45903*ADCreading)+499.097; break;		// Note this conversion is difference than the others		
				case 26: return (-0.1619*ADCreading)+110.119; break;
				case 27: return (-0.00832*ADCreading)+8.65507; break;		// Note this conversion is difference than the others
				case 28: return (-0.45903*ADCreading)+499.097; break;		// Note this conversion is difference than the others
				case 29: return (-0.1619*ADCreading)+110.119; break;
		
				case 30: return (-3.63883*ADCreading)+3734.761; break;		// Note this conversion is difference than the others
				case 31: return (-1.19146*ADCreading)+1221.044; break;		// Note this conversion is difference than the others
				case 32: return (-3.63883*ADCreading)+3734.761; break;		// Note this conversion is difference than the others
*/	
				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); break;
			}//switch
			break;
		}
	case BAT_I2C_ADDR:
		{
			// These conversions are taken from an email from Andrew at ClydeSpace dated 
			// November 18, 2011
			switch(channel)
			{
				// Charging/discharging
				case 0:
				case 5:
				case 10:
						 if(ADCreading>500) return -1.0; 
						else return 1.0; break;
				// Current
				case 1: 
				case 6:
				case 11:
						return (-3.49185 * ADCreading) + 3173.465; break;	// mA
				// Single Cell Voltage (do not use)
				case 2: 
				case 7:
				case 12:
						return (-0.00483 * ADCreading) + 4.852724; break;	// Do Not Use (V for single cell)
				
				// Voltage
				case 3: 
				case 8:
				case 13:
						return (-0.0096 * ADCreading) + 9.8276; break;	// Volts
				
				// Temperature
				case 4: 
				case 9:
				case 14:
						return (-0.163 * ADC) + 111.19; break;			// deg C


				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); break;
			}//switch
			





			// PREVIOUS CONVERSIONS
			// These conversions are taken from the Clyde Space CubeSat 3U Battery Board User Manual
			// Document ID: C3-USM-5005-CS-BAT
			// Issue: E
			// Date: 23rd July 2008
			/*switch(channel)
			{
				case 0: if(ADCreading>500) return -1.0; 
						else return 1.0; break;
				case 1: return (-3.20 * ADCreading)+2926.22; break;
				case 2: return (-0.00438 * ADCreading)+4.753; break;
				case 3: return (-0.00939 * TLMC)+9.791; break;
				case 4: return (-0.163*ADCreading)+110.7; break;

				case 5: if(ADCreading>500) return -1.0; 
						else return 1.0; break;
				case 6: return (-3.20 * ADCreading)+2926.22; break;
				case 7: return (-0.00438 * ADCreading)+4.753; break;
				case 8: return (-0.00939 * TLMC)+9.791; break;
				case 9: return (-0.163*ADCreading)+110.7; break;

				case 10: return 0; break;
				case 11: return (-3.20 * ADCreading)+2926.22; break;
				case 12: return (-0.00438 * ADCreading)+4.753; break;
				case 13: return (-0.00939 * TLMC)+9.791; break;
				case 14: return (-0.163*ADCreading)+110.7; break;

				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); break;
			}//switch
			*/


			break;
		}

		case RBB_I2C_ADDR:
		{
			// These conversions are taken from an email from Andrew at ClydeSpace dated 
			// November 18, 2011
			switch(channel)
			{
				case 0: return (0.00845*ADCreading)-0.040; break;	// Volts
				case 1: return (0.005515*ADCreading)-1.1013; break;
				case 2: return (4.449*ADCreading)-1255.449; break;	// mA
				case 3: return (0.2932*ADCreading)-60; break;		// deg C

				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); break;
			}//switch		




			// These conversions are taken from the Clyde Space Remote Battery Board User Manual
			// Document ID: CN-USM-5009-CS-RBB
			// Issue: A
			// Date: 24/11/2008
			/*		
			switch(channel)
			{
				case 0: return (0.01101*ADCreading)-2.368; break;
				case 1: return (0.005515*ADCreading)-1.1013; break;
				case 2: return (4.449*ADCreading)-1255.449; break;
				case 3: return (-0.1619*ADCreading)+110.119; break;

				default: sendDBGALL(USER_PORT,"\r\nFAILURE - Channel not found."); break;
			}//switch
			*/

			break;
		}
		default: sendDBGALL(USER_PORT,"\r\nFAILURE - Attempt to decode for incorrect device."); break;
	}

	return 0; // Default return
}


void testbytes2intTEN()
{
/*
	unsigned char bytes[2];
	signed int test;

	sendDBGALL(USER_PORT, "\r\nTesting Bytes to Int 10.  Numbers should go from 0 to 1023.");

	bytes[0]=0b11111100;
	bytes[1]=0b00000000;
	test = bytes2intTEN(bytes);
	sprintf(dbgbuf,"\r\nResult = %d",test); sendDBGALL(USER_PORT,dbgbuf);

	bytes[0]=0b00000000;
	bytes[1]=0b00000001;
	test = bytes2intTEN(bytes);
	sprintf(dbgbuf,"\r\nResult = %d",test); sendDBGALL(USER_PORT,dbgbuf);

	bytes[0]=0b00000000;
	bytes[1]=0b00000010;
	test = bytes2intTEN(bytes);
	sprintf(dbgbuf,"\r\nResult = %d",test); sendDBGALL(USER_PORT,dbgbuf);


	bytes[0]=0b00000010;
	bytes[1]=0b00000000;
	test = bytes2intTEN(bytes);
	sprintf(dbgbuf,"\r\nResult = %d",test); sendDBGALL(USER_PORT,dbgbuf);

	bytes[0]=0b00000011;
	bytes[1]=0b11111111;
	test = bytes2intTEN(bytes);
	sprintf(dbgbuf,"\r\nResult = %d",test); sendDBGALL(USER_PORT,dbgbuf);
*/
}

#endif // DEBUG
