#include "adcsmtq.h"
#include "interrupts.h"
#include "uart.h"
#include <stdint.h>
#include <stdlibm.h>

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

// ADCSMTQ Packet

void adcsmtq_pkt_init(adcsmtq_pkt_s* pkt) {
    adcsmtq_pkt_clear(pkt);
}

void adcsmtq_pkt_clear(adcsmtq_pkt_s* pkt) {
    pkt->fsm = ADCSMTQ_FSM_HEAD;
    pkt->i_args = 0;
    pkt->head = 0;
    pkt->idx = 0;
    pkt->cnt = 0;
    pkt->midx = 0;
    pkt->err = 0;
    memset(pkt->data, 0, MAX_BUF_LEN);
    pkt->csum = 0;
}

int1 adcsmtq_pkt_verify_csum(adcsmtq_pkt_s* pkt) {
    uint8_t sum = pkt->head + pkt->idx + pkt->cnt + ((pkt->midx << 4) | pkt->err);
    if (pkt->head == ADCSMTQ_HEAD_READ) sum += sum_buf(pkt->data, 4*pkt->cnt);
    sum += pkt->csum;
    return sum == 0;
}

// ADCSMTQ API 

void adcsmtq_init(adcsmtq_s* a, uint8_t port) {
    a->port = port;
    memcpy(a->reg_table, ADCSMTQ_INIT_REG_TABLE, sizeof(ADCSMTQ_INIT_REG_TABLE));
    memset(a->reg_idx_map, 0, ADCSMTQ_MAP_COUNT * ADCSMTQ_MAX_IDX_COUNT * sizeof(adcsmtq_reg_s*));
    ht_init(&a->reg_name_map);
    adcsmtq_pkt_init(&a->rcvpkt);

    // Init idx, name maps, register space
    uint8_t i;
    for (i = 0; i < ADCSMTQ_REG_TABLE_LEN; i++) {
        adcsmtq_reg_s* reg = &a->reg_table[i];
       
        // Populate idx map
        if (reg->midx < ADCSMTQ_MAP_COUNT && reg->idx < ADCSMTQ_MAX_IDX_COUNT)
            a->reg_idx_map[reg->midx][reg->idx] = reg;

        // Populate string name hash map
        ht_set(&a->reg_name_map, reg->name, reg);
       
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
        
//        fprintf(COM_D, "%s %u (%u,%u)\n", reg->name, h, reg->midx, reg->idx);
    }

    fprintf(COM_D, "%s[LPPM] ADCSMTQ initialized on UART%u\n", KWHT, a->port);
}

void adcsmtq_destroy(adcsmtq_s* a) {
    uint8_t i;
    for (i = 0; i < ADCSMTQ_REG_TABLE_LEN; i++) {
        free(a->reg_table[i].value);
    }

    adcsmtq_pkt_clear(&a->rcvpkt);
    ht_clear(&a->reg_name_map);
    memset(a->reg_idx_map, 0, ADCSMTQ_MAP_COUNT * ADCSMTQ_MAX_IDX_COUNT * sizeof(adcsmtq_reg_s*));
    memset(a->reg_table, 0, sizeof(ADCSMTQ_INIT_REG_TABLE));
}

adcsmtq_reg_s* adcsmtq_get_reg_by_name(adcsmtq_s* a, char* name) {
    return ht_get(&a->reg_name_map, name);
}

adcsmtq_reg_s* adcsmtq_get_reg_by_idx(adcsmtq_s* a, uint8_t midx, uint8_t idx) {
    if (midx >= ADCSMTQ_MAP_COUNT || idx >= ADCSMTQ_MAX_IDX_COUNT)
        return NULL;
    adcsmtq_reg_s* reg = a->reg_idx_map[midx][idx];
    if (reg != NULL && (reg->midx != midx || reg->idx != idx))
        return NULL;
    return reg;
}

