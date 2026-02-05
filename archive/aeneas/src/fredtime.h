/***************************************************************************

								fredtime.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne

Purpose: Provide functions to work with Fred's (iControl's) nonstandard
time vector

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
10-10-11	mra		Initial revision.		

	// Time format:
	// time[7]
	// MM DD YY WKday HH MM SS
	// 0   1  2   3    4  5  6

   When printed, looks like this
   DD/MM/YY Weekday HH:MM:SS
   
   Seconds range from 0 to 59
   Minutes range from 0 to 59
   Hours range from 0 to 23
   Weekdays are as follows:
      0 -- Sunday
      2 -- Monday
      ...etc
      6 -- Saturday
   Months range from 1 to 12
   Days range from 1 to dim[month] (usually 30 or 31)
   Years range from 0 to 99


****************************************************************************
*/

#ifndef __FREDTIME__
#define __FREDTIME__


//========================================
// 			Dependencies
//========================================
#include "common.h"
#include "errors.h"
#include "debug.h"

//========================================
// 			Useful Defines
//========================================
	// DO NOT CHANGE
	#define FRED_SEC 6
	#define FRED_MIN 5
	#define FRED_HOUR 4
	#define FRED_WKDAY 3
	#define FRED_YEAR 2
	#define FRED_DAY 1
	#define FRED_MONTH 0

//========================================
// 			Globals
//========================================
extern unsigned char time[7];					// The global time vector to be declared in main

//========================================
// 			Driver Functions
//========================================
// These first few functions work with the RTC clock, so be caseful about using them
void getTime(unsigned char * time,rtc_time_t * RTC);				// Gets the latest RTC time
void setTime(unsigned char month, unsigned char day, unsigned char year, unsigned char weekday, unsigned char hour, unsigned char minute, unsigned char second);	// Sets the RTC explicitly
void setTime(unsigned char * time);								// Sets the time using the array

// These are modular functions.
void incrementTime(unsigned char * t);							// Increments time by 1 second (unused?)
void getStringTime(unsigned char * tm,unsigned char * buf);				// Gets the time in human-readable format
void DeltaTime(unsigned char * time, unsigned char * delta_to_apply, unsigned char * returnTime); // Increments time by delta_to_apply time.
void PrintTime(unsigned char * tm,int port);						// Does what it says
ReturnErr_t checkTime(unsigned char * tm);								// Returns whether or not the time is valid. (Months under 12, days under 31, etc...)	
void setTimeVectorOnly(unsigned char * t, 
						unsigned char month, unsigned char day, unsigned char year,
						 unsigned char weekday, unsigned char hour, unsigned char minute,
						 unsigned char second);	// Sets the time vector only, not the RTC.

// Returns the number of seconds between two times. 
// t1 - t2
// If t1 is more in the future than t2, result will be positive.
// If t1 is more in the past than t2, result will be negative.
long diffTime(unsigned char * t1, unsigned char * t2);

//========================================
// 		Development Functions
//========================================

#ifdef DEBUG
void TestTimeFunctions();
#endif // DEBUG

#endif // __FREDTIME__
