#ifndef __MCPPKT_H__
#define __MCPPKT_H__

#include <stdint.h>
#include "common.h"
#include "framer.h"
#include "crcnew.h"

#define MCP_MAX_LEN             FRAME_MAX_SIZE - KISS_HEADER_SIZE - KISS_FOOTER_SIZE - CSP_HEADER_SIZE - CRC32_SIZE
#define MCP_HEADER_SIZE         6
#define MCP_FOOTER_SIZE         2
#define MCP_MAX_ID_LEN          20
#define MCP_MAX_ARGS_LEN        MCP_MAX_LEN - MCP_MAX_ID_LEN - MCP_HEADER_SIZE - MCP_FOOTER_SIZE - 2

// Command packet & reader FSM

typedef enum {
    CMD = 1,
    RES = 2,
    ACK = 3,
    TLM = 4,
    FILE = 5
} mcppkt_type_e;

typedef struct {
    kiss_parser_s parser;
    // Packet data
    uint8_t orgn;
    uint8_t dest;
    uint8_t echo;
    mcppkt_type_e ptype;
    uint8_t id_len;
    uint8_t args_len;
    char* id;
    char* args;
    uint16_t crc;
} mcppkt_s;

// Initialize this mcppkt empty
void mcppkt_init(mcppkt_s* pkt);

// Initialize this mcppkt populated with a mcp
void mcppkt_create(mcppkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len);
void mcppkt_create(mcppkt_s* pkt, uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, char* args);

// Clear this mcppkt
void mcppkt_clear(mcppkt_s* pkt);

// Parse packet fields out of its buffer. Requires pkt->buf and pkt->buf_len to be populated.
status_e mcppkt_parse_buf(mcppkt_s* pkt);

// Send a command packet along its appropriate route. Assumes pkt buf field is populated correctly.
void mcppkt_dispatch(mcppkt_s* pkt);

// Easy use command creation & dispatch
void mcp_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, uint8_t* args, uint8_t args_len);
void mcp_dispatch(uint8_t orgn, uint8_t dest, uint8_t echo, mcppkt_type_e ptype, char* id, char* args);

// Sends a response to a command with flipped o/d, specified type, and response args
void mcp_respond(mcppkt_s* pkt, mcppkt_type_e type, uint8_t* res, uint8_t res_len);
void mcp_respond(mcppkt_s* pkt, mcppkt_type_e type, char* res);

#endif
