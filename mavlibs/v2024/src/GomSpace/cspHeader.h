#ifndef _CSP_HEADER_H_
#define _CSP_HEADER_H_

#include <stdint.h>

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

#endif
