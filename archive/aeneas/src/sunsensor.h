/***************************************************************************

									sunsensor.h

Company: USC/ISI/iControl
License: Proprietary
Author(s): Michael Aherne (mra), Will Bezouska

Purpose: Functions for talking to the SS-411 Digital Sun Sensor

Notes:
	The sunsensor talks on UART using a series of commands such as INIT,
	PING, APPLICATION_COMMAND, etc... but in order to understand those command
	bytes, they must be wrapped in a protocol called NSP, Nanosatellite Protocol.
	
	The basic unit of communication in NSP is the message.  A message from the
	satellite to the sunsensor carries a single telecommand. The device may
	then reply to the sat with a single response. This may be an ack, or may 
	contain telemetry.
	
	The message format for NSP is as follows:
	Length 		| Field
	---------------------------------------
	1 byte		| Destination Address
	1 byte	 	| Source Address
	1 byte	 	| Message Control Field
	0+ bytes	| Data
	2 bytes		| Message CRC

	For any data that is 16-bit or higher, the LSB is transmitted first.

	260 bytes is a common upper limit for the data field.

	Message Control Field
	Bit 7 (MSB) | "Poll" Bit (1 to request reply from device)
	Bit 6		| "B" Bit
	Bit 5		| "A" (ACK) Bit
	Bits 4-0	| Command Code  (interpret as uint between 0 and 31.)

	CRC: see documentation.

	Each message is encapsulated in a SLIP Frame.  The properties of the SLIP
	frame are as follows:
	A special end character FEND (0xc0) marks both the beginning and end of 
	each NSP message.  Wherever FEND would occur within the message, it is 
	replaced by two bytes: FESC TFEND (0xdb 0xdc).  Wherever FESC would occur
	in the message, it is replaced by FESC TFESC (0xdb 0xdd).
	SLIP framing is used because not all of the link types supported by the
	makers of the sunsensor feature out-of-band message completion signals.

	Source/Destination Addresses
	By convention, 0x11 is used as the address for the primary computer.
	The sunsensor address may be hardcoded, or may be set using pins on the 
	device (not sure which applies to our sunsensor).

Usage:

Changelog:

Date 	 |  Au.  |	Notes
07-28-10	mra		Created.

****************************************************************************
*/

#ifndef __SUNSENSOR__
#define __SUNSENSOR__

//========================================
//  			Dependencies
//========================================
/*#include "ports.h"
#include "debug.h"	
#include "hyperterm.h"				// Allows use of Hex Print
#include "common.h"
#include "pins.h"
*/
//======================
//  	Globals
//======================
#define SS_WAIT_TIME 		3	// Time to wit for response from Sun Sensor (sec)
#define GOCODE_WAIT_TIME 	5 	// Time to wait for a Frame Cycle (sec)

//===============================
// 		Sun Sensor Specs
//===============================
#define SS_NUM_OF_CHANNELS		15
#define MAX_SS_DATA_SIZE 		255


//===============================
// 		Channel Names
//===============================

#define SS_GOCODE			0
#define SS_SETTINGS			1
#define SS_TELEMETRY		2
#define SS_TEMPERATURE		3
#define SS_SUN_VECTOR_FLOAT 6
#define SS_ITERATIONS		7
#define SS_PROCESSING_TIME	8
#define SS_SUN_VECTOR_INT	9
#define SS_DIAGNOSTICS		12
#define SS_TEMPERATURE_C	13

//================================
//  	Sun Sensor Structure
//================================

typedef struct 
{
	// Channel 0:
	unsigned int8 GoCode;

	// Channel 1:
	unsigned int32 MinExposure;
	unsigned int32 MaxExposure;
	unsigned int16 TargetBrightness;

	// Channel 2
	unsigned int16 MinBrightness;
	unsigned int16 MaxBrightness;
	unsigned int32 IntegratedBrightness;
	unsigned int32 ExposureLength;

	// Channel 3
	unsigned int16 Temperature;

	// Note: Commented out Image arrays to save space.
	
	// Channel 4
	//unsigned int8 OperatingImage[512];

	// Channel 5
	//unsigned int8 OfflineImage[512];

	// Channel 6
	float Vector[3];
	signed int8 FitQuality;
	signed int8 GeometryQuality;

	// Channel 7
	unsigned int8 Iterations;

	// Channel 8
	unsigned int32 ProcessingTime;

	// Channel 9
	signed int16 Vector_int[3];
	signed int8 FitQuality_int;
	signed int8 GeometryQuality_int;

	// Note: Channel 10-12 are for CAN functionality and not used.

	// Channel 13
	unsigned int16 Diagnostic[3];

	// Channel 14
	float CelsiusTemperature;

} sunsensor_s;

//========================================
// 			Driver Functions
//========================================

// Initialization functions
ReturnErr_t sunSensor_init(int port);

// Non-blocking Appliaction Command/Telemetry
ReturnErr_t getSunSensorReply(int port, unsigned char * data, int maxDataLength, int * dataLength);
ReturnErr_t getSunSensorData(int port, unsigned char * requestedChannels, unsigned char requestedChannelsLength, sunsensor_s * Data, short reset, short sync);

// Blocking Functions
ReturnErr_t startSunSensorFrameCycle(int port);
ReturnErr_t requestSunSensorTelemetry(int port, unsigned char channel);
ReturnErr_t decodeSunSensorTelemetry(unsigned char * data, int len, sunsensor_s * SunSensorData);
ReturnErr_t blockForSunSensorACK(int port);
ReturnErr_t blockForSunSensorReply(int port, unsigned char * data, int * lengthPtr);
ReturnErr_t blockForSunSensorGoCode(int port);

// Interface with Flight Software
ReturnErr_t convertFrameSStoBody(sunsensor_s * SunSensorData);


// Not yet implemented functions
//ReturnErr_t getSunSensorImage(int port, sunsensor_s * SunSensorData, short operating);
//ReturnErr_t setSunSensorImage(int port, sunsensor_s * SunSensorData, short operating);

//========================================
// 			Development Functions
//========================================

#ifdef DEBUG

// Blocking Functions
ReturnErr_t collectAllDataSunSensor(int port, sunsensor_s * SunSensorData);
ReturnErr_t pingSunSensor(int port);

// Printing Functions
void printSunSensorChannel(unsigned char channel, sunsensor_s * Data);
void printSunSensorStruct(sunsensor_s * Data);

// Testing Functions
void sunAutomaticDataCollection(long iterations, short continuous, short human);

#endif // DEBUG

#endif //__SUNSENSOR__
