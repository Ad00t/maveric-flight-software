#include "pch.h"
extern "C"
{
#include "downlinkmessage.h"
#include "linkmanager.h"
#include "transceiver_stub.c"
}

namespace LinkManagerTests
{
LinkManager linkManager;

TEST(LinkManager, transmitData) {
	LinkManager__init(&linkManager);

	// Define data to be sent
	char data[10] = {0};
	for (int i = 0; i < sizeof(data); ++i) {
		static char value = 0;
		data[i] = value++;
	}

	// Send data
	transmitData(&linkManager, data, sizeof(data), PAYLOAD_DATA);

	// Read in the "transmitted data frame"
	if (MessageTransmitted) {
		char frame[sizeof(DownlinkMessage)] = {0};
		_read(fdpipe[0], frame, sizeof(DownlinkMessage));

		// Define the values of the expected frame
		char expectedFrame[sizeof(DownlinkMessage) - (MAX_DOWNLINK_DATA_LENGTH - sizeof(data))] = {
			0x55, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x00, 0x01, 0x00, 0x01, 0x00,
			0x0a, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

		// Check frame (minus CRC value)
		EXPECT_TRUE(memcmp(frame, expectedFrame, sizeof(expectedFrame) - 2) == 0);

		// Reset Message transmission flag
		MessageTransmitted = 0;
	}
}
} // namespace LinkManagerTests