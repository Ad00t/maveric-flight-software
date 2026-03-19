// LOWER PPM COMMAND IMPLEMENTATIONS

#include "cmdimpl.h"
#include "cmdmgr.h"
#include "systime.h"
#include "hashtable.h"
#include "interrupts.h"
#include "scheduler.h"
#include "flashmgr.h"
#include "adcsmtq.h"
#include "m41t81s.h"
#include "adis16260.h"
#include "naviguider.h"
#include "housekeeping.h"
#include "common.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#module

extern uint8_t g_rbt_cause;         // restart_cause() in init 

extern irqmgr_s g_irqmgr;               // Interrupts manager
extern cmdmgr_s g_cmdmgr;               // Commands manager
extern flashmgr_s g_flashmgr;           // Flash manager
extern scheduler_s g_scheduler;         // Schedules manager
extern ertc_s g_ertc;                   // External RTC (on motherboard)
extern mtq_s g_mtq;                     // Magnetorquer
extern gyro_s g_gyro;                   // Gyroscope (x3)
extern nvg_s g_nvg;                     // Naviguider

void cmdimpl_init(void) {
    hashtable_s* ht = &g_cmdmgr.cmdimpls;

    ht_set(ht, "ppm_set_time", (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(ht, "ppm_get_time", (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(ht, "ppm_ping", (cmdimpl_f) cmdimpl_ppm_ping);
    ht_set(ht, "ppm_delay", (cmdimpl_f) cmdimpl_ppm_delay);
    ht_set(ht, "ppm_clear_bufs", (cmdimpl_f) cmdimpl_ppm_clear_bufs);
    ht_set(ht, "ppm_sched_cmd_in", (cmdimpl_f) cmdimpl_ppm_sched_cmd_in);

    ht_set(ht, "tlm_get_data", (cmdimpl_f) cmdimpl_tlm_get_data);

    ht_set(ht, "mtq_read_1", (cmdimpl_f) cmdimpl_mtq_read_1);
    ht_set(ht, "mtq_get_1", (cmdimpl_f) cmdimpl_mtq_get_1);
    ht_set(ht, "mtq_set_1", (cmdimpl_f) cmdimpl_mtq_set_1);
    ht_set(ht, "mtq_reset", (cmdimpl_f) cmdimpl_mtq_reset);

    ht_set(ht, "mtq_read_fast", (cmdimpl_f) cmdimpl_mtq_read_fast);
    ht_set(ht, "mtq_get_fast", (cmdimpl_f) cmdimpl_mtq_get_fast);
    ht_set(ht, "mtq_read_ctrl", (cmdimpl_f) cmdimpl_mtq_read_ctrl);
    ht_set(ht, "mtq_get_ctrl", (cmdimpl_f) cmdimpl_mtq_get_ctrl);
    ht_set(ht, "mtq_read_all", (cmdimpl_f) cmdimpl_mtq_read_all);
    ht_set(ht, "mtq_get_all", (cmdimpl_f) cmdimpl_mtq_get_all);
}

// HELPERS

cmdpkt_type_e stat2ack(status_e s) {
    return (s == SUCCESS ? ACK : NACK);
}

// COMMAND IMPLEMENTATIONS

void cmdimpl_ppm_set_time(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
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
            ertc_set_time(&g_ertc, time);
            systime_sync();
            scheduler_refresh_all(&g_scheduler, oldtime);

            mtq_set_datetime(&g_mtq, time);
            cmd_dispatch(NODE, NODE_UPPM, 0, REQ, "ppm_set_time", pkt->args);

            sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args,
                    g_ertc.time.tm_wday, g_ertc.time.tm_mon, g_ertc.time.tm_mday, g_ertc.time.tm_year, 
                    g_ertc.time.tm_hour, g_ertc.time.tm_min, g_ertc.time.tm_sec); log_info();
            break;
    }
}

void cmdimpl_ppm_get_time(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            sprintf(LOGBUF, "cmdimpl_ppm_get_time REQ"); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0};  
            rtc_to_str(g_ertc.time, res);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "ppm_get_time", res);
            break;
    }
}

void cmdimpl_ppm_ping(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            sprintf(LOGBUF, "cmdimpl_ppm_ping '%s'", pkt->args); log_info();
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "ppm_ping", "pong");
            break;
    }
}

void cmdimpl_ppm_delay(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint32_t delay = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_delay d=%u", delay); log_info();
            if (delay < 60000) {
                cmd_dispatch(NODE, pkt->orgn, pkt->echo, ACK, "ppm_delay", "");
                delay_ms(delay);
            } else {
                cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "ppm_delay", "");
            }
            break;
    }
}

void cmdimpl_ppm_clear_bufs(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
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
            break;
    }
}

void cmdimpl_ppm_get_schedules(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            break;
    }
}

void cmdimpl_ppm_sched_cmd_in(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint8_t schedule_id = strtoul(p, &p, 10);
            uint32_t start_delay_ms = strtoul(p, &p, 10);
            uint32_t period_ms = strtoul(p, &p, 10);
            uint16_t reps = strtoul(p, &p, 10);
            uint8_t orgn = strtoul(p, &p, 10);
            uint8_t dest = strtoul(p, &p, 10);
            uint8_t echo = strtoul(p, &p, 10);
            uint8_t ptype = strtoul(p, &p, 10);
            char sep[] = " ";
            char* cmd_id = strtok(++p, sep);
            char* args = strtok(0, sep);
            cmdpkt_s schedcmd;
            cmdpkt_create(&schedcmd, orgn, dest, echo, ptype, cmd_id, args);
            status_e s = scheduler_schedule_cmd_in(&g_scheduler, schedule_id, &schedcmd, start_delay_ms, period_ms, reps);
            char res[CMD_MAX_ARGS_LEN] = {0};
            sprintf(res, "%u", schedule_id);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, stat2ack(s), "ppm_sched_cmd_in", res); 
            break;
    }
}

