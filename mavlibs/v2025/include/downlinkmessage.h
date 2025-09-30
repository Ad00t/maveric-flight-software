#ifndef __DOWNLINK_MESSAGE_H__
#define __DOWNLINK_MESSAGE_H__

/***************************************************************************

								downlinkmessage.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Creates and stores messages to be transmitted to ground

***************************************************************************/

#include <stdint.h>

#define MAX_DOWNLINK_DATA_LENGTH 256

/*
  Stores the message data to be sent to the ground. Serializes the data before transmission.
  Performs various checks to ensure the data is valid.
*/
typedef struct DownlinkMessage {
	uint8_t messageType;
	uint16_t messageId;
	uint16_t dataLength;
	uint8_t data[MAX_DOWNLINK_DATA_LENGTH];
} DownlinkMessage;

void DownlinkMessage__init(DownlinkMessage* self);
/*
  Appends data to the data section of the message.

  Returns false if there is an issue with the amount of available space or
  with the passed variables
*/
int8_t appendDataToMessage(DownlinkMessage* self, char* data, uint16_t dataSize);
/*
  Serializes the message and stores it in the provided buffer.

  Returns size, in bytes, of serialized message upon success.
  Returns 0 if there is an issue.
*/
uint16_t serializeDownlinkMessage(DownlinkMessage* self, char* buffer, uint16_t bufferSize);

#endif // __DOWNLINK_MESSAGE_H__