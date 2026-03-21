#ifndef __SYSTIME_H__
#define __SYSTIME_H__

#include <stdint.h>
#include <time.h>

// THIS MODULE IS NOT OBJECT ORIENTED. STATIC GLOBAL SERVICE IS INTENTIONAL

// Copy type helpers
void cp_stm_to_rtc(rtc_time_t* rtc, struct_tm* stm);
void cp_rtc_to_stm(struct_tm* stm, rtc_time_t* rtc);

// Converts a rtc time struct to unix ms time format
uint64_t rtc_to_epoch_ms(rtc_time_t rtc);

// Converts a unix ms time to an rtc time struct
void epoch_ms_to_rtc(uint64_t epoch_ms, rtc_time_t* rtc);

// Creates string representation of rtc time
void rtc_to_str(rtc_time_t* rtc, char* out);

// Initializes system time service
void systime_init(uint64_t* irq_ms_ptr, rtc_time_t* rtc_ptr);

// Syncs ms_ticks with rtc_time
void systime_sync(void);

// Calculates current extended epoch time (milliseconds since epoch)
uint64_t systime_epoch_ms(void);

#endif
