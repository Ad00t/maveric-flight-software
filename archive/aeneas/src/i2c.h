/* I2C Functions


   Time format is 7 bytes
   DD/MM/YY Weekday HH:MM:SS
   
   Seconds range from 0 to 59
   Minutes range from 0 to 59
   Hours range from 0 to 23
   Weekdays are as follows:
      1 -- Sunday
      2 -- Monday
      ...etc
      7 -- Saturday
	  IMPORTANT: NOTE THAT FRED'S VERSION OF TIME USES 0-6 FOR THE WEEKDAYS
   Months range from 1 to 12
   Days range from 1 to dim[month] (usually 30 or 31)
   Years range from 0 to 99
 
*/
#ifndef __I2C__
#define __I2C__





//========================================
//  			Dependencies
//========================================
#include "common.h"
#include "pins.h"


//========================================
//  			Definitions
//========================================
#use i2c(master, sda=Device_SDA, scl=Device_SCL,FORCE_SW,STREAM=I2C_STREAM)  // restart wdt


//#INT_MI2C
//void i2c_ssp()
//{
//}


//========================================
//  			Globals
//========================================
extern static unsigned char I2C_Clock_OK;

//========================================
//  			Prototypes
//========================================
void enable_FPM_RTC(unsigned char * halted_time, unsigned char * time);			// Enables the i2c driver and gets both the normal time and the time of failiure
int getI2CTime(unsigned char *buf);				// Loads a time vector with I2C time.
int setI2CTime(unsigned char *buf);				// Sets the I2C time with the given time vector
int initI2CTime(unsigned char *buf);				// Initializes the I2C time with the given time vector
int validTime(unsigned char *tm);				// Unfinished

#endif
