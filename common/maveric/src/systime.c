#include "systime.h"
#include <stdint.h>
#include <time.h>

#module

// MODULE PRIVATE STATIC STATE 

static uint64_t s_epoch_sec_sync;
static uint64_t s_ms_sync;
static volatile uint64_t* s_irq_ms_ptr;
static struct_tm* s_rtc_ptr;

// SYSTIME API

void systime_init(uint64_t* irq_ms_ptr, struct_tm* rtc_ptr) {
    s_epoch_sec_sync = 0;
    s_ms_sync = 0;
    s_irq_ms_ptr = irq_ms_ptr;
    s_rtc_ptr = rtc_ptr;
    systime_sync();
}

void systime_sync(void) {
    struct_tm tmp;
    memcpy(&tmp, s_rtc_ptr, sizeof(struct_tm));
    tmp.tm_year += 100; // Check mktime function in <time.c> to see why this is necessary
    tmp.tm_mon -= 1;
    s_epoch_sec_sync = (uint64_t) mktime(&tmp);
    isr_disable_all();
    s_ms_sync = *s_irq_ms_ptr;
    isr_enable_all();
}


uint64_t systime_epoch_ms(void) {
    isr_disable_all();
    uint64_t ms_now = *s_irq_ms_ptr;
    isr_enable_all();
    return s_epoch_sec_sync * 1000 + (ms_now - s_ms_sync);
}
