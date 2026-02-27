#include "kiss.h"

#module

void kiss_prepend_header(uint8_t* msg, uint16_t* msgLength) {
	msg[0] = FEND;
	msg[1] = DATA_FRAME;
	*msgLength += 2;
}

void kiss_append_footer(uint8_t* msg, uint16_t* msgLength) {
	msg[*msgLength] = FEND;
	(*msgLength)++;
}

void kiss_apply_byte_check(uint8_t* message, uint16_t messageLength, uint8_t* frame, uint16_t* frameLength, uint16_t msgStartIdx) {
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

void kiss_remove_byte_check(uint8_t* incomingBuffer, uint16_t frameLength, uint8_t* msg, uint16_t* msgLength, uint16_t frameStartIdx) {
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
