#include "pch.h"
extern "C" {
    #include "uplinkmessage.h"
    #include "crc.h"
}

#include <string>

namespace UplinkMessageTests {

UplinkMessage uplinkMessage;

TEST(UplinkMessageProcessing, NullPointers) {
    UplinkMessage__init(&uplinkMessage);
    char incomingMessage[5] = "1234";
    EXPECT_FALSE(deserializeIncomingMessage(NULL, incomingMessage, 15));
    EXPECT_FALSE(deserializeIncomingMessage(&uplinkMessage, NULL, 15));
}

TEST(UplinkMessageProcessing, MessageSizeTooSmall) {
    UplinkMessage__init(&uplinkMessage);
    char incomingMessage[] = { 0x55, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x0, 0x1, 0x0, 0x2, 0x0, 0x1, 0x3, 0x0, 0x0 };
    EXPECT_FALSE(deserializeIncomingMessage(&uplinkMessage, incomingMessage, 14));
}

TEST(UplinkMessageProcessing, MessageSizeTooBig) {
    UplinkMessage__init(&uplinkMessage);
    char incomingMessage[] = { 0x55, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x0, 0x1, 0x0, 0x2, 0x0, 0x1, 0x3, 0x0, 0x0 };
    EXPECT_FALSE(deserializeIncomingMessage(&uplinkMessage, incomingMessage, 271));
}

TEST(UplinkMessageProcessing, BadCallsign) {
    UplinkMessage__init(&uplinkMessage);
    char incomingMessage[] = { 0x54, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x0, 0x1, 0x0, 0x2, 0x0, 0x1, 0x3, 0x0, 0x0 };
    EXPECT_FALSE(deserializeIncomingMessage(&uplinkMessage, incomingMessage, 15));
    strcpy(incomingMessage, "USCLAA");
    EXPECT_FALSE(deserializeIncomingMessage(&uplinkMessage, incomingMessage, 270));
}

TEST(UplinkMessageProcessing, BadCrc) {
    UplinkMessage__init(&uplinkMessage);
    char incomingMessage[15] = { 0x55, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x1, 0x0, 0x2, 0x0, 0x1, 0x0, 0x3, 0x0, 0x0 };
    EXPECT_FALSE(deserializeIncomingMessage(&uplinkMessage, incomingMessage, sizeof(incomingMessage)));
}

TEST(UplinkMessageProcessing, GoodMessage) {
    UplinkMessage__init(&uplinkMessage);
    char incomingMessage[] = { 0x55, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x0, 0x1, 0x0, 0x2, 0x0, 0x1, 0x3, 0x0, 0x0 };
    unsigned char incomingMessageCrc[2];
    getCRC((unsigned char*)incomingMessage, sizeof(incomingMessage) - sizeof(uplinkMessage.crc), incomingMessageCrc);
    incomingMessage[13] = incomingMessageCrc[1];
    incomingMessage[14] = incomingMessageCrc[0];
    EXPECT_TRUE(deserializeIncomingMessage(&uplinkMessage, incomingMessage, sizeof(incomingMessage)));
    EXPECT_TRUE(uplinkMessage.messageID == 1);
    EXPECT_TRUE(uplinkMessage.messageType == 2);
    EXPECT_TRUE(uplinkMessage.dataLength == 1);
}

// Tests message data provided by ground team. Makes sure we can deserialize the data they produced
TEST(UplinkMessageProcessing, GroundMessage) {
    UplinkMessage__init(&uplinkMessage);
    char incomingMessage[] = { 0x55, 0x53, 0x43, 0x4c, 0x41, 0x4a, 0x00, 0x37, 0x00, 0x2d, 0x00, 0x1d, 0x72, 0x20, 0x31, 0x33,
        0x34, 0x20, 0x34, 0x38, 0x35, 0x20, 0x32, 0x33, 0x34, 0x34, 0x35, 0x30, 0x20, 0x27, 0x39, 0x30,
        0x30, 0x3a, 0x34, 0x35, 0x36, 0x27, 0x20, 0x34, 0x35, 0x4f, 0xc1 };
    EXPECT_TRUE(deserializeIncomingMessage(&uplinkMessage, incomingMessage, sizeof(incomingMessage)));
    EXPECT_TRUE(uplinkMessage.messageID == 55);
    EXPECT_TRUE(uplinkMessage.messageType == 45);
    EXPECT_TRUE(uplinkMessage.dataLength == 29);
    std::string str((char*)&uplinkMessage.data);
    EXPECT_TRUE(str  == "r 134 485 234450 '900:456' 45");
}

}