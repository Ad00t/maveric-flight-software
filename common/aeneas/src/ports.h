/*    -= Basic Port I/O =-

			ports.h

This file controls port functionality for 4 ports, including:

	Inputs, Outputs, Handshaking

It does *not* handle formatting of data going to/from a port,
nor does it handle locating a cursor, or clearing a screen.
For those functions see the hyperterm.h file.

TODO:
	- Remove the SEC_TIMER dependency(?)

USAGE:
	First, #define the following baud rates
	COM_A_BAUD  xxxx
	COM_B_BAUD  xxxx
	COM_C_BAUD  xxxx
	COM_D_BAUD  xxxx

	Next, #define the following pins for TX/RX
	U1TX_PIN	PIN_XX
	U1RX_PIN	PIN_XX
	U2TX_PIN	PIN_XX
	U2RX_PIN	PIN_XX
	U3TX_PIN	PIN_XX
	U3RX_PIN	PIN_XX
	U4TX_PIN	PIN_XX
	U4RX_PIN	PIN_XX

	Next, #define the following pins for handshaking
	COM_A_CTS  PIN_XX
	COM_B_CTS  PIN_XX
	COM_C_CTS  PIN_XX
	COM_D_CTS  PIN_XX

	COM_A_RTS  PIN_XX
	COM_B_RTS  PIN_XX
	COM_C_RTS  PIN_XX
	COM_D_RTS  PIN_XX

	Use zero if the port will not have handshaking.

	Next, initialize each port's interrupt service with enable_interrupts(INT_RDAx)

	Next, initialize each port with setup_uart(baud,stream);
		- Baud can be any standard numeric value
		- stream is COM_A through COM_D

	Next, initialize handshaking by calling setPortHandshaking(port,ONOff)
	on each port.

	Now that the port is initialized, read data from it using:
		getByte(port, unsigned char * byte)
		getMSGsafe(port, unsigned char * msg, terminator character, maximum array index)

	And send data to it using:
		sendByte(port,character)
		sendMSG(port,msg,length)
		sendMSGALL(port,null-terminated string)

*/
#ifndef __BASIC_PORT_IO__
#define __BASIC_PORT_IO__

//========================================
//  			Dependencies
//========================================
#include "common.h" // Global Definitions, Clock Speeds, Interrupt Registers and Port Settings
#include "errors.h" // Return Error typedefs and functionality
#include "pins.h" // Pin settings
#include "config.h"
// extern unsigned long SEC_TIMER;			// TODO: Remove external dependency

//========================================
//  		Changeable Definitions
//========================================
#define BUFFER_SIZE_A 256 // Sets the buffer size of all com buffers
#define BUFFER_SIZE_B 256 // Sets the buffer size of all com buffers
#define BUFFER_SIZE_C 256 // Sets the buffer size of all com buffers
#define BUFFER_SIZE_D 512 // Sets the buffer size of all com buffers

//=====================================================
//  		   PORT DEFINITIONS (for ports.[h/c])
//=====================================================
// Names of Ports (note that they are 1 lower than the actual port.
// Eg: UART1 is Port 0.
/*
08-13-10    bez 	Updated port defitionts to support the IMI and the Sun Sensor
					Note that the USER_PORT is now port 1 (as opposed to 0).
					Updated SPI settings to be used for the Gyro and Magnetometer.
					Note: Gyro does not use hardware SPI, so does not need #pin_selects
					These may be changed for the actual flight vehicle.
					Swapped the pin defines for UARTS 3 and 4 to work with what I
					have on the development board currently.
*/
#ifdef UPPER_PPM
#define BEACON_PORT 0
#define TRANSCEIVER_PORT 1
#define USER_PORT 99 // Debug Port
#define OTHER_PPM_PORT 2
#define PAYLOAD_SMARTSAT_PORT 3
#else
#define IMI_PORT 0 // IMI-100 Port
#define USER_PORT 3 // USB Hyperterminal Port
#define OTHER_PPM_PORT 2 // To/From Other PPM
#define SUN_PORT 1 // Sun Sensor Port
#endif

// An extra setting for use on the Dev Board only
#ifdef CUSTOM_PINS
#undef USER_PORT
#undef MHX_PORT
#define USER_PORT \
	3 // This is most likely the USB on the Dev Board, though it depends on how the pins are set up
#define MHX_PORT \
	30 // If greater than 4 and not 99, this effectively dumps all characters sent to the MHX_PORT.
	   // Useful on the dev board.
#endif
//========================================
//  		Static Definitions
//========================================
#define NUM_PORTS 4 // Sets the number of ports (realistically cannot be changed)
#define HANDSHAKE_ON 1 // Value to use in set/getPortHandshaking
#define HANDSHAKE_OFF 0 // Value to use in set/getPortHandshaking

//========================================
//  			Globals
//========================================

//========================================
//  			Prototypes
//========================================
// Initialization and Configuration
void InitPorts(); // Initializes and Clears all ports
void setPortHandshaking(int port, unsigned char OnOff);
void resetPort(int port); // Resets a specific port
unsigned char getPortHandshaking(int port); // Returns the handshaking state of a port
unsigned char* getPointerToIncomingDataOnPort(int port);
void incrementNumReadBytesBy(int port, int numBytes);

// Typical Usage
int getByte(
	int port,
	unsigned char* data); // Grabs a byte from a specific port.  Returns 1 for Success, 0 for
						  // Failure (no byte).  Returns immediately with or without a byte
int CheckByte(int port); // Returns number of bytes available to read on a specified port.
void sendByte(int port, unsigned char b); // Sends a byte to a port
ReturnErr_t sendMSG(int port, unsigned char* buf,
					int len); // Sends a specific number of bytes to a port
void sendMSGALL(int port, unsigned char* msg); // Sends a null-terminated string to a port
ReturnErr_t getMSGsafe(
	int port, unsigned char* ptr, unsigned char term,
	int maxindex); // A newer version of getMSG that checks against a maxindex for safety
void getNumBytesOnPort(int port, int numBytes, unsigned char* buffer);

// Overflow
int CheckOverFlowFlag(int port); // Checks the overflow flag on a port.
void ClearOverFlow(int port); // Clears the overflow flag for a port

// RTS/CTS
void setRTS(int port); // Sets the RTS pin (stops incoming data flow)
void clearRTS(int port); // Clears the RTS pin (allows incoming data flow)
unsigned char checkRTS(int port); // Checks the RTS pin
unsigned char checkCTS(int port); // Checks the CTS pin (checks if we can send outgoing data)

// Testing functions
#ifdef DEBUG
void fakeDataOnPort(int port, int len); // Mimics data coming from a port.
void fakeStringOnPort(int port, unsigned char* string,
					  int len); // Writes a string to a port as if it came over serial
void fakeCharOnPort(
	int port,
	unsigned char fake); // Writes a single unsigned char to port as if it came over serial
#endif

#endif
