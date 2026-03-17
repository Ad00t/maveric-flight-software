/***************************************************************************

									gyro.c

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Provide hardware interface to ADIS16260 gyroscope.
Changelog:

Date 	 |  Au.  |	Notes
07-13-10   Will 	Initial revision.		


****************************************************************************
*/


#include "gyro.h"		// Public header

#module					// !!! Important: This command makes everything below scoped only to this file.

//================================
//  	Private Functions
//================================

void printGyroStuctCSV(gyro_s * Data, long line_number);

//================================
//  	Private Defines
//================================

#define GYRO_TEMP_CODE 			0x0C00		// 16-bit Request code for Gyro Temperatuers
#define GYRO_RATE_CODE 			0x0400		// 16-bit Request code for Gyro Rates
#define GYRO_ERROR_CODE			0x3C00		// 16-bit Alarm Bitfield

#define DEG_PER_SEC_PER_BIT 	0.018315	// For +/- 80 degrees per second
#define DEG_PER_BIT 			0.1453		// Temperatuer Sensor
#define DEG_OFFSET				25.0		// The zero value for the temperature is 25.0C
#define RAD_PER_SEC_PER_BIT		0.00031974431896536100 // For +/- 80 degrees per second range, taken by converting 0.01832 to rad/sec/LSB

// The following defines are commands straight from the datasheet
#define FLASH_CNT				0x0000		// Flash memory write count
#define SUPPLY_OUT				0x0200		// Output, power supply measurement
#define GYRO_OUT				0x0400		// Output, rate of rotation measurement
#define AUX_ADC					0x0A00		// Output, analog input channel measurement
#define TEMP_OUT				0x0c00		// Output, internal temperature measurement
#define	ANGL_OUT				0x0E00		// Output, angle displacement
#define GYRO_CAL_OFFSET			0x1400		// Calibration, offset/bias adjustment (Default: 0x0000)
#define GYRO_CAL_SCALE			0x1600		// Calibration, scale adjustment (Default: 0x0800)
#define ALM_MAG1				0x2000		// Alarm 1 magnitude/polarity setting (Default: 0x0000)
#define	ALM_MAG2				0x2200		// Alarm 2 magnitude/polarity setting (Default: 0x0000)
#define ALM_SMPL1				0x2400		// Alarm 1 dynamic rate of change setting (Default: 0x0000)
#define ALM_SMPL2				0x2600		// Alarm 2 dynamic rate of change setting (Default: 0x0000)
#define ALM_CTRL				0x2800		// Alarm control register (Default: 0x0000)
#define AUX_DAC					0x3000		//
#define GPIO_CTRL				0x3200
#define MSC_CTRL				0x3400
#define SMPL_PRD				0x3600
#define SENS_AVG				0x3800		// Control, dynamic range, filtering.  (Default: 0x0402)
// more skipped here...
#define LOT_ID_1				0x5200
#define LOT_ID_2				0x5400
#define PRODUCT_ID				0x5600		// Product ID (should be 0x3F84)
#define SERIAL_NUM				0x5800		// Serial Number (


//========================================
// 			Driver Functions
//========================================

// Note: The below functions are necessary to use hardware.

/*=================================================================
  PURPOSE: Read all gyros using a given command.
  UNMODIFIED:  request - a 16 bit request code
  MODIFIED VARIABLES: lastResponse - an array containing all the responses
		from all the gyroscopes from the last request.  The length 
		of this will be twice the number of gyroscopes
  GLOBALS: None
  RETURN VALUE: Error codes
  NOTES: The response contains data from the last time the 
		request was sent.  For example, if the last call to this 
		function was a request for temperature data, then the
		current call will return temperature data as the array
		lastResponse.
-----------------------------------------------------------
*/

