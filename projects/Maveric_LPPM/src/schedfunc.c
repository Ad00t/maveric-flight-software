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

extern irqmgr_s g_irqmgr;             // Interrupts manager
extern cmdmgr_s g_cmdmgr;             // Commands manager
extern scheduler_s g_scheduler;       // Schedules manager
extern ertc_s g_ertc;                 // External RTC (on motherboard)
extern mtq_s g_mtq;                   // Magnetorquer
extern gyro_s g_gyro;                 // Gyroscope (x3)
extern nvg_s g_nvg;                   // Naviguider

void scheduler_init_schedules(scheduler_s* scheduler) {
    scheduler_schedule_in(scheduler, 1, schedfunc_get_ertc_time, 1000, SCHEDULE_REPS_INFINITE, 500);
    scheduler_schedule_in(scheduler, 2, schedfunc_systime_sync, 1000, SCHEDULE_REPS_INFINITE, 5000);
    scheduler_schedule_in(scheduler, 3, schedfunc_print_hk, 1000, SCHEDULE_REPS_INFINITE, 500);
    scheduler_schedule_in(scheduler, 4, schedfunc_heartbeats, 1000, SCHEDULE_REPS_INFINITE, 3000);
    scheduler_schedule_in(scheduler, 5, schedfunc_read_sensors, 1000, SCHEDULE_REPS_INFINITE, 1000);
    // scheduler_schedule_in(scheduler, 6, schedfunc_test_disable_ertc, 15000, 1, 0);
}

// SCHEDULE FUNCTIONS
void schedfunc_get_ertc_time(void) {
    ertc_get_time(&g_ertc); 
}

void schedfunc_systime_sync(void) {
    systime_sync();
}

void schedfunc_print_hk(void) {
    uint64_t now = systime_epoch_ms();
    fprintf(FTDI_PORT, "%s[%s] housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u (%u) ertc=%u\n", KWHT, NODE_LBL, 
            g_ertc.time.tm_wday, g_ertc.time.tm_mon, g_ertc.time.tm_mday, g_ertc.time.tm_year, 
            g_ertc.time.tm_hour, g_ertc.time.tm_min, g_ertc.time.tm_sec, now, g_ertc.is_using_ertc);
}

void schedfunc_heartbeats(void) {
    int1 hb_ertc = ertc_heartbeat(&g_ertc);
    // int1 hb_mtq = mtq_heartbeat(&g_mtq);
    // int1 hb_nvg = nvg_heartbeat(&g_nvg);
    // int1 hb_gyro = gyro_heartbeat(&g_gyro);
}

void schedfunc_read_sensors(void) {
    mtq_read_ctrl(&g_mtq);
    // mtq_read_fast(&g_mtq);
    // gyro_read_all(&g_gyro);
}

void schedfunc_test_disable_ertc(void) {
    g_ertc.is_using_ertc = FALSE;
} 

