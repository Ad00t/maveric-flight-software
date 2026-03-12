#include "common.h"
#include <stdint.h>

#module

uint8_t bcdtohex(uint8_t bcd) {
    uint8_t zerosb, onesb, twosb, threesb;
    int retval;

    zerosb = bcd & 0x000f;
    onesb = (bcd & 0x00f0)>>4;
    twosb = (bcd & 0x0f00)>>8;
    threesb = (bcd & 0xf000)>>12;
    retval = threesb*1000 + twosb*100 + onesb*10 + zerosb;
    return retval;
}

uint8_t hextobcd(uint8_t hex) {
    uint8_t y;
    y = (hex / 10) <<4;
    y = y | (hex %10);
    return (y);
}