ReturnErr_t readFromAllGyros(int request, signed int * lastResponse)
{
//	unsigned char m;					// Counter variable for SPI write/read operation
//	unsigned int  mask;		// Mask used to isolate individual bits during SPI write operation
//	unsigned int value;		// Value clocked in via SPI
//	unsigned char measurement[2];	// Two bytes to hold response.
	unsigned char gyro;				// Counter variable to talk to each gyroscop in turn
	int res;
	int i,j;

	// Default Value for Gyros:
	memset(lastResponse,0,NUM_OF_GYROS*2);

	// For each gyroscope
	for(gyro=0;gyro<NUM_OF_GYROS;gyro++)
	{
		i=2*gyro;
		j=i+1;
		gyroTalk(gyro,request,&res);
		lastResponse[gyro]=res;

		//lastResponse[i]=make8(res,0);
		//lastResponse[j]=make8(res,1);

/*		Old Code

		// Select Chip for given Gyro
		gyroCS(gyro,0);			

		// Aquire Measurements (Note that the MSB arrives first and
		// that this compiler places MSB in the higher byte of an int,
		// hence the reverse indexing).
		measurement[1] = spi_read(request>>8);
		measurement[0] = spi_read(request&0x00ff);

		// Transfer the readings to the modified function argument
		memcpy(lastResponse+(2*gyro),measurement,2);	

		// Unselect this gyroscope.
		gyroCS(gyro,1);
*/

		// Debug
		//signed int temp;		// Temporary variable for display of data to screen
		//temp = *((signed int *)lastResponse);
		//sprintf(dbgbuf,"\r\nWrote 0x%lX to gyro %i.  Received: 0x%lX = %d",request,gyro,temp,temp);
		//sprintf(dbgbuf,"\r\nWrote 0x%X%X to gyro %i.  Received: 0x%lX = %d",requestPtr[1],requestPtr[0],gyro,temp,temp);
		//sendDBGALL(USER_PORT,dbgbuf);
	}


	return SUCCESS;
}

/*=================================================================
  PURPOSE: Convert data in a gyro structure to the body frame
  UNMODIFIED VARIABLES: 
  MODIFIED VARIABLES: Data -- a Gyro structure to be modified
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: 
	This function will convert the numbers in gyro_s Data such
	that:
		Gyro 0 = Body X Axis
		Gyro 1 = Body Y Axis
		Gyro 2 = Body Z Axis

	The current mounting scheme aligns all the axes, but their
	rotations are opposite.  Therefore, to convert to the body
	axis, all Gyro Rotation Data must be negated.

TODO: Make this changeable on orbit... just in case.
-----------------------------------------------------------------
*/
ReturnErr_t	convertFrameGyrosToBody(gyro_s * Data)		// Gyro frame conversion
{
	int i;
	for(i=0;i<3;i++)
	{
		Data->rate[i]      = -1 * Data->rate[i];		// Negate the counts
		Data->f_rateDPS[i] = -1.0 * Data->f_rateDPS[i];	// Negate the degrees/sec
		Data->f_rateRPS[i] = -1.0 * Data->f_rateRPS[i];	// Negate the radians/sec
	}	

	return SUCCESS;
}

/*=================================================================
  PURPOSE: send a request to a gyro, and simultaneously read the 
			results of the previous request
  UNMODIFIED VARIABLES: whichGyro -- The gyro number to talk to
						send - a 16-bit request
  MODIFIED VARIABLES: rcvd - the returned information
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Be careful about the byte order.
-----------------------------------------------------------
*/
void gyroTalk(int whichGyro, int send, int * rcvd)
{
	unsigned char temp[2];	

	if (whichGyro<0 || whichGyro>NUM_OF_GYROS) return;	// bail if it's a bad request

	// Set flash to gyro mode
	SetSPIMode(GYRO_SPI_MODE);

	gyroCS(whichGyro,0); 				// Select chip for given gyro
	delay_us(5); 						// give it a second to engage.	
	temp[0]=spi_read(send>>8); 			// Send in two parts
	temp[1]=spi_read(send&0x00ff); 		// 2nd part
	*rcvd=make16(temp[0],temp[1]);		// Remap the response back to a 16-bit int.
	gyroCS(whichGyro,1); 				// Deselect chip for given gyro

}

