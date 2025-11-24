#include "adcsmtq.h"
#include "interrupts.h"
#include "uart.h"
#include <stdint.h>

#module

// HELPERS

// Compute sum of all bytes in buf
uint16_t sum_buf(uint8_t* buf, size_t len) {
    uint16_t sum = 0;
    size_t i;
    for (i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}

// Generate checksum (two's complement of buf sum, so original sum + checksum should = 0)
uint8_t gen_csum(uint8_t* buf, size_t len) {
    uint16_t sum = sum_buf(buf, len);
    return 0xFF - sum + 1;
}

// Check that checksum is valid (buf sum + checksum = 0)
uint8_t verify_csum(uint8_t* buf, size_t len) {
    uint16_t sum = sum_buf(buf, len);
    return sum == 0;
}   

// API

void adcsmtq_init(adcsmtq_s* a, uint8_t port) {
    a->port = port;
    memcpy(a->reg_table, ADCSMTQ_INIT_REG_TABLE, sizeof(ADCSMTQ_INIT_REG_TABLE));
    memset(a->reg_idx_map, 0, ADCSMTQ_MAP_COUNT * ADCSMTQ_MAX_IDX_COUNT * sizeof(adcsmtq_reg_s*));
    ht_init(&a->reg_name_map);

    a->rcv_fsm = ADCSMTQ_FSM_HEAD;
    a->rcv_buf_len = 0;
    memset(a->rcv_buf, 0, MAX_BUF_LEN);

    // Init idx, name maps, register space
    size_t i;
    for (i = 0; i < ADCSMTQ_REG_TABLE_LEN; i++) {
        adcsmtq_reg_s* reg = &a->reg_table[i];
       
        // Populate idx map
        if (reg->map_idx < ADCSMTQ_MAP_COUNT && reg->idx < ADCSMTQ_MAX_IDX_COUNT)
            a->reg_idx_map[reg->map_idx][reg->idx] = reg;

        // Populate string name hash map
        int1 ht_status = ht_set(&a->reg_name_map, reg->name, reg);
       
        // Allocate register data space
        switch (reg->type) {
            case T_UINT8:
                reg->value_len = 4*reg->data_count / sizeof(uint8_t);
                reg->value = calloc(reg->value_len, sizeof(uint8_t));
                break;
            case T_INT8:
                reg->value_len = 4*reg->data_count / sizeof(int8_t);
                reg->value = calloc(reg->value_len, sizeof(int8_t));
                break;
            case T_UINT16:
                reg->value_len = 4*reg->data_count / sizeof(uint16_t);
                reg->value = calloc(reg->value_len, sizeof(uint16_t));
                break;
            case T_INT16:
                reg->value_len = 4*reg->data_count / sizeof(int16_t);
                reg->value = calloc(reg->value_len, sizeof(int16_t));
                break;
            case T_FLOAT:
                reg->value_len = 4*reg->data_count / sizeof(float);
                reg->value = calloc(reg->value_len, sizeof(float));
                break;
            case T_CHAR:
                reg->value_len = 4*reg->data_count / sizeof(char);
                reg->value = calloc(reg->value_len, sizeof(char));
                break;
        }
        
//        fprintf(COM_D, "%s %u (%u,%u)\r\n", reg->name, h, reg->map_idx, reg->idx);
    }

    fprintf(COM_D, "%s[LPPM] ADCSMTQ initialized on UART%u\r\n", KWHT, a->port);
}

void adcsmtq_destroy(adcsmtq_s* a) {
    size_t i;
    for (i = 0; i < ADCSMTQ_REG_TABLE_LEN; i++) {
        free(reg->value);
    }

    memset(a->rcv_buf, 0, MAX_BUF_LEN);
    ht_clear(&a->reg_name_map);
    memset(a->reg_idx_map, 0, ADCSMTQ_MAP_COUNT * ADCSMTQ_MAX_IDX_COUNT * sizeof(adcsmtq_reg_s*));
    memset(a->reg_table, 0, sizeof(ADCSMTQ_INIT_REG_TABLE));
}

adcsmtq_reg_s* adcsmtq_get_reg_by_name(adcsmtq_s* a, char* name) {
    return ht_get(&a->reg_name_map, name);
}

adcsmtq_reg_s* adcsmtq_get_reg_by_idx(adcsmtq_s* a, uint8_t map_idx, uint8_t idx) {
    if (map_idx >= ADCSMTQ_MAP_COUNT || idx >= ADCSMTQ_MAX_IDX_COUNT)
        return NULL;
    adcsmtq_reg_s* reg = a->reg_idx_map[map_idx][idx];
    if (reg != NULL && (reg->map_idx != map_idx || reg->idx != idx))
        return NULL;
    return reg;
}

void adcsmtq_read_start(adcsmtq_s* a, adcsmtq_reg_s* reg) {   
    uint8_t w_buf[4];
    w_buf[0] = ADCSMTQ_HEAD_READ;
    w_buf[1] = reg->idx;
    w_buf[2] = reg->data_count;
    w_buf[3] = (reg->map_idx << 4) | 0;
    uint8_t csum = gen_csum(w_buf, 4);
    
    fprintf(COM_D, "%s[LPPM] adcsmtq_read_start: port=UART%u reg='%s' data=[ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]\r\n", 
            KYEL, a->port, reg->name, w_buf[0], w_buf[1], w_buf[2], w_buf[3], csum);
    
    uart_write_buf(a->port, w_buf, 4); 
    uart_write_byte(a->port, csum);
    delay_ms(10);
}

void adcsmtq_read_start(adcsmtq_s* a, char* name) {
    adcsmtq_reg_s* reg = adcsmtq_get_reg_by_name(a, name);
    if (reg == NULL) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_read_start: register invalid '%s'\r\n", KRED, name);
        return;
    }
    adcsmtq_read_start(a, reg);
}

