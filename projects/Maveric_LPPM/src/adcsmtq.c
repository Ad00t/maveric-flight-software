#include "adcsmtq.h"
#include "interrupts.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>
#include <stdlibm.h>
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

// Generate checksum (two's complement of buf sum, so original sum + checksum should = 0)
uint8_t gen_csum(uint8_t* buf, uint8_t len) {
    uint16_t sum = sum_buf(buf, len);
    return 0xFF - sum + 1;
}

// Convert a decimal number to binary-coded decimal
uint8_t to_bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
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

void mtq_init(mtq_s* mtq, uint8_t port) {
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
        switch (reg->type) {
            case T_UINT8:
                reg->value_len = 4*reg->cnt / sizeof(uint8_t);
                reg->value = calloc(reg->value_len, sizeof(uint8_t));
                break;
            case T_INT8:
                reg->value_len = 4*reg->cnt / sizeof(int8_t);
                reg->value = calloc(reg->value_len, sizeof(int8_t));
                break;
            case T_UINT16:
                reg->value_len = 4*reg->cnt / sizeof(uint16_t);
                reg->value = calloc(reg->value_len, sizeof(uint16_t));
                break;
            case T_INT16:
                reg->value_len = 4*reg->cnt / sizeof(int16_t);
                reg->value = calloc(reg->value_len, sizeof(int16_t));
                break;
            case T_FLOAT:
                reg->value_len = 4*reg->cnt / sizeof(float);
                reg->value = calloc(reg->value_len, sizeof(float));
                break;
            case T_CHAR:
                reg->value_len = 4*reg->cnt / sizeof(char);
                reg->value = calloc(reg->value_len, sizeof(char));
                break;
        }
    }
    
    mtq_set_conf(&mtq, 0, MTQ_MODE_SAFE);

    fprintf(COM_D, "%s[%s] mtq_init: port=%u\n", KYEL, NODE_LBL, mtq->port);
}

void mtq_destroy(mtq_s* mtq) {
    uint8_t i;
    for (i = 0; i < MTQ_REG_TABLE_LEN; i++) {
        if (mtq->reg_table[i].value != NULL)
            free(mtq->reg_table[i].value);
    }
    
    fprintf(COM_D, "%s[%s] mtq_destroy\n", KYEL, NODE_LBL);
}

void mtq_clear(mtq_s* mtq) {
    uint8_t i;
    for (i = 0; i < MTQ_REG_TABLE_LEN; i++) {
        if (mtq->reg_table[i].value != NULL)
            memset(mtq->reg_table[i].value, 0, 4 * mtq->reg_table[i].cnt);
    }

    mtq_pkt_clear(&mtq->rcvpkt);
    memset(mtq->reg_idx_map, 0, MTQ_MAP_COUNT * MTQ_MAX_IDX_COUNT * sizeof(mtq_reg_s*));
    memset(mtq->reg_table, 0, sizeof(MTQ_INIT_REG_TABLE));
    
    fprintf(COM_D, "%s[%s] mtq_clear\n", KYEL, NODE_LBL);
}

