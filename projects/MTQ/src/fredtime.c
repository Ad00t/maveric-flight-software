/***************************************************************************

									fredtime.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne

Purpose: Provide functions for Fred's (iControl's) nonstandard time vector.

Changelog:

Date 	 |  Au.  |	Notes
10-10-11   mra		Initial revision.		

****************************************************************************
*/


#include <fredtime.h>		// Public header

#module					// !!! Important: This command makes everything below scoped only to this file.

//======================================
//				Defines
//======================================
	// Temporary defines for ease of reading code.
	#define SEC 6
	#define MIN 5
	#define HOUR 4
	#define WKDAY 3
	#define YEAR 2
	#define DAY 1
	#define MONTH 0

//========================================
// 			Driver Functions
//========================================
void incrementTime(unsigned char * t)
{
	unsigned char dim[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	unsigned char tzero;

    if (t[0] == 2 && (t[2] % 4 == 0))dim[1] = 29;  
    else dim[1] = 28; 			//take care of leap year      //?? Code will not work correctly in year 2100

	// Add a second
	t[6]++;

	// Check for Rollover    
	if (t[6] > 59) 		// Seconds Rollover	
   	{
       t[5]++; 			// increment minute
       t[6] = 0;		
   	}
   	if (t[5] > 59)    // Minutes Rollover
	{
       t[5] = 0;
       t[4]++;		// increment hour
   } 
   if (t[4] > 23)     // Hours Rollover
   {
       t[4] = 0;	   
       t[1]++;	    // Increment Day
	   t[3]++;		// Increment Weekday
   } 
   tzero = t[0];
   if (t[1] > dim[tzero - 1]) 	// Rollover Day
   {
       t[1] = 1; 				// Reset Day
       t[0]++; 					// Increment Month
   }
   if (t[3] > 7) 		// Rollover Weekday
   {
       t[3] = 1; 		// Reset Weekday
   }
   if (t[0] > 12) 		// Rollover Month
   {			
       t[0] = 1;		// Reset Month		
       t[2]++;			// Increment Year
   }

}


ReturnErr_t checkTime(unsigned char * t)
{
	//ReturnErr_t ret;
	unsigned char dim[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };	// days in month
	unsigned char tzero;

	tzero = t[0];

	// Check for Rollover    
	if (t[6] > 59) return FAILURE; 						// Bad seconds
   	if (t[5] > 59) return FAILURE;   					// bad minutes
	if (t[4] > 23) return FAILURE;        				// bad hours
	if (t[1] > dim[tzero - 1] || t[1]<1) return FAILURE; // bad day
	if (t[0] > 12 || t[0] < 1) return FAILURE; 			// bad month
	if (t[2] > 20) return FAILURE;						// bad year

	return SUCCESS;

}


// Applies a positive delta to any time
void DeltaTime(unsigned char * t, unsigned char * d, unsigned char * returnTime)
{
	// d is the delta to apply
	int temp;
	unsigned char bug_buster;		// Variable used to fight compiler bugs!

	unsigned char dim[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if ((t[2] % 4 == 0))dim[2] = 29;   //take care of leap year      //?? Code will not work correctly in year 2100  

	// Time format:
	// time[7]
	// MM DD YY WKday HH MM SS
	// 0   1  2   3    4  5  6

	#ifdef DEBUG

	sendDBGALL(USER_PORT,"\r\nCurrent Time: ");
	PrintTime(t,USER_PORT);

	sendDBGALL(USER_PORT,"\r\nDelta Time  : ");
	PrintTime(d,USER_PORT);

	#endif
	
	// Initialize the Month and Year
	returnTime[MONTH] = t[MONTH];	// Start by setting the returnTime to the "current" month
	returnTime[YEAR] = t[YEAR];	// Start by setting the returnTime to the "current" month

	//Add the seconds
	temp = t[SEC] + d[SEC];
	while(temp>=60) 
	{
		d[MIN]++;
		temp-=60;
	}
	// Store the remaining seconds
	returnTime[SEC] = temp;

	//Add the minutes
	temp = t[MIN] + d[MIN];
	while(temp>=60) 
	{
		d[HOUR]++;
		temp-=60;
	}
	// Store the remaining minutes
	returnTime[MIN] = temp;

	//Add the hours. 
	temp = t[HOUR] + d[HOUR];
	while(temp>=24) 
	{
		d[DAY]++;	// increment day
		d[WKDAY]++; // increment weekday
		temp-=24;	// Decrement temp
	}
	// Store the remaining hours
	returnTime[HOUR] = temp;

	//Add the days.  Be aware that the rollover condition is particularly hard, because it's based on the month during 
	// which calculations are taking place.
	temp = t[DAY] + d[DAY];

	// Adjust the weekday
	returnTime[WKDAY] = (t[WKDAY] + (d[DAY] % 7)) % 7;

	bug_buster = returnTime[MONTH];
	while(temp>dim[bug_buster])	 // As long as the days are greater than the current days in the month
	{
		// Rollover months
		temp-=dim[bug_buster]; // Decrement the day counter
		returnTime[MONTH]++;			// Increment month (not the delta month, the actual month)

		// Check for year rollover to avoid dim[] array overflow
		if(returnTime[MONTH]>12) { 
			returnTime[MONTH]=1;
			returnTime[YEAR]++;
		    if ((returnTime[YEAR] % 4 == 0))dim[2] = 29;   // Recheck for leapyear
			}
		bug_buster = returnTime[MONTH];  
	}
	// Store the remaining days
	returnTime[DAY] = temp;

	//Add the months. 
	temp = returnTime[MONTH] + d[MONTH];
	while(temp>12) 
	{
		// Rollover years
		temp-=12; 			// Decrement temp
		d[YEAR]++;			// Increment year
	}
	// Store the remaining months
	returnTime[MONTH] = temp;	

	//Add the years
	temp = returnTime[YEAR] + d[YEAR];
	
	// Store it
	returnTime[YEAR] = temp;			

	#ifdef DEBUG

	sendDBGALL(USER_PORT,"\r\nFuture Time : ");
	PrintTime(returnTime,USER_PORT);

	#endif

	return;

}

// RegEx Test
// test[anythn][anything]
// test[anythn[asdf]][anything]
// test[anythn[asdf]]
// test[anythn[0]]





// Converts a time vector to "Seconds from Jan 1, 2000, 00:00:00"
long timeToSeconds(unsigned char * t)
{

	const long SEC_IN_YEAR[21]={0,	31622400,	63158400,	94694400,	126230400,	157852800,	189388800,	220924800,	252460800,	284083200,	315619200,	347155200,	378691200,	410313600,	441849600,	473385600,	504921600,	536544000,	568080000,	599616000,	631152000};	// Seconds from 2000 in each year, 2000, 2001, 2002, etc... up to 2020.
	const long SEC_IN_MONTH[13]={0, 0,	2678400,	5097600,	7776000,	10368000,	13046400,	15638400,	18316800,	20995200,	23587200,	26265600,	28857600};
	const long SEC_IN_MONTH_LEAP[13]={0, 0,	2678400,	5184000,	7862400,	10454400,	13132800,	15724800,	18403200,	21081600,	23673600,	26352000,	28944000};
	const long SEC_IN_DAY=86400;
	const long SEC_IN_HOUR=3600;
	const long SEC_IN_MINUTE=60;

	long retval=0;
	unsigned char bug_buster;	// used to fight compiler bugs with nested arrays

	//Debug
//	sendDBGALL(USER_PORT,"A"); delay_ms(100);

	// Check for overflow
	if(t[YEAR]>20 || t[MONTH]>12) return 0;	// Bail wtih a zero (not the best way to do this)

//	sendDBGALL(USER_PORT,"B"); delay_ms(100);

	bug_buster = t[YEAR];
	retval = SEC_IN_YEAR[bug_buster];	// Start with years

	// Add months
    if ((t[YEAR] % 4 == 0))
	{
		bug_buster=t[MONTH];
		retval = retval + SEC_IN_MONTH_LEAP[bug_buster];
	}
	else
	{
		bug_buster=t[MONTH];
		retval = retval + SEC_IN_MONTH[bug_buster];
	}

//	sendDBGALL(USER_PORT,"C"); delay_ms(100);


	// Add days
	retval = retval + SEC_IN_DAY*(t[DAY]-1);

	// Add hours, minutes, seconds
	retval = retval + SEC_IN_HOUR*(long)t[HOUR] +
					  SEC_IN_MINUTE * (long)t[MIN] +
					  (long)t[SEC];

//	sendDBGALL(USER_PORT,"D"); delay_ms(100);

	return retval;
}


// Returns the number of seconds between two times. 
// t1 - t2
// If t1 is more in the future than t2, result will be positive.
// If t1 is more in the past than t2, result will be negative.
long diffTime(unsigned char * t1, unsigned char * t2)
{
	long a,b;	

	a = timeToSeconds(t1);
	b = timeToSeconds(t2);

//	sprintf(dbgbuf,"\r\nTime 1=%li\r\nTime 2=%li\r\nDelta=%li",a,b,a-b);

//	sendDBGALL(USER_PORT,dbgbuf);

	return a-b;

}





// Overloaded getTime() to use RTC
void getTime(unsigned char * t,rtc_time_t * RTC)
{
	// Read the Real Time Clock
	rtc_read(RTC);
	
	// Set our version of the time based on the RTC read
	t[0] = RTC->tm_mon;			// Month
	t[1] = RTC->tm_mday;			// Day
	t[2] = RTC->tm_year;			// Year
	t[3] = RTC->tm_wday;			// Weekday
	t[4] = RTC->tm_hour;			// Hour
	t[5] = RTC->tm_min;			// Minutes
	t[6] = RTC->tm_sec;			// Seconds
}


void setTime(unsigned char month, unsigned char day, unsigned char year, unsigned char weekday, unsigned char hour, unsigned char minute, unsigned char second)
{
   //int monthoffsets[]={0,3,3,6,1,4,6,2,5,0,3,5};
   rtc_time_t R;

   R.tm_mon = month;
   R.tm_mday = day;
   R.tm_year = year;
   R.tm_hour = hour;
   R.tm_min = minute;
   R.tm_sec = second;
   R.tm_wday = weekday;
	
   rtc_write(&R);

   //R.tm_wday = (5 + monthoffsets[month-1] + (year+year/4)%7 + day%7)%7;		// Weekday calculation. Valid for 21st century only. (i.e. 20xx)
   //if(year%4==0 && month<=2 && day <=28) R.tm_wday -= 1;					// Take a day off in leap years if before Feb 29.
   //if(R.tm_wday<0) R.tm_wday = 7;											// Take a day off in leap years if before Feb 29.
}

void setTime(unsigned char * t)
{
   rtc_time_t R;

   R.tm_mon = t[0];
   R.tm_mday =  t[1];
   R.tm_year =  t[2];
   R.tm_hour =  t[4];
   R.tm_min =  t[5];
   R.tm_sec =  t[6];
   R.tm_wday =  t[3];

   rtc_write(&R);
}


void setTimeVectorOnly(unsigned char * t, unsigned char month, unsigned char day, unsigned char year, unsigned char weekday, unsigned char hour, unsigned char minute, unsigned char second)
{
	t[0]=month;
	t[1]=day;
	t[2]=year;
	t[3]=weekday;
	t[4]=hour;
	t[5]=minute;
	t[6]=second;

}

void PrintTime(unsigned char * tm,int port)
{
	unsigned char stime[30];
    sprintf(stime, "%02u/%02u/%02u %u %02u:%02u:%02u", tm[0], tm[1], tm[2], tm[3], tm[4], tm[5], tm[6]);
	sendDBGALL(port,stime);
}

void getStringTime(unsigned char * tm,unsigned char * buf)
{
    sprintf(buf, "%02u/%02u/%02u %01u %02u:%02u:%02u", tm[0], tm[1], tm[2], tm[3], tm[4], tm[5], tm[6]);
}


#ifdef DEBUG



// This is a testing function to fully check out the DeltaTime() function, which is critically
// important for launch and early ops.
// Testing data comes from the Excel file:
// DeltaTime Tests.xls
// located on the SVN server under [SVN]\aeneas\Analysis\Test by ANalysis
void TestTimeFunctions()
{
	int i;	
	unsigned char r_time[7];

	sendDBGALL(USER_PORT,"\r\n\n\n");
	sendDBGALL(USER_PORT,"\r\nBegin Test of the FredTime Library");
	sendDBGALL(USER_PORT,"\r\n");

	// Test inputs
	#define NUMTEST	27
	unsigned char c_time[NUMTEST][7]={	
10,10,11,1,13,48,30,
1,1,12,0,1,1,50,
1,1,12,0,1,1,50,
1,1,12,0,1,50,1,
1,1,12,0,1,40,1,
1,1,12,0,22,1,1,
1,1,12,0,1,1,1,
1,28,12,6,1,1,1,
2,28,12,2,1,1,1,
3,28,12,3,1,1,1,
4,28,12,6,1,1,1,
5,28,12,1,1,1,1,
6,28,12,4,1,1,1,
7,28,12,6,1,1,1,
8,28,12,2,1,1,1,
9,28,12,5,1,1,1,
10,28,12,0,1,1,1,
11,28,12,3,1,1,1,
12,28,12,5,1,1,1,
6,25,12,1,1,59,55,
6,25,12,1,23,59,55,
6,30,12,6,23,59,55,
12,31,12,1,23,59,55,
6,30,12,6,23,59,55,
6,30,12,6,23,59,55,
6,30,12,6,23,59,55,
6,30,12,6,23,59,55
};

	unsigned char d_time[NUMTEST][7]={	
0,0,0,0,0,0,20,
0,0,0,0,0,0,20,
0,0,0,0,0,0,200,
0,0,0,0,0,20,0,
0,0,0,0,0,85,0,
0,0,0,0,4,0,0,
0,72,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,5,0,0,0,0,0,
0,0,0,0,0,0,10,
0,0,0,0,0,0,10,
0,0,0,0,0,0,10,
0,0,0,0,0,0,10,
0,0,0,0,0,45,0,
0,0,0,0,0,45,0,
0,0,0,0,0,45,0,
0,0,0,0,0,45,0};

	unsigned char check_time[NUMTEST][7]={
10,10,11,1,13,48,50,
1,1,12,0,1,2,10,
1,1,12,0,1,5,10,
1,1,12,0,2,10,1,
1,1,12,0,3,5,1,
1,2,12,1,2,1,1,
3,13,12,2,1,1,1,
2,2,12,4,1,1,1,
3,4,12,0,1,1,1,
4,2,12,1,1,1,1,
5,3,12,4,1,1,1,
6,2,12,6,1,1,1,
7,3,12,2,1,1,1,
8,2,12,4,1,1,1,
9,2,12,0,1,1,1,
10,3,12,3,1,1,1,
11,2,12,5,1,1,1,
12,3,12,1,1,1,1,
1,2,13,3,1,1,1,
6,25,12,1,2,0,5,
6,26,12,2,0,0,5,
7,1,12,0,0,0,5,
1,1,13,2,0,0,5,
7,1,12,0,0,44,55,
7,1,12,0,0,44,55,
7,1,12,0,0,44,55,
7,1,12,0,0,44,55
};


	sendDBGALL(USER_PORT,"\r\nThe following pairs of times should match:");
	for(i=0;i<NUMTEST;i++)
	{
		DeltaTime(&c_time[i][0],&d_time[i][0],r_time);
		sendDBGALL(USER_PORT,"\r\nReal Answer : ");
		PrintTime(&check_time[i][0],USER_PORT);
		sendDBGALL(USER_PORT,"\r\nResult      : ");
		if((memcmp(r_time, &check_time[i][0], 7)==0))
		{
			sendDBGALL(USER_PORT,"PASSED");
		}
		else
		{
			sendDBGALL(USER_PORT,"FAILED");
		}
		sendDBGALL(USER_PORT,"\r\n");
	
	}

}

	#undef SEC
	#undef MIN
	#undef HOUR
	#undef WKDAY
	#undef YEAR
	#undef DAY
	#undef MONTH


#endif

