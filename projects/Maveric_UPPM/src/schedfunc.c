#include "schedfunc.h"
#include "scheduler.h"
#include "systime.h"
#include "cmdmgr.h"
#include "interrupts.h"
#include "hashtable.h"
#include <stdint.h>
#include <time.h>

#module

extern irqmgr_s irqmgr;             // Interrupts manager
extern cmdmgr_s cmdmgr;             // Commands manager
extern scheduler_s scheduler;       // Schedules manager
extern struct_tm g_rtc_time;        // Global RTC time tracking instance (from lower PPM)       

void scheduler_init_schedules(scheduler_s* scheduler) {
    scheduler_schedule_in(scheduler, 1, schedfunc_print_hk, 1000, SCHEDULE_REPS_INFINITE, 500);
    scheduler_schedule_in(scheduler, 2, schedfunc_heartbeats, 1000, SCHEDULE_REPS_INFINITE, 3000);
    scheduler_schedule_in(scheduler, 3, schedfunc_read_sensors, 1000, SCHEDULE_REPS_INFINITE, 1000);
}

// SCHEDULE FUNCTIONS
void schedfunc_print_hk(void) {
    uint64_t now = systime_epoch_ms();
    fprintf(FTDI_PORT, "%s[%s] housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u (%u)\n", KWHT, NODE_LBL, 
            g_rtc_time.tm_wday, g_rtc_time.tm_mon, g_rtc_time.tm_mday, g_rtc_time.tm_year, 
            g_rtc_time.tm_hour, g_rtc_time.tm_min, g_rtc_time.tm_sec, now);
}

void schedfunc_heartbeats(void) {

}

void schedfunc_read_sensors(void) {

}