/*=================================================================
  PURPOSE: Select a specific gyroscope to read from
  UNMODIFIED VARIABLES: gy - Gyroscope Number
						th - Desired state of chipselect pin
  MODIFIED VARIABLES: None.
  GLOBALS: None
  RETURN VALUE: None.
  NOTES: Tested via AENEAS PPM on CAERUS.  From iControl.
-----------------------------------------------------------
*/

void gyroCS(unsigned char gy, unsigned char th) 
{
	switch(gy) {
		case 0:
			switch(th){
				case 0:
					output_low(GYROCS1);
					//sendDBGALL(USER_PORT,"1");
				  break;
				case 1:
					output_high(GYROCS1);
				  break;
				default:
				  break;
			}
		  break;
		case 1:
			switch(th){
				case 0:
					output_low(GYROCS2);
					//sendDBGALL(USER_PORT,"2");
				  break;
				case 1:
					output_high(GYROCS2);
				  break;
				default:
				  break;
			}

		  break;
		case 2:
			switch(th){
				case 0:
					output_low(GYROCS3);
					//sendDBGALL(USER_PORT,"3");
				  break;
				case 1:
					output_high(GYROCS3);
				  break;
				default:
				  break;
			}

		  break;
		default:
		  break;
	}
}


/*=================================================================
  PURPOSE: Initialize the gyroscope interface and device.
  UNMODIFIED VARIABLES: None
  MODIFIED VARIABLES: None
  GLOBALS: None
  RETURN VALUE: None
  NOTES: None.
-----------------------------------------------------------
*/

ReturnErr_t gyro_init()
{
	// Variable to place readings
	signed int reading[NUM_OF_GYROS];	

	// Turn on gyroscope.
	gyro_on();

	// Functional check
	if(gyro_functional_check()!=SUCCESS)
	{
		return FAILURE;		
	}

	// We are operating gyroscopes in full duplex mode, meaning that when we write a 
	// register request to the gyroscope, we are also receiving data from the last
	// register request.  To start this cycle off correctly, we perform only a write
	// and ignore the response.  We start with a request for error code since
	// that is the first decoded item in collectAllGyroData().

	// Execute a write while ignoring the returned bytes.
	if(readFromAllGyros(GYRO_ERROR_CODE,reading) != SUCCESS)
	{
		return FAILURE;		
	}
}	


/*=================================================================
  PURPOSE: Turn gyroscope power on.
  UNMODIFIED VARIABLES: None
  MODIFIED VARIABLES: None
  GLOBALS: None
  RETURN VALUE: None
  NOTES: None.
-----------------------------------------------------------
*/

void gyro_on()
{
	output_high(GYRO_ON);
	return;
}

/*=================================================================
  PURPOSE: Turn gyroscope power off.
  UNMODIFIED VARIABLES: None
  MODIFIED VARIABLES: None
  GLOBALS: None
  RETURN VALUE: None
  NOTES: None.
-----------------------------------------------------------
*/

void gyro_off()
{
	output_low(GYRO_ON);
	return;
}