mtq_reg_s* mtq_get_reg(mtq_s* mtq, uint8_t midx, uint8_t idx) {
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

void mtq_read_start(mtq_s* mtq, mtq_reg_s* reg) {   
    uint8_t w_buf[4];
    w_buf[0] = MTQ_HEAD_READ;
    w_buf[1] = reg->idx;
    w_buf[2] = reg->cnt;
    w_buf[3] = (reg->midx << 4) | 0;
    uint8_t csum = gen_csum(w_buf, 4);
    
    fprintf(COM_D, "%s[%s] mtq_read_start: port=UART%u reg=(%u,%u) data=[ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]\n", 
            KYEL, NODE_LBL, mtq->port, reg->midx, reg->idx, w_buf[0], w_buf[1], w_buf[2], w_buf[3], csum);
    
    uart_write_buf(mtq->port, w_buf, 4); 
    uart_write_byte(mtq->port, csum);
    delay_ms(10);
}

void mtq_read_start(mtq_s* mtq, uint16_t key) {
    mtq_reg_s* reg = mtq_get_reg(mtq, key);
    if (reg == NULL) {
        fprintf(COM_D, "%s[%s] mtq_read_start: register invalid (%u,%u)\n", KRED, NODE_LBL, key >> 8, key & 0x00FF);
        return;
    }
    mtq_read_start(mtq, reg);
}

void mtq_read_complete(mtq_s* mtq) {
    mtq_pkt_s* rcvpkt = &mtq->rcvpkt;
    if (!mtq_pkt_verify_csum(rcvpkt)) {
        fprintf(COM_D, "%s[%s] mtq_read_complete: ppm checksum error (%u,%u)\n", KRED, NODE_LBL, rcvpkt->midx, rcvpkt->idx);
        return;
    }

    switch (rcvpkt->err) {
        case 0: break; // No error
        case 1: // Checksum error
            fprintf(COM_D, "%s[%s] mtq_read_complete: rcv checksum error (%u,%u)\n", KRED, NODE_LBL, rcvpkt->midx, rcvpkt->idx);
            return;
        case 2: // Invalid register
            fprintf(COM_D, "%s[%s] mtq_read_complete: rcv invalid register (%u,%u)\n", KRED, NODE_LBL, rcvpkt->midx, rcvpkt->idx);
            return;
    }

    mtq_reg_s* reg = mtq_get_reg(mtq, rcvpkt->midx, rcvpkt->idx);
    if (reg == NULL) {
        fprintf(COM_D, "%s[%s] mtq_read_complete: ppm invalid register (%u,%u)\n", KRED, NODE_LBL, rcvpkt->midx, rcvpkt->idx);
        return;
    }
        
    uint8_t n_body_bytes = 4*rcvpkt->cnt;
    memcpy(reg->value, rcvpkt->data, n_body_bytes);
    
    fprintf(COM_D, "%s[%s] mtq_read_complete: port=UART%u reg=(%u,%u) count=%u err=%u data=[",
            KYEL, NODE_LBL, mtq->port, reg->midx, reg->idx, reg->cnt, rcvpkt->err);

    uint8_t i;
    switch (reg->type) {
        case T_UINT8: {
            for (i = 0; i < n_body_bytes; i++)
                fprintf(COM_D, " %u", ((uint8_t*)reg->value)[i]);
            break;
        }
        case T_INT8: {
            for (i = 0; i < n_body_bytes/2; i++)
                fprintf(COM_D, " %d", ((int8_t*)reg->value)[i]);
            break;
        }
        case T_UINT16: {
            for (i = 0; i < n_body_bytes/2; i++)
                fprintf(COM_D, " %u", ((uint16_t*)reg->value)[i]);
            break;
        }
        case T_INT16: {
            for (i = 0; i < n_body_bytes/2; i++)
                fprintf(COM_D, " %d", ((int16_t*)reg->value)[i]);
            break;
        }
        case T_FLOAT: {
//            for (i = 0; i < n_body_bytes; i++)
//                fprintf(COM_D, " 0x%02X", reg->value[i]);
            for (i = 0; i < n_body_bytes/4; i++) 
                fprintf(COM_D, " %.2f", ((float*)reg->value)[i]);
            break;
        }
        case T_CHAR: {
            ((char*)reg->value)[n_body_bytes] = '\0'; // Null terminate
            fprintf(COM_D, " '%s'", (char*)reg->value);
            break;
        }
    }    

    fprintf(COM_D, " ]\n");
}

void mtq_write_start(mtq_s* mtq, mtq_reg_s* reg, void* data) {
    uint8_t w_buf[MAX_BUF_LEN];
    uint8_t n_body_bytes = 4*reg->cnt;
    uint8_t w_buf_len = 4 + n_body_bytes;
    w_buf[0] = MTQ_HEAD_WRITE;
    w_buf[1] = reg->idx;
    w_buf[2] = reg->cnt;
    w_buf[3] = (reg->midx << 4) | 0;
    
    memcpy(&w_buf[4], data, n_body_bytes);
    uint8_t csum = gen_csum(w_buf, w_buf_len);
    
    uint8_t i;
    switch (reg->type) {
//        case T_UINT8:
//            break;
//        case T_INT8:
//            break;
//        case T_UINT16:
//            break;
//        case T_INT16:
//            break;
//        case T_FLOAT:
//            break;
        case T_CHAR:
            w_buf[4+n_body_bytes] = (uint8_t)('\0'); // Null terminate
            break;
    }
    
    fprintf(COM_D, "%s[%s] mtq_write_start: port=%u reg=(%u,%u) len=%u data=[", 
            KYEL, NODE_LBL mtq->port, reg->midx, reg->idx, w_buf_len+1);
    for (i = 0; i < w_buf_len; i++)
        fprintf(COM_D, " 0x%02X", w_buf[i]);
    fprintf(COM_D, " 0x%02X ]\n", csum);
    
    uart_write_buf(mtq->port, w_buf, w_buf_len);
    uart_write_buf(mtq->port, &csum, 1);
    delay_ms(10);
}

void mtq_write_start(mtq_s* mtq, uint16_t key, void* data) {
    mtq_reg_s* reg = mtq_get_reg(mtq, key);
    if (reg == NULL) {
        fprintf(COM_D, "%s[%s] mtq_write_start: register invalid (%u,%u)\n", KRED, NODE_LBL, key >> 8, key & 0x00FF);
        return;
    }
    mtq_write_start(mtq, reg, data);
}

void mtq_write_complete(mtq_s* mtq) {
    mtq_pkt_s* rcvpkt = &mtq->rcvpkt;
    if (!mtq_pkt_verify_csum(rcvpkt)) {
        fprintf(COM_D, "%s[%s] mtq_write_complete: ppm checksum error (%u,%u)\n", KRED, NODE_LBL, rcvpkt->midx, rcvpkt->idx);
        return;
    }

    switch (rcvpkt->err) {
        case 0: break; // No error
        case 1: // Checksum error
            fprintf(COM_D, "%s[%s] mtq_write_complete: rcv checksum error (%u,%u)\n", KRED, NODE_LBL, rcvpkt->midx, rcvpkt->idx);
            return;
        case 2: // Invalid register
            fprintf(COM_D, "%s[%s] mtq_write_complete: rcv invalid register (%u,%u)\n", KRED, NODE_LBL, rcvpkt->midx, rcvpkt->idx);
            return;
    }

    mtq_reg_s* reg = mtq_get_reg(mtq, rcvpkt->midx, rcvpkt->idx);
    if (reg == NULL) {
        fprintf(COM_D, "%s[%s] mtq_write_complete: ppm invalid register (%u,%u)\n", KRED, NODE_LBL, rcvpkt->midx, rcvpkt->idx);
        return;
    }
    
    fprintf(COM_D, "%s[%s] mtq_write_complete: port=%u reg=(%u,%u) count=%u err=%u\n", 
            KYEL, NODE_LBL, mtq->port, reg->midx, reg->idx, reg->cnt, rcvpkt->err);
}

void mtq_rcv_parser(mtq_s* mtq, circbuf_s* irqbuf) {
    mtq_pkt_s* rcvpkt = &mtq->rcvpkt;
    uint16_t iter = 0;
    while (iter < 2*CIRCBUF_MAX_SIZE) {
        uint8_t b;
        if (!cb_pop(irqbuf, 1, &b)) return;
      
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
                    case MTQ_HEAD_READ: rcvpkt->fsm = MTQ_FSM_DATA; break;
                    default: rcvpkt->fsm = MTQ_FSM_ERROR; break;
                }
                break;

            case MTQ_FSM_DATA:
                if (rcvpkt->i_args >= MAX_BUF_LEN) {
                    rcvpkt->fsm = MTQ_FSM_ERROR;
                    break;
                }
                rcvpkt->data[rcvpkt->i_args++] = b;
                if (rcvpkt->i_args >= 4*rcvpkt->cnt) {
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
                fprintf(COM_D, "%s[%s] mtq_rcv_parser: malformed packet\n", KRED, NODE_LBL);
                mtq_pkt_clear(rcvpkt);
                break;
        }

        iter++;
    }
}

