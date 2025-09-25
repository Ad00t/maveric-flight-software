#include "pch.h"
extern "C" {
    #include "downlinkmessage.h"
    #include "crc.h"
}

#include <string>

namespace DownlinkMessageTests {
DownlinkMessage downlinkMessage;

TEST(DownlinkMessageProcessing, AddNoDataToMessage) {
    DownlinkMessage__init(&downlinkMessage);
    char data1[1] = { 0 };
    EXPECT_FALSE(appendDataToMessage(&downlinkMessage, data1, 0));
}

TEST(DownlinkMessageProcessing, AddTooMuchDataToMessage) {
    DownlinkMessage__init(&downlinkMessage);
    char data1[MAX_DOWNLINK_DATA_LENGTH + 1] = { 0 };
    EXPECT_FALSE(appendDataToMessage(&downlinkMessage, data1, sizeof(data1)));

    DownlinkMessage__init(&downlinkMessage);
    char data2[MAX_DOWNLINK_DATA_LENGTH / 2] = { 0 };
    EXPECT_TRUE(appendDataToMessage(&downlinkMessage, data2, sizeof(data2)));
    EXPECT_FALSE(appendDataToMessage(&downlinkMessage, data2, sizeof(data2) + 1));
}

TEST(DownlinkMessageProcessing, AddDataToMessage) {
    DownlinkMessage__init(&downlinkMessage);
    char data[MAX_DOWNLINK_DATA_LENGTH] = { 0 };
    for (int i = 0; i < sizeof(data); ++i) {
        static char value = 0;
        data[i] = value++;
    }
    EXPECT_TRUE(appendDataToMessage(&downlinkMessage, data, sizeof(data)));
    EXPECT_TRUE(downlinkMessage.dataLength == sizeof(data));
    EXPECT_TRUE(memcmp(&downlinkMessage.data, data, sizeof(data)) == 0);
}

TEST(DownlinkMessageProcessing, NullPointers) {
    DownlinkMessage__init(&downlinkMessage);
    char messageBuffer[] = { 0 };
    EXPECT_FALSE(serializeDownlinkMessage(NULL, messageBuffer, sizeof(DownlinkMessage)));
    EXPECT_FALSE(serializeDownlinkMessage(&downlinkMessage, NULL, sizeof(DownlinkMessage)));
    EXPECT_FALSE(appendDataToMessage(NULL, messageBuffer, sizeof(messageBuffer)));
    EXPECT_FALSE(appendDataToMessage(&downlinkMessage, NULL, sizeof(messageBuffer)));
}

TEST(DownlinkMessageProcessing, BufferSizeTooSmall) {
    DownlinkMessage__init(&downlinkMessage);
    downlinkMessage.messageID = 1;
    downlinkMessage.messageType = 1;
    char data[1] = { 0 };
    for (int i = 0; i < sizeof(data); ++i) {
        static char value = 0;
        data[i] = value++;
    }
    appendDataToMessage(&downlinkMessage, data, sizeof(data));
    char messageBuffer[sizeof(DownlinkMessage) - MAX_DOWNLINK_DATA_LENGTH] = { 0 };
    EXPECT_FALSE(serializeDownlinkMessage(&downlinkMessage, messageBuffer, sizeof(messageBuffer)));
}

TEST(DownlinkMessageProcessing, NoDataToSend) {
    DownlinkMessage__init(&downlinkMessage);
    char messageBuffer[sizeof(DownlinkMessage)] = { 0 };
    EXPECT_FALSE(serializeDownlinkMessage(&downlinkMessage, messageBuffer, sizeof(messageBuffer)));
}

TEST(DownlinkMessageProcessing, GoodMessage) {
    // Initialize the downlink message structure with values
    DownlinkMessage__init(&downlinkMessage);
    downlinkMessage.messageID = 1;
    downlinkMessage.messageType = 1;

    // Add data to the structure
    char data[MAX_DOWNLINK_DATA_LENGTH] = { 0 };
    for (int i = 0; i < sizeof(data); ++i) {
        static char value = 0;
        data[i] = value++;
    }
    appendDataToMessage(&downlinkMessage, data, sizeof(data));

    // Serialize the structure into the message buffer
    char messageBuffer[sizeof(DownlinkMessage) - (MAX_DOWNLINK_DATA_LENGTH - sizeof(data))] = { 0 };
    uint16_t sizeOfMessageBuffer = 0;
    EXPECT_TRUE(sizeOfMessageBuffer = serializeDownlinkMessage(&downlinkMessage, messageBuffer, sizeof(messageBuffer)));
    EXPECT_TRUE(sizeOfMessageBuffer == (sizeof(DownlinkMessage) - (MAX_DOWNLINK_DATA_LENGTH - sizeof(data))));

    // Define the values of the expected serialized buffer
    char expectedBuffer[sizeof(DownlinkMessage) - (MAX_DOWNLINK_DATA_LENGTH - sizeof(data))] = { 0x55, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00 };
    for (int i = 0; i < sizeof(data); ++i) {
        static char value = 0;
        expectedBuffer[i + 12] = value++;
    }
    unsigned char crcValue[2] = { 0 };
    getCRC((unsigned char*)expectedBuffer, sizeof(expectedBuffer) - 2, crcValue);
    expectedBuffer[sizeof(expectedBuffer) - 2] = (char) crcValue[1];
    expectedBuffer[sizeof(expectedBuffer) - 1] = (char) crcValue[0];

    // Test whether the data was serialzed correctly
    EXPECT_TRUE(memcmp(messageBuffer, expectedBuffer, sizeof(expectedBuffer)) == 0);
}

// Tests message data provided by ground team. Makes sure we can deserialize the data they produced
TEST(DownlinkMessageProcessing, GroundMessage) {
    // Initialize the downlink message structure with values
    DownlinkMessage__init(&downlinkMessage);
    downlinkMessage.messageID = 6;
    downlinkMessage.messageType = 1;

    // Add data to the structure
    char data[10] = { 0 };
    for (int i = 0; i < sizeof(data); ++i) {
        static char value = 0;
        data[i] = value++;
    }
    appendDataToMessage(&downlinkMessage, data, sizeof(data));

    // Serialize the structure into the message buffer
    char messageBuffer[sizeof(DownlinkMessage) - (MAX_DOWNLINK_DATA_LENGTH - sizeof(data))] = { 0 };
    uint16_t sizeOfMessageBuffer = 0;
    EXPECT_TRUE(sizeOfMessageBuffer = serializeDownlinkMessage(&downlinkMessage, messageBuffer, sizeof(messageBuffer)));
    EXPECT_TRUE(sizeOfMessageBuffer == (sizeof(DownlinkMessage) - (MAX_DOWNLINK_DATA_LENGTH - sizeof(data))));

    // Define the values of the expected serialized buffer
    char expectedBuffer[sizeof(DownlinkMessage) - (MAX_DOWNLINK_DATA_LENGTH - sizeof(data))] = { 0x55, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x00,
        0x06, 0x00, 0x01, 0x00, 0x0a, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, (char)0xcd, 0x6c };

    unsigned char crcValue[2] = { 0 };
    getCRC((unsigned char*)expectedBuffer, sizeof(expectedBuffer) - 2, crcValue);
    expectedBuffer[sizeof(expectedBuffer) - 2] = (char)crcValue[1];
    expectedBuffer[sizeof(expectedBuffer) - 1] = (char)crcValue[0];

    // Test whether the data was serialzed correctly
    EXPECT_TRUE(memcmp(messageBuffer, expectedBuffer, sizeof(expectedBuffer)) == 0);
}
}