void adcsmtq_read_complete(adcsmtq_s* a, uint8_t* buf) {
    uint8_t idx = buf[1];
    uint8_t data_count = buf[2];
    uint8_t map_idx = buf[3] >> 4;
    uint8_t error_code = buf[3] & 0b1111;
    uint8_t* body = &buf[4];

    // TODO: verify csum, handle error code

    adcsmtq_reg_s* reg = adcsmtq_get_reg_by_idx(a, map_idx, idx);
    if (reg == NULL) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_read_complete: register invalid (%u,%u)\r\n", KRED, map_idx, idx);
        return;
    }
        
    size_t n_body_bytes = 4*data_count;
    memcpy(reg->value, body, n_body_bytes);
    
    fprintf(COM_D, "%s[LPPM] adcsmtq_read_complete port=UART%u reg='%s' idx=%u count=%u midx=%u err=%u data=[",
            KYEL, a->port, reg->name, idx, data_count, map_idx, error_code);

    size_t i;
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

    fprintf(COM_D, " ]\r\n");
}

void adcsmtq_write_start(adcsmtq_s* a, adcsmtq_reg_s* reg, void* data) {
    uint8_t w_buf[MAX_BUF_LEN];
    size_t n_body_bytes = 4*reg->data_count;
    size_t w_buf_len = 4 + n_body_bytes;
    w_buf[0] = ADCSMTQ_HEAD_WRITE;
    w_buf[1] = reg->idx;
    w_buf[2] = reg->data_count;
    w_buf[3] = (reg->map_idx << 4) | 0;
    
    memcpy(&w_buf[4], data, n_body_bytes);
    uint8_t csum = gen_csum(w_buf, w_buf_len);
    
    size_t i;
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
    fprintf(COM_D, " 0x%02X ]\r\n", csum);
    
    uart_write_buf(a->port, w_buf, w_buf_len);
    uart_write_buf(a->port, &csum, 1);
    delay_ms(10);
}

void adcsmtq_write_start(adcsmtq_s* a, char* name, void* data) {
    adcsmtq_reg_s* reg = adcsmtq_get_reg_by_name(a, name);
    if (reg == NULL) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_write_start: register invalid '%s'\r\n", KRED, name);
        return;
    }
    adcsmtq_write_start(a, reg, data);
}

