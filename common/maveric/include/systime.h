#ifndef __SYSTIME_H__
#define __SYSTIME_H__

#include <stdint.h>
#include <time.h>

// THIS MODULE IS NOT OBJECT ORIENTED. STATIC GLOBAL SERVICE IS INTENTIONAL

// Initializes system time service
void systime_init(uint64_t* irq_ms_ptr, struct_tm* rtc_ptr);

// Syncs ms_ticks with rtc_time
void systime_sync(void);

// Calculates current extended epoch time (milliseconds since epoch)
uint64_t systime_epoch_ms(void);

#endif
