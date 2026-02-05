#ifndef _KISS_H_
#define _KISS_H_

#include <stdint.h>

void applyKissByteCheck(uint8_t* message, uint16_t messageLength, uint8_t* frame,
						uint16_t* frameLength, uint16_t msgStartIdx);

void addKissHeader(uint8_t* msg, uint16_t* msgLength);
void addKissFooter(uint8_t* msg, uint16_t* msgLength);

uint16_t getKissHeaderSize(void);
uint16_t getKissFooterSize(void);

void removeKissByteCheck(uint8_t* frame, uint16_t frameLength, uint8_t* msg, uint16_t* msgLength,
						 uint16_t frameStartIdx);

#endif
