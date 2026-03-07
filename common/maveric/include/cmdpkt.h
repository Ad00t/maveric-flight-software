#ifndef __CMDPKT_H__
#define __CMDPKT_H__

#include <stdint.h>
#include "ringbuf.h"

#define CMD_MAX_FRAME_SIZE      RINGBUF_MAX_SIZE - 1  
#define CMD_MAX_LEN             CMD_MAX_FRAME_SIZE - KISS_HEADER_SIZE - KISS_FOOTER_SIZE
#define CMD_HEADER_SIZE         6
#define CMD_FOOTER_SIZE         2
#define CMD_MAX_ID_LEN          20
#define CMD_MAX_ARGS_LEN        CMD_MAX_LEN - CMD_MAX_ID_LEN - CMD_HEADER_SIZE - CMD_FOOTER_SIZE - 2

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

// NODE IDs
#define NODE_LPPM        1
#define NODE_EPS         2
#define NODE_UPPM        3
#define NODE_HOLONAV     4
#define NODE_ASTROBOARD  5
#define NODE_GS          6          
#define NODE_FTDI        7

// Command packet & reader FSM

typedef enum {
    ACK = 0,
    REQ = 1,
    RES = 2,
} cmdpkt_type_e;

typedef enum {
    KISS_WAIT_FEND = 0,
    KISS_IN_FRAME,
    KISS_IN_ESCAPE
} kiss_fsm_e;

typedef struct {
    // Packet parsing metadata
    uint8_t buf[CMD_MAX_LEN];
    uint16_t i_start;
    uint16_t buf_len;
    kiss_fsm_e fsm;
    // Packet data
    uint8_t orgn;
    uint8_t dest;
    uint8_t echo;
    cmdpkt_type_e ptype;
    uint8_t id_len;
    uint8_t args_len;
    char* id;
    char* args;
    uint16_t crc;
} cmdpkt_s;

// Initialize this cmdpkt empty
void cmdpkt_init(cmdpkt_s* pkt);

// Initialize this cmdpkt populated with a cmd
void cmdpkt_create(cmdpkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args);

// Clear this cmdpkt
void cmdpkt_clear(cmdpkt_s* pkt);

// Parse packet fields out of its buffer. Requires pkt->buf and pkt->buf_len to be populated.
uint8_t cmdpkt_parse_buf(cmdpkt_s* pkt);

// Create a frame buffer for transmission of a cmdpkt, with optional csp headers. Returns length of frame
uint16_t cmdpkt_setup_frame(cmdpkt_s* pkt, uint8_t frame, uint8_t frame_size, int1 csp);

// Send a command packet along its appropriate route. Assumes pkt buf field is populated correctly.
void cmdpkt_dispatch(cmdpkt_s* pkt);

// Create then send command packet
void cmd_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args);

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

// KISS 

int1 kiss_process_byte(cmdpkt_s* p, uint8_t byte);
void kiss_prepend_header(uint8_t* msg, uint16_t* msgLength);
void kiss_append_footer(uint8_t* msg, uint16_t* msgLength);
void kiss_apply_byte_check(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength, uint16_t msgStartIdx);
void kiss_remove_byte_check(uint8_t* buf, uint16_t frameLength, uint8_t* msg, uint16_t* msgLength, uint16_t frameStartIdx);

#endif
