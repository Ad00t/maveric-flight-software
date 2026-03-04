// UPPER PPM HOUSEKEEPING

#include "housekeeping.h"
#include "systime.h"
#include "interrupts.h"
#include "hashtable.h"
#include "common.h"
#include "scheduler.h"
#include "cmdmgr.h"
#include "ax100.h"
#include <stdint.h>
#include <time.h>
#include <string.h>

#module

extern irqmgr_s g_irqmgr;           // Interrupts manager
extern cmdmgr_s g_cmdmgr;           // Commands manager
extern scheduler_s g_scheduler;     // Schedules manager
extern struct_tm g_rtc_time;        // Global RTC time tracking instance (from lower PPM)       
extern ax100_s g_ax100;             // AX100 transceiver driver 

void hk_init() {
    scheduler_schedule_func_in(&g_scheduler, 0, hk_log, 2000, 500, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 1, hk_heartbeats, 2000, 3000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 2, hk_read_sensors, 2000, 1000, SCHEDULE_REPS_INFINITE);
    // scheduler_schedule_func_in(&g_scheduler, 3, hk_test_ax100, 2000, 3000, SCHEDULE_REPS_INFINITE);
}

// SCHEDULE FUNCTIONS
void hk_log(void) {
    sprintf(LOGBUF, "housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u", 
            g_rtc_time.tm_wday, g_rtc_time.tm_mon, g_rtc_time.tm_mday, g_rtc_time.tm_year, 
            g_rtc_time.tm_hour, g_rtc_time.tm_min, g_rtc_time.tm_sec); log_flush(LL_INFO);
}

void hk_heartbeats(void) {

}

void hk_read_sensors(void) {

}

void hk_test_ax100(void) {
    char test[] = "test";
    ax100_transmit_frame(&g_ax100, (uint8_t*) test, strlen(test));
}
