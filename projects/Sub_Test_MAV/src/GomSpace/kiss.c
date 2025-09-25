#include "kiss.h"
#include "debug.h"
#include "ports.h"

static const int FEND = 0xc0;
static const int FESC = 0xdb;
static const int TFEND = 0xdc;
static const int TFESC = 0xdd;

#define DATA_FRAME 0x00

////////////////////////////////////////////////////////////////////////////////

void addKissHeader(uint8_t* msg, uint16_t* msgLength) {
	msg[0] = FEND;
	msg[1] = DATA_FRAME;

	*msgLength += 2;
}

uint16_t getKissHeaderSize(void) {
	return (2);
}

////////////////////////////////////////////////////////////////////////////////

void addKissFooter(uint8_t* msg, uint16_t* msgLength) {
	msg[*msgLength] = FEND;
	(*msgLength)++;
}

uint16_t getKissFooterSize(void) {
	return (1);
}

////////////////////////////////////////////////////////////////////////////////

void applyKissByteCheck(uint8_t* message, uint16_t messageLength, uint8_t* frame,
						uint16_t* frameLength, uint16_t msgStartIdx) {
	uint16_t spot = msgStartIdx;

	int idx;
	for (idx = 0; idx < messageLength; idx++) {
		if (*(message + idx) == FEND) {
			frame[spot] = FESC;
			spot++;
			frame[spot] = TFEND;
			spot++;
		} else if (*(message + idx) == FESC) {
			frame[spot] = FESC;
			spot++;
			frame[spot] = TFESC;
			spot++;
		} else {
			frame[spot] = message[idx];
			spot++;
		}
	}

	*frameLength = spot;
}

////////////////////////////////////////////////////////////////////////////////

void removeKissByteCheck(uint8_t* incomingBuffer, uint16_t frameLength, uint8_t* msg,
						 uint16_t* msgLength, uint16_t frameStartIdx) {
	int len = frameLength;
	int spot = 0;

	int idx;
	for (idx = frameStartIdx; idx < frameLength; idx++) {
		if (*(incomingBuffer + idx) == FESC) {
			len--;
			idx++;

			if (*(incomingBuffer + idx) == TFESC) {
				msg[spot] = FESC;
			} else if (*(incomingBuffer + idx) == TFEND) {
				msg[spot] = FEND;
			} else {
				msg[spot] = incomingBuffer[idx];
			}
		} else {
			msg[spot] = incomingBuffer[idx];
		}

		spot++;
	}

	*msgLength = len;
}