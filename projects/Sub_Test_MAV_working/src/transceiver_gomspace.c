#include "transceiver.h"

#include "GomSpace/frame.c"
#include "pins.h"
#include "ports.h"

/*
  Provides the gomspace implementation of the transceiver interface
*/

static void TurnTransceiverPowerOn();
static void TurnTransceiverPowerOff();
static uint16_t FindIncomingMessage(int8_t* msgBuffer, int sizeOfMsgBuffer);

void TurnTransceiverPower(TransceiverPower state) {
	if (state == TRANSCEIVER_ON) {
		TurnTransceiverPowerOn();
	} else if (state == TRANSCEIVER_OFF) {
		TurnTransceiverPowerOff();
	}
	return;
}
static void TurnTransceiverPowerOn() {
	output_high(TRANSCEIVER_PWR);
	return;
}
static void TurnTransceiverPowerOff() {
	output_low(TRANSCEIVER_PWR);
	return;
}

TransceiverPower GetTransceiverPowerState() {
	return (TransceiverPower)input_state(TRANSCEIVER_PWR);
}

uint16_t GetAvailableMessageFromTransceiver(int8_t* messageBuffer, uint16_t sizeOfMessageBuffer) {
	// Find a message within the receive buffer
	return FindIncomingMessage(messageBuffer, sizeOfMessageBuffer);
}
static uint16_t FindIncomingMessage(int8_t* msgBuffer, int sizeOfMsgBuffer) {
	if (CheckByte(TRANSCEIVER_PORT) <=
		CRC32_SIZE + getKissFooterSize() + getKissHeaderSize() + getCspHeaderSize()) {
		return 0;
	}

	int frameStartIdx = 0;
	int frameEndIdx = 0;
	// Look through the incoming buffer on the desired port and see if there is an available frame
	uint8_t incomingBuffer[BUFFER_SIZE_B] = {0};
	getNumBytesOnPort(TRANSCEIVER_PORT, CheckByte(TRANSCEIVER_PORT), incomingBuffer);
	findFrame(incomingBuffer, CheckByte(TRANSCEIVER_PORT), &frameStartIdx, &frameEndIdx);

	// If there is, grab the message contained in the frame
	uint16_t msgLength = 0;
	if (frameStartIdx >= 0 && frameEndIdx >= 0) {
		uint16_t frameLength = frameEndIdx - frameStartIdx + 1;
		extractMessageFromFrame(incomingBuffer, frameLength, frameStartIdx, msgBuffer, &msgLength);
		incrementNumReadBytesBy(TRANSCEIVER_PORT, frameLength);
	}
	return msgLength;
}

void TransmitMessage(char* message, uint16_t messageSize) {
	uint8_t frame[512] = {0};
	uint16_t frameLength = 0;
	setupFrame((uint8_t*)message, messageSize, frame, &frameLength);
	sendMSG(TRANSCEIVER_PORT, frame, (int)frameLength);
	return;
}