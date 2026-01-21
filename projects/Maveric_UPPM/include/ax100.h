#ifndef __AX100_H__
#define __AX100_H__

/***************************************************************************

								ax100.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt, Adhit Siripurapu

Purpose: Provides a base definition for interfacing with the AX100 transceiver 

***************************************************************************/

#include <stdint.h>

// MAIN TRANSCEIVER INTERFACE

typedef enum
{
	TRANSCEIVER_OFF = 0,
	TRANSCEIVER_ON = 1
} TransceiverPower;

void TurnTransceiverPower(TransceiverPower state);
TransceiverPower GetTransceiverPowerState();
uint16_t GetAvailableMessageFromTransceiver(char* messageBuffer, uint16_t sizeOfMessageBuffer);
void TransmitMessage(char* message, uint16_t messageSize);

// FRAME

// const int MAX_MESSAGE_SIZE = 248;
// kiss header size, csp header size, crc32 size, kiss footer size
#define MIN_MESSAGE_SIZE (2 + 4 + 4 + 1)

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
void findFrame(uint8_t* msg, uint16_t msgLen, int* msgStartIdx, int* msgEndIdx);

/*
  Takes a frame and extracts the message out of it. Used for data coming from GomSpace
*/
void extractMessageFromFrame(uint8_t* msg, uint16_t msgLength, int msgStartIdx, uint8_t* frame,
							 uint16_t* frameLength);

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
