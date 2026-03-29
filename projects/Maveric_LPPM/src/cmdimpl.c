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
#include "globals.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#module

void cmdimpl_init(void) {
    hashtable_s* ht = &g_cmdmgr.cmdimpls;

    ht_set(ht, "ping", (cmdimpl_f) cmdimpl_ping);
    
    ht_set(ht, "ppm_reset", (cmdimpl_f) cmdimpl_ppm_reset);
    ht_set(ht, "ppm_get_time", (cmdimpl_f) cmdimpl_ppm_get_time);
    ht_set(ht, "ppm_set_time", (cmdimpl_f) cmdimpl_ppm_set_time);
    ht_set(ht, "ppm_delay", (cmdimpl_f) cmdimpl_ppm_delay);
    ht_set(ht, "ppm_clear_bufs", (cmdimpl_f) cmdimpl_ppm_clear_bufs);
    ht_set(ht, "ppm_get_scheds", (cmdimpl_f) cmdimpl_ppm_get_scheds);
    ht_set(ht, "ppm_sched_cmd_in", (cmdimpl_f) cmdimpl_ppm_sched_cmd_in);
    ht_set(ht, "ppm_deschedule", (cmdimpl_f) cmdimpl_ppm_deschedule);

    ht_set(ht, "tlm_get_data", (cmdimpl_f) cmdimpl_tlm_get_data);
    
    ht_set(ht, "gnc_set_mode", (cmdimpl_f) cmdimpl_gnc_set_mode);
    
    ht_set(ht, "flash_read", (cmdimpl_f) cmdimpl_flash_read);
    ht_set(ht, "flash_write", (cmdimpl_f) cmdimpl_flash_write);
    ht_set(ht, "flash_erase", (cmdimpl_f) cmdimpl_flash_erase);
    ht_set(ht, "flash_get_cfg", (cmdimpl_f) cmdimpl_flash_get_cfg);
    ht_set(ht, "flash_set_cfg", (cmdimpl_f) cmdimpl_flash_set_cfg);

    ht_set(ht, "mtq_heartbeat", (cmdimpl_f) cmdimpl_mtq_heartbeat);
    ht_set(ht, "mtq_reset", (cmdimpl_f) cmdimpl_mtq_reset);
    ht_set(ht, "mtq_read_1", (cmdimpl_f) cmdimpl_mtq_read_1);
    ht_set(ht, "mtq_get_1", (cmdimpl_f) cmdimpl_mtq_get_1);
    ht_set(ht, "mtq_set_1", (cmdimpl_f) cmdimpl_mtq_set_1);
    ht_set(ht, "mtq_read_fast", (cmdimpl_f) cmdimpl_mtq_read_fast);
    ht_set(ht, "mtq_get_fast", (cmdimpl_f) cmdimpl_mtq_get_fast);
    ht_set(ht, "mtq_read_ctrl", (cmdimpl_f) cmdimpl_mtq_read_ctrl);
    ht_set(ht, "mtq_get_ctrl", (cmdimpl_f) cmdimpl_mtq_get_ctrl);
    ht_set(ht, "mtq_read_all", (cmdimpl_f) cmdimpl_mtq_read_all);
    ht_set(ht, "mtq_get_all", (cmdimpl_f) cmdimpl_mtq_get_all);
    
    ht_set(ht, "nvg_heartbeat", (cmdimpl_f) cmdimpl_nvg_heartbeat);
    ht_set(ht, "nvg_reset", (cmdimpl_f) cmdimpl_nvg_reset);
    ht_set(ht, "nvg_power", (cmdimpl_f) cmdimpl_nvg_power);
    ht_set(ht, "nvg_get_1", (cmdimpl_f) cmdimpl_nvg_get_1);
    ht_set(ht, "nvg_set_1", (cmdimpl_f) cmdimpl_nvg_set_1);
    ht_set(ht, "nvg_start_all", (cmdimpl_f) cmdimpl_nvg_start_all);
    ht_set(ht, "nvg_stop_all", (cmdimpl_f) cmdimpl_nvg_stop_all);
}

