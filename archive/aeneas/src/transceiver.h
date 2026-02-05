#ifndef __TRANSCEIVER_H__
#define __TRANSCEIVER_H__

/***************************************************************************

								transceiver.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Provides a base definition for interfacing with transceivers

***************************************************************************/

#include <stdint.h>

typedef enum
{
	TRANSCEIVER_OFF = 0,
	TRANSCEIVER_ON = 1
} TransceiverPower;

void TurnTransceiverPower(TransceiverPower state);
TransceiverPower GetTransceiverPowerState();
uint16_t GetAvailableMessageFromTransceiver(char* messageBuffer, uint16_t sizeOfMessageBuffer);
void TransmitMessage(char* message, uint16_t messageSize);

#endif // __TRANSCEIVER_H__