/*=================================================================
  PURPOSE: Decode the gyro data based on what command was sent.
  UNMODIFIED VARIABLES: Write - The command code that was sent to get the data
		reading - An array of 16-bit values to be decoded.
  MODIFIED VARIABLES: Data - a struct containing all gyro data
  GLOBALS: None
  RETURN VALUE: None
  NOTES: 
---------------------------------------------------------------------
*/
ReturnErr_t decodeGyroReadings(int Write, gyro_s * Data, signed int * reading)
{
	// Counter variables
	int i;

	// For all gyroscopes
	for(i=0;i<NUM_OF_GYROS;i++)
	{
		// Switch on the write command that was sent out
		switch(Write)
		{
			// Rate Command
			case GYRO_RATE_CODE:
			{		
				// Assign raw byte to struct for debugging
				Data->rawrate[i]=reading[i];

				// Check for new data (MSB or 15th bit)
				if(bit_test(reading[i],15)) Data->rateND[i] = 1;
				else  Data->rateND[i] = 0;

				// Check for alarm (MSB minus 1 or 14th bit)
				if(bit_test(reading[i],14)) Data->rateEA[i] = 1;
				else  Data->rateEA[i] = 0;

				// Sign extend
				signExtend14to16(&reading[i]);

		/*
				// Will's Method of 14-bit 2's complement using sign bit extension
				if(bit_test(reading[i],13))
				{
					// Note: bit_set() and bit_clear() behave strangely.  If we try
					// to explicitly set bits 14 and 15 using bit_set(14) and bit_set(15)
					// calls back to back, nothing happens.  For whatever reason, it needs
					// to be within a for loop.  This is true for several other places
					// within this function.  Strange compiler.

					for(j=14;j<16;j++)
					{
						bit_set(reading[i],j);
					}
				}
				else
				{
					for(j=14;j<16;j++)
					{
						bit_clear(reading[i],j);
					}
				}
	*/
				// Assign the reading to the data structure
				Data->rate[i] = reading[i];

				// Convert the readings and store
				Data->f_rateDPS[i] = gyro_counts2DPS(reading[i]);
				Data->f_rateRPS[i] = gyro_counts2RPS(reading[i]);

				break;
			}

			// Temperature Command
			case GYRO_TEMP_CODE:
			{
				// Assign raw byte to struct for debugging
				Data->rawtemp[i]=reading[i];

				// Check for new data (MSB, aka 15th bit)
				if(bit_test(reading[i],15)) Data->tempND[i] = 1;
				else  Data->tempND[i] = 0;

				// Check for alarm (MSB minus 1, aka 14th bit)
				if(bit_test(reading[i],14)) Data->tempEA[i] = 1;
				else  Data->tempEA[i] = 0;

				// Sign Extend 12 to 16
				signExtend12to16(&reading[i]);
/*
				// Will's Method of 12-bit 2's complement using sign bit extension
				// (See note above concerning use of variables in bittest() )
				if(bit_test(reading[i],11))
				{	
					for(j=12;j<16;j++)
					{
						bit_set(reading[i],j);
					}
				}
				else
				{
					for(j=12;j<16;j++)
					{
						bit_clear(reading[i],j);
					}
				}
*/
				// Assign the reading to the data structure
				Data->temp[i] = reading[i];

				// Convert the readings and store
				Data->f_temp[i] = gyro_counts2C(reading[i]);
	
				break;
			}

			// Error Code
			case GYRO_ERROR_CODE:
			{
				// Assign raw byte to struct for debugging
				Data->rawerror[i]=reading[i];

				Data->error[i] = reading[i];
				break;
			}

			// Should not ever reach here.
			default:
			{
				return UNREACHABLE;
			}
		}
	}
	
	return SUCCESS;
} 

/*=================================================================
  PURPOSE: Perform a sign extension from 14 to 16 bits.
  UNMODIFIED VARIABLES: 
  MODIFIED VARIABLES: i - a 16-bit integer
  GLOBALS: None
  RETURN VALUE: None
  NOTES: None
-----------------------------------------------------------
*/
void signExtend14to16(int * i)
{
	if(bit_test(*i,13))
	{
		*i=*i|0b1100000000000000; // set the two highest bits via |
	}
	else
	{
		*i=*i&0b0011111111111111; // clear the two highest bits using &
	}
}

void signExtend12to16(int * i)
{
	if(bit_test(*i,11))
	{
		*i=*i|0b1111000000000000; // set the four highest bits via |
	}
	else
	{
		*i=*i&0b0000111111111111; // clear the four highest bits using &
	}
}