// COMMAND IMPLEMENTATIONS

void cmdimpl_ping(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_ping '%s'", pkt->args); log_info();
            cmd_respond(pkt, RES, "pong");
            break;
        }
    }
}

void cmdimpl_ppm_reset(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_ppm_reset"); log_info();
            g_superloop_running = FALSE;
            char res[8] = {0};
            sprintf(res, "%u", SUCCESS);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_get_time(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_time REQ"); log_info();
            char res[32] = {0};  
            systime_str(res);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_set_time(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
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

            mtq_set_datetime(&g_mtq, &time);
            cmd_dispatch(NODE, NODE_UPPM, 0, REQ, "ppm_set_time", pkt->args);

            rtc_time_t rtc;
            systime_rtc(&rtc);
            sprintf(LOGBUF, "cmdimpl_ppm_set_time '%s' [ %02u, %02u/%02u/20%02u %02u:%02u:%02u ]", pkt->args,
                    rtc.tm_wday, rtc.tm_mon, rtc.tm_mday, rtc.tm_year, 
                    rtc.tm_hour, rtc.tm_min, rtc.tm_sec); log_info();
            char res[8] = {0};
            sprintf(res, "%u", SUCCESS);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_delay(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint32_t delay = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_delay d=%u", delay); log_info();
            char res[8] = {0};
            if (delay < 60000) {
                sprintf(res, "%u", SUCCESS);
                cmd_respond(pkt, RES, res);
                delay_ms(delay);
            } else {
                sprintf(res, "%u", FAILURE);
                cmd_respond(pkt, RES, res);
            }
            break;
        }
    }
}

void cmdimpl_ppm_clear_bufs(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t mode = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_clear_bufs m=%u", mode); log_info();
            char res[8] = {0};
            switch (mode) {
                case 0: 
                    irqmgr_clear(&g_irqmgr);
                    sprintf(res, "%u", SUCCESS);
                    cmd_respond(pkt, RES, res);
                    break;
                case 1: 
                    i2cmgr_clear(&g_i2cmgr);
                    sprintf(res, "%u", SUCCESS);
                    cmd_respond(pkt, RES, res);
                    break;
                case 2:
                    cmdmgr_clear(&g_cmdmgr);
                    sprintf(res, "%u", SUCCESS);
                    cmd_respond(pkt, RES, res);
                    break;
                default:
                    sprintf(res, "%u", FAILURE);
                    cmd_respond(pkt, RES, res);
                    break;
            }
            break;
        }
    }
}

