#ifndef _FRAME_H_
#define _FRAME_H_

#include <stdint.h>

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

#endif