/*=================================================================
  PURPOSE: Read all data from all gyros.
  MODIFIED VARIABLES: Data - a struct that will contain all recorded data.
  UNMODIFIED: None.
  GLOBALS: None.
  RETURN VALUE: Error codes
  NOTES: As described during the init function, in order to operate in full
	duplex mode, we write a register request while simultaneously receiving
	data from the last register request.  That is why you will note in this
	function that a write of, say, GYRO_RATE_CODE is followed by a 
	decoding of the read GYRO_ERROR_CODE.  The pattern is this:

	1.	Write GYRO_RATE_CODE / Read GYRO_ERROR_CODE
	2.	Write GYRO_TEMP_CODE / Read GYRO_RATE_CODE	
	3. 	Write GYRO_ERROR_CODE / Read GYRO_TEMP_CODE
	4. 	Go to step 1.	

-----------------------------------------------------------
*/

ReturnErr_t collectAllGyroData(gyro_s * Data)
{
	signed int reading[NUM_OF_GYROS];

	// Read Rates
	if(readFromAllGyros(GYRO_RATE_CODE,reading) == SUCCESS)
	{
		// Decode Error Status
		if(decodeGyroReadings(GYRO_ERROR_CODE,Data,reading) != SUCCESS) return SUBFUNCTION_ERR;
	}
	else return SUBFUNCTION_ERR;

	// Read Temperatures
	if(readFromAllGyros(GYRO_TEMP_CODE,reading) == SUCCESS)
	{
		// Decode Rates
		if(decodeGyroReadings(GYRO_RATE_CODE,Data,reading) != SUCCESS) return SUBFUNCTION_ERR;
	}
	else return SUBFUNCTION_ERR;

	// Read Errors
	if(readFromAllGyros(GYRO_ERROR_CODE,reading) == SUCCESS)
	{
		// Decode Temperatures
		if(decodeGyroReadings(GYRO_TEMP_CODE,Data,reading) != SUCCESS) return SUBFUNCTION_ERR;
	}
	else return SUBFUNCTION_ERR;

	return SUCCESS;
}

// Averaging two points of data and storing it in A
void AverageGyroData(gyro_s * A, gyro_s * B, unsigned char reset)
{
	int i;
	static unsigned long divisor = 2;

	if(reset) divisor = 2;

//  DEBUG
//	sendDBGALL(USER_PORT,"\r\n");
//	printFloat(dbgbuf,A->f_rateDPS[0],6);
//	sendDBGALL(USER_PORT,dbgbuf);
//	sendDBGALL(USER_PORT,",");
//	printFloat(dbgbuf,B->f_rateDPS[0],6);
//	sendDBGALL(USER_PORT,dbgbuf);
//	sendDBGALL(USER_PORT,",");

	for(i=0;i<3;i++)
	{
		A->rawrate[i]=((A->rawrate[i]*(divisor-1))+B->rawrate[i])/divisor;
		A->rawtemp[i]=((A->rawtemp[i]*(divisor-1))+B->rawtemp[i])/divisor;

		A->rate[i]=((A->rate[i]*(divisor-1))+B->rate[i])/divisor;
		A->temp[i]=((A->temp[i]*(divisor-1))+B->temp[i])/divisor;

		A->f_rateDPS[i] = ((A->f_rateDPS[i]*(divisor-1))+B->f_rateDPS[i])/divisor;
		A->f_rateRPS[i] = ((A->f_rateRPS[i]*(divisor-1))+B->f_rateRPS[i])/divisor;
		A->f_temp[i] = ((A->f_temp[i]*(divisor-1))+B->f_temp[i])/divisor;
	}

//  DEBUG
//	printFloat(dbgbuf,A->f_rateDPS[0],6);
//	sendDBGALL(USER_PORT,dbgbuf);

	divisor++;
	return;
}

/*
void resetGyroData(gyro_s * A)
{

// Do nothing
return;
}
*/

