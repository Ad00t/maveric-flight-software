#ifndef __UPLINK_MESSAGE_H__
#define __UPLINK_MESSAGE_H__

/***************************************************************************

								uplinkmessage.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Handles incoming message data, sent from ground

***************************************************************************/

#include <stdint.h>

#define CALLSIGN_LENGTH 6
#define MAX_UPLINK_DATA_LENGTH 256

/*
  Deserializes and stores message data that is sent from the ground.
  Performs various checks to make sure the message is valid.
*/
typedef struct {
	int8_t projectId[CALLSIGN_LENGTH];
	uint16_t messageID;
	//uint16_t messageType;
	uint16_t dataLength;
	uint8_t data[MAX_UPLINK_DATA_LENGTH];
	//uint16_t crc;
} UplinkMessage;

void UplinkMessage__init(UplinkMessage* self);
/*
  Returns true upon success, false if there is a problem with the message
*/
int8_t deserializeIncomingMessage(UplinkMessage* self, char* message, uint16_t messageSize);

#endif // __UPLINK_MESSAGE_H__