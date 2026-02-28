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

uint16_t kiss_extract_frame(ringbuf_s* rcvbuf, uint8_t* frame_buf, uint16_t frame_buf_size) {
	int16_t frameStartIdx = 0;
	int16_t frameEndIdx = 0;
	// Look through the incoming buffer on the desired port and see if there is an available frame
	if (!kiss_find_frame(rcvbuf, &frameStartIdx, &frameEndIdx, 1)) 
        return 0;

	// If there is, grab the message contained in the frame
    // We are waiting for a full frame to enter the rcvbuf before processing it. Could replace with FSM.
    uint16_t frameLength = frameEndIdx - frameStartIdx + 1;
    if (frameLength <= frame_buf_size) {
        rb_pop(rcvbuf, frameStartIdx, NULL);
        rb_pop(rcvbuf, frameLength, frame_buf);
        return frameLength;
    } else {
        rb_pop(rcvbuf, frameStartIdx + frameLength, NULL);
        return 0; 
    }
}

int1 kiss_find_frame(ringbuf_s* rcvbuf, int* frameStartIdx, int* frameEndIdx, uint16_t minFrameSize) {
	*frameStartIdx = -1;
	*frameEndIdx = -1;

    uint8_t bufferLen = rb_len(rcvbuf);
	int startLimit = bufferLen - 1;
	uint8_t idx;
	for (idx = 0; idx < startLimit; idx++) {
        uint8_t b1, b2;
        rb_peek(rcvbuf, idx, &b1);
        rb_peek(rcvbuf, idx+1, &b2);
		if (b1 == FEND && b2 != FEND) {
            *frameStartIdx = idx;
			break;
        }
	}

	if (*frameStartIdx < 0) {
		return FALSE;
	}

	for (idx = *frameStartIdx + 1 + minFrameSize; idx < bufferLen; idx++) {
        uint8_t b;
        rb_peek(rcvbuf, idx, &b);
		if (b == FEND) {
			*frameEndIdx = idx;
			return TRUE;
		}
	}

    return FALSE;
}
