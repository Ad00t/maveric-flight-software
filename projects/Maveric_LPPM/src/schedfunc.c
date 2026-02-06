#include "schedfunc.h"
#include "scheduler.h"
#include "systime.h"
#include "cmdmgr.h"
#include "interrupts.h"
#include "hashtable.h"
#include "adcsmtq.h"
#include "m41t81s.h"
#include "adis16260.h"
#include "naviguider.h"
#include <stdint.h>

#module

extern irqmgr_s irqmgr;             // Interrupts manager
extern cmdmgr_s cmdmgr;             // Commands manager
extern scheduler_s scheduler;       // Schedules manager
extern ertc_s ertc;                 // External RTC (on motherboard)
extern mtq_s mtq;                   // Magnetorquer
extern gyro_s gyro;                 // Gyroscope (x3)
extern nvg_s nvg;                   // Naviguider

void scheduler_init_schedules(scheduler_s* scheduler) {
    scheduler_schedule_in(scheduler, 1, schedfunc_get_ertc_time, 1000, SCHEDULE_REPS_INFINITE, 500);
    scheduler_schedule_in(scheduler, 2, schedfunc_systime_sync, 1000, SCHEDULE_REPS_INFINITE, 5000);
    scheduler_schedule_in(scheduler, 3, schedfunc_print_hk, 1000, SCHEDULE_REPS_INFINITE, 1000);
    scheduler_schedule_in(scheduler, 4, schedfunc_heartbeats, 1000, SCHEDULE_REPS_INFINITE, 3000);
}

// SCHEDULE FUNCTIONS

void schedfunc_get_ertc_time(void) {
    ertc_get_time(&ertc); 
}

void schedfunc_systime_sync(void) {
    systime_sync();
}

void schedfunc_print_hk(void) {
    uint64_t now = systime_epoch_ms();
    fprintf(COM_D, "%s[%s] housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u (%u)\n", KWHT, NODE_LBL, 
            ertc.time.tm_wday, ertc.time.tm_mon, ertc.time.tm_mday, ertc.time.tm_year, 
            ertc.time.tm_hour, ertc.time.tm_min, ertc.time.tm_sec, now);
}

void schedfunc_heartbeats(void) {
    int1 hb_ertc = ertc_heartbeat(&ertc);
    // int1 hb_mtq = mtq_heartbeat(&mtq);
    // int1 hb_nvg = nvg_heartbeat(&nvg);
    // int1 hb_gyro = gyro_heartbeat(&gyro);
}

