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
    scheduler_schedule_func_in(&g_scheduler, 3, hk_heartbeats, 4000, 5000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 4, hk_read_sensors, 3000, 2000, SCHEDULE_REPS_INFINITE);
    scheduler_schedule_func_in(&g_scheduler, 5, hk_gnc_step, 4000, 10000, SCHEDULE_REPS_INFINITE);
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
    rtc_time_t rtc;
    epoch_ms_to_rtc(systime_epoch_ms(), &rtc);  
    sprintf(LOGBUF, "housekeeping %02u, %02u/%02u/20%02u %02u:%02u:%02u ertc=%u", 
            rtc.tm_wday, rtc.tm_mon, rtc.tm_mday, rtc.tm_year, 
            rtc.tm_hour, rtc.tm_min, rtc.tm_sec, g_ertc.is_using_ertc); log_info();
}

void hk_heartbeats(void) {
    ertc_check_heartbeat(&g_ertc);
    mtq_check_heartbeat(&g_mtq);
    nvg_check_heartbeat(&g_nvg);
    sprintf(LOGBUF, "heartbeats: ertc=%u mtq=%u nvg=%u", g_ertc.heartbeat, g_mtq.heartbeat, g_nvg.heartbeat);
}

void hk_read_sensors(void) {
    // mtq_read_ctrl(&g_mtq);
    mtq_read_fast(&g_mtq);
    // gyro_read_all(&g_gyro);
}

void hk_gnc_step(void) {
    float gyro_rate_rad[3] = {0};
    switch (g_flashmgr.config.gyro_rate_src) {
        case DATASRC_MTQ:
            mtq_get_data(&g_mtq, MTQ_RATE, gyro_rate_rad);
        case DATASRC_NVG:
            nvg_get_sensor_data(&g_nvg, NVG_GYROSCOPE_CAL, gyro_rate_rad);
        case DATASRC_GYRO:
            break;
    }
    gnc_step(&g_gnc, &g_mtq, gyro_rate_rad);
}
