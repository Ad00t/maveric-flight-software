// UPPER PPM HOUSEKEEPING

#include "housekeeping.h"
#include "systime.h"
#include "interrupts.h"
#include "hashtable.h"
#include "common.h"
#include "scheduler.h"
#include "mcppkt.h"
#include "mcpmgr.h"
#include "ax100.h"
#include "pldmgr.h"
#include "globals.h"
#include <stdint.h>
#include <time.h>
#include <string.h>

#module

void hk_init(void) {
    // IMPORTANT: AT LEAST ONE SCHEDULE FUNCTION MUST BE ACTIVE OR YOU WILL GET A SCHEDULER ERROR
    scheduler_schedule_func_in(&g_scheduler, 0, hk_get_rtc_time, 2000, 500, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 1, hk_log, 2500, 1000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 2, hk_update_tlm, 5000, 10000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, SCHED_ID_PPM_RST, hk_ppm_reset, 4*MS_PER_MIN, 0, 1);     // 120 min
}

// HOUSEKEEPING FUNCTIONS

void hk_get_rtc_time(void) {
    // rtc_read(&g_rtc_time); // This doesn't work on UPPM
    systime_rtc(&g_rtc_time);
}

void hk_log(void) {
    rtc_time_t rtc;
    systime_rtc(&rtc);
    sprintf(LOGBUF, "housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u", 
            rtc.tm_wday, rtc.tm_mon, rtc.tm_mday, rtc.tm_year, 
            rtc.tm_hour, rtc.tm_min, rtc.tm_sec); log_info();
}

void hk_update_tlm(void) {
    uint64_t now = systime_epoch_ms();
    g_tlm.ops_stage = g_flashmgr.config.ops_stage;
    g_tlm.uppm_rbt_cnt = g_flashmgr.rbt_cnt;
    g_tlm.uppm_rbt_cause = g_rbt_cause;
    g_tlm.uppm_time_to_rst = (g_scheduler.id_map[SCHED_ID_PPM_RST]->next_release - now);
    g_tlm.hn_state = g_pldmgr.hn_state;
    g_tlm.ab_state = g_pldmgr.ab_state;
    g_tlm.eps_heartbeat = (now - g_tlm.eps_heartbeat_time <= 11000);

    mcp_dispatch(NODE, NODE_LPPM, 0, CMD, "tlm_get_data", "");
    mcp_dispatch(NODE, NODE_EPS, 0, CMD, "tlm_get_data", "");
}

void hk_ppm_reset(void) {
    sprintf(LOGBUF, "SCHEDULED RESET"); log_info();
    g_superloop_running = FALSE;
}
