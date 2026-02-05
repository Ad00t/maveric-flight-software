/***************************************************************************

								gyro.h

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Header file for the ADIS16250 MEMS Gyroscope

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
07-15-10  Will	Initial revision.		

****************************************************************************
*/


/*  Notes on mounting:

	Identification of gryoscopes is very arbitrary.  Their numbering comes
	from the original numbering for the chip select lines.  These lables
 	(CS1, CS2, etc) have tended to shift around.  Therefore it has been
	chosen to identify the gyroscopes as mounted according to the obvious:

	Gyroscope 1 - X
	Gyroscope 2 - Y
	Gyroscope 3 - Z

	This is basically determined in one of the pin header files.  By choosing
	which chip select line GYROCS1 refers to, for example, one is essentially
	choosing which gyroscope to communicat with.  

	The X, Y, Z, axis refer to those found on confluence.  Z axis points out 
	through the antenna feedhorn.  The +X axis points out the RBF/USB panel 
	and the +Y axis is the one formed such that sweeping from +X to +Y produces
	+Z according to the right hand rule.

	Positive spin rate reading represents a clockwise rotation of
	the gyroscope when the gyroscope is viewed from the top. See datasheet.

	Gyroscope 1 - X
	Gyroscope 2 - 
	Gyroscope 3 - 

*/

#ifndef __GYRO_H__
#define __GYRO_H__

//========================================
//  			Dependencies
//========================================
#include "common.h"
#include "spi.h"						// SPI functionality					



//========================================
//  			DEFINES
//========================================
#define NUM_OF_GYROS			3

// These are for the debug function gyroAutomaticDataCollection()
#define GYRO_ITERATED			0
#define GYRO_CONTINUOUS			1

#define GYRO_NOT_HUMAN_READABLE	0
#define GYRO_HUMAN_READABLE		1

//========================================
// 				Data structures
//========================================

// Gyro data structure, holding all the most recent readings from the gyro.
typedef struct 
{
	// ***** Temporary members used for debugging.
	signed int rawrate[NUM_OF_GYROS];
	signed int rawtemp[NUM_OF_GYROS];
	signed int rawerror[NUM_OF_GYROS];

	signed int rate[NUM_OF_GYROS];		// Rates in counts
	signed int temp[NUM_OF_GYROS];		// Temperature in counts
	signed int error[NUM_OF_GYROS];		// Status Bitfield
	unsigned char	rateND[NUM_OF_GYROS];		// Rate New Data Indicator
	unsigned char 	tempND[NUM_OF_GYROS];		// Temp New Data Indicator
	unsigned char 	rateEA[NUM_OF_GYROS];		// Rate Alarm Indicator
	unsigned char 	tempEA[NUM_OF_GYROS];		// Temp Alarm Indicator 

	float f_rateDPS[NUM_OF_GYROS];		// Rates in deg/sec
	float f_rateRPS[NUM_OF_GYROS];		// Rates in rad/sec
	float f_temp[NUM_OF_GYROS];		// Temperature in degrees C

} gyro_s;

//========================================
//  		Driver Functions
//========================================
ReturnErr_t gyro_init();
void gyro_on();
void gyro_off();
ReturnErr_t gyro_functional_check();					// Checks the gyro by querying the manufacturer ID
ReturnErr_t collectAllGyroData(gyro_s * Data);			
ReturnErr_t	convertFrameGyrosToBody(gyro_s * Data);		// Gyro frame and unit conversion

// Generic function to write a request to all gyros and record the response.  
// The best way to call is to declare an unsigned int like this: unsigned int reading[NUM_OF_GYROS];
// Then call by casting to an unsigned char *, like this:  readFromAllGyros(GYRO_RATE_CODE,(unsigned char *) reading)
ReturnErr_t readFromAllGyros(int request, signed int * lastResponse);
void gyroCS(unsigned char gy, unsigned char th);
void gyroTalk(int whichGyro, int send,int * rcvd);
void signExtend14to16(int * i);
void signExtend12to16(int * i);
ReturnErr_t decodeGyroReadings(int Write, gyro_s * Data, signed int * reading);
void AverageGyroData(gyro_s * A, gyro_s * B, unsigned char reset);
//void resetGyroData(gyro_s * A);


//========================================
// 		Development Functions
//========================================

#ifdef DEBUG
void printGyroStruct(gyro_s * Data);
void gyroAutomaticDataCollection(long iterations, short continuous, short human);
float gyro_counts2DPS(int rate);
float gyro_counts2RPS(int rate);
int gyro_DPS2counts(float rate);	// Returns a sign-extended 16 bit int

float gyro_counts2C(int temperature);

void gyroDecodeTest();
#endif // DEBUG

#endif // __GYRO_H__