void adcsmtq_read_start(adcsmtq_s* a, adcsmtq_reg_s* reg) {   
    uint8_t w_buf[4];
    w_buf[0] = ADCSMTQ_HEAD_READ;
    w_buf[1] = reg->idx;
    w_buf[2] = reg->cnt;
    w_buf[3] = (reg->midx << 4) | 0;
    uint8_t csum = gen_csum(w_buf, 4);
    
    fprintf(COM_D, "%s[LPPM] adcsmtq_read_start: port=UART%u reg='%s' data=[ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]\n", 
            KYEL, a->port, reg->name, w_buf[0], w_buf[1], w_buf[2], w_buf[3], csum);
    
    uart_write_buf(a->port, w_buf, 4); 
    uart_write_byte(a->port, csum);
    delay_ms(10);
}

void adcsmtq_read_start(adcsmtq_s* a, char* name) {
    adcsmtq_reg_s* reg = adcsmtq_get_reg_by_name(a, name);
    if (reg == NULL) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_read_start: register invalid '%s'\n", KRED, name);
        return;
    }
    adcsmtq_read_start(a, reg);
}

void adcsmtq_read_complete(adcsmtq_s* a, adcsmtq_pkt_s* rcvpkt) {
    if (!adcsmtq_pkt_verify_csum(rcvpkt)) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_read_complete: ppm checksum error (%u,%u)\n", KRED, rcvpkt->midx, rcvpkt->idx);
        return;
    }

    switch (rcvpkt->err) {
        case 0: break; // No error
        case 1: // Checksum error
            fprintf(COM_D, "%s[LPPM] adcsmtq_read_complete: rcv checksum error (%u,%u)\n", KRED, rcvpkt->midx, rcvpkt->idx);
            return;
        case 2: // Invalid register
            fprintf(COM_D, "%s[LPPM] adcsmtq_read_complete: rcv invalid register (%u,%u)\n", KRED, rcvpkt->midx, rcvpkt->idx);
            return;
    }

    adcsmtq_reg_s* reg = adcsmtq_get_reg_by_idx(a, rcvpkt->midx, rcvpkt->idx);
    if (reg == NULL) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_read_complete: ppm invalid register (%u,%u)\n", KRED, rcvpkt->midx, rcvpkt->idx);
        return;
    }
        
    uint8_t n_body_bytes = 4*rcvpkt->cnt;
    memcpy(reg->value, rcvpkt->data, n_body_bytes);
    
    fprintf(COM_D, "%s[LPPM] adcsmtq_read_complete port=UART%u reg='%s' idx=%u count=%u midx=%u err=%u data=[",
            KYEL, a->port, reg->name, reg->idx, reg->cnt, reg->midx, rcvpkt->idx);

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

void adcsmtq_write_start(adcsmtq_s* a, adcsmtq_reg_s* reg, void* data) {
    uint8_t w_buf[MAX_BUF_LEN];
    uint8_t n_body_bytes = 4*reg->cnt;
    uint8_t w_buf_len = 4 + n_body_bytes;
    w_buf[0] = ADCSMTQ_HEAD_WRITE;
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
    
    fprintf(COM_D, "%s[LPPM] adcsmtq_write_start: port=%u reg='%s' len=%u data=[", 
            KYEL, a->port, reg->name, w_buf_len+1);
    for (i = 0; i < w_buf_len; i++)
        fprintf(COM_D, " 0x%02X", w_buf[i]);
    fprintf(COM_D, " 0x%02X ]\n", csum);
    
    uart_write_buf(a->port, w_buf, w_buf_len);
    uart_write_buf(a->port, &csum, 1);
    delay_ms(10);
}

void adcsmtq_write_start(adcsmtq_s* a, char* name, void* data) {
    adcsmtq_reg_s* reg = adcsmtq_get_reg_by_name(a, name);
    if (reg == NULL) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_write_start: register invalid '%s'\n", KRED, name);
        return;
    }
    adcsmtq_write_start(a, reg, data);
}