ReturnErr_t gyro_functional_check()
{
	int gyro_read[3];			// Read data from the gyros
	unsigned long timeout;		

	// Set the timeout in seconds
	timeout = SEC_TIMER + 3;

	// Loop checking the gyros.  Quit on success or timeout.
	do{
		// Query the gyros
		gyroTalk(0,PRODUCT_ID,&gyro_read[0]);			// get result and request status again
		gyroTalk(1,PRODUCT_ID,&gyro_read[1]);			// get result and request status again
		gyroTalk(2,PRODUCT_ID,&gyro_read[2]);			// get result and request status again
	
		// Examine the results
		if(gyro_read[0]==0x3f84 && gyro_read[1]==0x3f84 && gyro_read[2]==0x3f84) 
		{
			return SUCCESS;
		}
	}while(timeout>SEC_TIMER);

	return FAILURE;
}


//========================================
// 		Development Functions
//========================================

// Note: The below functions are only necessary for testing and development.  They
// can be removed during actual production

#ifdef DEBUG


/*=================================================================
  PURPOSE: Print out a human readable version of the struct
  MODIFIED VARIABLES: None.
  UNMODIFIED: gyroData - a struct containing all the data so far.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: Untested.
-----------------------------------------------------------
*/

void printGyroStruct(gyro_s * Data)
{
	int i;
	for(i=0;i<NUM_OF_GYROS;i++)
	{

		// Heading
		sprintf(dbgbuf,"\r\nGyro %d:",i);
		sendDBGALL(USER_PORT,dbgbuf);

		// Rate Data
		sprintf(dbgbuf,"\r\nRate: [Raw] %LX [Counts] %LX (%d) [Flags] (%d,%d) [Degs]",
			Data->rawrate[i],Data->rate[i],Data->rate[i],Data->rateND[i],Data->rateEA[i]);
		sendDBGALL(USER_PORT,dbgbuf);
		printFloat(dbgbuf,Data->f_rateDPS[i],3);
		sendDBGALL(USER_PORT,dbgbuf);
		

		// Temperature Data
		sprintf(dbgbuf,"\r\nTemp: [Raw] %LX [Counts] %LX (%d) [Flags] (%d,%d) [DegsC]",
			Data->rawtemp[i],Data->temp[i],Data->temp[i],Data->tempND[i],Data->tempEA[i]);
		sendDBGALL(USER_PORT,dbgbuf);
		printFloat(dbgbuf,Data->f_temp[i],3);
		sendDBGALL(USER_PORT,dbgbuf);

		// Error Data
		sprintf(dbgbuf,"\r\nError (n/a): %lX > %lX. Bits:",Data->rawerror[i],Data->error[i]);
		sendDBGALL(USER_PORT,dbgbuf);

		// Error Bits
		byteMe(Data->error[i]);
	}

	return;
}

/*=================================================================
  PURPOSE: Print out a machine readable version of the struct
  MODIFIED VARIABLES: None.
  UNMODIFIED: gyroData - a struct containing all the data so far.
		line_number - an externally supplied line number to print.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: Untested.
-----------------------------------------------------------
*/

void printGyroStuctCSV(gyro_s * Data, long line_number)
{	
	// Counter Variable
	int i;

	// Line Number
	sprintf(dbgbuf,"\r\n%Ld",line_number); 	
	sendDBGALL(USER_PORT,dbgbuf);

	// Rates
	for(i=0;i<NUM_OF_GYROS;i++)
	{
		sprintf(dbgbuf,",%d",Data->rate[i]);
		sendDBGALL(USER_PORT,dbgbuf);
	}

	// Float rates in Degrees Per Sec
	for(i=0;i<NUM_OF_GYROS;i++)
	{
		sendDBGALL(USER_PORT,",");
		printFloat(dbgbuf,Data->f_rateDPS[i],3);
		sendDBGALL(USER_PORT,dbgbuf);
	}

	// Float rates in Radians Per Sec
	for(i=0;i<NUM_OF_GYROS;i++)
	{
		sendDBGALL(USER_PORT,",");
		printFloat(dbgbuf,Data->f_rateRPS[i],3);
		sendDBGALL(USER_PORT,dbgbuf);
	}

	// Temps
	for(i=0;i<NUM_OF_GYROS;i++)
	{
		sprintf(dbgbuf,",%d",Data->temp[i]);
		sendDBGALL(USER_PORT,dbgbuf);
	}

	// Float temps in Degrees C
	for(i=0;i<NUM_OF_GYROS;i++)
	{
		sendDBGALL(USER_PORT,",");
		printFloat(dbgbuf,Data->f_temp[i],3);
		sendDBGALL(USER_PORT,dbgbuf);
	}

	// Rate New Data Indicators
	for(i=0;i<NUM_OF_GYROS;i++)
	{
		sprintf(dbgbuf,",%d",Data->rateND[i]);
		sendDBGALL(USER_PORT,dbgbuf);
	}

	// Temps New Data Indicators
	for(i=0;i<NUM_OF_GYROS;i++)
	{
		sprintf(dbgbuf,",%d",Data->tempND[i]);
		sendDBGALL(USER_PORT,dbgbuf);
	}
}



