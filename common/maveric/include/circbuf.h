#ifndef __CIRCBUF_H__
#define __CIRCBUF_H__

#include <stdint.h>

#define CIRCBUF_MAX_SIZE    256 

typedef struct {
    uint16_t w, r;
    uint8_t data[CIRCBUF_MAX_SIZE];
} circbuf_s;

// Initialize a circular buffer
void cb_init(circbuf_s* buf);

// Clears and resets a circular buffer
void cb_clear(circbuf_s* buf);

// Calculate the number of bytes in the buffer
uint16_t cb_len(circbuf_s* buf);

// Get byte at offset without advancing pointers
int1 cb_peek(circbuf_s* buf, uint16_t off, uint8_t* out);

// Push a byte into the buffer
int1 cb_push(circbuf_s* buf, uint8_t b);

// Pop n bytes from the buffer; pass NULL for out to just drop n bytes
int1 cb_pop(circbuf_s* buf, uint16_t n, uint8_t* out);

#endif
