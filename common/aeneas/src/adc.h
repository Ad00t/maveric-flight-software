/***************************************************************************

									adc.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide Analog-To-Digital drivers

Changelog:

Date 	 |  Au.  |	Notes
07-19-10	mra		Created.

****************************************************************************
*/

#ifndef __ADC__
#define __ADC__

//==============================================
//  				Dependencies
//==============================================
#include "common.h"							// Quotes indicate to search only the same directory as this file. ANgle brackets indicate to search the MPLAB-specified search path
#include "pins.h"

//==============================================
//  				Defines
//==============================================

// .865 (Pin 33)
// .864 (Pin 35
// Pins (32 and 34) were maxed.
// Vref+ -- 0.92 (Pin 65)
// Vref- -- 0.75 (Pin 69)
// Spread -- 0.17

// Vref+ -- 0.838 (Pin 65)
// Vref- -- 0.636 (Pin 69)
// Spread -- 0.202

// Vdd -- 3.3 
// Vref- -- 0.75
// Spread -- 2.55


// Unfortunately, on the dev upper board, it looks like
// the high side is at 1.66 Volts, a spread of 0.91V
// Also, the Vref- seems to be at ____


//#define MAX_ADC_READING			0x400				
#define MAX_ADC_READING			0x3FF				
//#define VOLTAGE_RANGE			0.91
#define VOLTAGE_RANGE			3.6775827
#define VOLTAGE_MINIMUM			0.6725116    // from testing

#define RTD1_CHAN	8
#define RTD2_CHAN	9
#define RTD3_CHAN	10
#define RTD4_CHAN	11

//==============================================
//  				Functions
//==============================================
int read_adc_channel(unsigned char channel);		// Reads a specific ADC channel
void collecTemperatures(float * temps);
void ADCTest(int port);		

#endif
