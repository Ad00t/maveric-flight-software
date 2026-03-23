// LOWER PPM HOUSEKEEPING

#include "housekeeping.h"
#include "common.h"
#include "scheduler.h"
#include "systime.h"
#include "cmdpkt.h"
#include "cmdmgr.h"
#include "flashmgr.h"
#include "interrupts.h"
#include "hashtable.h"
#include "logger.h"
#include "adcsmtq.h"
#include "m41t81s.h"
#include "adis16260.h"
#include "naviguider.h"
#include "globals.h"
#include <stdint.h>

#module

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
    char req[CMD_MAX_ARGS_LEN] = {0};
    rtc_time_t rtc;
    epoch_ms_to_rtc(systime_epoch_ms(), &rtc);  
    rtc_to_str(&rtc, req);
    cmd_dispatch(NODE, NODE_UPPM, 0, REQ, "ppm_set_time", req);
}

void hk_log(void) {
    sprintf(LOGBUF, "housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u ertc=%u", 
            g_ertc.time.tm_wday, g_ertc.time.tm_mon, g_ertc.time.tm_mday, g_ertc.time.tm_year, 
            g_ertc.time.tm_hour, g_ertc.time.tm_min, g_ertc.time.tm_sec, g_ertc.is_using_ertc); log_info();
}

void hk_heartbeats(void) {
    // status_e hb_ertc = ertc_heartbeat(&g_ertc);
    // status_e hb_mtq = mtq_heartbeat(&g_mtq);
    // status_e hb_nvg = nvg_heartbeat(&g_nvg);
    // status_e hb_gyro = gyro_heartbeat(&g_gyro);
    // sprintf(LOGBUF, "heartbeats: %u %u %u %", hb_ertc, hb_mtq, hb_nvg, /* hb_gyro */ 0);
}

void hk_read_sensors(void) {
    // mtq_read_ctrl(&g_mtq);
    // mtq_read_fast(&g_mtq);
    // gyro_read_all(&g_gyro);
}

void hk_test_disable_ertc(void) {
    g_ertc.is_using_ertc = FALSE;
} 

