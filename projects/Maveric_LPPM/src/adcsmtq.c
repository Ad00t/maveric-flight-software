#include "adcsmtq.h"
#include "interrupts.c"
#include "uart.c"
#include <stdint.h>

#module


// HELPERS

// Generate hash of name for reg_name_map
uint8_t hash_name(char* s) {
    uint8_t h = 0;
    while (*s) h = (h * 31u) + (uint8_t)(*s++);
    return h % ADCSMTQ_NAME_HASH_SIZE;
}

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

void ADCSMTQ_init(ADCSMTQ* a, uint8_t port) {
    memset(a, 0, sizeof(ADCSMTQ));
    a->port = port;
    memcpy(a->reg_table, ADCSMTQ_INIT_REG_TABLE, sizeof(ADCSMTQ_INIT_REG_TABLE));

    // Init idx, name maps
    size_t i;
    for (i = 0; i < ADCSMTQ_REG_TABLE_LEN; i++) {
        const ADCSMTQ_Reg* r = &a->reg_table[i];
        if (r->map_idx < ADCSMTQ_MAP_COUNT && r->idx < ADCSMTQ_MAX_IDX_COUNT)
            a->reg_idx_map[r->map_idx][r->idx] = r;
        uint8_t h = hash_name(r->name);
        a->reg_name_map[h] = r;
    }

    // Initialize buffers for register values
    for (int i = 0; i < ADCSMTQ_REG_TABLE_LEN; i++) {
        ADCSMTQ_Reg* r = &a->reg_table[i];
        switch (r->type) {
            case T_UINT8:
                r->value_len = 4*r->data_count / sizeof(uint8_t);
                r->value = calloc(r->value_len, sizeof(uint8_t));
                break;
            case T_INT8:
                r->value_len = 4*r->data_count / sizeof(int8_t);
                r->value = calloc(r->value_len, sizeof(int8_t));
                break;
            case T_UINT16:
                r->value_len = 4*r->data_count / sizeof(uint16_t);
                r->value = calloc(r->value_len, sizeof(uint16_t));
                break;
            case T_INT16:
                r->value_len = 4*r->data_count / sizeof(int16_t);
                r->value = calloc(r->value_len, sizeof(int16_t));
                break;
            case T_FLOAT:
                r->value_len = 4*r->data_count / sizeof(float);
                r->value = calloc(r->value_len, sizeof(float));
                break;
            case T_CHAR:
                r->value_len = 4*r->data_count / sizeof(char);
                r->value = calloc(r->value_len, sizeof(char));
                break;
        }
    }
}

ADCSMTQ_Reg* ADCSMTQ_get_reg_by_name(ADCSMTQ* a, char* name) {
    uint8_t h = hash_name(name);
    ADCSMTQ_Reg* r = a->reg_name_map[h];
    if (r && strcmp(r->name, name) == 0)
        return r;
    // could extend for collision resolution
    return NULL;
}

void ADCSMTQ_read_start(ADCSMTQ* a, char* name) {
    ADCSMTQ_Reg* reg = ADCSMTQ_get_reg_by_name(a, name);
    uint8_t w_buf[4];
    w_buf[0] = ADCSMTQ_HEAD_READ;
    w_buf[1] = reg.idx;
    w_buf[2] = reg.data_count;
    w_buf[3] = (reg.map_idx << 4) | 0;
    uint8_t csum = gen_csum(w_buf, 4);
    uart_write_buf(a->port, w_buf, 4); 
    uart_write_buf(a->port, &csum, 1);
    delay_ms(1);
}

