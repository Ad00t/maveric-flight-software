#include "transceiver.h"

#include <fcntl.h>
#include <io.h>

/*
  Stub code for transceiver interface. For testing on Windows
*/

int fdpipe[2];
int MessageTransmitted = 0;

void TurnTransceiverPower(PowerState state) {
	return;
}

uint16_t GetAvailableData(char* data) {
	return 0;
}

/*
  Simulates sending message to the transceiver by opening a pipe and
  sending the data through the pipe. Test code can read data on other
  end of pipe and check it.
*/
void TransmitMessage(char* message, uint16_t messageSize) {
	if (_pipe(fdpipe, 256, O_BINARY) == -1)
		return;
	_write(fdpipe[1], message, messageSize);
	MessageTransmitted = 1;
	return;
}