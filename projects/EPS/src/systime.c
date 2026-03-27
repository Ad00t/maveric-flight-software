#include "systime.h"
#include <stdint.h>
#include <time.h>

#module

// MODULE PRIVATE STATIC STATE 

uint64_t _s_epoch_sec_sync;
uint64_t _s_ms_sync;
volatile uint64_t* _s_irq_ms_ptr;
volatile rtc_time_t* _s_rtc_ptr;

// PUBLIC HELPERS

void cp_stm_to_rtc(rtc_time_t* rtc, struct_tm* stm) {
    rtc->tm_wday = stm->tm_wday;
    rtc->tm_mon = stm->tm_mon;
    rtc->tm_mday = stm->tm_mday;
    rtc->tm_year = stm->tm_year;
    rtc->tm_hour = stm->tm_hour;
    rtc->tm_min = stm->tm_min;
    rtc->tm_sec = stm->tm_sec;
}

void cp_rtc_to_stm(struct_tm* stm, rtc_time_t* rtc) {
    stm->tm_wday = rtc->tm_wday;
    stm->tm_mon = rtc->tm_mon;
    stm->tm_mday = rtc->tm_mday;
    stm->tm_year = rtc->tm_year;
    stm->tm_hour = rtc->tm_hour;
    stm->tm_min = rtc->tm_min;
    stm->tm_sec = rtc->tm_sec;
}

uint64_t rtc_to_epoch_ms(rtc_time_t* rtc) {
    struct_tm stm;
    cp_rtc_to_stm(&stm, rtc);
    stm.tm_year += 100; // Check mktime function in <time.c> to see why this is necessary
    stm.tm_mon -= 1;
    return (uint64_t) mktime(&stm) * 1000;
}

void epoch_ms_to_rtc(uint64_t epoch_ms, rtc_time_t* rtc) {
    struct_tm stm;
    time_t epoch_time = epoch_ms / 1000;
    memcpy(&stm, localtime(&epoch_time), sizeof(struct_tm));
    stm.tm_year -= 100; // Check localtime function in <time.c> to see why this is necessary
    stm.tm_mon += 1;
    stm.tm_wday -= 1;
    cp_stm_to_rtc(rtc, &stm);
}

void rtc_to_str(rtc_time_t rtc, char* out) {
    sprintf(out, "%u %u %u %u %u %u %u", rtc.tm_wday, rtc.tm_mon, rtc.tm_mday, rtc.tm_year, rtc.tm_hour, rtc.tm_min, rtc.tm_sec);
}

// SYSTIME API

void systime_init(uint64_t* irq_ms_ptr, rtc_time_t* rtc_ptr) {
    _s_epoch_sec_sync = 0;
    _s_ms_sync = 0;
    _s_irq_ms_ptr = irq_ms_ptr;
    _s_rtc_ptr = rtc_ptr;
    setup_rtc(RTC_ENABLE | RTC_OUTPUT_SECONDS, 0);
    rtc_write(rtc_ptr);
    systime_sync();
}

void systime_sync(void) {
    _s_epoch_sec_sync = rtc_to_epoch_ms(_s_rtc_ptr);
    disable_interrupts(INT_TIMER1);
    _s_ms_sync = *_s_irq_ms_ptr;
    enable_interrupts(INT_TIMER1);
}


uint64_t systime_epoch_ms(void) {
    disable_interrupts(INT_TIMER1);
    uint64_t ms_now = *_s_irq_ms_ptr;
    enable_interrupts(INT_TIMER1);
    uint64_t diff = ms_now - _s_ms_sync; // THIS SEPARATION OF ARITHMETIC STEPS IS NECESSARY.
    uint64_t now = _s_epoch_sec_sync + diff;
    return now;
}
