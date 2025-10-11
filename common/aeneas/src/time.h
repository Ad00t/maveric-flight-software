#ifndef __TIME_H__
#define __TIME_H__

#ifdef LOWER_PPM
	#define ROLLOVER 	260			// Rollover for the lower (extra time given for IMI
#else
	#define ROLLOVER 	250			// Rollover for the upper (exactly 250ms).
#endif
//#define ROLLOVER 	250


#define FAST_TIMER					// Sets up the millisecond int_timer as opposed to the 100msec int timer.

// Here are the values for RunTasks:
#define SCHEDULED_TASKS 'S'
#define BACKGROUND_TASKS 'B'


#endif //__TIME_H__
