#ifndef __CMDPKT_H__
#define __CMDPKT_H__

#include <stdint.h>
#include "kiss.h"

#define CMD_MAX_FRAME_SIZE      255
#define CMD_MAX_LEN             CMD_MAX_FRAME_SIZE - KISS_HEADER_SIZE - KISS_FOOTER_SIZE
#define CMD_HEADER_SIZE         6
#define CMD_FOOTER_SIZE         2
#define CMD_MAX_ID_LEN          20
#define CMD_MAX_ARGSSTR_LEN     CMD_MAX_LEN - CMD_MAX_ID_LEN - CMD_HEADER_SIZE - CMD_FOOTER_SIZE

// Command packet & reader FSM

typedef enum {
    REQUEST = 0,
    RESPONSE = 1
} cmdpkt_type_e;

typedef struct {
    // Packet parsing metadata
    char buf[CMD_MAX_LEN];
    uint8_t buf_len;
    uint8_t n_skip;
    int1 busy;
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

#endif