void cmdimpl_tlm_get_data(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            sprintf(LOGBUF, "cmdimpl_tlm_get_data REQ"); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0};
            sprintf(res, "%u %u", g_flashmgr.rbt_cnt, g_rbt_cause);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "tlm_get_data", res);
            break;
    }
}

void cmdimpl_mtq_read_1(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint8_t key = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_read_1 key=%u", key); log_info();
            status_e s = mtq_read_start(&g_mtq, key);
            char res[CMD_MAX_ARGS_LEN] = {0};
            sprintf(res, "%u", key);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, stat2ack(s), "mtq_read_1", res); 
            break;
    }
}

void cmdimpl_mtq_get_1(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint8_t key = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_get_1 key=%u", key); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0};
            uint8_t j = 0;
            j += sprintf(&res[j], "%u", key);
            status_e s = mtq_print_reg_data(&g_mtq, key, res, &j);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, stat2ack(s), "mtq_get_1", res); 
            break;
    }
}

void cmdimpl_mtq_set_1(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint8_t key = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_set_1 key=%u", key); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0};
            uint8_t j = 0;
            j += sprintf(&res[j], "%u", key);
            status_e s = mtq_print_reg_data(&g_mtq, key, res, &j);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, stat2ack(s), "mtq_set_1", res); 
            break;
    }
}

void cmdimpl_mtq_reset(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            status_e s = mtq_reset(&g_mtq);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, stat2ack(s), "mtq_reset", "");
            break;
    }
}

void cmdimpl_mtq_read_fast(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            sprintf(LOGBUF, "cmdimpl_mtq_read_fast"); log_info();
            status_e s = mtq_read_fast(&g_mtq); 
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, stat2ack(s), "mtq_read_fast", ""); 
            break;
    }
}

void cmdimpl_mtq_get_fast(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            uint8_t j = 0;
            char res[CMD_MAX_ARGS_LEN] = {0};
            j += sprintf(&res[j], "%u", page);
            if (page > MTQ_FAST_FRAME_REGS / MTQ_PAGE_SIZE) {
                cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_fast", res); 
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_NUM_FAST_REGS);
            sprintf(LOGBUF, "cmdimpl_mtq_get_fast pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint8_t i;
            for (i = i1; i < i2; i++) {
                uint16_t key = MTQ_FAST_FRAME_REGS[i];
                j += sprintf(&res[j], " %u", key);
                mtq_print_reg_data(&g_mtq, key, res, &j);
            }
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_fast", res); 
            break;
    }
}

void cmdimpl_mtq_read_ctrl(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            sprintf(LOGBUF, "cmdimpl_mtq_read_ctrl"); log_info();
            status_e s = mtq_read_ctrl(&g_mtq);
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, stat2ack(s), "mtq_read_ctrl", ""); 
            break;
    }
}

void cmdimpl_mtq_get_ctrl(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            uint8_t j = 0;
            char res[CMD_MAX_ARGS_LEN] = {0};
            j += sprintf(&res[j], "%u", page);
            if (page > MTQ_CTRL_FRAME_REGS / MTQ_PAGE_SIZE) {
                cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_ctrl", res); 
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_NUM_CTRL_REGS);
            sprintf(LOGBUF, "cmdimpl_mtq_get_ctrl pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint8_t i;
            for (i = i1; i < i2; i++) {
                uint16_t key = MTQ_CTRL_FRAME_REGS[i];
                j += sprintf(&res[j], " %u", key);
                mtq_print_reg_data(&g_mtq, key, res, &j);
            }
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_ctrl", res); 
            break;
    }
}

void cmdimpl_mtq_read_all(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            sprintf(LOGBUF, "cmdimpl_mtq_read_all"); log_info();
            status_e s = mtq_read_all(&g_mtq); 
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, stat2ack(s), "mtq_read_all", ""); 
            break;
    }
}

void cmdimpl_mtq_get_all(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ:
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            uint8_t j = 0;
            char res[CMD_MAX_ARGS_LEN] = {0};
            j += sprintf(&res[j], "%u", page);
            if (page > MTQ_REG_TABLE_LEN / MTQ_PAGE_SIZE) {
                cmd_dispatch(NODE, pkt->orgn, pkt->echo, NACK, "mtq_get_all", res); 
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_REG_TABLE_LEN);
            sprintf(LOGBUF, "cmdimpl_mtq_get_all pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint8_t i;
            for (i = i1; i < i2; i++) {
                mtq_reg_s* reg = &g_mtq.reg_table[i];
                uint16_t key = reg->midx << 8 | reg->idx;
                j += sprintf(&res[j], " %u", key);
                mtq_print_reg_data(&g_mtq, reg, res, &j);
            }
            cmd_dispatch(NODE, pkt->orgn, pkt->echo, RES, "mtq_get_all", res); 
            break;
    }
}
