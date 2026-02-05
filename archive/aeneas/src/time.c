#include "time.h"

// These will all eventually go into time.h and time.c ---------------
// TODO: Create time module.  Be sure to include timer stuff in Init_Device()
unsigned char RunTasks;				// 
int t_sec=0;				// 1/10 second timer (rolls over)
unsigned long SEC_TIMER=0;	// Second timer (unsigned long = int32.  Rolls over at 4,294,967,295 seconds, which is 136.102208 years)
unsigned int64 MSEC=0;		// Millisecond timer (not super accurate!)


int d_cnt=ROLLOVER;


#int_timer1 // Global timer 
void rtc_isr()
{
	MSEC++;
	d_cnt--;
	if(d_cnt>0) return;
	d_cnt=ROLLOVER;								// NUmber of ms to consider 1 "interval"
	t_sec++; 									// interval timer
   	RunTasks = SCHEDULED_TASKS;						// Set RunTask flag.. This flag is used in the main function
    if (t_sec > 3) 								// Intervals in a second (1 less than actual)
    {   
        t_sec = 0; 								// Reset
    	SEC_TIMER++;							// Increment the 1-second timer.   Rolls over every 136 years.  Resets on reboot.
    }
//	fputc('T',COM_B);
}

// Pin-triggered Interrupt
#int_EXT1
void int_ext1()
{
	t_sec++; 									// interval timer
   	RunTasks = SCHEDULED_TASKS; 					// Set RunTask flag.. This flag is used in the main function
    if (t_sec > 3) 								// Intervals in a second (1 less than actual)
    {   
        t_sec = 0; 								// Reset
    	SEC_TIMER++;							// Increment the 1-second timer.   Rolls over every 136 years.  Resets on reboot.
    }
	d_cnt=ROLLOVER;								// Reset the backup timer
//	fputc('I',COM_B);
}
