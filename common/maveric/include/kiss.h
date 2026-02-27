#ifndef __KISS_H__
#define __KISS_H__

#include <stdint.h>

#define KISS_HEADER_SIZE    2
#define KISS_FOOTER_SIZE    1 

#define FEND                    0xC0

#define DATA_FRAME              0x00
#define FESC                    0xDB
#define TFEND                   0xDC
#define TFESC                   0xDD

void kiss_prepend_header(uint8_t* msg, uint16_t* msgLength);

void kiss_append_footer(uint8_t* msg, uint16_t* msgLength);

void kiss_apply_byte_check(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength, uint16_t msgStartIdx);

void kiss_remove_byte_check(uint8_t* frame, uint16_t frameLength, uint8_t* msg, uint16_t* msgLength, uint16_t frameStartIdx);

#endif