/*=================================================================
  PURPOSE: Puts fake data into the gyro structure and looks for correct conversions
  MODIFIED VARIABLES: None.
  UNMODIFIED: 
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: 
-----------------------------------------------------------
*/
void gyroDecodeTest()
{
/*
	int fakeRates[5]={0x1110,0x0001,0x0000,0x3FFF,0x2EF0};	// Max Pos, Min Pos, Zero, Min Neg, Max Neg
	int fakeTemps[5]={0x227,0x001,0x000,0xFFF,0xE41};		// Degres C: 105,25.1453,25,24.8547,-40
	
	int f_reading[NUM_OF_GYROS];
	char discard;

	gyro_s Data;		// The data structure to use for each iteration
	long i = 0;			// Iteration counter
	long timeout;		// Timeout used to pause before data collection

	// Let user know how to escape!
	sendDBGALL(USER_PORT,"\r\nFaking Gyro data.");

	// Set the timeout timer
	timeout=SEC_TIMER+1;

	// Loop until current time passes timeout.
	while(SEC_TIMER<timeout)
	{
		// Do nothing.
	}

	for(i=0;i<6;i++)
	{
		// Pretend collection of rates
		f_reading[0]=fakeRates[i];
		f_reading[1]=fakeRates[i];
		f_reading[2]=fakeRates[i];
		// Conversion
		decodeGyroReadings(GYRO_RATE_CODE, &Data, f_reading);

		// Pretend collection of temps
		f_reading[0]=fakeTemps[i];
		f_reading[1]=fakeTemps[i];
		f_reading[2]=fakeTemps[i];
		// Conversion
		decodeGyroReadings(GYRO_TEMP_CODE, &Data, f_reading);

		// Show results
		printGyroStruct(&Data);
		
		sendDBGALL(USER_PORT,"\r\nResults should match datasheet.");

		sendDBGALL(USER_PORT,"\r\nPress a key to continue...");
		while(!getByte(USER_PORT,&discard)) {restart_wdt();};

	}

	return;	
*/

}


/*=================================================================
  PURPOSE: Automatically collect lots of gyroscope data (user specified
	or infinite).  Then, printit out in either human readable form or
	CSV.
  MODIFIED VARIABLES: None.
  UNMODIFIED: iterations - number of times to collect data.
		continuous - if true, collect data until user presses "q".
		human - if true, print in human readable form.  Otherwise, CSV.
  GLOBALS: None.
  RETURN VALUE: None.
  NOTES: All data is sampled at 5Hz.
-----------------------------------------------------------
*/

