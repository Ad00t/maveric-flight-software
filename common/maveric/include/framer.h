#ifndef __FRAMER_H__
#define __FRAMER_H__

#include <stdint.h>
#include "ringbuf.h"

#define FRAME_MAX_SIZE          RINGBUF_MAX_SIZE - 1

#define CSP_HEADER_SIZE         4
#define CSP_NORMAL_PRIORITY     2L
#define FSW_NODE                8L
#define TX_NODE                 0L
#define GOMSPACE_NODE           5L
// mimic csp lib default (cnofig's port_max_bind)
#define MIN_PING_SRC_PORT       24L
#define GND_WDT_RESET_PORT      9L

#define KISS_HEADER_SIZE        2
#define KISS_FOOTER_SIZE        1 

#define FEND                    0xC0

#define DATA_FRAME              0x00
#define FESC                    0xDB
#define TFEND                   0xDC
#define TFESC                   0xDD

// FRAMER

// Create a frame buffer for transmission of an arbitrary message, with optional csp headers. Returns length of frame, max of FRAME_MAX_SIZE
uint16_t framer_create(uint8_t* msg, uint16_t msg_len, uint8_t* frame, int1 csp);

// KISS 

typedef enum {
    KISS_WAIT_FEND = 0,
    KISS_IN_FRAME,
    KISS_IN_ESCAPE
} kiss_fsm_e;

typedef struct {
    uint8_t buf[FRAME_MAX_SIZE];
    uint16_t i_start;
    uint16_t buf_len;
    kiss_fsm_e fsm;
} kiss_parser_s;

int1 kiss_process_byte(kiss_parser_s* parser, uint8_t byte);
void kiss_prepend_header(uint8_t* msg, uint16_t* msgLength);
void kiss_append_footer(uint8_t* msg, uint16_t* msgLength);
void kiss_apply_byte_check(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength, uint16_t msgStartIdx);
void kiss_remove_byte_check(uint8_t* buf, uint16_t frameLength, uint8_t* msg, uint16_t* msgLength, uint16_t frameStartIdx);

// CSPHEADER

typedef struct {
	uint8_t crc : 1;
	uint8_t rdp : 1;
	uint8_t xtea : 1;
	uint8_t hmac : 1;
	uint8_t reserved : 4;
	uint8_t srcPort : 6;
	uint8_t dstPort : 6;
	uint8_t dstNode : 5;
	uint8_t srcNode : 5;
	uint8_t priority : 2;
} CSPHeaderBitMap;

typedef struct {
	uint32_t value;
} CSPHeader;

void addCspHeader(uint8_t* msg, uint16_t* msgLength, uint16_t startLocation);
void addCspHeaderWdtReset(uint8_t* msg, uint16_t* msgLength, uint16_t startLocation);
void setupWdtReset(uint8_t* msg, uint16_t* msgLength);

// INET 

uint32_t htonl(uint32_t hostLong);
uint16_t htons(uint16_t hostShort);
uint32_t ntohl(uint32_t netLong);
uint16_t ntohs(uint16_t netShort);

#endif
