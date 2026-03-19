#include "ringbuf.h"
#include <stdint.h>

#module

// Initialize a circular buffer
void rb_init(ringbuf_s* buf) {
    rb_clear(buf);
}

// Clears and resets a circular buffer
void rb_clear(ringbuf_s* buf) {
    buf->w = 0;
    buf->r = 0;
    memset(buf->data, 0, RINGBUF_SIZE);
}

// Calculate the number of bytes in the buffer
uint16_t rb_len(ringbuf_s* buf) {
    if (buf->w >= buf->r) return buf->w - buf->r;
    return RINGBUF_SIZE - (buf->r - buf->w);
}

// Get byte at offset without advancing pointers
int1 rb_peek(ringbuf_s* buf, uint16_t off, uint8_t* out) {
    if (off >= rb_len(buf)) return 0;
    uint16_t i = (buf->r + off) % RINGBUF_SIZE;
    *out = buf->data[i];
    return 1;
}

// Push a byte into the buffer
int1 rb_push(ringbuf_s* buf, uint8_t b) {
    uint16_t i = (buf->w + 1) % RINGBUF_SIZE;
    if (i == buf->r) return 0;
    buf->data[buf->w] = b;
    buf->w = i;
    return 1;
}

// Pop n bytes from the buffer; pass NULL for out to just drop n bytes
int1 rb_pop(ringbuf_s* buf, uint16_t n, uint8_t* out) {
    if (n > rb_len(buf)) return 0;
    if (out == NULL) {
        buf->r = (buf->r + n) % RINGBUF_SIZE;
    } else {
        size_t j;
        for (j = 0; j < n; j++) {
            out[j] = buf->data[buf->r]; 
            buf->r = (buf->r + 1) % RINGBUF_SIZE;
        }
    }
    return 1;
}
