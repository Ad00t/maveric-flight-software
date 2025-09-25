/***************************************************************************

									common.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: Common definitions, chip and #fuse settings... basically 
		 anything required by everything.

Changelog:

Date 	 |  Au.  |	Notes
03-03-10	mra		Created.
07-28-10	mra		Removed unsused registers.

****************************************************************************
*/
#ifndef __COMMON_H__
#define __COMMON_H__

#include "config.h"
#include <24FJ256GA110.h>			// Device Header File.  Switched to brackets to use version in PICC Library
#device PASS_STRINGS=IN_RAM			// Should allow for things like sendMSGALL(port,"Message") <--- const weren't allowed there before
#device ADC=16						// ??
#build(stack=0x300)					// Use a larger stack size.
#fuses NOPROTECT					// Code not protected from reading
#fuses NOWDT						// No automatic WDT -- it must be enabled by software.
//#fuses XT							// Primary Clock Select
//#fuses FRC_PLL						// Internal Fast RC Oscillator with Phase Lock Loop gives 32 MHz
#fuses HS
#fuses PR_PLL
#fuses WPOSTS13						// Watchdog Postscaler.  
									// At current processor settings: WPOSTS12 = ~12 seconds
									//								  WPOSTS13 = ~20 seconds
									//								  default  = ~120 seconds
#fuses IESO							// Internal-External Switchover
//#fuses IOL1WAY						// Locks the I/O Lock after setting it once.
#fuses NOIOL1WAY					// Allows multiple changes to PIC register settings.
#fuses WRT
#fuses CKSFSM						// Clock fail-safe monitor	
#pragma case						// Makes all code case-sensitive

#use delay(clock=32MHZ,internal=8M)  // Tells compiler what the clock speed is
//#use delay(clock=8MHZ,internal=8M)  // Tells compiler what the clock speed is

//========================================
//  		PIC Registers 
//========================================
#word RCON = 0x0740
#word OSCCON = getenv("SFR:OSCCON") 
#bit IOLOCK = OSCCON.6
#word RPINR20 = getenv("SFR:RPINR20")
#word RPOR1 = getenv("SFR:RPOR1")
/* // DEBUG DISPLAY
#warning OSCCON is located at getenv("SFR:OSCCON")
#warning IOLOCK is located at getenv("BIT:IOLOCK")
#warning RPINR20 is located at getenv("SFR:RPINR20")
#warning RPOR1 is located at getenv("SFR:RPOR1")
*/

//========================================
//  		Baud Rates
//========================================
#ifdef UPPER_PPM
	#define COM_A_BAUD		 38400  // Beacon
	#define COM_B_BAUD       19200 // Transceiver
	#define COM_C_BAUD       115200 // Other PPM
	#define COM_D_BAUD       115200  // Payload
#else
	#define COM_A_BAUD       115200 // ADACS
	#define COM_B_BAUD       115200 // USB
	#define COM_C_BAUD       115200 // To/From Other PPM 
	#define COM_D_BAUD       57600  // SunSensor Port
#endif



/********* BINARY PRE-PROCESSOR MACROS ************/
// Turns a numeric literal into a hex constant
#define HEX__(n) 0x##n##LU

/* 8-bit conversion function */
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)

/* *** user macros *** */

/* for upto 8-bit binary constants */
#define B8(d) ((unsigned char)B8__(HEX__(d)))

/* for upto 16-bit binary constants, MSB first */
#define B16(dmsb,dlsb) (((unsigned long)B8(dmsb)<< 8) + (unsigned long)B8(dlsb))

/* for upto 32-bit binary constants, MSB first */
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<< 8) \
+ B8(dlsb))

/* Sample usage:
B8(01010101) = 85
B16(10101010,01010101) = 43605
B32(10000000,11111111,10101010,01010101) = 2164238933
*/


//////////////////////////////////////
//       OTHER DEFINITIONS
/////////////////////////////////////

//#define NUM_ACD_VALUES (8)
//#define NUM_IO_PORTS (3)
#define NUM_ALARM_BYTES (4)
#define MAX_UNIT_ID 		17
#define NUM_LABELS 			12
#define MAX_LABEL_LEN 		16
#define NUM_SFACTORS 		16
#define MAX_NUM_SFACTORS 	2


