#include "tad102063.h"
#include <stdint.h>
#include <stdio.h>

#module

void TAD102063_init(TAD102063* t, unsigned int8 port) {
    t->port = port;
}

void write_buf(TAD102063* t, const uint8_t* buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        fputc(buf[i], t->port);
    }
}

uint16_t sum_buf(const uint8_t* buf, size_t len) {
    uint16_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += buf[i];
    }
}

uint8_t gen_csum(const uint8_t* buf, size_t len) {
    uint16_t sum = sum_buf(buf, len);
    return 0xFF - sum + 1;
}

int1 verify_csum(uint8_t* buf, size_t len) {
    uint16_t sum = sum_buf(buf, len);
    return sum == 0;
}

void TAD102063_read_reg(TAD102063* t, TAD102063_Reg reg, void* data, TAD102063_RW_Status* status) {
    static const uint8_t head = (TAD102063_FACTORY_ID << 1) + 1;
    uint8_t buf[] = { head, reg.idx, reg.data_count, (reg.map_idx << 4) | 0x00 };
    size_t buf_len = sizeof(buf)/sizeof(uint8_t);
    uint8_t csum = gen_csum(buf, buf_len);

    write_buf(t->port, buf, buf_len); 
    write_buf(t->port, &csum, 1);
    delay_ms(1);

}

void TAD102063_write_reg(TAD102063* t, TAD102063_Reg reg, void* data, TAD102063_RW_Status* status) {

}   
