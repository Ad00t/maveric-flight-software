#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <stdint.h>

#define RINGBUF_MAX_SIZE    256 

typedef struct {
    volatile uint16_t r;
    volatile uint16_t w;
    volatile uint8_t data[RINGBUF_MAX_SIZE];
} ringbuf_s;

// Initialize a circular buffer
void rb_init(ringbuf_s* buf);

// Clears and resets a circular buffer
void rb_clear(ringbuf_s* buf);

// Calculate the number of bytes in the buffer
uint16_t rb_len(ringbuf_s* buf);

// Get byte at offset without advancing pointers
int1 rb_peek(ringbuf_s* buf, uint16_t off, uint8_t* out);

// Push a byte into the buffer
int1 rb_push(ringbuf_s* buf, uint8_t b);

// Push n bytes into the buffer
void rb_push_n(ringbuf_s* buf, uint8_t* in, uint16_t n);

// Pop n bytes from the buffer; pass NULL for out to just drop n bytes
int1 rb_pop(ringbuf_s* buf, uint16_t n, uint8_t* out);

#endif
