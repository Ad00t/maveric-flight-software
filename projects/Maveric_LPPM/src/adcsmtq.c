#include "adcsmtq.h"
#include <stdint.h>
#include <stdio.h>

#module

// HELPERS

void write_buf(ADCSMTQ* a, uint8_t* buf, uint8_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        fputc(buf[i], TAD102063_PORT);
    }
}

void read_buf(ADCSMTQ* a, uint8_t* buf, size_t len) {
    size_t i = 0;
    for (i = 0; i < len; i++) {
        buf[i] = fgetc(TAD102063_PORT);
    }
}

uint16_t sum_buf(uint8_t* buf, size_t len) {
    uint16_t sum = 0;
    size_t i;
    for (i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}

uint8_t gen_csum(uint8_t* buf, size_t len) {
    uint16_t sum = sum_buf(buf, len);
    return 0xFF - sum + 1;
}

uint8_t verify_csum(uint8_t* buf, size_t len) {
    uint16_t sum = sum_buf(buf, len);
    return sum == 0;
}   

// API

void ADCSMTQ_init(ADCSMTQ* a, uint8_t port) {
    a->port = port;
}

void ADCSMTQ_read_start(ADCSMTQ* a, ADCSMTQ_Reg reg) {
    uint8_t w_buf[4];
    w_buf[0] = ADCSMTQ_HEAD_READ;
    w_buf[1] = reg.idx;
    w_buf[2] = reg.data_count;
    w_buf[3] = (reg.map_idx << 4) | 0;
    uint8_t csum = gen_csum(w_buf, 4);
    write_buf(a, w_buf, 4); 
    write_buf(a, &csum, 1);
    delay_ms(1);
}

void ADCSMTQ_read_complete(ADCSMTQ* a, char* rcv_buf, void* data, uint8_t* status) {
    uint8_t r_buf[BUF_MAX_LEN];
    size_t r_buf_len = 4 + 4*reg.data_count + 1;
    read_buf(a, r_buf, r_buf_len);
    memcpy(data, r_buf + 4, 4*reg.data_count);
    *status = verify_csum(r_buf, r_buf_len); 
}

void ADCSMTQ_write_start(ADCSMTQ* a, ADCSMTQ_Reg reg, void* data) {
    static const uint8_t head = (ADCSMTQ_FACTORY_ID << 1 | 0);
    uint8_t w_buf[BUF_MAX_LEN];
    size_t w_buf_len = 4 + 4*reg.data_count;
    w_buf[0] = ADCSMTQ_HEAD_WRITE;
    w_buf[1] = reg.idx;
    w_buf[2] = reg.data_count;
    w_buf[3] = (reg.map_idx << 4) | 0;
    memcpy(w_buf + 4, data, 4*reg.data_count);
    uint8_t csum = gen_csum(w_buf, w_buf_len);
    write_buf(a, w_buf, w_buf_len);
    write_buf(a, &csum, 1);
}

void ADCSMTQ_write_complete(ADCSMTQ* a, char* rcv_buf, uint8_t* status) {
    uint8_t r_buf[5];
    read_buf(a, r_buf, 5);
    *status = verify_csum(r_buf, 5);
}