// HIGH LEVEL API

int1 mtq_heartbeat(mtq_s* mtq) {
    mtq_reg_s* reg = mtq_get_reg(mtq, MTQ_SNID);
    if (reg == NULL) return FALSE;
    
    char* snid = (char*) reg->value;
    fprintf(COM_D, "%s[%s] mtq_heartbeat: \"%s\"\n", KYEL, NODE_LBL, snid);
    int1 heartbeat = strncmp(snid, "TAD102063", reg->value_len) == 0; 
    
    memset(snid, 0, reg->value_len);
    mtq_read_start(mtq, MTQ_SNID);
    return heartbeat;
}

void mtq_reset(mtq_s* mtq) {
    uint8_t req = 1;
    mtq_write_start(mtq, MTQ_NVM, &req);
}

// NOT WORKING. may need to use state machine to read 1 reg per superloop iteration.
void mtq_read_all(mtq_s* mtq) {
    uint8_t i;
    for (i = 0; i < MTQ_REG_TABLE_LEN; i++) {
        mtq_read_start(mtq, &mtq->reg_table[i]);
    }
}

void mtq_read_fast(mtq_s* mtq) {
    uint8_t i;
    for (i = 0; i < MTQ_NUM_FAST_REGS; i++) {
        mtq_read_start(mtq, MTQ_FAST_FRAME_REGS[i]);
    }
}

void mtq_read_ctrl(mtq_s* mtq) {
    uint8_t i;
    for (i = 0; i < MTQ_NUM_CTRL_REGS; i++) {
        mtq_read_start(mtq, MTQ_CTRL_FRAME_REGS[i]);
    }
}

void mtq_set_date_time(mtq_s* mtq, struct_tm rtc) {
    uint8_t date[4]; 
    date[3] = to_bcd(rtc.tm_year);
    date[2] = to_bcd(rtc.tm_mon);
    date[1] = to_bcd(rtc.tm_mday);
    date[0] = to_bcd(rtc.tm_wday); 
    mtq_write_start(mtq, MTQ_DATE, date);
    
    uint8_t time[4];
    time[3] = to_bcd(rtc.tm_hour);
    time[2] = to_bcd(rtc.tm_min);
    time[1] = to_bcd(rtc.tm_sec);
    time[0] = to_bcd(0); // Unused
    mtq_write_start(mtq, MTQ_TIME, time);
}

void mtq_set_conf(mtq_s* mtq, uint8_t elevation, uint8_t mode) {
    uint8_t conf[4];
    conf[3] = elevation;
    conf[2] = 0; // Unused
    conf[1] = 0; // Unused
    conf[0] = mode | (1 << 7);
    mtq_write_start(mtq, MTQ_CONF, conf);
}
