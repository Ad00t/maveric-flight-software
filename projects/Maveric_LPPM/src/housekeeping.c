// LOWER PPM HOUSEKEEPING

#include "housekeeping.h"
#include "scheduler.h"
#include "systime.h"
#include "cmdpkt.h"
#include "cmdmgr.h"
#include "interrupts.h"
#include "hashtable.h"
#include "mtq.h"
#include "ertc.h"
#include "gyro.h"
#include "naviguider.h"
#include "logger.h"
#include <stdint.h>

#module

extern irqmgr_s g_irqmgr;             // Interrupts manager
extern cmdmgr_s g_cmdmgr;             // Commands manager
extern scheduler_s g_scheduler;       // Schedules manager
extern ertc_s g_ertc;                 // External RTC (on motherboard)
extern mtq_s g_mtq;                   // Magnetorquer
extern gyro_s g_gyro;                 // Gyroscope (x3)
extern nvg_s g_nvg;                   // Naviguider

void hk_init(void) {
    // IMPORTANT: AT LEAST ONE SCHEDULE FUNCTION MUST BE ACTIVE OR YOU WILL GET A SCHEDULER ERROR
    scheduler_schedule_func_in(&g_scheduler, 0, hk_get_ertc_time, 2000, 500, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 1, hk_log, 2500, 500, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 2, hk_systime_sync, 5000, 30000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 3, hk_heartbeats, 4000, 3000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 4, hk_read_sensors, 3000, 1000, SCHEDULE_REPS_INFINITE);
    // scheduler_schedule_func_in(&g_scheduler, 6, hk_test_disable_ertc, 15000, 0, 1);
}

// HOUSEKEEPING FUNCTIONS

void hk_get_ertc_time(void) {
    ertc_get_time(&g_ertc); 
}

void hk_systime_sync(void) {
    systime_sync();
    char tm_str[32] = {0};
    rtc_to_str(tm_str, g_ertc.time);
    cmd_dispatch(NODE, NODE_UPPM, 0, REQ, "ppm_set_time", tm_str);
}

void hk_log(void) {
    sprintf(LOGBUF, "housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u ertc=%u", 
            g_ertc.time.tm_wday, g_ertc.time.tm_mon, g_ertc.time.tm_mday, g_ertc.time.tm_year, 
            g_ertc.time.tm_hour, g_ertc.time.tm_min, g_ertc.time.tm_sec, g_ertc.is_using_ertc); log_info();
}

void hk_heartbeats(void) {
    // int1 hb_ertc = ertc_heartbeat(&g_ertc);
    int1 hb_mtq = mtq_heartbeat(&g_mtq);
    int1 hb_nvg = nvg_heartbeat(&g_nvg);
    // int1 hb_gyro = gyro_heartbeat(&g_gyro);
}

void hk_read_sensors(void) {
    mtq_read_ctrl(&g_mtq);
    // mtq_read_fast(&g_mtq);
    // gyro_read_all(&g_gyro);
}

void hk_test_disable_ertc(void) {
    g_ertc.is_using_ertc = FALSE;
} 

