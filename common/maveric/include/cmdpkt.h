#ifndef __CMDPKT_H__
#define __CMDPKT_H__

#include <stdint.h>
#include "common.h"
#include "framer.h"
#include "crcnew.h"

#define CMD_MAX_LEN             FRAME_MAX_SIZE - KISS_HEADER_SIZE - KISS_FOOTER_SIZE - CSP_HEADER_SIZE - CRC32_SIZE
#define CMD_HEADER_SIZE         6
#define CMD_FOOTER_SIZE         2
#define CMD_MAX_ID_LEN          20
#define CMD_MAX_ARGS_LEN        CMD_MAX_LEN - CMD_MAX_ID_LEN - CMD_HEADER_SIZE - CMD_FOOTER_SIZE - 2

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
    REQ = 1,
    RES = 2,
    ACK = 3,
    NACK = 4,
} cmdpkt_type_e;

typedef struct {
    kiss_parser_s parser;
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
void cmdpkt_create(cmdpkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len);
void cmdpkt_create(cmdpkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args);

// Clear this cmdpkt
void cmdpkt_clear(cmdpkt_s* pkt);

// Parse packet fields out of its buffer. Requires pkt->buf and pkt->buf_len to be populated.
status_e cmdpkt_parse_buf(cmdpkt_s* pkt);

// Send a command packet along its appropriate route. Assumes pkt buf field is populated correctly.
void cmdpkt_dispatch(cmdpkt_s* pkt);
// Easy use command creation & dispatch
void cmd_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len);
void cmd_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, cmdpkt_type_e ptype, char* id, char* args);

// Converts SUCCESS -> ACK, anything else -> NACK
cmdpkt_type_e stat2ack(status_e s);

// Sends a response to a command with flipped o/d, specified type, and response args
void cmd_respond(cmdpkt_s* pkt, cmdpkt_type_e type, char* res);

#endif
