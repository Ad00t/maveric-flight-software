#include "adcsmtq.h"
#include "interrupts.h"
#include "uart.h"
#include "systime.h"
#include "common.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#module

// HELPERS

// Compute sum of all bytes in buf
uint16_t sum_buf(uint8_t* buf, uint8_t len) {
    uint16_t sum = 0;
    uint8_t i;
    for (i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}

uint8_t mtq_stat_parse_mode(uint32_t stat) {
    return (uint8_t)(stat & 0b0000111); // Remember the documentation is wrong, mode is the bottom 3 bits
}

int1 mtq_stat_parse_sun(uint32_t stat) {
    return (stat & (1U << 13)) != 0;
}

int1 mtq_stat_parse_tumb(uint32_t stat) {
    return (stat & (1U << 11)) != 0;
}

// Generate checksum (two's complement of buf sum, so original sum + checksum should = 0)
uint8_t gen_csum(uint8_t* buf, uint8_t len) {
    uint16_t sum = sum_buf(buf, len);
    return 0xFF - sum + 1;
}

// MTQ Packet

void mtq_pkt_init(mtq_pkt_s* pkt) {
    mtq_pkt_clear(pkt);
}

void mtq_pkt_clear(mtq_pkt_s* pkt) {
    memset(pkt, 0, sizeof(mtq_pkt_s));
}

int1 mtq_pkt_verify_csum(mtq_pkt_s* pkt) {
    uint8_t sum = pkt->head + pkt->idx + pkt->cnt + ((pkt->midx << 4) | pkt->err);
    if (pkt->head == MTQ_HEAD_READ) sum += sum_buf(pkt->data, 4*pkt->cnt);
    sum += pkt->csum;
    return sum == 0;
}

// MTQ API 

status_e mtq_init(mtq_s* mtq, uint8_t port, float* paxs, char* tle) {
    mtq->is_init = TRUE;
    mtq->port = port;
    memcpy(mtq->reg_table, MTQ_INIT_REG_TABLE, sizeof(MTQ_INIT_REG_TABLE));
    memset(mtq->reg_idx_map, 0, MTQ_MAP_COUNT * MTQ_MAX_IDX_COUNT * sizeof(mtq_reg_s*));
    mtq_pkt_init(&mtq->rcvpkt);

    // Init indices, register space
    uint8_t i;
    for (i = 0; i < MTQ_REG_TABLE_LEN; i++) {
        mtq_reg_s* reg = &mtq->reg_table[i];
       
        // Populate idx map
        if (reg->midx < MTQ_MAP_COUNT && reg->idx < MTQ_MAX_IDX_COUNT)
            mtq->reg_idx_map[reg->midx][reg->idx] = reg;

        // Allocate register data space
        uint8_t l = MTQ_REG_TYPE_SIZES[reg->type];
        reg->value_len = 4*reg->cnt / l;
        reg->value = calloc(reg->value_len, l);
    }
    
    rtc_time_t rtc;
    systime_rtc(&rtc);
    status_e s1 = mtq_set_datetime(mtq, &rtc);
    status_e s2 = mtq_set_mode(mtq, MTQ_MODE_SAFE);
    status_e s3 = mtq_write_start(mtq, MTQ_POINTING_AXIS, paxs);
    status_e s4 = mtq_write_start(mtq, MTQ_TLE, tle);
    sprintf(LOGBUF, "mtq_init: port=%u", mtq->port); log_info();
    return (s1 == SUCCESS && s2 == SUCCESS && s3 == SUCCESS && s4 == SUCCESS) ? SUCCESS : FAILURE;
}

void mtq_destroy(mtq_s* mtq) {
    if (!mtq->is_init) return;
    mtq_clear(mtq);
    uint8_t i;
    for (i = 0; i < MTQ_REG_TABLE_LEN; i++) {
        if (mtq->reg_table[i].value != NULL)
            free(mtq->reg_table[i].value);
    }
    mtq->is_init = FALSE; 
    sprintf(LOGBUF, "mtq_destroy"); log_info();
}

void mtq_clear(mtq_s* mtq) {
    if (!mtq->is_init) return;
    uint8_t i;
    for (i = 0; i < MTQ_REG_TABLE_LEN; i++) {
        if (mtq->reg_table[i].value != NULL)
            memset(mtq->reg_table[i].value, 0, 4 * mtq->reg_table[i].cnt);
    }
    mtq_pkt_clear(&mtq->rcvpkt);
    memset(mtq->reg_idx_map, 0, MTQ_MAP_COUNT * MTQ_MAX_IDX_COUNT * sizeof(mtq_reg_s*));
    memset(mtq->reg_table, 0, sizeof(MTQ_INIT_REG_TABLE));
    sprintf(LOGBUF, "mtq_clear"); log_info();
}

mtq_reg_s* mtq_get_reg(mtq_s* mtq, uint8_t midx, uint8_t idx) {
    if (!mtq->is_init) return NULL;
    if (midx >= MTQ_MAP_COUNT || idx >= MTQ_MAX_IDX_COUNT)
        return NULL;
    mtq_reg_s* reg = mtq->reg_idx_map[midx][idx];
    if (reg != NULL && (reg->midx != midx || reg->idx != idx))
        return NULL;
    return reg;
}

mtq_reg_s* mtq_get_reg(mtq_s* mtq, uint16_t key) {
    return mtq_get_reg(mtq, key >> 8, key & 0x00FF);
}

status_e mtq_print_reg_data(mtq_s* mtq, mtq_reg_s* reg, char* out, uint16_t* j) {
    uint8_t i;
    switch (reg->type) {
        case T_UINT8:
            for (i = 0; i < reg->value_len; i++) 
                *j += sprintf(&out[*j], " %u", ((uint8_t*)reg->value)[i]);
            break;
        case T_INT8:
            for (i = 0; i < reg->value_len; i++) 
                *j += sprintf(&out[*j], " %d", ((int8_t*)reg->value)[i]);
            break;
        case T_UINT16:
            for (i = 0; i < reg->value_len; i++) 
                *j += sprintf(&out[*j], " %u", ((uint16_t*)reg->value)[i]);
            break;
        case T_INT16:
            for (i = 0; i < reg->value_len; i++) 
                *j += sprintf(&out[*j], " %d", ((int16_t*)reg->value)[i]);
            break;
        case T_FLOAT:
            for (i = 0; i < reg->value_len; i++) {
                *j += sprintf(&out[*j], " ");
                *j += ftoa(((float*)reg->value)[i], &out[*j], 6, 'f'); 
            } 
            break;
        case T_CHAR:
            *j += sprintf(&out[*j], " %s", ((char*)reg->value));
            break;
    }
    return SUCCESS;
}

status_e mtq_print_reg_data(mtq_s* mtq, uint16_t key, char* out, uint16_t* j) {
    mtq_reg_s* reg = mtq_get_reg(mtq, key);
    if (reg == NULL) return FAILURE;
    return mtq_print_reg_data(mtq, reg, out, j);
}

status_e mtq_read_start(mtq_s* mtq, mtq_reg_s* reg) {   
    if (!mtq->is_init) return FAILURE;
    uint8_t w_buf[4];
    w_buf[0] = MTQ_HEAD_READ;
    w_buf[1] = reg->idx;
    w_buf[2] = reg->cnt;
    w_buf[3] = (reg->midx << 4) | 0;
    uint8_t csum = gen_csum(w_buf, 4);
    
    sprintf(LOGBUF, "mtq_read_start: reg=(%u,%u) data=[ %02X %02X %02X %02X %02X ]", 
            reg->midx, reg->idx, w_buf[0], w_buf[1], w_buf[2], w_buf[3], csum); log_trace();
    
    uart_write_buf(mtq->port, w_buf, 4); 
    uart_write_byte(mtq->port, csum);
    delay_ms(10);
    return SUCCESS;
}

status_e mtq_read_start(mtq_s* mtq, uint16_t key) {
    if (!mtq->is_init) return FAILURE;
    mtq_reg_s* reg = mtq_get_reg(mtq, key);
    if (reg == NULL) {
        sprintf(LOGBUF, "mtq_read_start: register invalid (%u,%u)", key >> 8, key & 0x00FF); log_error();
        return FAILURE;
    }
    return mtq_read_start(mtq, reg);
}

void mtq_read_complete(mtq_s* mtq) {
    if (!mtq->is_init) return;
    mtq_pkt_s* rcvpkt = &mtq->rcvpkt;
    if (!mtq_pkt_verify_csum(rcvpkt)) {
        sprintf(LOGBUF, "mtq_read_complete: ppm checksum error (%u,%u)", rcvpkt->midx, rcvpkt->idx); log_error();
        return;
    }

    switch (rcvpkt->err) {
        case 0: break; // No error
        case 1: // Checksum error
            sprintf(LOGBUF, "mtq_read_complete: rcv checksum error (%u,%u)", rcvpkt->midx, rcvpkt->idx); log_error();
            return;
        case 2: // Invalid register
            sprintf(LOGBUF, "mtq_read_complete: rcv invalid register (%u,%u)", rcvpkt->midx, rcvpkt->idx); log_error();
            return;
    }

    mtq_reg_s* reg = mtq_get_reg(mtq, rcvpkt->midx, rcvpkt->idx);
    if (reg == NULL) {
        sprintf(LOGBUF, "mtq_read_complete: ppm invalid register (%u,%u)", rcvpkt->midx, rcvpkt->idx); log_error();
        return;
    }
        
    uint8_t n_body_bytes = 4*rcvpkt->cnt;
    memcpy(reg->value, rcvpkt->data, n_body_bytes);
   
    uint16_t j = 0;
    j += sprintf(&LOGBUF[j], "mtq_read_complete: reg=(%u,%u) count=%u err=%u data=[",
                 reg->midx, reg->idx, reg->cnt, rcvpkt->err); 
    mtq_print_reg_data(mtq, reg, LOGBUF, &j); 
    j += sprintf(&LOGBUF[j], " ]"); log_trace();
}

status_e mtq_write_start(mtq_s* mtq, mtq_reg_s* reg, void* data) {
    if (!mtq->is_init) return FAILURE;
    uint8_t w_buf[MTQ_MAX_PKT_LEN] = {0};
    uint8_t n_body_bytes = 4*reg->cnt;
    uint8_t w_buf_len = 4 + n_body_bytes;
    w_buf[0] = MTQ_HEAD_WRITE;
    w_buf[1] = reg->idx;
    w_buf[2] = reg->cnt;
    w_buf[3] = (reg->midx << 4) | 0;

    memcpy(&w_buf[4], data, n_body_bytes);
    uint8_t csum = gen_csum(w_buf, w_buf_len);

    uint16_t j = sprintf(LOGBUF, "mtq_write_start: reg=(%u,%u) len=%u data=[", reg->midx, reg->idx, w_buf_len+1);
    uint8_t i; 
    for (i = 0; i < w_buf_len; i++)
        j += sprintf(&LOGBUF[j], " %02X", w_buf[i]);
    j += sprintf(&LOGBUF[j], " %02X ]", csum); log_trace();

    uart_write_buf(mtq->port, w_buf, w_buf_len);
    uart_write_buf(mtq->port, &csum, 1);
    delay_ms(10);
    return SUCCESS;
}

status_e mtq_write_start(mtq_s* mtq, uint16_t key, void* data) {
    if (!mtq->is_init) return FAILURE;
    mtq_reg_s* reg = mtq_get_reg(mtq, key);
    if (reg == NULL) {
        sprintf(LOGBUF, "mtq_write_start: register invalid (%u,%u)", key >> 8, key & 0x00FF); log_error();
        return FAILURE;
    }
    return mtq_write_start(mtq, reg, data);
}

void mtq_write_complete(mtq_s* mtq) {
    if (!mtq->is_init) return;
    mtq_pkt_s* rcvpkt = &mtq->rcvpkt;
    if (!mtq_pkt_verify_csum(rcvpkt)) {
        sprintf(LOGBUF, "mtq_write_complete: ppm checksum error (%u,%u)", rcvpkt->midx, rcvpkt->idx); log_error();
        return;
    }

    switch (rcvpkt->err) {
        case 0: break; // No error
        case 1: // Checksum error
            sprintf(LOGBUF, "mtq_write_complete: rcv checksum error (%u,%u)", rcvpkt->midx, rcvpkt->idx); log_error();
            return;
        case 2: // Invalid register
            sprintf(LOGBUF, "mtq_write_complete: rcv invalid register (%u,%u)", rcvpkt->midx, rcvpkt->idx); log_error();
            return;
    }

    mtq_reg_s* reg = mtq_get_reg(mtq, rcvpkt->midx, rcvpkt->idx);
    if (reg == NULL) {
        sprintf(LOGBUF, "mtq_write_complete: ppm invalid register (%u,%u)", rcvpkt->midx, rcvpkt->idx); log_error();
        return;
    }

    sprintf(LOGBUF, "mtq_write_complete: reg=(%u,%u) count=%u err=%u", 
            reg->midx, reg->idx, reg->cnt, rcvpkt->err); log_trace();

    // Readback
    mtq_read_start(mtq, reg);
}

void mtq_parse_stream(mtq_s* mtq, ringbuf_s* irqbuf) {
    if (!mtq->is_init) return;
    mtq_pkt_s* rcvpkt = &mtq->rcvpkt;
    uint16_t iter = 0;
    while (iter < 2*RINGBUF_MAX_CAPACITY) {
        uint8_t b;
        if (!rb_pop(irqbuf, 1, &b)) return;
      
        switch (rcvpkt->fsm) {
            case MTQ_FSM_HEAD:
                if (b == MTQ_HEAD_READ || b == MTQ_HEAD_WRITE) {
                    rcvpkt->head = b;
                    rcvpkt->fsm = MTQ_FSM_IDX;
                } 
                break;

            case MTQ_FSM_IDX:
                rcvpkt->idx = b;
                rcvpkt->fsm = MTQ_FSM_CNT;
                break;

            case MTQ_FSM_CNT:
                rcvpkt->cnt = b;
                rcvpkt->fsm = MTQ_FSM_MIDXERR;
                break;

            case MTQ_FSM_MIDXERR:
                rcvpkt->midx = b >> 4;
                rcvpkt->err = b & 0b1111; 
                switch (rcvpkt->head) {
                    case MTQ_HEAD_WRITE: rcvpkt->fsm = MTQ_FSM_CSUM; break;
                    case MTQ_HEAD_READ: rcvpkt->fsm = MTQ_FSM_PAYLOAD; break;
                    default: rcvpkt->fsm = MTQ_FSM_ERROR; break;
                }
                break;

            case MTQ_FSM_PAYLOAD:
                if (rcvpkt->i_payload >= MTQ_MAX_PAYLOAD_LEN) {
                    rcvpkt->fsm = MTQ_FSM_ERROR;
                    break;
                }
                rcvpkt->data[rcvpkt->i_payload++] = b;
                if (rcvpkt->i_payload >= 4*rcvpkt->cnt) {
                    rcvpkt->fsm = MTQ_FSM_CSUM;
                }
                break;

            case MTQ_FSM_CSUM:
                rcvpkt->csum = b;
                rcvpkt->fsm = MTQ_FSM_DONE;
                break;
        } 

        // Could wait till next superloop iteration or just handle end states immediately (currently doing the latter)
        switch (rcvpkt->fsm) {
            case MTQ_FSM_DONE:
                switch (rcvpkt->head) {
                    case MTQ_HEAD_READ: mtq_read_complete(mtq); break;
                    case MTQ_HEAD_WRITE: mtq_write_complete(mtq); break;
                }
                mtq_pkt_clear(rcvpkt);
                break;
            case MTQ_FSM_ERROR:
                sprintf(LOGBUF, "mtq_parse_stream: malformed packet"); log_error();
                mtq_pkt_clear(rcvpkt);
                break;
        }

        iter++;
    }
}

// HIGH LEVEL API

status_e mtq_get_data(mtq_s* mtq, uint16_t key, void* out) {
    if (!mtq->is_init) return FAILURE;
    mtq_reg_s* reg = mtq_get_reg(mtq, key);
    if (reg == NULL) return FAILURE;
    memcpy(out, reg->value, reg->value_len);
    return SUCCESS;
}

void mtq_check_heartbeat(mtq_s* mtq) {
    if (!mtq->is_init) {
        mtq->heartbeat = FAILURE;
        return;
    }
    mtq_reg_s* reg = mtq_get_reg(mtq, MTQ_SNID);
    if (reg == NULL) {
        mtq->heartbeat = FAILURE;
        return;
    }
    char* snid = (char*) reg->value;
    status_e hb = (strncmp(snid, "TAD102063", reg->value_len) == 0) ? SUCCESS : FAILURE; 
    
    memset(snid, 0, reg->value_len);
    mtq_read_start(mtq, MTQ_SNID);
    mtq->heartbeat = hb;
}

status_e mtq_reboot(mtq_s* mtq) {
    if (!mtq->is_init) return FAILURE;
    uint8_t req = 1;
    status_e s = mtq_write_start(mtq, MTQ_NVM, &req);
    delay_ms(100);
    return s;
}

status_e mtq_reset(mtq_s* mtq) {
    if (!mtq->is_init) return FAILURE;
    float paxs[3] = {0};
    char tle[140] = {0};
    status_e s1 = mtq_get_data(mtq, MTQ_POINTING_AXIS, paxs);
    status_e s2 = mtq_get_data(mtq, MTQ_TLE, tle);
    status_e s3 = mtq_reboot(mtq);
    rtc_time_t rtc;
    systime_rtc(&rtc);
    status_e s4 = mtq_set_datetime(mtq, &rtc);
    status_e s5 = mtq_set_mode(mtq, MTQ_MODE_SAFE);
    status_e s6 = mtq_write_start(mtq, MTQ_POINTING_AXIS, paxs);
    status_e s7 = mtq_write_start(mtq, MTQ_TLE, tle);
    return (s1 == SUCCESS && s2 == SUCCESS && s3 == SUCCESS && s4 == SUCCESS 
            && s5 == SUCCESS && s6 == SUCCESS && s7 == SUCCESS) ? SUCCESS : FAILURE;
}

status_e mtq_read_fast(mtq_s* mtq) {
    if (!mtq->is_init) return FAILURE;
    uint8_t i;
    status_e s = SUCCESS;
    for (i = 0; i < MTQ_NUM_FAST_REGS; i++) {
        if (mtq_read_start(mtq, MTQ_FAST_FRAME_REGS[i]) == FAILURE)
            s = FAILURE;
    }
    return s;
}

status_e mtq_read_ctrl(mtq_s* mtq) {
    if (!mtq->is_init) return FAILURE;
    uint8_t i;
    status_e s = SUCCESS;
    for (i = 0; i < MTQ_NUM_CTRL_REGS; i++) {
        if (mtq_read_start(mtq, MTQ_CTRL_FRAME_REGS[i]) == FAILURE)
            s = FAILURE;
    }
    return s;
}

status_e mtq_read_all(mtq_s* mtq) {
    if (!mtq->is_init) return FAILURE;
    uint8_t i;
    status_e s = SUCCESS;
    for (i = 0; i < MTQ_REG_TABLE_LEN; i++) {
        if (mtq_read_start(mtq, &mtq->reg_table[i]) == FAILURE)
            s = FAILURE;
    }
    return s;
}

status_e mtq_set_datetime(mtq_s* mtq, rtc_time_t* rtc) {
    if (!mtq->is_init) return FAILURE;
    uint8_t date[4] = {0}; 
    date[3] = hextobcd(rtc->tm_year);
    date[2] = hextobcd(rtc->tm_mon);
    date[1] = hextobcd(rtc->tm_mday);
    date[0] = hextobcd(rtc->tm_wday); 
    status_e s1 = mtq_write_start(mtq, MTQ_DATE, date);
    uint8_t time[4] = {0};
    time[3] = hextobcd(rtc->tm_hour);
    time[2] = hextobcd(rtc->tm_min);
    time[1] = hextobcd(rtc->tm_sec);
    time[0] = hextobcd(0); // Unused
    status_e s2 = mtq_write_start(mtq, MTQ_TIME, time);
    return (s1 == SUCCESS && s2 == SUCCESS) ? SUCCESS : FAILURE;
}

status_e mtq_set_mode(mtq_s* mtq, uint8_t mode) {
    if (!mtq->is_init) return FAILURE;
    uint8_t conf[4] = {0};
    conf[0] = mode | (1 << 7);
    return mtq_write_start(mtq, MTQ_CONF, conf);
}