void adcsmtq_write_complete(adcsmtq_s* a, uint8_t* buf) {
    uint8_t idx = buf[1];
    uint8_t data_count = buf[2];
    uint8_t map_idx = buf[3] >> 4;
    uint8_t error_code = buf[3] & 0b1111;'
    uint8_t csum = buf[4];

    // TODO: verify csum, handle error code

    adcsmtq_reg_s* reg = adcsmtq_get_reg_by_idx(a, map_idx, idx);
    if (reg == NULL) {
        fprintf(COM_D, "%s[LPPM] adcsmtq_write_complete: register invalid (%u,%u)\r\n", KRED, map_idx, idx);
        return;
    }
    reg->dirty = TRUE;
    
    fprintf(COM_D, "%s[LPPM] adcsmtq_write_complete: port=%u reg='%s' idx=%u count=%u midx=%u err=%u\r\n", 
            KYEL, a->port, reg->name, idx, data_count, map_idx, error_code);
}

void adcsmtq_readback(adcsmtq_s* a) {   
    size_t i;
    for (i = 0; i < ADCSMTQ_REG_TABLE_LEN; i++) {
        if (a->reg_table[i].dirty) {
            adcsmtq_read_start(a, &a->reg_table[i]);
            a->reg_table[i].dirty = FALSE;
        }
    }
}

void adcsmtq_rcv_fsm(adcsmtq_s* a, circbuf_s* irqbuf) {
    uint16_t iter = 0;
    while (iter < CIRCBUF_MAX_SIZE) {
        uint8_t b;
        if (!cb_pop(irqbuf, 1, &b)) return;

        switch (a->rcv_fsm) {
            case ADCSMTQ_FSM_HEAD:
                if (b == ADCSMTQ_HEAD_READ || b == ADCSMTQ_HEAD_WRITE) {
                    a->rcv_buf[a->rcv_buf_len++] = b;
                    a->rcv_fsm = ADCSMTQ_FSM_IDX;
                } 
                break;

            case ADCSMTQ_FSM_IDX:
                a->rcv_buf[a->rcv_buf_len++] = b;
                a->rcv_fsm = ADCSMTQ_FSM_CNT;
                break;

            case ADCSMTQ_FSM_CNT:
                a->rcv_buf[a->rcv_buf_len++] = b;
                a->rcv_fsm = ADCSMTQ_FSM_MIDX;
                break;

            case ADCSMTQ_FSM_MIDX:
                a->rcv_buf[a->rcv_buf_len++] = b;
                switch (a->rcv_buf[0]) {
                    case 0xC8: a->rcv_fsm = ADCSMTQ_FSM_CSUM; break;
                    case 0xC9: a->rcv_fsm = ADCSMTQ_FSM_DATA; break;
                    default: a->rcv_fsm = ADCSMTQ_FSM_ERROR; break;
                }
                break;

            case ADCSMTQ_FSM_DATA:
                a->rcv_buf[a->rcv_buf_len++] = b;
                if (a->rcv_buf_len >= MAX_BUF_LEN) {
                    a->rcv_fsm = ADCSMTQ_FSM_ERROR;
                } else if (a->rcv_buf_len >= 4+4*a->rcv_buf[2]) {
                    a->rcv_fsm = ADCSMTQ_FSM_CSUM;
                }
                break;

            case ADCSMTQ_FSM_CSUM:
                a->rcv_buf[a->rcv_buf_len++] = b;
                a->rcv_fsm = ADCSMTQ_FSM_DONE;
                break;
        } 

        // Could wait till next superloop iteration or just handle end states immediately (currently doing the latter)
        switch (a->rcv_fsm) {
            case ADCSMTQ_FSM_DONE:
                switch (a->rcv_buf[0]) {
                    case ADCSMTQ_HEAD_READ: adcsmtq_read_complete(a, buf); break;
                    case ADCSMTQ_HEAD_WRITE: adcsmtq_write_complete(a, buf); break;
                }
            // Fall-through to reset rcv buf & FSM
            case ADCSMTQ_FSM_ERROR:
                a->rcv_fsm = ADCSMTQ_FSM_HEAD;
                a->rcv_buf_len = 0;
                memset(a->rcv_buf, 0, MAX_BUF_LEN);
                break;
        }

        iter++;
    }
}

