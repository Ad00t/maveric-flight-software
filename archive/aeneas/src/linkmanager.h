#ifndef __LINK_MANAGER_H__
#define __LINK_MANAGER_H__

/***************************************************************************

								linkmanager.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Manages the up and downlink interfaces. Receives and processes
		 incoming data when needed. Processes and sends downlink data
		 when needed.

***************************************************************************/

#include <stdint.h>

// From BUFFER_SIZE_X (ports.h)
#define MAX_MESSAGE_SIZE 256
#define NUM_TRACKED_IDS 50

typedef enum
{
	PAYLOAD_DATA = 1,
	HEALTH_AND_STATUS = 2,
	COMMAND = 3
} DataType;

typedef enum
{
	RECEIVE_MODE = 0,
	TRANSMIT_MODE = 1
} TransceiverMode;

typedef enum
{
	RADIO_STATE_OFF = 0,
	RADIO_STATE_ON = 1
} RadioPowerState;

typedef struct LinkManager {
	uint16_t previouslyReceivedMessageIds[NUM_TRACKED_IDS];
	uint16_t indexOfLastReceivedMessageId;
	uint16_t nextHealthAndStatusMessageId;
	uint16_t numCommandsReceived;
	TransceiverMode transceiverMode;
} LinkManager;

void LinkManager__init(LinkManager* self);
void switchTransceiverPowerTo(LinkManager* self, RadioPowerState state);
void switchBeaconTransmitEnabledTo(LinkManager* self, int8_t enabled);
void switchTransceiverModeTo(LinkManager* self, TransceiverMode mode);
RadioPowerState getTransceiverPowerState(LinkManager* self);
uint16_t getAvailableCommand(LinkManager* self, char* commandBuffer, uint16_t sizeOfCommandBuffer);
void transmitData(LinkManager* self, char* data, int dataSize, DataType dataType,
				  uint16_t messageId);

#endif // __LINK_MANAGER_H__