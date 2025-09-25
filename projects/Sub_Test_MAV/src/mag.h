/***************************************************************************

								mag.h

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Provide hardware interface to magnetometer.

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
07-13-10  Will	Initial revision.		

****************************************************************************
*/

#ifndef __MAG_H__
#define __MAG_H__

//========================================
//  			Dependencies
//========================================
#include "common.h"
#include "spi.h"						// SPI functionality					



//========================================
// 			Data Structure
//========================================
typedef struct {
	signed int counts[3];
	float microTesla[3];
} mag_s;

typedef enum
{
	X_AXIS_X_COIL = 0,
	X_AXIS_Y_COIL = 1,
	X_AXIS_Z_COIL = 2,
	Y_AXIS_X_COIL = 3,
	Y_AXIS_Y_COIL = 4,
	Y_AXIS_Z_COIL = 5,
	Z_AXIS_X_COIL = 6,
	Z_AXIS_Y_COIL = 7,
	Z_AXIS_Z_COIL = 8,
	NUM_COEFFICIENTS = 9
} BiasCoefficientVariables;
float BiasCoefficients[NUM_COEFFICIENTS] = {0.7567,         -1.0 * 0.4181, -1.0 * 0.8263,
											16.7923,        0.1310,        -1.0 * 52.4873,
											-1.0 * 76.6455, 5.7955,        -1.0 * 263.9334};

//========================================
// 			Driver Functions
//========================================

ReturnErr_t mag_init();								// Initialize Magnetometer SPI pins
ReturnErr_t readAllMagAxis(signed int * reading, short reset);	// Read all three axis for magnetometer
ReturnErr_t convertFrameMagToBody(signed int* readings, float* torqueCoilCommand,
								  mag_s* Data); // Mag frame and unit conversion
ReturnErr_t mag_functional_check(unsigned int max_time_seconds);


//========================================
// 		Development Functions
//========================================

#ifdef DEBUG
void printMagData(signed int * reading);
void magAutomaticDataCollection(long iterations, short continuous, short human);
#endif // DEBUG

#endif // __MAG_H__