void gyroAutomaticDataCollection(long iterations, short continuous, short human)
{
/*
	gyro_s Data;		// The data structure to use for each iteration
	long i = 0;			// Iteration counter
	long timeout;		// Timeout used to pause before data collection
	short userFlag = 1;	// Indicator that user wants to quit.  (False = quit)
	unsigned char temp;			// Temporary character holder to check user keystroke
	int64 msec_timeout;
	
	// Let user know how to escape!
	sendDBGALL(USER_PORT,"\r\nCollecting Gyro data.  Press 'q' to quit.  Starting in 1 seconds ...");

	// Set the timeout timer
	timeout=SEC_TIMER+1;

	// Loop until current time passes timeout.
	while(SEC_TIMER<timeout)
	{
		// Do nothing.
	}

	sendDBGALL(USER_PORT,"\r\nStart!");

	if(!human) 	sendDBGALL(USER_PORT,"\r\nLine,Rate Count X,Rate Count Y,Rate Count Z,Rates_DPS X,Rates_DPS Y,Rates_DPS Z,");	// Header for CSV output.
	if(!human) 	sendDBGALL(USER_PORT,"Rates_RPS X,Rates_RPS Y,Rates_RPS Z,Temp Counts X,Temp Counts Y,Temp Counts Z,Temps_C X,Temps_C Y,Temps_C Z,R_New Data,T_New Data");	// Header for CSV output.

	// userFlag defaults to true and will remain true until user
	// hits a 'q' keystroke.  If the function call has continuous as
	// true, then it will ignore the i vs. iterations condition.  
	// if not, it will watch the counter and then quit when it supersedes
	// iterations.
	while(userFlag && (continuous || (i < iterations)))
	{
		// Set millisecond timeout to 250ms
		msec_timeout=MSEC+250;

		// Iterate
		i++;

		// Collect all data across the gyroscopes
		if(collectAllGyroData(&Data) != SUCCESS)
		{
			// Let user know
			sendDBGALL(USER_PORT,"\r\n ... failed!");
			return;
		}
		else
		{
			// Print out the information in some form
			if(human) printGyroStruct(&Data);
			else printGyroStuctCSV(&Data,i);
		}

		// Check if a character is available
		if(getByte(USER_PORT,&temp)) 
		{
			// If it is any form of 'q', signal that we want to quit.
			if(temp == 'q' || temp == 'Q') userFlag = 0;
		}
				
		// Force into 4Hz Mode
		while(msec_timeout>MSEC)
		{
			// do nothing
		}

	}
	
	return;
*/
}

/*=================================================================
  PURPOSE: Return a floating point representation of the gyroscope 
	rate reading in degrees per second
  MODIFIED VARIABLES: None.
  UNMODIFIED: None.
  GLOBALS: None.
  RETURN VALUE: gyroscope spin rate in degrees per second.
  NOTES: Positive spin rate reading represents a clockwise rotation of
	the gyroscope when the gyroscope is viewed from the top. See datasheet.
-----------------------------------------------------------
*/
float gyro_counts2DPS(int rate)
{
	return rate*DEG_PER_SEC_PER_BIT;
}


int gyro_DPS2counts(float x)	  // Returns a sign-extended 16 bit int
{
	int temp;
	x=x/DEG_PER_SEC_PER_BIT;	// Convert to counts
	temp = (x >= 0) ? (int)(x+0.5) : (int)(x-0.5);	// Round off
	return temp;
	
}


/*=================================================================
  PURPOSE: Return a floating point representation of the gyroscope 
	rate reading in radians per second
  MODIFIED VARIABLES: None.
  UNMODIFIED: None.
  GLOBALS: None.
  RETURN VALUE: gyroscope spin rate in radians per second.
  NOTES: Positive spin rate reading represents a clockwise rotation of
	the gyroscope when the gyroscope is viewed from the top. See datasheet.
-----------------------------------------------------------
*/
float gyro_counts2RPS(int rate)
{
	return rate*RAD_PER_SEC_PER_BIT;
}

/*=================================================================
  PURPOSE: Return a floating point representation of the gyroscope 
	temperature in degrees Celcius
  MODIFIED VARIABLES: None.
  UNMODIFIED: None.
  GLOBALS: None.
  RETURN VALUE: gyroscope temperature in degrees Celcius
  NOTES: Note that per the datasheet, 0x0000 = 25 degrees C
-----------------------------------------------------------
*/
float gyro_counts2C(int temperature)
{
	return temperature*DEG_PER_BIT + DEG_OFFSET;
}



#endif // DEBUG
