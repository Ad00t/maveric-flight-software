// LOWER PPM HOUSEKEEPING

#include "housekeeping.h"
#include "common.h"
#include "scheduler.h"
#include "systime.h"
#include "mcppkt.h"
#include "mcpmgr.h"
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
    scheduler_schedule_func_in(&g_scheduler, 1, hk_log, 2500, 1000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 2, hk_systime_sync, 5000, 42000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 3, hk_heartbeats, 4000, 5000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 4, hk_read_sensors, 3000, 2000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 5, hk_gnc_step, 4000, 10000, SCHEDULE_REPS_INFINITE);
    // scheduler_schedule_func_in(&g_scheduler, 6, hk_ppm_reset, 4*MS_PER_MIN, 0, 1);       // 120 min
}

// HOUSEKEEPING FUNCTIONS

void hk_get_ertc_time(void) {
    ertc_get_time(&g_ertc); 
}

void hk_systime_sync(void) {
    systime_sync();
    char req[MCP_MAX_ARGS_LEN] = {0};
    systime_str(req);
    mcp_dispatch(NODE, NODE_UPPM, 0, CMD, "ppm_set_time", req);
}

void hk_log(void) {
    rtc_time_t rtc;
    systime_rtc(&rtc);
    sprintf(LOGBUF, "housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u ertc=%u", 
            rtc.tm_wday, rtc.tm_mon, rtc.tm_mday, rtc.tm_year, 
            rtc.tm_hour, rtc.tm_min, rtc.tm_sec, g_ertc.is_using_ertc); log_info();
}

void hk_heartbeats(void) {
    ertc_check_heartbeat(&g_ertc);
    mtq_check_heartbeat(&g_mtq);
    nvg_check_heartbeat(&g_nvg);
    sprintf(LOGBUF, "heartbeats: ertc=%u mtq=%u nvg=%u", g_ertc.heartbeat, g_mtq.heartbeat, g_nvg.heartbeat); log_info();
}

void hk_read_sensors(void) {
    mtq_read_hk(&g_mtq);
}

void hk_gnc_step(void) {
    float gyro_rate[4] = {0}; // Make sure we have enough space for extra nvg value(s)
    config_s* cfg = &g_flashmgr.config; 
    switch (cfg->gyro_rate_src) {
        case DATASRC_MTQ:
            mtq_get_data(&g_mtq, MTQ_CAL_IMU_B, gyro_rate); // rad/s
            break;
        case DATASRC_NVG:
            nvg_get_sensor_data(&g_nvg, NVG_GYROSCOPE_UNCAL, gyro_rate); // rad/s
            break;
    }
    gnc_step(&g_gnc, &g_mtq, gyro_rate);
}

void hk_ppm_reset(void) {
    g_superloop_running = FALSE;
}