// Definitions for packetizing
#define MISC_PACKET 0
#define TLM_PACKET 1
#define PLY_PACKET 2
#define ACK_PACKET 3
#define SYNC_PACKET 4
#define AWESOME_PACKET 42


/**************************************************************** Board Configurations */

typedef struct
{
    unsigned char len;
    unsigned long eventMask;
    int crc;
} macroEventInfo_t;



// LEGACY CODE BELOW

/*
enum {
    scAwake = 0,
    scRadio = 'R',
    scDecommisioned = 'N',
};

typedef enum {
    wakeReasonNone = 'x',
    wakeButtonPressed = 'B',
    wakeShackleChanged = 'S',
    wakeCradleChanged = 'C',
    wakeMacroProcessing = 'M',
    wakeTransmitQueueNotEmpty = 'T',
    wakeReceivingMessages = 'r',
    wakeUpload = 'U',
    wakeUserInput = 'K',
    wakeRadio = 'R',
    wakeCommand16 = 'c',
    wakeP0ExternalInterrupt = '0',
    wakeP1ExternalInterrupt = '1',
    wakeP2ExternalInterrupt = '2',
    wakeAssociation = 'A',
    wakeDisassociation = 'D',
    wakeDeviceInit = 'b',
    wakeEvent = 'E',
    wakeEventLockMoter = 'L',
    wakeEventPicReset = 'P',
} wakeReason_t;
*/

/*
// System Events
enum {
    seCOLDBOOT                   = (0x00000001L),
    seWDBOOT                     = (0x00000002L),
    seBATT_CRITICALLY_LOW        = (0x00000004L),
    seSYSTEM_ALARM_DECLARED      = (0x00000008L),

    // WLAN Events
    seASSOCIATED                 = (0x00000010L),
    seDISASSOCIATED              = (0x00000020L),
    seNOT_DEFINDED_6             = (0x00000040L),
    seNOT_DEFINDED_7             = (0x00000080L),

    // Waypoint Events
    seWAYPOINT_ENTRANCE          = (0x0000100L),
    seWAYPOINT_EXIT              = (0x0000200L),
    seTRUST_ZONE_ENTRANCE        = (0x0000400L),
    seTRUST_ZONE_EXIT            = (0x0000800L),

    seNOT_DEFINDED_12            = (0x0001000L),
    seNOT_DEFINDED_13            = (0x0002000L),
    seNOT_DEFINDED_14            = (0x0004000L),
    seNOT_DEFINDED_15            = (0x0008000L),

    // Motion Events
    seDWELL                      = (0x00010000L),
    seMOVING                     = (0x00020000L),
    seENROUTE                    = (0x00040000L),
    seSTOPPED                    = (0x00080000L),

    // mLOCK Events
    seLONG_BUTTON_PRESS          = (0x00100000L),
    seBUTTON_PRESS               = (0x00200000L),
    seLONG_BUTTON_PRESS_SAFE     = (0x00400000L), //button held, but in a safe zone
    seRESERVED_FOR_SHACKLE_CUT   = (0x00800000L), // not yet implemented

    seLOCK_STATE_CHANGE          = (0x01000000L),
    seSHACKLE_STATE_CHANGE       = (0x02000000L),
    seNOT_DEFINDED_26            = (0x04000000L),
    seCRADLE_STATE_CHANGE        = (0x08000000L),

    seNOT_DEFINDED_28            = (0x10000000L),
    seNOT_DEFINDED_29            = (0x20000000L),
    seNOT_DEFINDED_30            = (0x40000000L),
    seNOT_DEFINDED_31            = (0x80000000L),
};

// System Alarms
enum {
    saNONE                       = 0,
    saCOLDBOOT                   = (1 << 0),
};

// Packet Types
enum {
	packetAENEAS,		
	packetBEACON,
	packetPLYMOUTH
};

typedef enum {
    eErased = 0xff,
    eCleared = 0,
    eValid =  0xfe,
    eBadLength = 'L',
    eBadCrc = 'C'
} storedRecordDataState_t;
*/

/*
enum {
    RADIO_POWERED    = 0x1,
    GPS_AWAKE        = 0x2,
    CPU_AWAKE        = 0x4,
};
*/


#endif //__COMMON_H__