void cmdimpl_ppm_get_scheds(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_ppm_get_scheds"); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0}; 
            uint8_t i;
            uint16_t j = 0;
            for (i = 0; i < SCHEDULER_MAX_TASKS; i++) {
                schedtask_s* st = &g_scheduler.tasks[i];
                j += sprintf(&res[j], "%u:%u,%u,%u ", i, st->id, st->active, st->type);
            }
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_ppm_sched_cmd_in(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
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
            sprintf(LOGBUF, "cmdimpl_ppm_sched_cmd_in sid=%u del=%u per=%u rep=%u o=%u d=%u e=%u p=%u id='%s' args='%s'",
                    schedule_id, start_delay_ms, period_ms, reps, orgn, dest, echo, ptype, cmd_id, args); log_info();
            cmdpkt_s schedcmd;
            cmdpkt_create(&schedcmd, orgn, dest, echo, ptype, cmd_id, args);
            status_e s = scheduler_schedule_cmd_in(&g_scheduler, schedule_id, &schedcmd, start_delay_ms, period_ms, reps);
            char res[8] = {0};
            sprintf(res, "%u %u", s, schedule_id);
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_ppm_deschedule(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t sched_id = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_ppm_deschedule id=%u", sched_id); log_info();
            status_e s = scheduler_deschedule(&g_scheduler, sched_id);
            char res[8] = {0};
            sprintf(res, "%u %u", s, sched_id);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_gnc_set_mode(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t mode = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_gnc_set_mode mode=%u", mode); log_info();
            // TODO: set gnc mode
            char res[8] = {0};
            sprintf(res, "%u", FAILURE);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_tlm_get_data(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_tlm_get_data REQ"); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0};
            uint16_t j = sprintf(res, "%u %u %u %u %u", 
                                 g_flashmgr.rbt_cnt, g_rbt_cause, g_ertc.heartbeat, g_mtq.heartbeat, g_nvg.heartbeat);

            float gyro_rate_rad[3] = {0};
            switch (g_flashmgr.config.gyro_rate_src) {
                case DATASRC_MTQ:
                    mtq_get_data(&g_mtq, MTQ_RATE, gyro_rate_rad);
                    break;
                case DATASRC_NVG:
                    nvg_get_sensor_data(&g_nvg, NVG_GYROSCOPE_CAL, gyro_rate_rad);
                    break;
                case DATASRC_GYRO:
                    break;
            }
            uint8_t i;
            for (i = 0; i < 3; i++) {
                j += sprintf(&res[j], " ");
                j += ftoa(gyro_rate_rad[i], &res[j], 3, 'f');
            }
            
            float attitude[5] = {0};
            switch (g_flashmgr.config.attitude_src) {
                case DATASRC_MTQ:
                    mtq_get_data(&g_mtq, MTQ_Q, attitude);
                    break;
                case DATASRC_NVG:
                    nvg_get_sensor_data(&g_nvg, NVG_QUAT_RV, attitude);
                    break;
                case DATASRC_GYRO:
                    break;
            }
            for (i = 0; i < 4; i++) {
                j += sprintf(&res[j], " ");
                j += ftoa(attitude[i], &res[j], 3, 'f');
            }

            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_read(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint32_t addr = strtoul(p, &p, 16);
            uint8_t len = maxu8(strtoul(p, &p, 10), 200);
            sprintf(LOGBUF, "cmdimpl_flash_read addr=0x%02X len=%u", addr, len); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0};
            uint8_t j = sprintf(res, "0x%02X %u ", addr, len);
            flashRead(addr, len, &res[j]); 
            cmd_respond(pkt, RES, res, j+len);
            break;
        }
    }
}

void cmdimpl_flash_write(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint32_t addr = strtoul(p, &p, 16);
            uint8_t len = maxu8(strtoul(p, &p, 10), 200);
            sprintf(LOGBUF, "cmdimpl_flash_write addr=0x%02X len=%u", addr, len); log_info();
            status_e s = flashWriteSafe(addr, len, p+1, addr, addr + FLASH_BLOCK_SIZE - 1); 
            char res[32] = {0};
            sprintf(res, "%u 0x%02X %u", s, addr, len);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_erase(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint32_t addr = strtoul(p, &p, 16);
            sprintf(LOGBUF, "cmdimpl_flash_erase addr=%u", addr); log_info();
            flashEraseBlockByAddr(addr); 
            char res[16] = {0};
            sprintf(res, "0x%02X", addr);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_get_cfg(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char res[CMD_MAX_ARGS_LEN] = {0};
            config_s* cfg = &g_flashmgr.config;
            sprintf(res, "%u %u %u", cfg->log_level, cfg->gyro_rate_src, cfg->attitude_src);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_flash_set_cfg(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            config_s* cfg = &g_flashmgr.config;
            cfg->log_level = strtoul(p, &p, 10);
            cfg->gyro_rate_src = strtoul(p, &p, 10);
            cfg->attitude_src = strtoul(p, &p, 10);
            status_e s = flashmgr_config_flush(&g_flashmgr);
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_heartbeat(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            mtq_check_heartbeat(&g_mtq);
            char res[8] = {0};
            sprintf(res, "%u", g_mtq.heartbeat);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_reset(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            status_e s = mtq_reset(&g_mtq);
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_read_1(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t midx = strtoul(p, &p, 10);
            uint8_t idx = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_read_1 midx=%u idx=%u", midx, idx); log_info();
            uint16_t key = (uint16_t) midx << 8 | idx;
            status_e s = mtq_read_start(&g_mtq, key);
            char res[16] = {0};
            sprintf(res, "%u %u %u", s, midx, idx);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_get_1(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t midx = strtoul(p, &p, 10);
            uint8_t idx = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_get_1 midx=%u idx=%u", midx, idx); log_info();
            char res[CMD_MAX_ARGS_LEN] = {0};
            uint16_t j = sprintf(res, "%u %u", midx, idx);
            uint16_t key = ((uint16_t) midx << 8) | idx;
            mtq_print_reg_data(&g_mtq, key, res, &j);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_set_1(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t midx = strtoul(p, &p, 10);
            uint8_t idx = strtoul(p, &p, 10);
            sprintf(LOGBUF, "cmdimpl_mtq_set_1 midx=%u idx=%u", midx, idx); log_info();
            uint16_t key = ((uint16_t) midx << 8) | idx;
            mtq_reg_s* reg = mtq_get_reg(&g_mtq, key);
            char res[16] = {0};
            if (reg == NULL) {
                sprintf(res, "%u %u %u", FAILURE, midx, idx);
                cmd_respond(pkt, RES, res);
                break;
            }
            uint8_t i;
            uint8_t data[MTQ_MAX_PAYLOAD_LEN] = {0};
            uint8_t l = MTQ_REG_TYPE_SIZES[reg->type];
            switch (reg->type) {
                case T_UINT8:
                    for (i = 0; i < reg->value_len; i++) {
                        uint8_t val = strtoul(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_INT8: 
                    for (i = 0; i < reg->value_len; i++) {
                        int8_t val = strtol(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_UINT16:
                    for (i = 0; i < reg->value_len; i++) {
                        uint16_t val = strtoul(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_INT16:
                    for (i = 0; i < reg->value_len; i++) {
                        int16_t val = strtol(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_FLOAT:
                    for (i = 0; i < reg->value_len; i++) {
                        float val = strtol(p, &p, 10);
                        memcpy(&data[i*l], &val, l);
                    }
                    break;
                case T_CHAR:
                    memcpy(data, p+1, strlen(p+1)); // Skip 1 space
                    break;
            }
            status_e s = mtq_write_start(&g_mtq, reg, data);
            sprintf(res, "%u %u %u", s, midx, idx);
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_mtq_read_fast(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_mtq_read_fast"); log_info();
            status_e s = mtq_read_fast(&g_mtq); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_get_fast(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            char res[CMD_MAX_ARGS_LEN] = {0};
            if (page > MTQ_FAST_FRAME_REGS / MTQ_PAGE_SIZE) {
                sprintf(res, "%u %u", FAILURE, page);
                cmd_respond(pkt, RES, res);
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_NUM_FAST_REGS);
            sprintf(LOGBUF, "cmdimpl_mtq_get_fast pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint16_t j = sprintf(res, "%u %u", SUCCESS, page);
            uint8_t i;
            for (i = i1; i < i2; i++) {
                uint16_t key = MTQ_FAST_FRAME_REGS[i];
                j += sprintf(&res[j], " %u,%u", key >> 8, key & 0xFF);
                mtq_print_reg_data(&g_mtq, key, res, &j);
            }
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_mtq_read_ctrl(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_mtq_read_ctrl"); log_info();
            status_e s = mtq_read_ctrl(&g_mtq);
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_mtq_get_ctrl(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            char res[CMD_MAX_ARGS_LEN] = {0};
            if (page > MTQ_CTRL_FRAME_REGS / MTQ_PAGE_SIZE) {
                sprintf(res, "%u %u", FAILURE, page);
                cmd_respond(pkt, RES, res);
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_NUM_CTRL_REGS);
            sprintf(LOGBUF, "cmdimpl_mtq_get_ctrl pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint16_t j = sprintf(res, "%u %u", SUCCESS, page);
            uint8_t i;
            for (i = i1; i < i2; i++) {
                uint16_t key = MTQ_CTRL_FRAME_REGS[i];
                j += sprintf(&res[j], " %u,%u", key >> 8, key & 0xFF);
                mtq_print_reg_data(&g_mtq, key, res, &j);
            }
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_mtq_read_all(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_mtq_read_all"); log_info();
            status_e s = mtq_read_all(&g_mtq); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_mtq_get_all(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t page = strtoul(p, &p, 10);
            char res[CMD_MAX_ARGS_LEN] = {0};
            if (page > MTQ_REG_TABLE_LEN / MTQ_PAGE_SIZE) {
                sprintf(res, "%u %u", FAILURE, page);
                cmd_respond(pkt, RES, res);
                break;
            }
            uint8_t i1 = MTQ_PAGE_SIZE * page;
            uint8_t i2 = minu8(i1 + MTQ_PAGE_SIZE, MTQ_REG_TABLE_LEN);
            sprintf(LOGBUF, "cmdimpl_mtq_get_all pg=%u i1=%u i2=%u", page, i1, i2); log_info();
            uint16_t j = sprintf(res, "%u %u", SUCCESS, page);
            uint8_t i;
            for (i = i1; i < i2; i++) {
                mtq_reg_s* reg = &g_mtq.reg_table[i];
                j += sprintf(&res[j], " %u,%u", reg->midx, reg->idx);
                mtq_print_reg_data(&g_mtq, reg, res, &j);
            }
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_nvg_heartbeat(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            nvg_check_heartbeat(&g_nvg);
            char res[8] = {0};
            sprintf(res, "%u", g_nvg.heartbeat);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_nvg_reset(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_nvg_reset"); log_info();
            status_e s = nvg_reset(&g_nvg); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_nvg_power(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_nvg_power"); log_info();
            status_e s = nvg_power(&g_nvg); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_nvg_send_cmd(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            sprintf(LOGBUF, "cmdimpl_nvg_send_cmd '%s'", p); log_info();
            status_e s = nvg_send_cmd(&g_nvg, p); 
            char res[16] = {0};
            sprintf(res, "%u %s", s, p);
            cmd_respond(pkt, RES, res);
            break;
        }
    }
}

void cmdimpl_nvg_get_1(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t sensor_id = strtoul(p, &p, 10);  
            sprintf(LOGBUF, "cmdimpl_nvg_get_1 sid=%u", sensor_id); log_info();
            float data[8] = {0}; 
            status_e s = nvg_get_sensor_data(&g_nvg, sensor_id, data); 
            if (s == SUCCESS) {
                char res[CMD_MAX_ARGS_LEN] = {0};
                nvg_sensor_s* sensor = &g_nvg.sensors[sensor_id];
                uint16_t j = sprintf(&res[j], "%u %u ", s, sensor_id);
                j += ftoa(sensor->ts, &res[j], 6, 'f');
                uint8_t i;
                for (i = 0; i < sensor->len; i++) {
                    j += sprintf(&res[j], " ");
                    j += ftoa(data[i], &res[j], 6, 'f');
                }
                cmd_respond(pkt, RES, res); 
            } else {
                char res[8] = {0};
                sprintf(res, "%u", FAILURE);
                cmd_respond(pkt, RES, res); 
            }
            break;
        }
    }
}

void cmdimpl_nvg_set_1(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            char* p = pkt->args;
            uint8_t sensor_id = strtoul(p, &p, 10);  
            uint8_t rate = strtoul(p, &p, 10);  
            sprintf(LOGBUF, "cmdimpl_nvg_set_1 sid=%u rate=%u", sensor_id, rate); log_info();
            status_e s = nvg_set_sensor(&g_nvg, sensor_id, rate); 
            char res[16] = {0};
            sprintf(res, "%u %u %u", s, sensor_id, rate);
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_nvg_start_all(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_nvg_start_all"); log_info();
            status_e s = nvg_start_all_sensors(&g_nvg); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}

void cmdimpl_nvg_stop_all(cmdpkt_s* pkt) {
    switch (pkt->ptype) {
        case REQ: {
            sprintf(LOGBUF, "cmdimpl_nvg_stop_all"); log_info();
            status_e s = nvg_stop_all_sensors(&g_nvg); 
            char res[8] = {0};
            sprintf(res, "%u", s);
            cmd_respond(pkt, RES, res); 
            break;
        }
    }
}