void ADCSMTQ_read_complete(ADCSMTQ* a, uint8_t* status) {
    uint8_t idx = INTERRUPT_RCV_BUF[1]
    uint8_t data_count = INTERRUPT_RCV_BUF[2];
    uint8_t map_idx = INTERRUPT_RCV_BUF[3] >> 4;
    uint8_t error_code = INTERRUPT_RCV_BUF[3] & 0b1111;
    uint8_t* body = &INTERRUPT_RCV_BUF[4];
    ADCSMTQ_Reg* reg = a->reg_idx_map[map_idx][idx];
    
    size_t n_body_bytes = 4*reg->data_count;
    size_t i;
    switch (reg->type) {
        case T_UINT8: {
            uint8_t* dst = (uint8_t*)reg->value;
            for (i = 0; i < n_body_bytes; i++)
                dst[i] = body[i];
            break;
        }
        case T_INT8: {
            int8_t* dst = (int8_t*)reg->value;
            for (i = 0; i < n_body_bytes; i++)
                dst[i] = (int8_t)body[i];
            break;
        }
        case T_UINT16: {
            uint16_t* dst = (uint16_t*)reg->value;
            size_t elems = n_body_bytes / 2;
            for (i = 0; i < elems; i++)
                dst[i] = ((uint16_t)body[2*i+1] << 8) | body[2*i];
            break;
        }
        case T_INT16: {
            int16_t* dst = (int16_t*)reg->value;
            size_t elems = n_body_bytes / 2;
            for (i = 0; i < elems; i++)
                dst[i] = ((int16_t)body[2*i+1] << 8) | body[2*i];
            break;
        }
        case T_FLOAT: {
            float* dst = (float*)reg->value;
            size_t elems = n_body_bytes / 4;
            for (i = 0; i < elems; i++)
                memcpy(&dst[i], &body[4*i], 4);
            break;
        }
        case T_CHAR: {
            memcpy(reg->value, body, n_body_bytes);
            ((char*)reg->value)[n_body_bytes] = '\0'; // Null terminate
            break;
        }
    }    
    *status = (verify_csum(r_buf, r_buf_len) << 4) | error_code; 
}

void ADCSMTQ_write_start(ADCSMTQ* a, char* name, void* data) {
    ADCSMTQ_Reg* reg = ADCSMTQ_get_reg_by_name(a, name);
    if (reg == NULL) return;

    uint8_t w_buf[MAX_BUF_LEN];
    size_t n_body_bytes = 4*reg->data_count;
    size_t w_buf_len = 4 + n_body_bytes;
    w_buf[0] = ADCSMTQ_HEAD_WRITE;
    w_buf[1] = reg->idx;
    w_buf[2] = reg->data_count;
    w_buf[3] = (reg.map_idx << 4) | 0;
    
    uint8_t body[MAX_BUF_LEN];
    size_t i;
    switch (reg->type) {
        case T_UINT8:
            for (i = 0; i < n_body_bytes; i++) 
                body[i] = (uint8_t)data[i];
            break; 
        case T_INT8:
            for (i = 0; i < n_body_bytes; i++) 
                body[i] = (uint8_t)data[i];
            break;
        case T_UINT16:
            for (i = 0; i < n_body_bytes; i+=2) {
                body[i] = (uint8_t)data[i/2]; // Extract least significant byte
                body[i+1] = (uint8_t)(data[i/2] >> 8); // Extract most significant byte
            }
            break;
        case T_INT16:
            for (i = 0; i < n_body_bytes; i+=2) {
                body[i] = (uint8_t)data[i/2]; // Extract least significant byte
                body[i+1] = (uint8_t)(data[i/2] >> 8); // Extract most significant byte
            }
            break;
        case T_FLOAT:
            for (i = 0; i < n_body_bytes; i+=4)
                memcpy(&body[i], &data[i/4], 4);
            break;
        case T_CHAR:
            memcpy(body, data, n_body_bytes);
            body[n_body_bytes] = (uint8_t)('\0'); // Null terminate
            break;
    }
    memcpy(&w_buf[4], body, n_body_bytes);

    uint8_t csum = gen_csum(w_buf, w_buf_len);
    uart_write_buf(a->port, w_buf, w_buf_len);
    uart_write_buf(a->port, &csum, 1);
}

void ADCSMTQ_write_complete(ADCSMTQ* a, uint8_t* status) {
    uint8_t error_code = INTERRUPT_RCV_BUF[3] & 0b1111;
    *status = (verify_csum(r_buf, r_buf_len) << 4) | error_code; 
}
