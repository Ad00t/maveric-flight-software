#include "systime.h"
#include <stdint.h>
#include <time.h>

#module

// MODULE PRIVATE STATIC STATE 

uint64_t _s_epoch_sec_sync;
uint64_t _s_ms_sync;
volatile uint64_t* _s_irq_ms_ptr;
struct_tm* _s_rtc_ptr;

// SYSTIME API

uint64_t rtc_to_epoch_ms(struct_tm rtc) {
    rtc.tm_year += 100; // Check mktime function in <time.c> to see why this is necessary
    rtc.tm_mon -= 1;
    return (uint64_t) mktime(&rtc) * 1000;
}

void systime_init(uint64_t* irq_ms_ptr, struct_tm* rtc_ptr) {
    _s_epoch_sec_sync = 0;
    _s_ms_sync = 0;
    _s_irq_ms_ptr = irq_ms_ptr;
    _s_rtc_ptr = rtc_ptr;
    systime_sync();
}

void systime_sync(void) {
    _s_epoch_sec_sync = rtc_to_epoch_ms(*_s_rtc_ptr);
    isr_disable_all();
    _s_ms_sync = *_s_irq_ms_ptr;
    isr_enable_all();
}


uint64_t systime_epoch_ms(void) {
    isr_disable_all();
    uint64_t ms_now = *_s_irq_ms_ptr;
    isr_enable_all();
    uint64_t diff = ms_now - _s_ms_sync; // THIS SEPARATION OF ARITHMETIC STEPS IS NECESSARY.
    uint64_t now = _s_epoch_sec_sync + diff;
    return now;
}
