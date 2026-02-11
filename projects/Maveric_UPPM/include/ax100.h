#ifndef __AX100_H__
#define __AX100_H__

/***************************************************************************

								ax100.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Provides a base definition for interfacing with the AX100 transceiver 

***************************************************************************/

#include <stdint.h>
#include "circbuf.h"

// kiss header size (2), csp header size (4), crc32 size (4), kiss footer size (1)
#define MIN_MESSAGE_SIZE        11
#define MAX_MESSAGE_SIZE        248

#define FEND                    0xC0
#define FESC                    0xDB
#define TFEND                   0xDC
#define TFESC                   0xDD

#define DATA_FRAME              0x00

#define CSP_NORMAL_PRIORITY     2L
#define FSW_NODE                8L
#define TX_NODE                 0L
#define GOMSPACE_NODE           5L
// mimic csp lib default (cnofig's port_max_bind)
#define MIN_PING_SRC_PORT       24L
#define GND_WDT_RESET_PORT      9L

// MAIN TRANSCEIVER INTERFACE

typedef struct {
    uint8_t port; 
    circbuf_s cmdbuf;
} ax100_s;

void ax100_init(ax100_s* a, uint8_t port);
void ax100_set_power(ax100_s* a, int1 on);
int1 ax100_is_on(ax100_s* a);
uint8_t ax100_get_avail_msg(ax100_s* a, circbuf_s* irqbuf);
void ax100_transmit_msg(ax100_s* a, uint8_t* buf, uint8_t len);

// FRAME


/*
  Takes in a message and builds a frame, adding KISS header/footer, CSP header, and a CRC

  Input:
	message: The message to be wrapped into a frame
	messageLength: The length of the message
  Output:
	frame: Where the frame will get stored
	frameLength: The length of the resulting frame
*/
void setupFrame(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength);

/*
  Looks through a buffer to see if a full frame is available for processing. Used
  for data coming from GomSpace
*/
int1 findFrame(circbuf_s* irqbuf, int* frameStartIdx, int* frameEndIdx, uint16_t minFrameSize);

/*
  Takes a frame and extracts the message out of it. Used for data coming from GomSpace
*/
void extractMessageFromFrame(uint8_t* framebuf, uint16_t frameLength, circbuf_s* cmdbuf, uint16_t* msgLength);

void setupWdtReset(uint8_t* msg, uint16_t* msgLength);

// INET

// Converts unsigned integer from host byte order to network byte order
uint32_t htonl(uint32_t hostLong);
// Converts unsigned short from host byte order to network byte order
uint16_t htons(uint16_t hostShort);
// Converts unsigned integer from network byte order to host byte order
uint32_t ntohl(uint32_t netLong);
// Converts unsigned short from network byte order to host byte order
uint16_t ntohs(uint16_t netShort);

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

uint16_t getCspHeaderSize(void);

void addCspHeaderWdtReset(uint8_t* msg, uint16_t* msgLength, uint16_t startLocation);

// KISS

void applyKissByteCheck(uint8_t* message, uint16_t messageLength, uint8_t* frame,
						uint16_t* frameLength, uint16_t msgStartIdx);

void addKissHeader(uint8_t* msg, uint16_t* msgLength);
void addKissFooter(uint8_t* msg, uint16_t* msgLength);

uint16_t getKissHeaderSize(void);
uint16_t getKissFooterSize(void);

void removeKissByteCheck(uint8_t* frame, uint16_t frameLength, uint8_t* msg, uint16_t* msgLength,
						 uint16_t frameStartIdx);

#endif
