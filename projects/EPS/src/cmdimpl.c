// LOWER PPM COMMAND IMPLEMENTATIONS

#include "cmdimpl.h"
#include "cmdmgr.h"
#include "systime.h"
#include "hashtable.h"
//#include "interrupts.h"
#include "interrupt.h"
//#include "scheduler.h"
//#include "flashmgr.h"
//#include "mtq.h"
//#include "ertc.h"
//#include "gyro.h"
//#include "naviguider.h"
//#include "housekeeping.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

extern uint8_t g_rbt_cause;         // restart_cause() in init 

extern irqmgr_s g_irqmgr;               // Interrupts manager
extern cmdmgr_s g_cmdmgr;               // Commands manager
//extern flashmgr_s g_flashmgr;           // Flash manager
//extern scheduler_s g_scheduler;         // Schedules manager
//extern ertc_s g_ertc;                   // External RTC (on motherboard)
//extern mtq_s g_mtq;                     // Magnetorquer
//extern gyro_s g_gyro;                   // Gyroscope (x3)
//extern nvg_s g_nvg;                     // Naviguider

void cmdimpl_init(void) {
    hashtable_s* ht = &g_cmdmgr.cmdimpls;
    char cmd_ppm_set_time[] = "ppm_set_time";
    char cmd_ppm_get_time[] = "ppm_get_time";
    char cmd_ppm_ping[] = "ppm_ping";
    char cmd_ppm_delay[] = "ppm_delay";
    char cmd_ppm_clear_bufs[] = "ppm_clear_bufs";
    
    ht_set(ht, cmd_ppm_set_time, (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(ht, cmd_ppm_get_time, (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(ht, cmd_ppm_ping, (cmdimpl_f) cmdimpl_ppm_ping);
    ht_set(ht, cmd_ppm_delay, (cmdimpl_f) cmdimpl_ppm_delay);
    ht_set(ht, cmd_ppm_clear_bufs, (cmdimpl_f) cmdimpl_ppm_clear_bufs);

    //ht_set(ht, "tlm_get_data", (cmdimpl_f) cmdimpl_tlm_get_data);

    //ht_set(ht, "mtq_get_conf", (cmdimpl_f) cmdimpl_mtq_get_conf);
    //ht_set(ht, "mtq_set_conf", (cmdimpl_f) cmdimpl_mtq_set_conf);
    //ht_set(ht, "mtq_get_datetime", (cmdimpl_f) cmdimpl_mtq_get_datetime);
    //ht_set(ht, "mtq_set_datetime", (cmdimpl_f) cmdimpl_mtq_set_datetime);
    //ht_set(ht, "mtq_get_tle", (cmdimpl_f) cmdimpl_mtq_get_tle);
    //ht_set(ht, "mtq_set_tle", (cmdimpl_f) cmdimpl_mtq_set_tle);
    //ht_set(ht, "mtq_get_paxs", (cmdimpl_f) cmdimpl_mtq_get_paxs);
    //ht_set(ht, "mtq_set_paxs", (cmdimpl_f) cmdimpl_mtq_set_paxs);
    //ht_set(ht, "mtq_get_mtquser", (cmdimpl_f) cmdimpl_mtq_get_mtquser);
    //ht_set(ht, "mtq_set_mtquser", (cmdimpl_f) cmdimpl_mtq_set_mtquser);
    //ht_set(ht, "mtq_reset", (cmdimpl_f) cmdimpl_mtq_reset);
    //ht_set(ht, "mtq_get_stat", (cmdimpl_f) cmdimpl_mtq_get_stat);
}

// COMMAND IMPLEMENTATIONS

void cmdimpl_ppm_set_time(cmdpkt_s* pkt) {
    char* p = pkt->args;
    rtc_time_t time;
    time.tm_wday = strtoul(p, &p, 10); // Other options: strtok(), strtod(), strotol()
    time.tm_mon = strtoul(p, &p, 10); 
    time.tm_mday = strtoul(p, &p, 10);
    time.tm_year = strtoul(p, &p, 10);
    time.tm_hour = strtoul(p, &p, 10);
    time.tm_min = strtoul(p, &p, 10);
    time.tm_sec = strtoul(p, &p, 10);
    
    uint64_t oldtime = systime_epoch_ms();
    //ertc_set_time(&g_ertc, time);
    systime_sync();
    //scheduler_refresh_all(&g_scheduler, oldtime);

    //mtq_set_datetime(&g_mtq, time);
    //cmd_dispatch(NODE, NODE_UPPM, 0, CMD, "ppm_set_time", pkt->args);

    //sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args
    //        g_ertc.time.tm_wday, g_ertc.time.tm_mon, g_ertc.time.tm_mday, g_ertc.time.tm_year, 
    //        g_ertc.time.tm_hour, g_ertc.time.tm_min, g_ertc.time.tm_sec); log_info();
}

void cmdimpl_ppm_get_time(cmdpkt_s* pkt) {
    char cmd_ppm_get_time[] = "ppm_get_time";
    
    if (pkt->ptype != CMD) return;
    // Only implement CMD here because LPPM should never be replacing its time from another subsystem
    char tm_str[32] = {0};  
    //rtc_to_str(g_ertc.time, tm_str);
    cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, cmd_ppm_get_time, tm_str);
    sprintf(LOGBUF, "cmdimpl_ppm_get_time CMD '%s'", tm_str); log_info();
    break;
}

void cmdimpl_ppm_ping(cmdpkt_s* pkt) {
    char cmd_ppm_ping[] = "ppm_ping";
    char cmd_ans_pong[] = "pong";
    sprintf(LOGBUF, "cmdimpl_ppm_ping '%s'", pkt->args); log_info();
    if (pkt->ptype == CMD) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, cmd_ppm_ping, cmd_ans_pong);
    }
}

void cmdimpl_ppm_delay(cmdpkt_s* pkt) {
    char* p = pkt->args;
    char cmd_ppm_delay[] = "ppm_delay";
    char cmd_empty[] = "";
    uint32_t delay = strtoul(p, &p, 10);
    sprintf(LOGBUF, "cmdimpl_ppm_delay d=%u", delay); log_info();
    if (delay < 60000) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, cmd_ppm_delay, cmd_empty);
        delay_ms(delay);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, cmd_ppm_delay, cmd_empty);
    }
}

