#include "circbuf.h"
#include <stdint.h>

#module

// Initialize a circular buffer
void cb_init(circbuf_s* buf) {
    cb_clear(buf);
}

// Clears and resets a circular buffer
void cb_clear(circbuf_s* buf) {
    buf->w = 0;
    buf->r = 0;
    memset(buf->data, 0, CIRCBUF_MAX_SIZE);
}

// Calculate the number of bytes in the buffer
uint16_t cb_len(circbuf_s* buf) {
    if (buf->w >= buf->r) return buf->w - buf->r;
    return CIRCBUF_MAX_SIZE - (buf->r - buf->w);
}

// Get byte at offset without advancing pointers
int1 cb_peek(circbuf_s* buf, uint8_t off, uint8_t* out) {
    if (off >= cb_len(buf)) return 0;
    uint8_t i = (buf->r + off) % CIRCBUF_MAX_SIZE;
    *out = buf->data[i];
    return 1;
}

// Push a byte into the buffer
int1 cb_push(circbuf_s* buf, uint8_t b) {
    uint16_t i = (buf->w + 1) % CIRCBUF_MAX_SIZE;
    if (i == buf->r) return 0;
    buf->data[buf->w] = b;
    buf->w = i;
    return 1;
}

// Pop n bytes from the buffer; pass NULL for out to just drop n bytes
int1 cb_pop(circbuf_s* buf, uint16_t n, uint8_t* out) {
    if (n > cb_len(buf)) return 0;
    if (out == NULL) {
        buf->r = (buf->r + n) % CIRCBUF_MAX_SIZE;
    } else {
        size_t j;
        for (j = 0; j < n; j++) {
            out[j] = buf->data[buf->r]; 
            buf->r = (buf->r + 1) % CIRCBUF_MAX_SIZE;
        }
    }
    return 1;
}
