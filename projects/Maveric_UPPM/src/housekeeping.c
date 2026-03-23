// UPPER PPM HOUSEKEEPING

#include "housekeeping.h"
#include "systime.h"
#include "interrupts.h"
#include "hashtable.h"
#include "common.h"
#include "scheduler.h"
#include "cmdmgr.h"
#include "ax100.h"
#include "globals.h"
#include <stdint.h>
#include <time.h>
#include <string.h>

#module

void hk_init(void) {
    // IMPORTANT: AT LEAST ONE SCHEDULE FUNCTION MUST BE ACTIVE OR YOU WILL GET A SCHEDULER ERROR
    scheduler_schedule_func_in(&g_scheduler, 0, hk_get_rtc_time, 2000, 500, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 1, hk_log, 2500, 500, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 2, hk_update_tlm, 5000, 5000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 3, hk_tlm_beacon, 7000, 7000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 4, hk_heartbeats, 4000, 3000, SCHEDULE_REPS_INFINITE);
    // scheduler_schedule_func_in(&g_scheduler, 4, hk_test_ax100, 2000, 3000, SCHEDULE_REPS_INFINITE);
}

// HOUSEKEEPING FUNCTIONS

void hk_get_rtc_time(void) {
    // rtc_read(&g_rtc_time); // This doesn't work on UPPM
    epoch_ms_to_rtc(systime_epoch_ms(), &g_rtc_time);  
}

void hk_log(void) {
    sprintf(LOGBUF, "housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u", 
            g_rtc_time.tm_wday, g_rtc_time.tm_mon, g_rtc_time.tm_mday, g_rtc_time.tm_year, 
            g_rtc_time.tm_hour, g_rtc_time.tm_min, g_rtc_time.tm_sec); log_info();
}

void hk_update_tlm(void) {
    cmd_dispatch(NODE, NODE_LPPM, 0, REQ, "tlm_get_data", "");
    cmd_dispatch(NODE, NODE_EPS, 0, REQ, "tlm_get_data", "");
    cmd_dispatch(NODE, NODE_HOLONAV, 0, REQ, "tlm_get_data", "");
    cmd_dispatch(NODE, NODE_ASTROBOARD, 0, REQ, "tlm_get_data", "");
}

void hk_tlm_beacon(void) {
    tlm_beacon(&g_tlm, 1);
}

void hk_heartbeats(void) {

}

void hk_test_ax100(void) {
    cmd_dispatch(NODE, NODE_GS, 0, REQ, "hello", "world");
}