void cmdimpl_ppm_clear_bufs(cmdpkt_s* pkt) {
    char* p = pkt->args;
    uint8_t mode = strtoul(p, &p, 10);
    sprintf(LOGBUF, "cmdimpl_ppm_clear_bufs m=%u", mode); log_info();
    switch (mode) {
        case 0:
            irqmgr_clear(&g_irqmgr);
            break;
        case 1:
            cmdmgr_clear(&g_cmdmgr);
            break;
    }
}
/*
void cmdimpl_tlm_get_data(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    sprintf(LOGBUF, "cmdimpl_tlm_get_data CMD"); log_info();
    char args[CMD_MAX_ARGS_LEN] = {0};
    sprintf(args, "%u %u", g_flashmgr.rbt_cnt, g_rbt_cause);
    cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "tlm_get_data", args);
}

void cmdimpl_mtq_get_conf(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    uint8_t conf[4] = {0};
    if (mtq_get_data(&g_mtq, MTQ_CONF, conf) == SUCCESS) {
        char args[32] = {0};
        sprintf(args, "%u", conf[0]);
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_conf", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_conf", "");
    }
}

void cmdimpl_mtq_set_conf(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    char* p = pkt->args;
    uint8_t data[4] = {0};
    data[3] = strtoul(p, &p, 10);
    data[2] = 0;
    data[1] = 0;
    data[0] = strtoul(p, &p, 10);
    
    if (mtq_write_start(&g_mtq, MTQ_CONF, data) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_conf", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_conf", "");
    }
}

void cmdimpl_mtq_get_datetime(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    uint8_t date[4] = {0};
    uint8_t time[4] = {0};
    if (mtq_get_data(&g_mtq, MTQ_DATE, date) == SUCCESS && mtq_get_data(&g_mtq, MTQ_TIME, time) == SUCCESS) {
        char args[32] = {0}; 
        sprintf(args, "%u %u %u %u %u %u %u", 
                bcdtohex(date[0]), bcdtohex(date[2]), bcdtohex(date[1]), bcdtohex(date[3]),
                bcdtohex(time[3]), bcdtohex(time[2]), bcdtohex(time[1]));
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_datetime", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_datetime", "");
    }
}

void cmdimpl_mtq_set_datetime(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    char* p = pkt->args;
    rtc_time_t time;
    time.tm_wday = strtoul(p, &p, 10); // Other options: strtok(), strtod(), strotol()
    time.tm_mon = strtoul(p, &p, 10); 
    time.tm_mday = strtoul(p, &p, 10);
    time.tm_year = strtoul(p, &p, 10);
    time.tm_hour = strtoul(p, &p, 10);
    time.tm_min = strtoul(p, &p, 10);
    time.tm_sec = strtoul(p, &p, 10);
    
    if (mtq_set_datetime(&g_mtq, &time) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_datetime", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_datetime", "");
    }
}

void cmdimpl_mtq_get_tle(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    char tle[140] = {0};
    if (mtq_get_data(&g_mtq, MTQ_TLE, tle) == SUCCESS) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_tle", tle);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_tle", "");
    }
}

void cmdimpl_mtq_set_tle(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    if ((mtq_write_start(&g_mtq, MTQ_TLE, pkt->args)) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_tle", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_tle", "");
    }
}

void cmdimpl_mtq_get_paxs(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    float paxs[3] = {0};
    if (mtq_get_data(&g_mtq, MTQ_POINTING_AXIS, paxs) == SUCCESS) {
        char args[32] = {0};
        sprintf(args, "%.3f %.3f %.3f", paxs[0], paxs[1], paxs[2]);
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_paxs", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_paxs", "");
    }
}

void cmdimpl_mtq_set_paxs(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    char* p = pkt->args;
    float data[3] = {0};
    data[0] = strtof(p, &p);
    data[1] = strtof(p, &p);
    data[2] = strtof(p, &p);
    
    if (mtq_write_start(&g_mtq, MTQ_POINTING_AXIS, data) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_paxs", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_paxs", "");
    }
}

void cmdimpl_mtq_get_mtquser(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    float mtquser[3] = {0};
    if (mtq_get_data(&g_mtq, MTQ_MTQ_USER, mtquser) == SUCCESS) {
        char args[32] = {0};
        sprintf(args, "%.3f %.3f %.3f", mtquser[0], mtquser[1], mtquser[2]);
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_mtquser", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_mtquser", "");
    }
}

void cmdimpl_mtq_set_mtquser(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    char* p = pkt->args;
    float data[3] = {0};
    data[0] = strtof(p, &p);
    data[1] = strtof(p, &p);
    data[2] = strtof(p, &p);
    
    if (mtq_write_start(&g_mtq, MTQ_MTQ_USER, data) == SUCCESS) { 
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_set_mtquser", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_set_mtquser", "");
    }
}

void cmdimpl_mtq_reset(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    if (mtq_reset(&g_mtq) == SUCCESS) {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "mtq_reset", "");
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_reset", "");
    }
}

void cmdimpl_mtq_get_stat(cmdpkt_s* pkt) {
    if (pkt->ptype != CMD) return;
    uint8_t stat[4] = {0};
    if (mtq_get_data(&g_mtq, MTQ_STAT, stat) == SUCCESS) {
        char args[32] = {0};
        sprintf(args, "%u %u %u %u", stat[0], stat[1], stat[2], stat[3]);
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_stat", args);
    } else {
        cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_stat", "");
    }
}
*/
