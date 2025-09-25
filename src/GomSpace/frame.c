#include "frame.h"

#include "crc32.c"
#include "cspHeader.c"
#include "inet.h"
#include "kiss.c"

static void findFrameMinFrameSize(uint8_t* msg, uint16_t msgLen, int* msgStartIdx, int* msgEndIdx,
								  uint16_t minFrameSize);

void setupFrame(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength) {
	addKissHeader(frame, frameLength);

	addCspHeader(frame, frameLength, getKissHeaderSize());

	applyKissByteCheck(message, messageLength, frame, frameLength,
					   getKissHeaderSize() + getCspHeaderSize());

	uint32_t crc32 = computeCrc32(message, messageLength);
	crc32 = htonl(crc32);

	applyKissByteCheck((uint8_t*)&crc32, sizeof(crc32), frame, frameLength, *frameLength);

	addKissFooter(frame, frameLength);
}

void findFrame(uint8_t* incomingBuffer, uint16_t bufferLen, int* frameStartIdx, int* frameEndIdx) {
	findFrameMinFrameSize(incomingBuffer, bufferLen, frameStartIdx, frameEndIdx, 1);
}

static void findFrameMinFrameSize(uint8_t* incomingBuffer, uint16_t bufferLen, int* frameStartIdx,
								  int* frameEndIdx, uint16_t minFrameSize) {
	*frameStartIdx = -1;
	*frameEndIdx = -1;

	int startLimit = bufferLen - 1;

	int idx;
	for (idx = 0; idx < startLimit; idx++) {
		if ((*(incomingBuffer + idx) == FEND) && (*(incomingBuffer + idx + 1) != FEND)) {
			*frameStartIdx = idx;
			break;
		} else {
			incrementNumReadBytesBy(TRANSCEIVER_PORT, 1);
		}
	}

	if (*frameStartIdx < 0) {
		return;
	}

	for (idx = *frameStartIdx + 1 + minFrameSize; idx < bufferLen; idx++) {
		if (*(incomingBuffer + idx) == FEND) {
			*frameEndIdx = idx;
			return;
		}
	}
}

#define CRC32_SIZE 4
void extractMessageFromFrame(uint8_t* incomingBuffer, uint16_t frameLength, int frameStartIdx,
							 uint8_t* msg, uint16_t* msgLength) {
	removeKissByteCheck(incomingBuffer, frameLength, msg, msgLength, frameStartIdx);

	*msgLength -= getKissHeaderSize() + getCspHeaderSize() + CRC32_SIZE + getKissFooterSize();

	int i;
	for (i = 0; i < *msgLength; ++i) {
		msg[i] = msg[i + getKissHeaderSize() + getCspHeaderSize()];
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////

void setupWdtReset(uint8_t* msg, uint16_t* msgLength) {
	addKissHeader(msg, msgLength);

	addCspHeaderWdtReset(msg, msgLength, getKissHeaderSize());

	// empty message, so CRC32 == 0
	int crcLoc = *msgLength;

	msg[crcLoc] = 0;
	msg[crcLoc + 1] = 0;
	msg[crcLoc + 2] = 0;
	msg[crcLoc + 3] = 0;
	*msgLength += 4;

	addKissFooter(msg, msgLength);
}