void adcsmtq_write_complete(adcsmtq_s* a, adcsmtq_pkt_s* rcvpkt) {
    if (!adcsmtq_pkt_verify_csum(rcvpkt)) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_write_complete: ppm checksum error (%u,%u)\n", KRED, rcvpkt->midx, rcvpkt->idx);
        return;
    }

    switch (rcvpkt->err) {
        case 0: break; // No error
        case 1: // Checksum error
            fprintf(COM_D, "%s[LPPM] adcsmtq_write_complete: rcv checksum error (%u,%u)\n", KRED, rcvpkt->midx, rcvpkt->idx);
            return;
        case 2: // Invalid register
            fprintf(COM_D, "%s[LPPM] adcsmtq_write_complete: rcv invalid register (%u,%u)\n", KRED, rcvpkt->midx, rcvpkt->idx);
            return;
    }

    adcsmtq_reg_s* reg = adcsmtq_get_reg_by_idx(a, rcvpkt->midx, rcvpkt->idx);
    if (reg == NULL) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_write_complete: ppm invalid register (%u,%u)\n", KRED, rcvpkt->midx, rcvpkt->idx);
        return;
    }
    
    reg->dirty = TRUE;
    
    fprintf(COM_D, "%s[LPPM] adcsmtq_write_complete: port=%u reg='%s' idx=%u count=%u midx=%u err=%u\n", 
            KYEL, a->port, reg->name, reg->idx, reg->cnt, reg->midx, rcvpkt->err);
}

void adcsmtq_readback(adcsmtq_s* a) {   
    uint8_t i;
    for (i = 0; i < ADCSMTQ_REG_TABLE_LEN; i++) {
        if (a->reg_table[i].dirty) {
            adcsmtq_read_start(a, &a->reg_table[i]);
            a->reg_table[i].dirty = FALSE;
        }
    }
}

void adcsmtq_rcv_fsm(adcsmtq_s* a, circbuf_s* irqbuf) {
    adcsmtq_pkt_s* rcvpkt = &a->rcvpkt;
    uint16_t iter = 0;
    while (iter < CIRCBUF_MAX_SIZE) {
        uint8_t b;
        if (!cb_pop(irqbuf, 1, &b)) return;
      
        switch (rcvpkt->fsm) {
            case ADCSMTQ_FSM_HEAD:
                if (b == ADCSMTQ_HEAD_READ || b == ADCSMTQ_HEAD_WRITE) {
                    rcvpkt->head = b;
                    rcvpkt->fsm = ADCSMTQ_FSM_IDX;
                } 
                break;

            case ADCSMTQ_FSM_IDX:
                rcvpkt->idx = b;
                rcvpkt->fsm = ADCSMTQ_FSM_CNT;
                break;

            case ADCSMTQ_FSM_CNT:
                rcvpkt->cnt = b;
                rcvpkt->fsm = ADCSMTQ_FSM_MIDXERR;
                break;

            case ADCSMTQ_FSM_MIDXERR:
                rcvpkt->midx = b >> 4;
                rcvpkt->err = b & 0b1111; 
                switch (rcvpkt->head) {
                    case ADCSMTQ_HEAD_WRITE: rcvpkt->fsm = ADCSMTQ_FSM_CSUM; break;
                    case ADCSMTQ_HEAD_READ: rcvpkt->fsm = ADCSMTQ_FSM_DATA; break;
                    default: rcvpkt->fsm = ADCSMTQ_FSM_ERROR; break;
                }
                break;

            case ADCSMTQ_FSM_DATA:
                if (rcvpkt->i_args >= MAX_BUF_LEN) {
                    rcvpkt->fsm = ADCSMTQ_FSM_ERROR;
                    break;
                }
                rcvpkt->data[rcvpkt->i_args++] = b;
                if (rcvpkt->i_args >= 4*rcvpkt->cnt) {
                    rcvpkt->fsm = ADCSMTQ_FSM_CSUM;
                }
                break;

            case ADCSMTQ_FSM_CSUM:
                rcvpkt->csum = b;
                rcvpkt->fsm = ADCSMTQ_FSM_DONE;
                break;
        } 

        // Could wait till next superloop iteration or just handle end states immediately (currently doing the latter)
        switch (rcvpkt->fsm) {
            case ADCSMTQ_FSM_DONE:
                switch (rcvpkt->head) {
                    case ADCSMTQ_HEAD_READ: adcsmtq_read_complete(a, rcvpkt); break;
                    case ADCSMTQ_HEAD_WRITE: adcsmtq_write_complete(a, rcvpkt); break;
                }
                adcsmtq_pkt_clear(rcvpkt);
                break;
            case ADCSMTQ_FSM_ERROR:
                fprintf(COM_D, "%s[LPPM] adcsmtq_rcv_fsm: malformed packet\n", KRED);
                adcsmtq_pkt_clear(rcvpkt);
                break;
        }

        iter++;
    }
}

