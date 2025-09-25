//==================================================================================
//
//  main.c
//
//  Copyright: USC/ISI.
//
//  	This routine is the core software for a cubesat.
//
//==================================================================================

//=======================================
//         To Dos (By Will)
//=======================================

/*
	Telemetry Packet Sizes - We need to provide some sort of validation that
		the telmetry packets being requested are no bigger than the maximum size.
		First, what is the maximum size of a packet from the spacecraft?  I think
		it was arbitrarily set in the Java ground software as 200 byes - but that
		was simply to prevent a spurious length field from indicating an absurdly
		large packet.  Second, once this maximum packet size is defined, we
		need to filter out requests for telemetry packets taht would be too big.
		For example, if a user requests all the GNC values, that is approximately
		350 floats which is 1400 bytes.  Is this too big?  Also, related to this
		is the size of the actual command (scheduled, macro, or otherwise) that
		creates this packet.  The maximum command size (if I remember correctly)
		is somewhere around 250 btytes or something.  Since each telemetry element
		requires two bytes to identify it (for example *0001* refers to the "1"
		telemetry which is aprt of ExtIn[]), then you can only get 125 telemetry
		elements into a packet request.  Anyways, we need to clarify this and
		enforce it in three places: the C-function that accepts the telemetry
		packet command, the Java-function that processes the received packet,
		and finally Nate's PHP code that creates the command in the first place.

	PPM Specific Commands - When dealing with the user port (i.e. HyperTerminal),
		we can simply use "r" or "f" to denote which PPM we should send a command
		to.  However, when sending the commands over the MHX port, we need to
		preface any commands with the forward command (command 131)

	Buffer Size Validation - In buildTelemetryDataField(), the size of the buffer
		being built is never checked.

	What is the limit of PPM transfer packets?  Whats is the limit of MHX transfer packets?
		What happens when the Ground Software gets a packet that is defined differently
		than in the datebase?  What about when it is too big? Or too small? Or of
		a different format?

	Ground Station check for size in TelemetryDecoder.jar

	Signed/Unsigned Integers - How does a signed integer end up in the database, how
		about an unsigned integer.  Java does not support unsigned values.  So what do we do?
		Also, how do we easily convert two bytes into an unsigned value in Java?

	iTag commands - Document those itag commands inside the commands.txt file.

*/
/* *****jtb revision history*****
5/7/2014 set ifdef upper
		enable Maestro.c inclusion, breaks build BatteryState_g
		revert to lower for USB op
		build upper, uncomment BatteryState_g definition
		5464 ,line added-missing why?
		uncomment getBatteryState at end of main
		comment out #flight
		CMR: don't use "type" as an identifier
		#define custom pins
		CMR: substitute custompins.h from MaestroIntegration
		CMR: provide dummy MHXConnected routine.
		CMR: enable Maestro code
		CMR: disable the bootloader
		CMR: disable the iTag
		CMR: disable the burn wire
*/

#include "config.h"

#if defined(UPPER_PPM) && defined(LOWER_PPM)
#error "Can not compile for both PPMs"
#endif

#if !defined(UPPER_PPM) && !defined(LOWER_PPM)
#error "You must choose which PPM to compile for"
#endif

//========================================
//    		 	Super Global Defines
//========================================
#ifdef UPPER_PPM
#define PPM_NUM 0x02
//	#define CUSTOM_PINS	//jja commented 09/17/18					//jtb looking for Maestro
// operation
#else
#define PPM_NUM 0x01
//#define CUSTOM_PINS					// Used by Mike for pins on the lower PPM that are not
// compatible with the clean room. Comment out when in clean room.
#endif

//***CMR* #define _bootloader					// If defined, will load the bootloader.  Otherwise
// just reserves the empty space so it's not accidentally overwritten (LIES!  MPLAB/CCS overwrite it
// anyway.)

#define __VERSION__ "    Compiled " __DATE__ " at " __TIME__ ""
#warning __VERSION__

//========================================
//  		    DEFINITIONS
//========================================
#define GENERAL_BUFFER_SIZE 256 // Sets the buffer size of the general-purpose unsigned char Lbuf
#define MAX_PPM_PKT_SIZE \
	256 // Sets the total size for packets (not including sync characters) between PPM.

// other defines
#define CR 13 // used.  To find, search for CR; or CR)
#define LF 10 // used.  To find, search for LF; or LF)

// Communications
#define I_ACK 0x02 // Acknowledge bit
#define I_NAK 0x00 // No Acknowledge bit
#define LAST_CMD_SIZE 15 // Size of the storage space for last command executed

// Telemetry Defines
#define STORE_ONLY 0
#define STORE_AND_BEACON 1
#define BEACON_ONLY 2

// Battery States
#define BATTERY_STATE_LOW 0
#define BATTERY_STATE_MEDIUM 1
#define BATTERY_STATE_HIGH 2
#define BATTERY_STATE_UNKNOWN 99

//========================================
//    		 	Dependencies
//========================================
#include "common.h" // Global Definitions, Clock Speeds, Interrupt Registers and Port Settings
#include "pins.h" // Contains all the I/O pin settings
#include <limits.h> // Standard library for int,long,float limits
#include <string.h> // Standard string lib
#include <stdlib.h> // Standard lib
#include <stdio.h> // Added by Siamak for T6 test case!!!
#include <math.h> // Standard math lib
#include "address.h" // Addresses and size allocations for flash memory as well as global configuration

// Common Includes
#include "time.c" // Timer code
#include "crc.c" // Creating and checking crc's
#include "debug.c" // File for debugging functions
#include "errors.c" // Defines the ReturnErr_t type and the various errors it represents
#include "hyperterm.c" // Hyperterminal Functions, such as clearing the screen, locating the cursor, etc..
#include "macros.c" // Fred's macros, split into a library.
#include "ports.c" // Basic Port functionality, such as interrupts, unformatted input and output.
#include "mystack.c" // A generic stack implementation
#include "schedule.c" // Fred's schedule checker, split into a library
#include "spi.c" // SPI Module Support
#include "fredtime.c" // Fred's time vector and associated functions
#include "flash_AT25DF641.c" // Generic Flash functionality for the ATMEL chip
#include "loader_pcd.c" // Loads the bootloader or reserves the space.
#include "inet.c"
#include "globals.h"
#include "commandmanager.c"

// PPM-dependent Includes
#if defined(UPPER_PPM)
#warning--==| Compiling for UPPER PPM |= = --
#include "nonblock.c" // Generic Non-blocking functionality.
#include "tx_manager.c" // A nonblocking file manager and MHX TX manager
#include "linkmanager.c"
#include "payloadmanager.c"
#include "flashmanager.c"
#include "stensat.c"
#include "burn.c"
#define __PAYLOADS__
#include "adc.c" // Basic ADC functions.
//    Payload
#elif defined(LOWER_PPM)
#warning--==|  Compiling for LOWER PPM |= = --
#include "i2c.c" // Real time clock via i2c
#include "orbit.c" // Orbit propogator
#include "lower_tx.c" // Functions for tying the lower stack to the upper stack
#include "FSW.c" // Latest Flight Software
#include "sunsensor.c" // Functions for TX/RX from SS.
#include "mag.c" // Magnetometer Driver
#include "gyro.c" // Gyro driver
#include "imi100.c" // ADACS driver
#include "eps.c" // Power board driver
#include "geomag.c" // Geomagnetic model
#include "linalg.h" // Linear algebra routines for sensor scaling and rotation
#endif

//========================================
//    		 Unused Definitions
//========================================
/*
//#define version "10.05.09-v928"				// used once in getIO.  To find, search "version,"
//#define WAKEUP_CHANNEL_SWITCH_DELAY (8)		// delay to get the ACK out before changing channel

*/

//========================================
//  		Function Prototypes
//========================================
//----------------------------------------------------- Most important functions
int processCmd(int port, unsigned char* source, unsigned char* rbcmd, int ncmds, unsigned char ack);

//----------------------------------------------------- i/o functions
void checkUser(int port); // Checks for commands on the USER_PORT
ReturnErr_t check_PPM(int port); // Checks for inter-PPM comms
ReturnErr_t sendPPM(int port, unsigned char* data, unsigned char len); // Sends data to other PIC
ReturnErr_t sendPPMCmd(int port, unsigned char* data); // Sends an ASCII command to other PIC
ReturnErr_t check_ITAG(int port); // Check iTag Port
ReturnErr_t sendiTagCmd(unsigned char* data); // Send command to iTag
void check_LAN(int port); // Checking the MHX port
// Should the check_XXX functions should be combined?  Depacketizing/Packetizing should just above
// the port level? Maybe... The layers should be better enforced: Physical (ports, handshaking),
// Network (packetization/depacketization), Transport (acks).  These need to be better defined.
#ifdef UPPER_PPM
void getAndStorePayloadData();
void getAndProcessIncomingCommand();
void sendPayloadDataWhenReady();
#endif

void sendACK(int port, unsigned char value, unsigned char Seq_Num, unsigned char PK_ID,
			 unsigned char* source, unsigned char ready,
			 unsigned char encrypt); // A highly basterdized function.  Now it only sends the global
									 // AckFlag to the MHX, for TX/RX work with the ground.
void echo(void); // Connects two ports for serial debugging
void echoPorts(int portA, int portB,
			   int portTerm = USER_PORT); // echoes anything on Port A to Port B and vice versa.
										  // Looks for Termination code (~ESC) on PortTerm, which
										  // can also be either A or B.
unsigned char checkData(
	unsigned char* buf); // Validates the CRC and checks the TX_Code of an incoming message packet.
void showSchedulePacketized(); // packetizes and pushes onto MISC a printout of Schedules
void showMacroPacketized(); // packetizes and pushes onto MISC a printout of Macros

//------------------------------------------------------ menu functions
void showSchedule(int port, unsigned char page); // Display schedule to given port
void showMacro(int port, unsigned char page); // Display macros to given port
void showMainMenu(int port); // Display menu to given port

//------------------------------------------------------ Initialization and configuration functions
void Init_Device(void); // Initializes nearly everything
void Init_Handlers(void); // Initializes various data TX handlers
ReturnErr_t restoreGlobalConfig(
	TFlashProtected* config); // Copies from flash the globals struct into RAM
ReturnErr_t checkGlobalCRC(TFlashProtected* g); // Checks the CRC of the globals struct
ReturnErr_t restoreExtIn(
	float* ext); // Copies from flash the inputs to GNC Input vector.  Checks crc as it goes

//------------------------------------------------------ GNC functions
#ifdef __AENEAS__
void loadExtInDefaults(); // Initializes the GNC vector with defaults
ReturnErr_t loadExtIn(); // Loads up the GNC vector.
void showGNCvalues(int port, int page); // Temporary Testing function
void WriteNewRot(char SC, char* newnums);
void ChangeScaleRotation(char ReadWrite, char SensorChoice, char ScaleRot, char* newnums);
void WriteNewScale(char SC, char* newnums);
#endif

//------------------------------------------------------- ADC functions
#ifdef __ADC__
void readADC(int port, unsigned char channel);
#endif

//------------------------------------------------------- MHX Functions
#ifdef __MHX__
void CheckSync();
#else
int MHXConnected() {
	return 0;
} // ***CMR* Define a dummy routine, it is
  // simpler than proper conditional code.
#endif
void sendPACKETIZED(int port, unsigned char* msg, int len,
					unsigned char type = 1); // Send a packetized msg out a port.
void getPACKETIZED(unsigned char* msg, int* len, unsigned char type = 1,
				   unsigned int pageaddr = 0x574D,
				   int MAX_SIZE = 0); // Converts a msg to packetized data.  Checks for enough space
									  // in the array via the MAX_SIZE parameter

//------------------------------------------------------- Beacon Functions
// ReturnErr_t check_Stensat(int port);					// Debugging function to watch beacon return
// acks

//------------------------------------------------------- schedule functions
void once_per_second(void); // Bundler function for anything running at 1 Hz
// showSchedule under menu functions

//------------------------------------------------------ flash functions
void commitGlobalsToFlash(); // a new version that makes use of ATMEL's bigger page size
void commitExtInToFlash(); // commits the ExtIn vector to flash.
void Init_Flash(
	unsigned long* RWptr, unsigned long Lower_Boundary, unsigned long Upper_Boundary,
	unsigned char findedge = 0); // Unprotects sectors and initializes the read-write pointer
void flashPrintSectorProtection(int port); // Displays the sector protection of each sector in flash
void flashReadTest(unsigned long address,
				   int port); // Displays data from a particular flash location to a port
ReturnErr_t IncrementRebootCounter(); // Does what it says
void ResetRebootCounter(); // Does what it says

//------------------------------------------------------ Bootloader Functions
#ifdef __BOOTLOADER_H__
ReturnErr_t uploadHexFile(int listenPort, int reportPort, unsigned long initial_offset,
						  unsigned long base_addr, unsigned long alloc_size);
ReturnErr_t uploadBootableFile(int listenPort, int reportPort);
ReturnErr_t checkHexFile(int port, unsigned char* crc);
ReturnErr_t verifyHexLine(unsigned char* line, unsigned int MAX_SIZE);
#endif

//------------------------------------------------------ Stack Functions
#ifdef __MYSTACK__

void stack_print(int port, stack_t* S); // Displays the stack on the given port

ReturnErr_t PacketizeAndPush(
	unsigned char* msg, int len, stack_t* S, int max_len = 0, int pkt_type = 0,
	int page_addy =
		0); // Packetizes data and puts it onto a stack.  Be sure to specify the maximum length.

// int CheckStackAndPrint();
#endif // __MYSTACK__

//------------------------------------------------------- Orbit functions
#ifdef __ORBIT__
void orbitCmd(unsigned char* cmd);
// void orbitTest();
// void orbitSendPPM();		// Sends binary orbit data to the other PPM
#endif

//------------------------------------------------------- Test functions
void RunCommand(unsigned char* ascii_cmd); // Runs any ascii command in ProcessCmd
void PadCommand(
	unsigned char* rcmd); // Pads a command to put it in the correct format for ProcessCmd
int checkMode(); // debug function for checking the FSW mode

//------------------------------------------------------ TX_Manager Functions
#ifdef __TXMANAGE__
void PrintTX_t(TX_t* T, int port);
#endif

//------------------------------------------------------ Sensors
#ifdef LOWER_PPM
void attitudeSensorsBackgroundTask();
void attitudeSensors4HzTask();
#endif

//------------------------------------------------------ EPS and Batteries
#ifdef __EPS_H__
void EPSBackgroundTask(short reset);
int checkSun();
int checkBatteryState(eps_s* E); // Returns the state of the batteries (HIGH/MED/LOW)
void updateOtherProcessBatteryState(); // Update the other processor
#endif

//------------------------------------------------------ Telemetry
int addTelmetryElement(unsigned char* data, int index);
int16 buildTelemtryDataField(int cmd_length, unsigned char* rbcmd, unsigned char startIndex,
							 unsigned char* buf, int max_length);
void printTelemetryElements(int cmd_length, unsigned char* rbcmd, unsigned char startIndex);

//------------------------------------------------------ Helper Functions
int fbit(int value, int n); // Returns the nth bit of an int, converted to an int.
void rcpy(unsigned char* buf1, unsigned char* buf2, unsigned char len); // Reverse copy function
unsigned int C2I(unsigned char* buf); // Converts two bytes to equivalent 16-bit integer.
									  // Deprecated, use make16() instead.
ReturnErr_t getFloats(float* floatarray, unsigned char* floatstring,
					  int num_floats); // Obtains a certain number of floats from a string, and
									   // loads into a float array. Useful for input processing.

//------------------------------------------------------ Unsorted
void clearBuf(unsigned char value); // Clears macros, schedules and other global items.

//------------------------------------------------------ Unused

// unsigned char checkAddr(unsigned char *addr1, unsigned char *addr2);	// Used to compare four
// bytes from any two given addresses ReturnErr_t UnParse(unsigned char* original, unsigned char*
// processed);		// UnParses commands to ASCII text
// void check_Plymouth();						// Checking the Plymouth Port (deprecated.  Now it's
// read by the nonblocking lifo buffer) void getCFG(unsigned char *buf);
// // originally used in getIO void MultiPrint(int port, unsigned char * msg, int len, unsigned char
// ascii=1); void checkSystemEvents();										//

//==================================================================
//  				Globals (by category)
//=================================================================
//---------------------------------------- CONFIG
TFlashProtected globals; // ***Huge*** custom data structure for saving things to flash
int manual_restart_reason; // Set to the Restart register
int restart_reason; // Global to hold the latest reason for restarting (resettable)
int prev_restart_reason; // Global to permanently hold last restart reason (for beaconing)
int num_reboots = 0; // Global to hold the number of reboots
#ifdef UPPER_PPM
LinkManager gLinkManager;
PayloadManager gPayloadManager;
FlashManager gFlashManager;
#endif

//--------------------------------------- ENABLE/DISABLE Flags
// All flags have been moved to the TFlashProtected globals structure

#ifdef __GEOMAG__
// short Geo_Enable=1;									// Flag for geomag model.
// short ReadGeomagFromFlash=0;						// Signal if we read geomag from flash or not.
#endif

#ifdef __ORBIT__
// short orbitEnable=1;								// Flag for orbit model
#endif

#ifdef __AENEAS__
char showGNCscreen = 0; // Debugging screen
// short GNC_Enable=1;									// Flag for GNC model running
// short load_Enable=1;								// Flag for loading values into GNC
#endif

// short attitudeSensorsEnable=1;						// Flag for Attitude Sensors

//--------------------------------------- BUFFERS
unsigned char Lbuf[GENERAL_BUFFER_SIZE]; // general purpose buffer

#ifdef __MYSTACK__
#define MISC_STACK_ITEMS 1 // Settings for the Miscellaneous Stack
#define MISC_STACK_ITEM_SIZE 256 // Settings for the Miscellaneous Stack
#endif

#ifdef UPPER_PPM
const int BEACON_STACK_ITEMS = 2; // Settings for the Beacon Stack
const int BEACON_STACK_ITEM_SIZE = 256; // Settings for the Beacon Stack
#endif

//--------------------------------- COMMUNICATIONS
#ifdef __MHX__
unsigned char AckFlag =
	0x00; // Flag that flips on every recieved MHX command, to enable a simple acking system.
unsigned char RX_Seq_Num =
	0; // This is the Receiver sequence number from the last received packet.  Never really used
unsigned char mhx_status = 0; // Contains the last reading of the MHX pins.
#endif
unsigned char g_last_cmd[LAST_CMD_SIZE + 1]; // A global unsigned char to hold the last executed
											 // command coming in over MHX

//---------------------------------------- HANDLERS
// Nonblocking handlers
#ifdef __NONBLOCK__
nonBlock_t
	BEA_nb; // Nonblock handler for Beacon Data Transfers (controls flow of nonblocking functions)
nonBlock_t
	MISC_nb; // Nonblock handler for Misc Data Transfers (controls flow of nonblocking functions)
#endif

// Stack Handlers
#ifdef __MYSTACK__
#ifdef UPPER_PPM
stack_t* BEA; // Global Beacon Stack Handler Pointer
#endif
stack_t* MISC; // Global Miscellaneous Stack Handler Pointer
#endif

#ifdef __TXMANAGE__
// TX Handlers
TX_t TX_data; // Transfer Handler for CAERUS->Ground Transfers of Plymouth Data (controls flow of
			  // data to MHX or flash)
TX_t BEA_data; // Transfer Handler for Beacon Data Transfers (controls flow of data to MHX or flash)
TX_t MISC_data; // Transfer Handler for Misc Data Transfers (controls flow of data to MHX or flash)
#endif

#ifdef __LOWER_TX__
tx_properties_t LOWER_TX_HANDLER; // A handler for connecting the stacks between processors.
#endif

//------------------------------------------- TIME
enum
{
	showTimeOFF,
	showTimeOVERWRITE,
	showTimeNEWLINE
};
static unsigned char I2C_Clock_OK =
	TRUE; // Used to indicate the I2C clock status.  Needs to be incorporated into a Time driver.
unsigned char showTimeFlag = showTimeOFF; // Used to decide how to show the time in once_per_second
#ifdef __I2C__
unsigned char halted_time[7]; // time array used for finding when last reboot was.
#endif
rtc_time_t rtcTime; // Real time clock
unsigned char time[7]; // 7-byte timetag used in scheduling.

//---------------------------------------- DISPLAY
unsigned char NextPage;
// unsigned char displayFormat[6];								// Used in setting the precision of
// displayed numbers
#ifdef __AENEAS__
void PrintExt(float* data, int start, int stop); // Displays the ExtOut Vector
#endif

//---------------------------------------- STORAGE
//#ifdef UPPER_PPM
unsigned long FlashWriteAddr; // A pointer used for storing stuff in flash
unsigned long TelemWriteAddr; // A pointer used for storing telemetry
//#endif
unsigned long MiscWriteAddr; // A pointer used for storing miscellaneous packets
unsigned char flash_status[3]; // Contains the last reading of flash status

//---------------------------------------- SENSORS / ACTUATORS

//............Gyro Data Structure and Flag
#ifdef __GYRO_H__
gyro_s GyroData; // Gyro Data
gyro_s
	GyroAveragingData; // Another struct used for getting multiple gyro readings and averaging them.
ReturnErr_t GyroFlag = FAILURE; // Indicates Gyro Data Found
#endif

//............IMI Data Structure and Flag
#ifdef __IMI100_H__
imi_s IMIData; // IMI-100 Data
imi_converted_s IMIConvertedData; // Used for unit and frame conversions to/from the FSW and IMI
ReturnErr_t IMIFlag = FAILURE; // Indicates IMI Sensor Data Found
// unsigned char IMIEnable = 0;						// Enable/Disable for the wheels at the PIC
// level
#endif

//............Mag Data Structure and Flag
#ifdef __MAG_H__
signed int MagData[3]; // Mag Data
mag_s MagConvertedData; // Structure of converted Mag data
ReturnErr_t MagFlag = FAILURE; // Indicates Mag Sensor Data Found
#endif

//............Sun Sensor Data Structure and Flag
#ifdef __SUNSENSOR__
// Data Structure
sunsensor_s SunData; // Sun Sensor Data
// Flag
ReturnErr_t SunFlag = FAILURE; // Indicates Sun Sensor Data Found
// Channels of interest (see header file for channel names)
unsigned char sunChannels[5] = {
	// Channels to collect
	SS_TELEMETRY, SS_TEMPERATURE, SS_SUN_VECTOR_FLOAT, SS_PROCESSING_TIME, SS_SUN_VECTOR_INT,
};
int sunNumChannels = 5; // Channels to collect#endif
short SunSync =
	0; // Sun sensor sync bit. Used to sync up state with the sun sensor to avoid duplicate packets.
#endif

#ifdef LOWER_PPM
#ifdef DEBUG
short displaySensors = 0; // Allow user to control turn the display of data on or off
#endif
#endif

//------------------------------------- POWER AND BATTERIES
#ifdef __EPS_H__
eps_s EPSData;
short StartEPSAquireFlag = 0;
ReturnErr_t EPSFlag = FAILURE;
#endif
unsigned char BatteryState_g =
	BATTERY_STATE_UNKNOWN; // Global to hold the state of charge of the battery (LOW, MEDIUM, HIGH)

//------------------------------------- ADC SENSORS
#ifdef __ADC__
float temperatureSensors[4]; // Mounted RTDs
// float senseUpperPPM;					// Sense for Upper PPM (PPM)
// float senseLowerPPM;					// Sense for Lower PPM (FPM)
#endif

//------------------------------------------ DEBUG
// unsigned char echomode;							// Echos anything on the USER_PORT to the
// "Plymouth" Port
#ifdef __AENEAS__
// short showExtIn=0;									// Shows GNC input data
// short showExtOut=0;									// Shows GNC output data
#endif

//------------------------ UNSURE / UNUSED / OTHER
unsigned char DataByte[10]; // ?? used in some of the macro commands??
unsigned char ScheduleMask[8]; // Mask to enable or disable individual schedules
// int dumpPtr;										//?? No idea.  Probably a pointer to the stored
// records. Weird that it's an int... register unsigned int storePtr;					// Stored
// Data stuff
// register unsigned char storedDataFlag;						// A flag signifying that data has
// been stored unsigned int storedDataPid;						// The id # for stored data unsigned
// char USER_PORT;
// // Stores the port used for menu interactions unsigned char nTextMsg;
// // used in storing Text Messages. unsigned char debugMSGFlag = 0;							//
// Global flag used for debug messages
// unsigned char debuglvl = 0;								// Set but never used
// long up_connection_quality=100;					// Something to simulate lossy connections (0 to
// 100)
// long down_connection_quality=100;					// Something to simulate lossy connections
// (0 to 100) register unsigned char rebootCounter;						//?? unused except for
// display? unsigned char stime[];										//?? Never set but used
// unsigned char debugNoisy;									//?? Set but never used.
// unsigned char SeqNum;										// Sequence number. //?? This number is
// never initialized, but is used in SendPacket register static float seconds;					//??
// Possible conflicts with utc2gmt() and getTime().  Never used otherwise.  [fixed] unsigned int
// BeaconTimer;
// //?? unused [fixed]
// static unsigned char Command_ID, ack;						//?? Command_ID and ack have
// multiple scope conflicts! [fixed] static long p_RTC = 0;							// unused?
// signed char menuMacroId;
// // set once, never used.
// unsigned char RunCmd[10];									// Set but never used?
// long sleepCycleOutputTimer;						// Set but never used?
// long prevRTC;										// used in readUptime(), which is never
// called by anything
// unsigned long uptRemainder;						// used in readUptime(), which is never called
// by anything
// unsigned long uptime;								// used in readUptime(), which is never
// called
// by anything signed char beaconRSSI;							// set but never used unsigned int
// wakeUp;								// set but never used unsigned char
// networkCoordinatorAddress[8];				// set but never used unsigned int commTimer;
// // Communications Timer.
// unsigned char Channel;										// Network Channel.  //?? Possible conflict
// in GetIO() with local channel variable[fixed]
// unsigned char Associated;							// Associated with Zigbee network
// unsigned char sleepMode;							// unsigned char to hold sleep status.  //??
// Possibly redundant with global variables: globals.Sleepmode and sleep unsigned int nsamples;
// // used in 2-parameter commands. unsigned char alarmBytes[NUM_ALARM_BYTES];					//
// used in alarms?  Not sure how that works.
// unsigned char RTU_mode;							    // Radio Transmission Unit mode [caused a
// problem with getIO in oncepersecond -- fixed by initializing in Init Device] unsigned char
// boardCFG;
// //?? Redundant with globals.boardConfig
// unsigned char sleep;											// something to do with sleeping?
// //?? Possibly redundant with global variables: globals.Sleepmode and sleepMode wakeReason_t
// lastPowerSaveResetReason =
// wakeReasonNone;  //?? unsigned char activityCounter;						//?? Redundant with
// globals.wakeCount unsigned char BrdAddr, ExtAddr;								// Board address
// and Extended address. unsigned long globalEventRegister = 0;				// Used in
// CheckSystemEvents and others, but those functions are commented out unsigned long
// globalWakeEvents = 0;
// // Used in CheckSystemEvents and others, but those functions are commented out unsigned char
// send_Data[NUM_PORTS+1];				// Controls...? unsigned char display[NUM_PORTS+1];
// // Controls the type of display. 's'tring, 'b' (Normal mode), 'F'alse (Data display is off).
// unsigned char format[NUM_PORTS+1];					// Controls the format of display. 'A'scii,
// 'B'inary, or 'N'one. unsigned char menu[NUM_PORTS+1];					// Menu flag. Can be
// T,F,A,M,P,or D.  No idea what those
// are for. unsigned char PC[NUM_PORTS+1];						// PC mode Flag. unsigned char
// uploadSource[4], uploadID, nSent;			// Uploading variables
// int uploadCounter;								// counter to make sure we only pass through the
// data once unsigned int uploadPort;							// Global to hold the upload port
// unsigned char upload;								// ?? unsigned long SEC_TIMER=0;
// // A 1-second timer.   Rolls over every 136
// years. unsigned char RunTasks;									// Can be either 'B'ackground or
// 'S'cheduled. int t_sec=0;										// 10th second timer counter.
// Loops 0->9
// unsigned char SatClk;										// Used once as a return value in
// setting the time? Mostly unused

//========================================
//  		Global Constants
//========================================
// const unsigned char IDLE [] =        "Idle       ";
// const unsigned char IN_PROGRESS [] = "In Progress";
// const unsigned char ERASED [] = "Erased";
/// const unsigned char CLEARED [] = "Cleared";
// const unsigned char VALID [] = "Valid";
// const unsigned char BAD_LENGTH [] = "Bad Length";
// const unsigned char BAD_CRC [] = "Bad CRC";

#ifdef DEBUG
const unsigned char showText1[] = "     University of Southern California   ";
#if defined(UPPER_PPM)
const unsigned char showText2[] = "            Upper PPM     ";
#elif defined(LOWER_PPM)
const unsigned char showText2[] = "            Lower PPM     ";
#endif
const unsigned char showText3[] = __VERSION__;
const unsigned char showText4[] = " Hyperterminal Commands:";
const unsigned char showText5[] = " 'mm' Show this menu";
const unsigned char showText6[] = " 'ma' Show macros";
const unsigned char showText7[] = " 'mp' Show schedules";
const unsigned char showText8[] = " 'T[TEST NUMBER]' -- Run a specific test (Eg: T1, T2..)";
const unsigned char showText9[] = " 'r [COMMAND]' -- Run a command";
const unsigned char showText10[] = " 'f [COMMAND]' -- Run a command on other processor";
const unsigned char showText11[] = " 'i [COMMAND]' -- Run a command on the iTag";
const unsigned char showText12[] = " 'c' -- Clear Screen";
const unsigned char showText13[] = " 'p' -- Toggle Time";
const unsigned char showText14[] = " Type your choice >> ";
#endif // DEBUG

// Quick function to test stuff
/*
void testStuff(int num_to_print)
{
	#ifdef DEBUG
	sprintf(dbgbuf,"\r\nRequested Number: %d",num_to_print);
	sendDBGALL(USER_PORT,dbgbuf);
	delay_ms(250);
	#endif
}
*/

#ifdef __TXMANAGE__
// QUick function to check out the TX data.
/*
void PrintTX_t(TX_t * T,int port)
{
	sprintf(dbgbuf,"\r\nTable Ptr,Value: 0x%Lx\t0x%Lx",T->table_ptr,*T->table_ptr);
sendMSGALL(port,dbgbuf); sprintf(dbgbuf,"\r\nRead Ptr,Value:
0x%Lx\t0x%Lx",T->read_index_ptr,*T->read_index_ptr); sendMSGALL(port,dbgbuf);
	sprintf(dbgbuf,"\r\nworking_ptr: %i",T->working_ptr); sendMSGALL(port,dbgbuf);
	sprintf(dbgbuf,"\r\ntypeinfo: %i",T->typeinfo); sendMSGALL(port,dbgbuf);
	sprintf(dbgbuf,"\r\nRW_ptr Ptr,Value: 0x%Lx\t0x%Lx",T->RW_ptr,*T->RW_ptr);
sendMSGALL(port,dbgbuf); sprintf(dbgbuf,"\r\nNewData: %i",T->NewData); sendMSGALL(port,dbgbuf);
	sprintf(dbgbuf,"\r\nallocation_start_addr: 0x%Lx",T->allocation_start_addr);
sendMSGALL(port,dbgbuf); sprintf(dbgbuf,"\r\nallocation_size: 0x%Lx",T->allocation_size);
sendMSGALL(port,dbgbuf);
}
*/
#endif

//====================================================
//  		Interrupt on IMI-100 Pin Change
//====================================================

#ifdef __IMI100_H__
/*
This interrupt is for checking the magnetometer only when data is ready.
However, because the Mag and other devices share an SPI line, this can
cause data corruptions when the interrupt occurs during other SPI operations.
Therefore, mag checking was re-coded as a nonblocking function that must be called "often."

#int_CNI
void int_cni()
{
	fputc('M',COM_B);

	// If we have enabled sensors
	if(globals.attitudeSensorsEnable)
	{
		#ifdef __MAG_H__
		// Read the Mag
		if(readAllMagAxis(MagData,0) == SUCCESS)
		{
			MagFlag = SUCCESS;
		}
		#endif
	}
}
*/
#endif

//==================================================================
//  							Main
//==================================================================
void main(void) {
// Decclarations
#ifdef LOWER_PPM
	int64 timeout; // Delay variable for running background Attitude tasks
#endif
#ifdef __AENEAS__
	short loadSuccess = 0; // Flag indicating Sensor values were successfully loaded into ExtIn[]
#endif
	manual_restart_reason = RCON; // Records version 1 of the restart reason straight from the chip
	restart_reason = restart_cause(); // Records the reason for restarting using internal function.
	prev_restart_reason = restart_reason; // Stores restart_reason

	output_low(USB_OE); // Turns on the USB

#ifdef DEBUG
	delay_ms(500); // Small delay used during programming to avoid spurious writes to the reboot
				   // counter in flash
#endif

#ifdef LOWER_PPM
	delay_ms(3000); // 3-second delay to let the lower ppm boot up slower than the upper, and avoid
					// setting the boot timer early.
#endif

#ifdef __MYSTACK__
	// Create the message stacks.
	// 1. Beacon Stack -- used to store formatted beacon data that is going to be sent to MHX/Flash
	// 2. Misc Stack -- used to store health and status
	// TODO: Create a single stack that is used for everything.  Have the messages themselves
	// describe where they're going.

#ifdef UPPER_PPM
	MAKE_STACK(BEA_stack, BEAstackHandle, BEACON_STACK_ITEMS, BEACON_STACK_ITEM_SIZE);
	BEA = &BEAstackHandle;
#endif

	MAKE_STACK(MISC_stack, MISCstackHandle, MISC_STACK_ITEMS, MISC_STACK_ITEM_SIZE);
	MISC = &MISCstackHandle;
#endif

	// Initialize almost everything
	Init_Device();

	for (;;) {
		switch (RunTasks) {
		case SCHEDULED_TASKS: // Scheduled tasks (runs every 250 M_SEC)
			// List of scheduled Tasks
			// 	 -- [LOWER] Load all measured values into the GNC input array.
			//	 -- [LOWER] Start sensor polling.
			//	 -- [LOWER] Run GNC
			//   -- [LOWER] Write commands to IMI
			//   -- [LOWER] Propogate orbit
			//   -- [LOWER] Propogate geomagnetic model
			//   -- [BOTH] Run Macro Commands
			//	 -- [UPPER] Check MHX and Sync
			//	 -- [BOTH] Once per second functions

			if (MeasureFlag)
				MeasureTime(MEASURE_TIME_INIT);

// --------------------------------------------------   Load in all values to ExtIn[] (5tks)
#ifdef __AENEAS__
			if (globals.load_Enable) {
// Convert all inputs into correct units/frame
#ifdef __SUNSENSOR__
				convertFrameSStoBody(&SunData); // Sun Sensor frame and unit conversion
#endif
#ifdef __MAG_H__
				convertFrameMagToBody(
					MagData, IMIData.gf_torqueCoilCmd,
					&MagConvertedData); // Mag frame and unit conversion (MagData is an array, not a
										// struct. Don't need address operator)
#endif
#ifdef __GYRO_H__
				convertFrameGyrosToBody(&GyroData); // Gyro frame and unit conversion
#endif
				convertFrameIMIToBody(&IMIData, &IMIConvertedData); // IMI frame and unit conversion

// TODO:Additional scale and rotate
#ifdef __SUNSENSOR__
				ScaleAndRotate(&SunData.Vector[0], &SunData.Vector[1], &SunData.Vector[2],
							   &globals.SSScale[0], &globals.SSRot[0]);
#endif

				ScaleAndRotate(&IMIConvertedData.rwa_SpeedRPSBodyFrame[0],
							   &IMIConvertedData.rwa_SpeedRPSBodyFrame[1],
							   &IMIConvertedData.rwa_SpeedRPSBodyFrame[2], &globals.RWAScale[0],
							   &globals.RWARot[0]);

#ifdef __MAG_H__
				ScaleAndRotate(&MagConvertedData.microTesla[0], &MagConvertedData.microTesla[1],
							   &MagConvertedData.microTesla[2], &globals.MagScale[0],
							   &globals.MagRot[0]);
#endif
#ifdef __GYRO_H__
				ScaleAndRotate(&GyroData.f_rateRPS[0], &GyroData.f_rateRPS[1],
							   &GyroData.f_rateRPS[2], &globals.GyroScale[0], &globals.GyroRot[0]);
#endif

				// Load them into the FSW input vector
				if (loadExtIn() == SUCCESS)
					loadSuccess = 1;
				else
					loadSuccess = 0;
			}
#endif

#ifdef LOWER_PPM
			// ----------------------------------------------------------------- Start aquisition
			// cycle
			if (globals.attitudeSensorsEnable) {
				attitudeSensors4HzTask(); // Start polling sensors.  3 ms

				// Run the background task for a while to make sure things get started correctly.
				timeout = MSEC + 5;
				while (timeout > MSEC) {
					attitudeSensorsBackgroundTask(); // Read Data from sensors (Non-blocking)
				}
			}
#endif

#ifdef __AENEAS__
			// ------------------------------------------------------------------ Run GNC and
			// Command Wheels
			if ((prev_restart_reason == RESTART_POWER_UP || // Previous restart was a POWER UP
				 prev_restart_reason == RESTART_SOFTWARE) // OR a software restart
				&& globals.GNC_Enable) // AND the user has requested GNC to be run
			{
				SCHEDULER(); // Run GNC (Duration: 63 ms)
				safetyCheck(); // Checks to see if the attitude quaternion has exploded

				//				sendByte(USER_PORT,'G');
				//				if(loadSuccess) printMagData(MagData);

// Now run the wheels
#ifdef __IMI100_H__
#ifdef __AENEAS__
				// Convert the GNC Outputs to IMI Frame
				convertFrameBodyToIMI(&IMIConvertedData, ExtOut[Cmded_Whl_Torque_x_OUT],
									  ExtOut[Cmded_Whl_Torque_y_OUT],
									  ExtOut[Cmded_Whl_Torque_z_OUT], ExtOut[Cmd_Dipole_X_OUT],
									  ExtOut[Cmd_Dipole_Y_OUT], ExtOut[Cmd_Dipole_Z_OUT]);

				// Generic Scale and rotate
				ScaleAndRotate(&IMIConvertedData.rwa_TorqueCmdIMIFrame[0],
							   &IMIConvertedData.rwa_TorqueCmdIMIFrame[1],
							   &IMIConvertedData.rwa_TorqueCmdIMIFrame[2], &globals.RWAScale[0],
							   &globals.RWARot[0]);

				ScaleAndRotate(&IMIConvertedData.gc_torqueCoilCmd[0],
							   &IMIConvertedData.gc_torqueCoilCmd[1],
							   &IMIConvertedData.gc_torqueCoilCmd[2], &globals.MTRScale[0],
							   &globals.MTRRot[0]);

				// For real, write to IMI
				if (globals.IMIEnable) {
					setIMIReactionWheelTorque_mNm(IMI_PORT,
												  IMIConvertedData.rwa_TorqueCmdIMIFrame[0],
												  IMIConvertedData.rwa_TorqueCmdIMIFrame[1],
												  IMIConvertedData.rwa_TorqueCmdIMIFrame[2]);
					setIMICoilDipole_Am2(IMI_PORT, IMIConvertedData.gc_torqueCoilCmd[0],
										 IMIConvertedData.gc_torqueCoilCmd[1],
										 IMIConvertedData.gc_torqueCoilCmd[2]);
					//				printIMICnvData(&IMIConvertedData);
				}
#endif //__AENEAS__
#endif //__IMI100_H__
			} //
			else {
				//				sendByte(USER_PORT,'X');
			}
#endif

#ifdef __ORBIT__
			// --------------------------------------------------------------------- Run the Orbit
			// Model (~9 tks)
			if (globals.orbitEnable) {
				orbit(ORBIT_NO_INIT, time, 0.0,
					  orbitData); // Using 0.0 as dt makes the orbit determine the integration step
								  // from the RTC.
				/*
									printFloat(dbgbuf,orbitData[6],6);
				   locateString(USER_PORT,row_start+3,col_start,dbgbuf);
									printFloat(dbgbuf,orbitData[7],6);
				   locateString(USER_PORT,row_start+4,col_start,dbgbuf);
									printFloat(dbgbuf,orbitData[8],6);
				   locateString(USER_PORT,row_start+5,col_start,dbgbuf);
									printFloat(dbgbuf,orbitData[9],6);
				   locateString(USER_PORT,row_start+6,col_start,dbgbuf);
									printFloat(dbgbuf,orbitData[10],6);
				   locateString(USER_PORT,row_start+7,col_start,dbgbuf);
									printFloat(dbgbuf,orbitData[11],6);
				   locateString(USER_PORT,row_start+8,col_start,dbgbuf);
									printFloat(dbgbuf,orbitData[15],6);
				   locateString(USER_PORT,row_start+9,col_start,dbgbuf);
				*/
			}
#endif

#ifdef __GEOMAG__
			// --------------------------------------------------------------------- Run the Geomag
			// Model (~46 to 51 ms)
			if (globals.Geo_Enable) {
				geomag(B_field_ECI, &orbitData[6], orbitData[15], time[2] * 365.25,
					   globals.ReadGeomagFromFlash);
				//					printFloat(dbgbuf,B_field_ECI[0],6);
				// locateString(USER_PORT,row_start,col_start,dbgbuf);
				//					printFloat(dbgbuf,B_field_ECI[1],6);
				// locateString(USER_PORT,row_start+1,col_start,dbgbuf);
				//					printFloat(dbgbuf,B_field_ECI[2],6);
				// locateString(USER_PORT,row_start+2,col_start,dbgbuf);
			}
#endif

			// ----------------------------------------------------------------------- Macros and
			// Comms
			restart_wdt(); // Kick the dog
#ifdef __MACROS__
			checkMacros(); // Fred's Macros
#endif
#ifdef UPPER_PPM

// check_LAN(MHX_PORT); // Checks the MHX for incoming data
// CheckSync(); // Checks the synchronization to the ground
#endif

			// -----------------------------------------------------------------------
			// Frequency-Based Tasks
			switch (t_sec) {
			// sprintf(dbgbuf,"\r\n switch t_sec: %d",t_sec);
			case 0: // 1 Hz
				once_per_second(); // anything that runs once per second goes in this function
			case 1: // 250 ms
					// twice_per_second (notice the lack of a "break", all cases fall through)
			case 2: // 500 ms
					// 3x_per_second
			case 3: // 750 ms
					// 4x_per_second
			default:
				break;
			}

			RunTasks =
				BACKGROUND_TASKS; // Switch to bakground tasks. (Reset to scheduled tasks by ISR)

			if (MeasureFlag)
				MeasureTime(MEASURE_TIME_SHOW);

			break;

		case BACKGROUND_TASKS: // ---------------------------------- Background tasks
		default: {
#ifdef LOWER_PPM
			if (globals.attitudeSensorsEnable)
				attitudeSensorsBackgroundTask(); // Read Data from sensors (Non-blocking)
#endif

#ifdef UPPER_PPM
			getAndStorePayloadData();
			getAndProcessIncomingCommand();
			sendPayloadDataWhenReady();
#endif

#ifdef __EPS_H__
			EPSBackgroundTask(0); // Aquire data from EPS - no restart (Non-blocking)
#endif

#ifdef DEBUG
			checkUser(USER_PORT); // Checks USER_PORT for commands
#endif

			check_PPM(OTHER_PPM_PORT); // Check for commands from other Processor

#ifdef __ITAG_H__
#ifdef MAESTRO_QUELL_ITAG
			if (!maestro_quell_itag()) {
				check_ITAG(ITAG_PORT); // Check for commands from iTag radio
			}
#else // MAESTRO_QUELL_ITAG
			check_ITAG(ITAG_PORT); // Check for commands from iTag radio
#endif // MAESTRO_QUELL_ITAG
#endif // __ITAH_H__

#ifdef __TXMANAGE__
			TX_nonBlockProcess(&BEA_nb); // Manages all Telemetry data Transmission
			TX_nonBlockProcess(&MISC_nb); // Manages all Misc data transmission
#endif

#ifdef __LOWER_TX__
			CheckAndSend(&LOWER_TX_HANDLER); // Manages the lower PPM MISC stack (sends anything on
											 // it to the upper)
#endif

			//			#ifdef __BEACON__
			//				check_Stensat(BeaconPort);				// Prints to the screen anything
			// that comes back from the beacon 			#endif

#ifdef __MAESTRO__
			run_maestro_test();
#endif

			break;
		}
		} // Run Tasks
	} // While 1
}

//======================================//
//           Init Device                //
//======================================//
void Init_Device(void) {
	int n;
	unsigned long timeout;
	unsigned char response;
	ReturnErr_t err;

// TODO: Put this in an SPI section that specifically disables ALL chip selects, to be safe.
#ifdef LOWER_PPM
	// Explicitly disable both flashes to start.
	output_high(FLASH_CHIP_SELECT);
	output_high(SECOND_FLASH_CS);

#else

	// On the upper too?
	// output_low(SCK1OUT);

#endif

	// ---------------------------------------------------------------------- Oscillators and
	// Interrupts
	setup_rtc(
		RTC_ENABLE | RTC_OUTPUT_SECONDS,
		0); // Sets up Real Time Clock.  Set Calibration to 0x00, which was 0xFF for some reason

	//(SPI_MASTER | 0x0100 | SPI_CLK_DIV_16 | SPI_SCK_IDLE_HIGH);		// TODO: Check if these
	// settings are best setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_16 | SPI_SCK_IDLE_HIGH
	// );		// New settings for gyro?  32Mhz/16 = 2 Mhz

	/* SPI Setup

	Table showing various units and their requirements. Defaults in parenthesis, I think... Who
	knows what the defaults are with this compiler...

	slave	| Modes		| Idle (0)	| Sample (Falling)	| Max Speed	Mhz	|
	--------|-----------|-----------|-------------------|---------------|-----
	gryo	|	3		|	1		| Rising			|	2.5			|
	mag		| 	?		| 	?		| Rising			|	1.0			|
	flash	| 0 or 3	| 0 or 1	| Rising			|	50.0		|

	*/
	//	setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_32 | SPI_SCK_IDLE_HIGH |
	// SPI_SAMPLE_AT_END); // Not bad, but flash is terrible 	setup_spi(SPI_MASTER |
	// SPI_XMIT_L_TO_H | SPI_CLK_DIV_1 | SPI_SCK_IDLE_HIGH | SPI_SAMPLE_AT_MIDDLE); // REally bad.
	// Flash and mag both broken! 	setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_1 |
	// SPI_SCK_IDLE_LOW | SPI_SAMPLE_AT_END); // AWESOME for Flash.  Not so much for mag...
	// setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_32 | SPI_SCK_IDLE_LOW |
	// SPI_SAMPLE_AT_END); // Gyro broken, flash (geomag) unclean.  Mag is good though.
	// setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_1 | SPI_SCK_IDLE_HIGH |
	// SPI_SAMPLE_AT_END); // Mag broken, gyro broken, flash half-broken 	setup_spi(SPI_MASTER |
	// SPI_XMIT_L_TO_H | SPI_CLK_DIV_16 | SPI_SCK_IDLE_HIGH | SPI_SAMPLE_AT_END); // Gyro Good. Mag
	// looks good. Flash terrible.

	//	setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_1 | SPI_SCK_IDLE_LOW |
	// SPI_SAMPLE_AT_END);  // Good 	setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_1 |
	// SPI_SCK_IDLE_HIGH | SPI_SAMPLE_AT_END);  // Bad 	setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H |
	// SPI_CLK_DIV_16 | SPI_SCK_IDLE_HIGH | SPI_SAMPLE_AT_MIDDLE); // There's an extra factor of 2
	// for some reason, so this is actually 1 Mhz. 	setup_spi(SPI_MASTER | SPI_XMIT_H_TO_L |
	// SPI_CLK_DIV_16 | SPI_SCK_IDLE_HIGH | SPI_SAMPLE_AT_END); // Phenomaenally broken
	//	setup_spi(SPI_MASTER | SPI_XMIT_H_TO_L | SPI_CLK_DIV_16 | SPI_SCK_IDLE_LOW |
	// SPI_SAMPLE_AT_END); // Phenomaenally broken again 	setup_spi(SPI_MASTER | SPI_XMIT_H_TO_L |
	// SPI_CLK_DIV_1 | SPI_SCK_IDLE_LOW | SPI_SAMPLE_AT_END); 	setup_spi(SPI_MASTER |
	// SPI_XMIT_L_TO_H | SPI_CLK_DIV_16 | SPI_SCK_IDLE_HIGH | SPI_SAMPLE_AT_MIDDLE); // Gyro Good.
	// Mag looks good. Flash terrible.

	//	setup_spi(SPI_MASTER | SPI_XMIT_L_TO_H | SPI_CLK_DIV_1 | SPI_SCK_IDLE_LOW |
	// SPI_SAMPLE_AT_END); // AWESOME for Flash.  Not so much for mag...

	// Forget CCS -- I'm using my own function
	SetSPIMode(FLASH_SPI_MODE); // sets PIC registers for specific spi mode.

	// ========================= WATCHDOG TIMER (should be first to init) ===============
	setup_wdt(WDT_ON); // turn on watchdog timer

	//========================== Interrupts and Ports ==========================

	/*   Timer 1 Math

	  I found an equation for timer math off the internet:

			delay (in ms) = (# ticks) * 4 * prescaler * 1000 / (clock frequency)
			( http://mdubuc.freeshell.org/Robotics/Tips.html#timers )

	  Is it an inaccurate equation?

	  For a 32MHz clock with a DIV_BY_64 prescaler, and desiring a 1/10 sec (100 milliseconds)
	  delay,

	  100 = x * 4 * 64 * 1000 / 32 000 000
	  x = 12500 = 0x30D4;

	  Experimentally, this was false.

	  We use 0x61A8, which is 25000, to get 100ms interrupts.  That's double the value expected from
	  above. So that means, in the equation,  either the 4 is a 2... the 64 is a 32 (unlikely)...
							 the 1000 is a 500 (very unlikely)...
							 or the 32Mhz is 64Mhz (very unlikely)...
	  Therefore, The 4 must be a 2.

	  Accurate Equation for the PIC we have:
	  delay (in ms) = (# ticks) * 2 * prescaler * 1000 / (clock frequency)

	*/
	//#ifdef FAST_TIMER
	setup_timer1(TMR_INTERNAL | TMR_DIV_BY_64,
				 0x00FA); // 0x00FA is a period of 250, which at 32MHz and a prescaler of 64, should
						  // rollover every millisecond
	// setup_timer1(TMR_INTERNAL|TMR_DIV_BY_64,0x01F4); // 0x01F4 is a period of 500, which at 32MHz
	// and a prescaler of 64, should rollover every 2 milliseconds
	// setup_timer1(TMR_INTERNAL|TMR_DIV_BY_64,0x03E8); // 0x03E8 is a period of 1000, which at
	// 32MHz and a prescaler of 64, should rollover every 4 milliseconds #else
	// setup_timer1(TMR_INTERNAL|TMR_DIV_BY_64,0x61A8);	// 0x61A8 is a period of 25000, which rolls
	// over every 100msec #endif
	t_sec = 0; //
	enable_interrupts(INT_TIMER1);

// IMI Heartbeat Interrupt
#ifdef __IMI100_H__
	ext_int_edge(1, H_TO_L);
	// ext_int_edge(1,L_TO_H);
	enable_interrupts(INT_EXT1);
#endif

	InitPorts(); // Clear the ports and enable.
	restart_wdt(); // Slap the dog

// Once ports are initialized, we can print to the screen...
#ifdef DEBUG
	clrscrn(USER_PORT);
	sendDBGALL(USER_PORT, "\r\n=====================================");
	sendDBGALL(USER_PORT, "\r\n             Initializing:");
	sendDBGALL(USER_PORT, "\r\n=====================================");

// State upper or lower
#if defined(UPPER_PPM)
	sendDBGALL(USER_PORT, "\r\n   -= UPPER PPM =-   ");
#elif defined(LOWER_PPM)
	sendDBGALL(USER_PORT, "\r\n   -= LOWER PPM =-   ");
#endif

	// Restart Reason
	sprintf(dbgbuf, "\r\nRestart Reason: %d  ", restart_reason);
	sendDBGALL(USER_PORT, dbgbuf);
	byteMe(manual_restart_reason);
#endif

	// Timer 2 is used during bootup to show time progression.  It just throws dots on the
	// screen.  (Like ... )
	// setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,0x61A8);	// Internal is 32Mhz with 256 Divisor =
	// enable_interrupts(INT_TIMER2);

	// ========================= Time ====================================================
	restart_wdt(); // Slap the dog
	sendDBGALL(USER_PORT, "\r\nTime...");

#ifdef __I2C__
	// There are 2 real time clocks on board, an I2C and a non-battery-backed RTC.
	// If we restart under certain conditions, stick with the I2C clock
	if (restart_reason == RESTART_POWER_UP || restart_reason == RESTART_SOFTWARE ||
		restart_reason == RESTART_BROWNOUT || restart_reason == RESTART_MCLR) {
		I2C_Clock_OK = TRUE;
		enable_FPM_RTC(halted_time, time); // Loads halted time and time, if applicable
		setTime(time); // Sets the Real time clock as well, for backup
		sendDBGALL(USER_PORT, "USING I2C");
	}
	// Otherwise, use the default RTC
	else {
		I2C_Clock_OK = FALSE;
		getTime(time, &rtcTime);
		sendDBGALL(USER_PORT, "I2C BROKEN - USING RTC");
	}

#else
	// If there's no I2C driver, just default to using RTC.
	I2C_Clock_OK = FALSE;
	getTime(time, &rtcTime);
	sendDBGALL(USER_PORT, "NO I2C - USING RTC");

#endif

	// Check the time and fix it if it's messed up
	if (checkTime(time) != SUCCESS) {
		setTime(1, 1, 11, 6, 0, 0, 0); // Set Default time of January 1, 2011 Saturday 00:00:00
		getTime(time, &rtcTime); // Load it back into the local clock
// Set I2C, if applicable
#ifdef __I2C__
		if (I2C_Clock_OK)
			setI2CTime(time); // Also load the I2C clock
#endif
	}

	// Check it again just to be sure
	if (checkTime(time) != SUCCESS) {
		setTimeVectorOnly(time, 1, 1, 11, 6, 0, 0,
						  0); // Set Default time of January 1, 2011 Saturday 00:00:00
	}

#ifdef DEBUG
	sendDBGALL(USER_PORT, "\t");
	PrintTime(time, USER_PORT); // Show the time
#endif

	// ================ Config stuff =====================
	restart_wdt(); // Slap the dog
	sendDBGALL(USER_PORT, "\r\nConfiguration..........................");

	RunTasks = BACKGROUND_TASKS; // Default to background tasks in the main loop
	NextPage = 0; // No next page default in menu display
	showTimeFlag = showTimeOFF; // Default to NOT show the time

	sendDBGALL(USER_PORT, "Done");

	//============= Macros ====================
	restart_wdt(); // Slap the dog
#ifdef __MACROS__
	sendDBGALL(USER_PORT, "\r\nMacros & Schedules.....................");

	for (n = 0; n < NUM_MACROS; n++)
		MacroStop(n); // Stop all macros. (Fred's stuff)

	memset(MacroCflag + NUM_MACROS, 0xff,
		   3); // Used for delaying MAcros during an upload (0xff disables)
	memset(
		ScheduleMask, 0,
		sizeof(
			ScheduleMask)); // Initialize schedule mask (1 for enable, 0 for disable) (Fred's stuff)
	ScheduleMask[0] = 0xff; // Enable First 8 schedules
	ScheduleMask[1] = 0xff; // Enable Second 8 schedules
	ScheduleMask[2] = 0xff; // Enable Third 8 schedules

	sendDBGALL(USER_PORT, "Done");
#endif

	// ================ Increment the Reboot Counter ==========================
	restart_wdt(); // Slap the dog
	sendDBGALL(USER_PORT, "\r\nReboot Counter.........................");

	err = IncrementRebootCounter(); // Increments the reboot counter in flash

	PrintDbgErr(USER_PORT, err); // Prints an error, if necessary
#ifdef DEBUG
	sprintf(dbgbuf, " #%01i", num_reboots);
	sendDBGALL(USER_PORT, dbgbuf);
#endif

	//====================== Global Configuration =====================
	restart_wdt(); // Slap the dog
	sendDBGALL(USER_PORT, "\r\nGlobal Configuration: ");

	// Explicitly reset crcs before restoring
	globals.CRC[0] = 0xff;
	globals.CRC[1] = 0xff;

	if (restoreGlobalConfig(&globals) != SUCCESS) // If the restore fails.
	{
		loadDefaults(&globals); // Use hardcoded defaults
		sendDBGALL(USER_PORT, "DEFAULTS");
	} else {
		sendDBGALL(USER_PORT, "FLASH"); // Otherwise, go with what we got from flash.
	}

	// ====================== Storage ==========================
	restart_wdt(); // Slap the dog
#ifdef __FLASH_DRIVER__
	sendDBGALL(USER_PORT, "\r\nStorage Scan: (ESC TO CANCEL)");

	flashGlobalUnprotect(); // Unprotect everything in flash

	if (restart_reason == RESTART_POWER_UP || restart_reason == RESTART_SOFTWARE ||
		restart_reason == RESTART_BROWNOUT || restart_reason == RESTART_MCLR) {
// Initializes the given Read/Write Pointer to the first empty page in flash between
// lower and upper boundaries.
// Unprotects all the sectors between lower and upper boundaries. (redundant)
#ifdef UPPER_PPM
		sendDBGALL(USER_PORT, "\r\n   Payload.........");
		Init_Flash(&FlashWriteAddr, PAYLOAD_PROGRAM_ADDR,
				   PAYLOAD_PROGRAM_ADDR + PAYLOAD_PROGRAM_ALLOC_SIZE - 1, 1);
		sendDBGALL(USER_PORT, "Done");
#ifdef DEBUG
		sprintf(dbgbuf, " (%01u of %01u bytes full)", (FlashWriteAddr - PAYLOAD_PROGRAM_ADDR),
				PAYLOAD_PROGRAM_ALLOC_SIZE);
		sendDBGALL(USER_PORT, dbgbuf);
		delay_ms(10);
#endif

		restart_wdt(); // Slap the dog
		sendDBGALL(USER_PORT, "\r\n   Telemetry.......");
		Init_Flash(&TelemWriteAddr, PAYLOAD_DATA_ADDR,
				   PAYLOAD_DATA_ADDR + PAYLOAD_DATA_ALLOC_SIZE - 1, 1);
		sendDBGALL(USER_PORT, "Done");
#ifdef DEBUG
		sprintf(dbgbuf, " (%01u of %01u bytes full)", (TelemWriteAddr - PAYLOAD_DATA_ADDR),
				PAYLOAD_DATA_ALLOC_SIZE);
		sendDBGALL(USER_PORT, dbgbuf);
		delay_ms(10);
#endif
#endif // UPPER

		restart_wdt(); // Slap the dog
		sendDBGALL(USER_PORT, "\r\n   Miscellaneous...");
		Init_Flash(&MiscWriteAddr, MISC_ADDR, MISC_ADDR + MISC_ALLOC_SIZE - 1, 1);
		sendDBGALL(USER_PORT, "Done");
#ifdef DEBUG
		sprintf(dbgbuf, " (%01u of %01u bytes full)", (MiscWriteAddr - MISC_ADDR), MISC_ALLOC_SIZE);
		sendDBGALL(USER_PORT, dbgbuf);
		delay_ms(10);
#endif
	} // if good reboot
	else // bad reboot, don't bother scanning (to avoid infinite reboot)
	{
#ifdef UPPER_PPM
		FlashWriteAddr = PAYLOAD_PROGRAM_ADDR;
		TelemWriteAddr = PAYLOAD_DATA_ADDR;
#endif
		MiscWriteAddr = MISC_ADDR;
		sendDBGALL(USER_PORT, "Scan cancelled. Bad reboot.");
	}
#endif //__FLASH_DRIVER__

	//======================= Initialize Analog to Digital  ================================
	restart_wdt(); // Slap the dog
#ifdef __ADC__
	sendDBGALL(USER_PORT, "\r\nAnalog to Digital Setup................");

	// Enable Temperature Sensor Board
	output_high(TEMP_ENABLE);

	// Setup ADC to use the VREF- and VREF+ for RTDs

	//  NOTE THAT THESE ARE PINS: PIN_B8, PIN_B9, PIN_B10, PIN_B11
	setup_adc_ports(sAN8 | sAN9 | sAN10 | sAN11, VREF_VDD);

	//	setup_adc_ports(sAN8|sAN9|sAN10|sAN11,VREF_VREF); // FLIGHT WORKING
	//	setup_adc_ports(sAN8|sAN9|sAN10|sAN11,VSS_VDD);

	//	setup_adc(ADC_CLOCK|ADC_TAD_MUL_8);	// Working
	setup_adc(ADC_CLOCK_DIV_32 | ADC_TAD_MUL_8); // FLIGHT WORKING

	//	setup_adc(ADC_CLOCK_DIV_128|ADC_TAD_MUL_31);	// Unnknown why these constants are being
	// used.

	// Setup ADC to use the VSS and VDD for SENSE PPM/FPM
	//	setup_adc_ports2(sAN12|sAN13,VREF_VREF);
	//	setup_adc2(ADC_CLOCK_DIV_128|ADC_TAD_MUL_31);

	sendDBGALL(USER_PORT, "Done");
#endif

//============================= Power ===============================
#ifdef __EPS_H__
	restart_wdt(); // Slap the dog
	sendDBGALL(USER_PORT, "\r\nPower..................................");
	eps_init();
	err = eps_functional_check(&EPSData, 5);
	PrintDbgErr(USER_PORT, err);

	if (err == SUCCESS) {
		eps_show_batteries(&EPSData);
	}

#endif

//============================= Orbit ===============================
#ifdef __ORBIT__
	//	sendDBGALL(USER_PORT,"\r\nOrbit model............................");
	initOrbit(&globals.orbitInitTime[0], &globals.orbitInit[0], &globals.orbitInit[3], time);
//	sendDBGALL(USER_PORT,"Done");
#endif

	//============================= Geomag  ================================
	// TODO: Expand to include CRC of geomag coefficients...
	restart_wdt(); // Slap the dog
#ifdef __GEOMAG__
	sendDBGALL(USER_PORT, "\r\nGeomag model...........................");
	geomag(B_field_ECI, &orbitData[6], orbitData[15], time[2] * 365.25,
		   globals.ReadGeomagFromFlash); // Run the geomag model once
	sendDBGALL(USER_PORT, "Done");
#endif

//============================= SunModel ================================
#ifdef __ORBIT__
	//	sendDBGALL(USER_PORT,"\r\nSun model..............................");
	sunmodel(time, Sun_ECI); // 19 ms
	//	sendDBGALL(USER_PORT,"Done");
#endif

	//=============================== GNC  ================================
	restart_wdt(); // Slap the dog
#ifdef __AENEAS__
	sendDBGALL(USER_PORT, "\r\nGuidance, Navigation and Control:");

	// Restore the input array or use defaults
	if (restoreExtIn(ExtIn) != SUCCESS) {
		loadExtInDefaults(); // If not initialized, use the defaults
		sendDBGALL(USER_PORT, "DEFAULTS");
	} else {
		sendDBGALL(USER_PORT, "FLASH"); // Otherwise, go with what we got.
	}

	// Initialize the model
	initModel(); // Flight Software Init

#endif

	//======================= Initialize Buffers  ================================
	//	sendDBGALL(USER_PORT,"\r\nClear Buffers..........................");

	restart_wdt(); // Slap the dog
	memset(Lbuf, 0x00, GENERAL_BUFFER_SIZE);
#ifdef DEBUG
	memset(dbgbuf, 0x00, DBG_BUF_SIZE);
#endif
	//	others...?

	//	sendDBGALL(USER_PORT,"Done");

	// =================== Communications Inits ===============
	restart_wdt(); // Slap the dog
	sendDBGALL(USER_PORT, "\r\nCommunications:");
#ifdef __MHX__
	SyncTime = 0; // Holds the timeout value for syncs
#endif

	// ========================= Initialize Handlers ===========================
	restart_wdt(); // Slap the dog
	sendDBGALL(USER_PORT, "\r\n  Message Stack Handlers...............");

	Init_Handlers(); // Initialize all handlers
	srand(12345); // initialize the random number generator (for testing purposs))

	sendDBGALL(USER_PORT, "Done");

#ifdef __ITAG_H__
	// ========================= Initialize iTag ===========================

	restart_wdt(); // Slap the dog
	sendDBGALL(USER_PORT, "\r\n  iTag.................................");
	err = itag_functional_check(ITAG_PORT,
								5); // Tries to get a command through the itag for 5 seconds
	PrintDbgErr(USER_PORT, err);

#endif

	// ================== Configure the Beacon ======================
	restart_wdt(); // Slap the dog
#ifdef __BEACON__
	sendDBGALL(USER_PORT, "\r\n  Beacon...............................");
	if (globals.BeaconOnFlag) {
		BeaconPower(BEACON_ON);
		err = configureBeacon();
		PrintDbgErr(USER_PORT, err); // Prints result
	} else {
		BeaconPower(BEACON_OFF);
		sendDBGALL(USER_PORT, "OFF");
	}
#endif

	//========================== Magnetometer ==========================
	restart_wdt(); // Slap the dog
#ifdef __MAG_H__
	sendDBGALL(USER_PORT, "\r\nMagnetometer...........................");
	mag_init();
	err = mag_functional_check(5);
	PrintDbgErr(USER_PORT, err); // Prints result
#endif //__MAG_H__

	//========================== Gyroscope ==========================
	restart_wdt(); // Slap the dog
#ifdef __GYRO_H__
	sendDBGALL(USER_PORT, "\r\nGyroscope..............................");
	err = gyro_init();
	PrintDbgErr(USER_PORT, err); // Prints result
#endif //__GYRO_H__

	//========================== Sun Sensor ==========================
	restart_wdt(); // Slap the dog
#ifdef __SUNSENSOR__
	sendDBGALL(USER_PORT, "\r\nSun Sensor.............................");
	err = sunSensor_init(SUN_PORT);
	PrintDbgErr(USER_PORT, err); // Prints result
#endif //__SUNSENSOR__

	//========================== IMI-100 ==========================
	restart_wdt(); // Slap the dog
#ifdef __IMI100_H__
	sendDBGALL(USER_PORT, "\r\nIMI-100................................");
	imi100_init(IMI_PORT, 1);
	delay_ms(10); // IMPORTANT: Give the IMI a little time to get its bits in order.  Otherwise the
				  // following command to set to Test Mode doesn't make it.
	setIMItoTestMode(IMI_PORT); // Sets the IMI to receive commands directly, instead of generating
								// them on its own from onboard GNC
	imiDataInit(&IMIData); // Clears out the IMI Data structure
	err = blockForIMITelemetry(IMI_PORT, &IMIData,
							   5); // Blocks for 5 seconds looking for a good read of the IMI
	PrintDbgErr(USER_PORT, err); // Prints result
#endif //__IMI100_H__

	// ================== Power On and Enable the MHX ==============
	restart_wdt(); // Slap the dog
#ifdef __MHX__
	sendDBGALL(USER_PORT, "\r\n  MHX..................................");

	// Set the MHX Reset pin to High to avoid an accidental Reset
	output_high(MHX_RESET);

	// Set the MHX to use hardware handshaking
	setPortHandshaking(MHX_PORT, HANDSHAKE_ON);

	// Set the RTS line for the MHX Low indicating we are ready to talk!
	// Note: this should be done automatically somewhere else.
	output_low(MHX_REQUEST_TO_SEND);

	// Set the DTR line for the MHX low indicating we are ready to talk.
	// Note: this should be done automatically somewhere else.
	output_low(MHX_DATA_TERMINAL_READY);

	// Turn on MHX
	MHXOn();

	// Enable receiving data from MHX
	clearRTS(MHX_PORT);

	// Functional check (wait for it to boot first)
	// delay_ms(250);
	err = mhx_functional_check(10);
	PrintDbgErr(USER_PORT, err); // Prints result
#endif

//========================== MAESTRO ==========================
#ifdef __MAESTRO__
	initialize_maestro();
#endif // __MAESTRO__

//============ MACRO_RUN whatever is in the last macro on bootup ====================
#ifdef __MACROS__
#ifdef DEBUG
	sprintf(dbgbuf, "\r\nScheduling Macro %i to run now.........", BOOT_MACRO);
	sendDBGALL(USER_PORT, dbgbuf);
#endif
	RunMacro(BOOT_MACRO, MACRO_RUN, 1);
	sendDBGALL(USER_PORT, "Done");
#endif

	//========================== Signal End of Boot Sequence ======================
	// disable_interrupts(INT_TIMER2);
	sendDBGALL(USER_PORT, "\r\nDevice Initialized.\r\n\n");

//========================== Display the Main Menu ==========================
#ifdef DEBUG
#ifndef FLIGHT
	sendDBGALL(USER_PORT, "\r\nPress any key to pause...");
	timeout = SEC_TIMER + 3;
	while (SEC_TIMER < timeout) {
		if (getByte(USER_PORT, &response)) {
			sendDBGALL(USER_PORT, "PAUSED. Press any key to continue...");
			while (!getByte(USER_PORT, &response)) {
				restart_wdt();
			}
			timeout = SEC_TIMER;
		}
	}
	showMainMenu(USER_PORT);
#endif
#endif // DEBUG
} // Init Device

//================== Show Schedule ===================================
void showSchedule(int port, unsigned char page) {
	unsigned char m, len, width;
	int n;
	unsigned char mask;
	unsigned char temp;
	unsigned char ran = 'x';
	unsigned char schedulebuf[GENERAL_BUFFER_SIZE];

	clrscrn(port);

	if (page == 2 && NUM_STORED_CMDS > 20)
		page = 20; // page is used to split the schedules for display
	else
		page = 0;

	sendDBGALL(port, "\r\nScheduled Commands");

	for (m = 0; m < 20; m++) {
		// Safety check
		if ((m + page) >= NUM_STORED_CMDS)
			break; // can't display more than we have

		// Initialize
		width = 0;
		memset(schedulebuf, 0, GENERAL_BUFFER_SIZE); // Clears schedulebuf

		// Get length
		len = globals.StoredCmds[m + page][0];

		// Safety check on length.
		if (len >= MAX_STORED_CMD_SIZE) {
			sprintf(dbgbuf, "\r\n%2u ERR - Len too big.", m + page);
			sendDBGALL(USER_PORT, dbgbuf);
			goto NEXTLOOP;
		}

		// Safety check on length.
		if (len > 0 && len < 8) {
			sprintf(dbgbuf, "\r\n%2u ERR - Len too small.", m + page);
			sendDBGALL(USER_PORT, dbgbuf);
			goto NEXTLOOP;
		}

		// Get mask Flag
		if (fbit(ScheduleMask[(m + page) / 8], (m + page) % 8))
			mask = 'E'; // Mark schedule if it is enabled
		else
			mask = 'D';

		// Get Run Flag
		if (globals.StoredCmds[m][1] == 1)
			ran = 'R';
		else if (globals.StoredCmds[m][1] == 2)
			ran = 'W';
		else
			ran = '0';

		// Print first part
		sprintf(schedulebuf, "\r\n%2u %c%c %2u ", m + page, mask, ran, len);
		width = strlen(schedulebuf);

		if (len > 0) {
			for (n = 1; n < 8; n++) {
				temp = globals.StoredCmds[m + page][n + 1];
				// For wildcards, just print 'X'
				if (temp == 0xff) {
					sprintf(schedulebuf + width, "X ");
				} else {
					// For the time, if the high bit is set (10000000), then print as %(number) ?
					if (temp & 0x80) {
						sprintf(schedulebuf + width, "%%%u ", temp & 0x3f);
					} else {
						sprintf(schedulebuf + width, "%u ", temp & 0x3f);
					}
				}
				width = strlen(schedulebuf);
			}
			sprintf(schedulebuf + width, " : ");
			width = strlen(schedulebuf);

			// Note:  The following loop can go forever if len=255.  That's because len+1 = 256,
			// which is unattainable by an 8-bit unsigned char. It can also go forever if len+1 is 8
			// or less. It can also run off the end of the array if len+1 > MAX_STORED_CMD_SIZE - 1
			// TODO: Fix all that!
			for (n = 8; n < len + 1; n++) {
				sprintf(schedulebuf + width, "%u ", globals.StoredCmds[m + page][n + 1]);
				width = strlen(schedulebuf);
				if (len + 1 <= 8)
					break; // Safety check.  N is supposed to count from 8 to len+1.  But if
						   // len+1<=8, it will countinue forever.
			}
		} // if len>0

		sendDBGALL(port, schedulebuf);

	NEXTLOOP:;
	} // for loop

	sendDBGALL(port, "\r\n'mp<CR>'refreshes,'n'Page2,'mm<CR>'exit,");
}

/*
void showSchedule(int port, unsigned char page) {
	int i;

	for(i=0;i<NUM_STORED_CMDS;i++)
	{
		sprintf(Lbuf,"\r\n%i: ",i);
		sendMSGALL(port,Lbuf);
		HexPrint(port, &globals.StoredCmds[i][0], globals.StoredCmds[i][0]);
	}
}
*/

void once_per_second(void) {
//===================================================================== Power Reading
#ifdef __EPS_H__

	int temp;

	temp = checkBatteryState(&EPSData); // Records the battery state

	if (temp != BatteryState_g) {
		BatteryState_g = temp; // If they are not equal
		updateOtherProcessBatteryState(); // Update the other processor
	}

	StartEPSAquireFlag = 1; // Sets the flag for getting new data from the EPS.
	EPSBackgroundTask(1); // Resets and asks for the next one
#endif //__EPS_H__

//===================================================================== Sun Mode Propogation
#ifdef __ORBIT__
	sunmodel(time, Sun_ECI); // 19 ms
#endif

//===================================================================== Debug Screen
#ifdef __AENEAS__
	if (showGNCscreen && ((SEC_TIMER % 5) == 0))
		showGNCvalues(USER_PORT, 1);
#endif

	//===================================================================== Time
	// TODO: Encapsulte these in updateTime() and displayTime() functions
	if (I2C_Clock_OK) // If I2c is working, use that
	{
#ifdef __I2C__
		getI2CTime(time); // Loads time
#endif
		setTime(time); // Sets the Real time clock as well, for backup
	} else // Otherwise, use the RTC
	{
		getTime(time, &rtcTime); // Load the local clock

// TODO: Manually advance the time by 1 second here?
#ifdef UPPER_PPM
// incrementTime(time);
// setTime(time);					// Sets the Real time clock as well, for backup
#endif
	}

//================================================================== Display time
#ifdef LOWER_PPM
#ifdef DEBUG
	if (showTimeFlag == showTimeOVERWRITE) {
		locxy(USER_PORT, 22, 40);
		PrintTime(time, USER_PORT);
	} else if (showTimeFlag == showTimeNEWLINE) {
		sendDBGALL(USER_PORT, "\r\n");
		PrintTime(time, USER_PORT);
	}
#endif
#endif

	//===================================================================== Schedule
	checkSchedule();

	// ======================================================= Restart Debug Message (TODO: move to
	// end of Init_Device?)
	if (restart_reason != RESTART_POWER_UP) {
		sprintf(dbgbuf, "\r\nRestart due to %i at ", restart_reason);
		getStringTime(time, &dbgbuf[strlen(dbgbuf)]);
		sendDBGALL(USER_PORT, dbgbuf);
#ifdef __MYSTACK__
		PacketizeAndPush(
			dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
			PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address
#endif
		restart_reason = RESTART_POWER_UP; // TODO: Find another way to do this.  This line alters
										   // the variable to be incorrect.
	}

// ====================================================================== Flash Status
// Get the flash status
#ifdef __FLASH_DRIVER__
	flashStatus(flash_status); // 2 bytes
#endif

// ====================================================================== MHX Status
#ifdef __MHX__
	// Get the MHX status
	MHXStatus(&mhx_status); // 1 byte
#endif

// ========================================================================== ADC Temperature
// Sensors
#ifdef __ADC__
	collecTemperatures(temperatureSensors);
#endif

// ========================================================================== More Debug
#ifdef __AENEAS__
// if(showExtIn) PrintExt(ExtIn,nT_ECI_x_IN,NUMIN);
// if(showExtOut) PrintExt(ExtOut,Cmded_Whl_Torque_x_OUT,Cmded_Whl_Torque_z_OUT);
#endif
}

//====================================================================
//                 MACRO FUNCTIONS (move to macros library?)
//====================================================================

//================== Show Macros ===================================
void showMacro(int port, unsigned char page) {
#ifdef __MACROS__
	// Declarations
	int m, n, len;
	// 2 buffers are needed in this function.
	// One for printing, the other for sending off to macro functions for data.
	unsigned char buf[76];
	unsigned char* macro;
	unsigned char tempbuf[GENERAL_BUFFER_SIZE];

	macro = tempbuf;

	// If the port is a hyperterminal, clear it
	clrscrn(port);

	if (page == 1)
		page = 0;
	if (page == 2)
		page = 20;

	sendDBGALL(port, "\r\n           Stored Macro Commands          ");
	sendDBGALL(port, "\r\n#|State|Len(dec)|Contents(hex)--->      ");

	for (m = 0; m < 20; m++) {
		// locxy(port,m+3,2);
		// Figure out the length and load the macro into a local buffer
		len = readMacro('M', m + page, macro);
		sprintf(buf, "\r\n%2d %c %3d:", m + page, Macro_state[m + page], len);
		sendDBGALL(port, buf);

		// Print out the hex values of the macro, stopping if we go off the screen
		for (n = 1; n < len + 1; n++) {
			sprintf(buf, "%02x ", macro[n]);
			sendDBGALL(port, buf);
			if (n > 21) {
				sprintf(buf, "...");
				sendMSG(port, buf, strlen(buf));
				n = len;
			}
		}
	}

	sendDBGALL(port, "\r\n'ma<CR>'refreshes,'n'Page 2,'mm<CR>'exit");

#endif //__MACROS__
}

#ifdef DEBUG

//===========================================================
//                     Check User
//===========================================================
void checkUser(int port) {
	int cmd_start;
	unsigned char ack, encrypt;
	int len;
	unsigned char cuBuf[GENERAL_BUFFER_SIZE];
	unsigned char* rcmd; // rcmd (ASCII commands) are converted and copied back to rcmd
	unsigned char* ptr;
	unsigned char ncmds;
	unsigned char data;
	unsigned char msgSource[8], msgDest[8];
	unsigned char protocol;
	int ret;
	ReturnErr_t errval; // error values
	static unsigned char last_cmd[LAST_CMD_SIZE + 1];
	//	static char magtestvar=0;

	rcmd = cuBuf;

	if (!getByte(port, &data))
		return; // if no data, bail out and try again

	sendDBGALL(USER_PORT, "-");

	memset(rcmd, 0, GENERAL_BUFFER_SIZE); // Clears out the buffer with zeroes.
	memset(msgSource, 0, 8); // Loading anything in the upper bytes treats the source as 8 bytes
	memset(msgDest, 0, 8); // Loading anything in the upper bytes treats the destination as 8 bytes
	protocol = 'I'; // Default communication protocol

	switch (data) {
	case '/': // Probably a comment at the start of a line
		errval =
			getMSGsafe(port, rcmd, CR, GENERAL_BUFFER_SIZE - 1); // just read upto the Line Feed
		if (errval != SUCCESS) // If there was some kind of problem
		{
			resetPort(port); // clear out the port
		}
		break;

	case 'a': //==Switch to ASCII data formating
	case 'A':
		sendDBGALL(port, "\r\nERROR: Fred's Ascii Mode Disabled.");
		//        format[port] = 'A';
		break;

	case 'b': //==Switch to binary mode
	case 'B':
		sendDBGALL(port, "\r\nERROR: Fred's Binary Mode Disabled.");
		//        format[port] = 'B';
		break;

	case 'c':
	case 'C':
		clrscrn(port);
		break;

	case 'p': //== Cycle through time formats
	case 'P':
		if (showTimeFlag == showTimeOFF)
			showTimeFlag = showTimeOVERWRITE;
		else if (showTimeFlag == showTimeOVERWRITE)
			showTimeFlag = showTimeNEWLINE;
		else if (showTimeFlag == showTimeNEWLINE)
			showTimeFlag = showTimeOFF;
		else
			showTimeFlag = showTimeOFF;
		break;

	case 'd': //==Switch to data display
	case 'D':
		sendDBGALL(port, "\r\nERROR: Data Display Disabled in Code.");
		break;

	case '+': //== Thumb through the stored (Forward in time)
	case '-': //							  (Backward in time)
	case '>': //== Thumb through the stored (Forward in time)
	case '<': //							  (Backward in time)
		break;

	case 'e': // Echo Port to user interface...used for debugging serial ports
		sendDBGALL(port, "\r\nERROR: Echo Command moved to Cmd 132.");
		break;

		//------------------------------------ Commands
	case 'Z': // These are to send Zigbee commands 	(Z= 8 bit destination address MAC)
	case 'z': //(z=	4 bit destination address, 8 bit source..loaded later)
		protocol = data; // Default is 'I' (at the top of Check_User)

	case 's': //(s= 4 bit source, 4 destination)
	case 'S': // Process "S" commands which require source and destination addresses (Cap S requires
			  // CRC)
	case 'R': // Commands for this iDAC, no address is required
	case 'r':
		// sprintf(cuBuf,cmdProcessing);
		// placeString(port, 24, 1, cuBuf);
		len = 0; // Initialize length
		ncmds = 0; // Assume no commands to start
		for (;;) // Loop forever loading multiple commands, runs once if not a "load"
		{
			cmd_start = len; // Use load command functions
			ptr = rcmd + 1;
			errval = getMSGsafe(port, ptr, CR, GENERAL_BUFFER_SIZE - 2); // load a message into rcmd
			if (errval != SUCCESS) {
				PrintDbgErr(port, errval);
				return;
			} else {
				/*
				 * This line stomps on the beginning of the prior debugging
				 * output.  Let's not do that.
				  clearChars(port, 24, 1, 40);   // what is the purpose of this line?
				*/

				errval =
					ParseCommandString(ptr, rcmd); // Parse the line to make it ready for processcmd
				if (errval != SUCCESS) {
					PrintDbgErr(port, errval);
					return;
				}
				ncmds++;
				break; // break from for-loop to process the cmd
			}
		} // while(1)

		//        rcmd[0] = len; // First byte of rbcmd must be total length
		//        rcmd[len + 1] = '\0'; // Terminate string

		if (data == 'r' || data == 'R') // This is a non addressed command  (Run locally)
		{
			memcpy(msgSource, globals.Dest_ID, 4); // Source and Destination don't matter
			memcpy(msgDest, globals.RTU_ID, 4); // Destination doesn't matter
			PadCommand(rcmd); // Shift the command to the right for processing in ProcessCmd
		}
		if (rcmd[0] < 9)
			return; // too short to process bail out here

		// PROCESS THE COMMAND
		memcpy(last_cmd, rcmd, LAST_CMD_SIZE);
		ret = processCmd(port, msgSource, rcmd, ncmds, ack | encrypt);
		break;
	case 'n': // Indicates "N"ext page for multi-page displays
	case 'N':
		if (NextPage == 'S')
			showSchedule(port, 2); // Bring up page 2 of schedule Menu
		if (NextPage == 'M')
			showMacro(port, 2); // Bring up page 2 of Macro Menu
		break;

	case 'm': // These are requests for menu displays
	case 'M':
		//       PC[port] = 'F';
		//       display[port] = 'F';
		//       format[port] = 'A';
		//       disableDebugMSG();

		getMSGsafe(port, rcmd, CR, GENERAL_BUFFER_SIZE - 1);
		switch (rcmd[0]) {
		case 'a': // Macros
		case 'A':
			// menu[port] = 'A'; // enable menu functions
			NextPage = 'M'; // Set the nextpage command to show macros
			showMacro(port, 1);
			// menuMacroId = -1;
			break;

		case 'd':
		case 'D':
			//			menu[port]='D';
			break;

		case 'p': // received "P for "Program"
		case 'P':
			// menu[port] = 'P'; 		// enable main menu clock
			NextPage = 'S'; // Set the nextpage command to show schedule
			showSchedule(port, 1); // Bring up page 1 of schedule Menu
			break;

		case 'm': // Main menu
		case 'M':
			//            send_Data[port] = 1; // Enable the transmission of iControl data over this
			//            port menu[port] = 'T';
			showMainMenu(port);
			break;
		}
		break;

//======================
//  	Testing cases
//======================
#ifdef DEBUG
	case 'x': // Show/Hide GNC Screen
	case 'X':
		//	#ifdef __AENEAS__
		//	showGNCscreen = !showGNCscreen;
		//	#endif
		break;

	case 'y':
		// MeasureFlag=!MeasureFlag;
		break;

	case 't': // General test cases.  Use as "T1<ENTER>"  or "T2<ENTER>" etc... up to T9...
	case 'T': // General test cases.  Use as "T1<ENTER>"  or "T2<ENTER>" etc... up to T9...
		signed int temp_i;
		getMSGsafe(port, rcmd, CR, GENERAL_BUFFER_SIZE - 1);
		temp_i = atoi(rcmd);
		switch (temp_i) {
		case 0: // Test 0
			// HexPrint(USER_PORT,&BatteryState_g,1);
			break;

		case 1: // Test 1
#ifdef __EPS_H__
			HexPrint(USER_PORT, globals.BatteryThreshold, 4);
#endif
			break;

		case 2: // Test 2
#ifdef __ADC__
#ifndef FLIGHT
			ADCTest(USER_PORT);
#endif
#endif
			break;

		case 3: // Test 3
			break;

		case 4: // Test 4
			break;

		case 5: // Test 5
			break;

		case 6: // Test 6
			break;

		case 7: // Test 7
			break;
		case 8: // Test 8
			RunCommand("118 D"); // Sensors
			RunCommand("108 D"); // Orbit
			RunCommand("109 D"); // GeoMag
			RunCommand("110 D"); // GNC
			RunCommand("111 D"); // Load ExtIn
			RunCommand("119 D"); // Disable real IMI writes
			RunCommand("119 1 \"0.0 0.0 0.0\""); // Stop all wheels
			RunCommand("119 2 \"0.0 0.0 0.0\""); // Stop all coils
			sendDBGALL(USER_PORT, "\r\nAll FSW Modules disabled.");
			break;
		case 9: // Test 9
			RunCommand("118 E"); // Sensors
			RunCommand("107 \"421.53 6820999.95 649.0 4215.3 -0.856 6490.0\""); // Orbit
			RunCommand("109 E"); // GeoMag
			RunCommand("110 E"); // GNC
			RunCommand("111 E"); // Load ExtIn
			sendDBGALL(USER_PORT,
					   "\r\nAll FSW Modules enabled (except real IMI writes -- Test 10).");
			break;

		case 10: // Test 10
			sendDBGALL(USER_PORT, "\r\nReal IMI Writes Activatd");
			RunCommand("119 E"); // Enable real IMI writes
			break;

		case 11: // Test 11
			break;

		case 12: // Test 12
// EPS Testing
#ifdef __EPS_H__
			epsAutomaticDataCollection(0, 1, 1);
#endif
			break;

		case 13: // Test 13
			break;

		case 14: // Test 14
			break;

		case 15: // Test 15
			break;

		case 16: // Test 16
			ShowInputRegisters();
			break;

		case 17: // Test 17
			ShowOutputRegisters();
			break;

		case 18: // Test 18 :Flip the flash CS
			break;

#endif // ifdef DEBUG

		default:
			sendDBGALL(USER_PORT, "\r\nInvalid Test");
			break;
		}
		break;

	case 'f':
	case 'F': // Forwarded Command.  Works the same as 'r' commands, but forwards them to other
			  // processor. Example: f 158 1 12 "Test Command"
		errval = getMSGsafe(port, rcmd, CR, GENERAL_BUFFER_SIZE - 1);
		if (errval != SUCCESS) {
			PrintDbgErr(port, errval);
			return;
		} else {
			sendPPMCmd(OTHER_PPM_PORT, rcmd);
		}
		break;

	case 'i':
	case 'I': // ITag Command.
		// Works the same as an r command run locally.
		// For example, if you want to run the following command (when directly connected to iTag):
		// r 39 1 0 "hello" (enter)
		// You simply use:
		// i 39 1 0 "hello" (enter)
		errval = getMSGsafe(port, rcmd, CR, GENERAL_BUFFER_SIZE - 1);
		if (errval != SUCCESS) {
			PrintDbgErr(port, errval);
			return;
		} else {
			sendiTagCmd(rcmd);
		}
		break;

	default:
		break; // default break
	} // switch(data) outer loop
} // checkUser
#endif // DEBUG

//===========================================================
//                     Process Commands
//===========================================================
// The big enchillada function
// TODO:
//   -- remove all addressing (source/dest) from rbcmd.  ProcessCmd should be agnostic
//      about where commands come from or are going.  All of that should be handled at
//      the transport layer one level up.
//   -- ditto for the ack.  processCmd should return a value, and the transport layer
//      should decide what to do with that value. Keep the passing of messages and the
//      content of messages seperate.
int processCmd(int port, unsigned char* source, unsigned char* rbcmd, int ncmds, unsigned char ack)
// A command looks like:
// (length) (4 byte source) (4 byte dest) (cmd ID) (command parameters)
// ACK is 1 or 0 to disable or send ACK.  If MSB of ACK is set, ACK is encrypted
// Source and port are destination for ACK
{
	int n, m, i; // Generic ints for use throughout this function
	unsigned char c; // Generic char
	int cmd_length;
	unsigned char* buf;
	unsigned char Command_ID;
	//	static unsigned char value;
	unsigned char temper;
	const unsigned char cmdBaseAddr = 9;
	const unsigned char cmd1 = 10, cmd2 = 11, cmd3 = 12, cmd4 = 13, cmd5 = 14, cmd6 = 15, cmd7 = 16,
						cmd8 = 17, cmd9 = 18;
	//	unsigned char index;
	unsigned char encrypt, TX_Code;
	//	unsigned char *next;
	int retVal = 0;
	//    static unsigned char cmd_history[3];
	unsigned long addr;
	unsigned char ProcessCMDBuf[GENERAL_BUFFER_SIZE];
	int wrPtr = 0; // Used in cases 126 and 127

	// For command 163
	unsigned char temptime[7];

	//	unsigned char num2cpy;
	encrypt = ack & 0x80; // Keep track whether an ACK or packet needs to be sent with encryption
	ack &= 0x7f; // Remove encryption bit
	TX_Code = 0;

	cmd_length = rbcmd[0];
	rbcmd[cmd_length + 1] = '\0';
	buf = Lbuf;

	//#define DBG_INFO_CMD

	//	sendDBGALL(USER_PORT,"\r\nReceived Command: ");
	//	HexPrint(USER_PORT,rbcmd,rbcmd[0]+1);
	//	sprintf(dbgbuf,"\r\nInterpreted as cmd %u",rbcmd[cmdBaseAddr]);
	//	sendDBGALL(USER_PORT,dbgbuf);

	while (ncmds > 0) //== If a command load, Process as many commands are present
	{
		Command_ID = rbcmd[cmdBaseAddr];
		// memset(buf,0,strlen(buf));

		switch (Command_ID) {
		case 1: //==Set RTU mode
			/*
			  memset(RunCmd, 0, 20); // Zero out RunCmds with each command change
			  RTU_mode = rbcmd[cmd1];
			  switch (RTU_mode) {
			  case 0: // One parameter commands
			  case 1:
			  case 2:
				  memcpy(RunCmd, rbcmd + cmd2, 3);
				  break;

			  case 5: // Two-Parameter commands
			  case 7: // This mode executes a command with each one second increment
				  nsamples = rbcmd[cmd2]; // Collect up to 255 samples
			  case 6:
				  memcpy(RunCmd, rbcmd + cmd3, cmd_length - 11);
				  break;

			  case 10:// Three parameter commands		// send a record to desired source
			  case 11:
				  if (ack)sendACK(port, I_ACK,RX_Seq_Num++, Command_ID, source, 0,encrypt);
				  ack = 0;
				  if (getIO(rbcmd[cmd2], rbcmd[cmd3], buf)) {
					  if (RTU_mode == 10)
						  sendPacket(rbcmd[cmd4], globals.MAC_ADDR,
								  globals.Dest_ID, buf, 'I' | encrypt);
					  if (RTU_mode == 11)
						  sendPacket(rbcmd[cmd4], globals.MAC_ADDR, rbcmd + 14,
								  buf, 'I' | encrypt);
				  }
				  RTU_mode = 0; // Only do this once

			  default:
				  break;
			  }
			  */
			break;

		case 2: //== One byte ACK from server or USER_PORT
				//            if (cmd_length == 9)clearTX(1);                   //  No Sequence or
				//            PK_ID... probably menu ACK else { //rbcmd[cmd1]
				//            // Use whatever is saved in TX_Dest to check
				//                if (checkACK(TX_Dest, TX_Seq_Num))clearTX(1); // Check source and
				//                sequence number... this really does nothing now
				//            }
				//            return 0; // return from here, don't ACK or increment cmd counter

		case 4: //== Acknowledge, No-Op, Communication test
			/*
					  if (ack)sendACK(port, I_ACK,RX_Seq_Num++, Command_ID, source, retVal,encrypt);
					  if(sleep)                   // If we are asleep and responding to a command,
			   wakeup here
					  {
						  delay_ms(WAKEUP_CHANNEL_SWITCH_DELAY);   		        // delay to get the
			   ACK out before changing channel wakeUpAndResetPS(wakeRadio); 	// If the ASIC is
			   asleep, wake up to the appropriate configuration (and channel)
						  if(cmd_length>9)NetworkChannel=rbcmd[cmd1];           // Only do this if
			   the radio byte is present
					  }
					  if (commTimer==0) commTimer = globals.wakeCount;
					  ack=0;                         // Don't ack again
			*/
			break;
		case 5: //== Forward message to next destination
			/*
				switch (rbcmd[cmd1]) {
				//   1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
				case 1: // one forward..convert Packet ID		  // s 0 0 0 0 0 0 0 0 5  1  9  0 0
			   0  1  1

				  {
					  unsigned char dest[4];
					  rcpy(dest, rbcmd + 11, 4); // This is the destination, reverse byte order
					  rbcmd[14] = cmd_length - 14; // load new command length

					  load_TX(globals.LANPort[0], globals.RTU_ID, dest, rbcmd + 14,'I' | encrypt);
				  }
				  break;

				default: // continue with forward Command ID
					break;
				}
			*/
			break;

		case 6: // Print Register
				// Format: r 6 (2-byte register)
				// Example: r 6 *0082*
		{
			unsigned char* reg;

			// Validate cmd_length
			if (cmd_length != cmd2)
				break;

			// Create pointer to address
			reg = make16(rbcmd[cmd1], rbcmd[cmd2]);

			// Create debug message using Hex
			sprintf(dbgbuf, "\r\nRegister 0x%02x%02x = 0x%02x", rbcmd[cmd1], rbcmd[cmd2], *reg);
			sendDBGALL(port, dbgbuf);

// Send a copy over the radio
#ifdef __MYSTACK__
			PacketizeAndPush(
				dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
				PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address
#endif

			// Print the binary output to the screen.
			sendDBGALL(port, "\r\n");
			byteMe(*reg);
		} break;

		case 7: //== Set Our RTU ID
			// Example: r 7 3 2 1 0 // Sets ID to 3.2.1.0
			// r 1 1 1 1
			// f 2 2 2 2
			globals.RTU_ID[3] = rbcmd[cmd1];
			globals.RTU_ID[2] = rbcmd[cmd2];
			globals.RTU_ID[1] = rbcmd[cmd3];
			globals.RTU_ID[0] = rbcmd[cmd4];
			break;

		case 10: //== Set Time absolute
			// Format: r 10 <month> <day> <2-digit year> <wkday, 0=Sunday> <hour> <min> <sec>
			// Example: r 10 10 21 11 5 17 26 30
			if (cmd_length > 11) // If we receive a set time command, reset commTimer
			{
				if (I2C_Clock_OK) // If I2c is working, use that
				{
#ifdef __I2C__
					setI2CTime(rbcmd + cmd1); // Sets time
					getI2CTime(time); // Loads time
#endif
					setTime(time); // Backup with RTC
				} else // Otherwise, use the RTC
				{
					setTime(rbcmd + cmd1); // Set the RTC
					getTime(time, &rtcTime); // Read the RTC into local time buffer
				}
			}
			break;

		case 11: // Define Network Type and Encryption keys
			break;

		case 12: //== Set power configuration
			/*
						switch (rbcmd[cmd1]) {
						  case 'B':
							globals.boardConfig = rbcmd[cmd2];
							break;
						  case 'S':
							globals.sleepMode = rbcmd[cmd2];
							break;
						  case 'W':
							globals.wakeCount = rbcmd[cmd2];
							break;
						}
			*/
			break;

		case 13: //== Change IO Data ID's
			// BrdAddr = rbcmd[cmd1];
			// ExtAddr = rbcmd[cmd2];
			break;

		case 14: //== Assign Default Destination ID
			rcpy(globals.Dest_ID, rbcmd + cmd1, 4);

			break;
		case 15: // Command (override) power configuration
			/*
						switch (rbcmd[cmd1]) {
						  case 'E':
							boardCFG |= rbcmd[cmd2];
							break;

						  case 'D':
							boardCFG &= ~rbcmd[cmd2];
							break;

						  case 'S':
							boardCFG = rbcmd[cmd2];
							break;
						}
			*/
			break;

		case 16: //== Set the sleep cycle "mode"
			/*
				  switch (rbcmd[cmd1]) {
					case scRadio:
					case scDecommisioned:
					  sleepMode = rbcmd[cmd1];
					  break;
				  }
			*/
			break;

		case 17: // Reinitialize the ports
			InitPorts();
			break;

		case 21: //== Enable schedule checking
			/*
						if (rbcmd[cmd1]=='M')                // Sets schedule checking MASK
							{
							  for(n=0;n<4;n++){
								if(fbit(rbcmd[cmd2],n))     // identifies schedule group
								{
								  switch(rbcmd[cmd3])      // allows setting, 'anding, oring
			   schedule masks
								  {
								  case '=': ScheduleMask[n]= rbcmd[cmd4]; break;
								  case '&': ScheduleMask[n]&=rbcmd[cmd4]; break;
								  case '|': ScheduleMask[n]|=rbcmd[cmd4]; break;
								  default:break;
								  }
								}
							  }
							}
						else if (rbcmd[cmd1]==0)globals.schedule = 'F'; //  'F'= Enable schedule
			   checking else globals.schedule = 'T';                    //  'T'= Disable schedule
			   checking
			*/
			break;

		case 22: //== Set Preamble,Tail,BSpacing,RXmode used for RF transmitting
			/*
			globals.Tpreamble = (int) (rbcmd[cmd1]) * 4; // Preamble
			globals.Ttail = (int) rbcmd[cmd2]; // Tail
			globals.Bspacing = (int) rbcmd[cmd3]; // Byte Spacing
			*/
			break;

		case 23:
			break;

		case 25: //== Set Radio type (use only for setup)
				 //   initComm('R', rbcmd[cmd1], globals.LANPort[0]); //   primary LAN device
			break;

		case 26: //== Set number of attempts to deliver iDAC command
			/*
						if (rbcmd[cmd1] != 0) {
							globals.discard = rbcmd[cmd1];
							globals.RetryTime = rbcmd[cmd2];
						}
			*/
			break;

		case 33: // Generic flash read/write/erase [under construction]
#ifdef __FLASH_DRIVER__
				 // Format: r 33 (R/W) (3-byte flash address) (Data to write OR size of data to
				 // read) Example: r 33 R *010000* 128 // Reads 128-bytes from flash to the calling
				 // port (max length set by general buffer size) Example: r 33 W *0C0000* "Write
				 // this to flash" // Writes a string to flash (max length set by maximum command
				 // length)
			addr = CharsToFlashAddr(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
			switch (rbcmd[cmd1]) {
			case 'R':
			case 'r':
				flashRead(addr, rbcmd[cmd5], Lbuf);
				sendMSG(port, Lbuf, rbcmd[cmd5]);
				HexPrint(USER_PORT, Lbuf, rbcmd[cmd5]); // Send a copy to the USER_PORT
#ifdef __MYSTACK__
				PacketizeAndPush(Lbuf, rbcmd[cmd5], MISC, GENERAL_BUFFER_SIZE, MISC_PACKET,
								 PPM_NUM); // Send a copy to the ground.
#endif
				break;

			case 'W':
			case 'w':
				// HexPrint(USER_PORT,&rbcmd[cmd5],cmd_length-cmd4);
				flashWrite(addr, cmd_length - cmd4, &rbcmd[cmd5]);
				break;

			default:
				break;
			}
#endif
			break;

		case 30: //== Store current IO data
				 // if (getIO(rbcmd[cmd1], rbcmd[cmd2], buf)) saveStoredDataRecord(buf); // Don't
				 // store if zero length, also..
			break;

		case 31: //== Upload stored IO data   (30)
				 //         if (! hasStoredData())break; // Bail out here if no data is stored
		case 201: //== Text Messages     (200)
		case 98: //== Dump Macro Command   (99)
		case 104: // Event <-> Macro binding (105)
			break;

		case 34: // not used Radio Paramaters(Payload commands)
			break;

		case 39: //== Unused
			break;

		case 40: //== Perform a general purpose R/W
			/*
						n = (int) rbcmd[cmd1]; // n== 0 Read, n== 1,2,3, Write
						m = (int) rbcmd[cmd2]; // m is board address or flash address
						value = (int) rbcmd[cmd3]; // Value to be writen or where it is placed
						switch (n) {
						case 0: //== Read Cooresponding BUS address
							//                  DataByte[value]= BusRead(m);
							break;
						case 1: //== Write value to Cooresponding BUS address
							//                  BusWrite(m,value);
							break;

						case 2: //== Write to Port 0 control registers
							globals.EXT_INT = rbcmd[cmd2];          // Which ports are enabled for
			   interrupt globals.PICTL = rbcmd[cmd3];            // Enable interrupt from P0_4-7 and
			   on falling edge globals.INT_PIN = rbcmd[cmd4];          // Enable interrupt pin 0-7
							globals.WAKE_MASK0 = rbcmd[cmd5];       // Mask used by ISR to select
			   interrupt for wakeup globals.WAKE_MASK1 = rbcmd[cmd5+1];     // Mask used by ISR to
			   select interrupt for wakeup globals.WAKE_MASK2 = rbcmd[cmd5+2];     // Mask used by
			   ISR to select interrupt for wakeup break;

						case 5: // Programs ADC_Channel
							globals.ADC_Channel=C2I(rbcmd+cmd2);
							break;

						default:
							break;
						}
			*/
			break;

		case 55: // set alarm bytes
		{
			/*
						  index = rbcmd[cmd1];
						   if (index < NUM_ALARM_BYTES) {
							   switch (rbcmd[cmd2]) {
								 case 'R'://aise
								   if (index == SYSTEM_ALARM_BYTE) {
									   raiseSystemAlarm(rbcmd[cmd3]);
								   } else {
									   getAlarmBytes()[index] |= rbcmd[cmd3];
								   }
								   break;

								 case 'C'://lear
								   getAlarmBytes()[index] &= ~rbcmd[cmd3];
								   break;

								 case 'S'://et
								   getAlarmBytes()[index] = rbcmd[cmd3];
								   break;
							   }
						   } else {
							   // system alarm enable 'M'ask
							   if (index == 'M') {
								   setSystemAlarmMask(rbcmd[cmd2]);
							   }
							   if (index == 'm') {
								   globals.systemAlarmEnable = rbcmd[cmd2];
							   }
						   }
		   */
		} break;

		case 62: // 10 second delay
			unsigned long timeout;
			timeout = SEC_TIMER + 10;
			while (timeout > SEC_TIMER) {
				restart_wdt();
			}
			break;

		case 63: // Reset the satellite
#ifdef LOWER_PPM
				 // Clear all schedules except the last 4
			// This is better than clearing everything in loadDefaults() beacuse if we have to do
			// that during flight, we end up killing our beacon.
			for (c = 0; c < NUM_STORED_CMDS - 4; c++) {
				clearSchedule(&globals.StoredCmds[c][0]);
				sprintf(dbgbuf, "\r\nSchedule %d cleared.", c);
				sendDBGALL(USER_PORT, dbgbuf);
			}
// Clear boot macro
#ifdef __MACROS__
			clearMacro(BOOT_MACRO);
#endif

			// Load the defaults
			loadDefaults(&globals);
#ifdef __AENEAS__
			loadExtInDefaults();
#endif

			// Save both the globals
			commitGlobalsToFlash();
#ifdef __AENEAS__
			commitExtInToFlash();
#endif

			// Disable IMI and stop all wheels/coils:
			globals.IMIEnable = 0;
#ifdef __IMI100_H__
			setIMIReactionWheelSpeeds_DegPerSec(IMI_PORT, 0.0, 0.0, 0.0);
			setIMICoilDipole_Am2(IMI_PORT, 0.0, 0.0, 0.0);
#endif

			// Clear the boot macro on the upper PPM
			sprintf(Lbuf, "99 39 0");
			sendPPMCmd(OTHER_PPM_PORT, Lbuf);

			// Load Defaults on Upper (this clears the beaconflag)
			sprintf(Lbuf, "241");
			sendPPMCmd(OTHER_PPM_PORT, Lbuf);

			// Save the globals on the upper
			sprintf(Lbuf, "240");
			sendPPMCmd(OTHER_PPM_PORT, Lbuf);

#endif
			break;

		case 65: // Init all FSW modules (GNC and wheels, essetially)
#ifdef LOWER_PPM
			globals.Geo_Enable = 1;
			globals.orbitEnable = 1;
			globals.GNC_Enable = 1;
			globals.load_Enable = 1;
			globals.attitudeSensorsEnable = 1;
			globals.IMIEnable = 1;

			sendDBGALL(USER_PORT, "All FSW modules enabled.");
#endif
			break;

		case 66: //
			break;

		case 70: // Suncheck and Scheduling (sun check)
#ifdef __EPS_H__
			unsigned char epst[7];
			unsigned char deltat1[7] = {0, 0, 0, 0, 0, 0, 30}; // Delta Time for first burn
			unsigned char deltat2[7] = {0, 0, 0, 0, 0, 0, 60}; // Delta Time for second burn
			unsigned char deltat3[7] = {0, 0, 0, 0, 0, 0, 90}; // Delta Time for FSW enable
			unsigned char eps_cmd[3][2] = {97, 4, 97, 5, 97, 6}; // Commands (Macros to run)

			// Override in Cmd1
			sendDBGALL(USER_PORT, "\r\nChecking for Sun...");
			if (checkSun() || rbcmd[cmd1]) {
				sendDBGALL(USER_PORT, "\r\nSUN FOUND!");
				if (rbcmd[cmd1])
					sendDBGALL(USER_PORT, " --- OVERRIDE!");

				DeltaTime(time, deltat1, epst);
				setSchedule(&globals.StoredCmds[4][0], MAX_STORED_CMD_SIZE, &eps_cmd[0][0], 2,
							epst); // Set the Fourth Domino Schedule (must use DeltaTime)
				DeltaTime(time, deltat2, epst);
				setSchedule(&globals.StoredCmds[5][0], MAX_STORED_CMD_SIZE, &eps_cmd[1][0], 2,
							epst); // Set the Fifth Domino Schedule (must use DeltaTime)
				DeltaTime(time, deltat3, epst);
				setSchedule(&globals.StoredCmds[6][0], MAX_STORED_CMD_SIZE, &eps_cmd[2][0], 2,
							epst); // Set the Sixth Domino Schedule (must use DeltaTime)
				globals.StoredCmds[3][0] = 0;
				commitGlobalsToFlash();
				sendDBGALL(USER_PORT, "\r\nGlobals saved.");
			}
#endif
			break;

		case 90: // Load Menus
			// cmd_length = rbcmd[0] - 11;
			// flash('p', (long)rbcmd[cmd1] * 64 + Menu_Addr, cmd_length + 2, rbcmd + 11);
			break;

#ifdef __MACROS__ //------------------------------------------------------------------------- MACRO
				  // COMMANDS
		case 96: //== Stop Macro

			n = (int)rbcmd[cmd1]; // n is Macro ID
			if (n == 255) // 255 stops all Macros(except background)
			{
				for (n = 0; n < NUM_MACROS; n++) {
					if (Macro_state[n] != MACRO_BACKGROUND)
						MacroStop(n);
				}
			} else
				MacroStop(n);

			break;

		case 97: //== Run Macro
			temper = MACRO_RUN;
			c = rbcmd[cmd1];
			if (Macro_state[c] == MACRO_RUN)
				break; // Don't restart if already running
			if (rbcmd[cmd2] == 255)
				temper = MACRO_BACKGROUND; // Infinite loop
			MacroCmdRun(rbcmd[cmd1], temper, rbcmd[cmd2], source); // If not Running, Start normally
			break;

		case 99: //== Load a Macro Command
				 // r 99 <macroId> <flag> <macroContents>
				 // macroId :== 0..NUM_MACROS-1 (NUM_MACROS == 40 for mLock)
				 // flag :== {0 (clear), 'A' (append EXACTLY ONE command), any other value 'L' by
				 // convention (load macro) macorContents :==
				 //            flag == 0 : NULL
				 //            flag == 'A' : exactly one command NOT prefixed by length (i.e. '240',
				 //            not '1 240')  <--- I don't know if this is true anymore. flag == <not
				 //            0 and not 'A'> : a series of length-prefixed commands (i.e. 4 50 'LL'
				 //            100 4 100 10 0 1 3 30 33 83 2 31 2)
			cmd_length = rbcmd[0] - 11; // remove cmd len and macro command, and macro ID
			if (rbcmd[cmd2] == 0) {
				clearMacro(rbcmd[cmd1]);
			} else {
				loadMacro(rbcmd[cmd1], buf, rbcmd + cmd3, cmd_length);
			}

			break;

		case 100: //== Defines one second Delays between Macro commands
			c = rbcmd[cmd1];
			if (c > NUM_MACROS - 1)
				return -1;
			delay[c] = 10 * C2I(rbcmd + cmd2);
			break;

		case 101: //== Defines interrupt based Delays between Macro commands
			c = rbcmd[cmd1];
			if (c > NUM_MACROS - 1)
				return -1;
			delay[c] = C2I(rbcmd + cmd2);
			break;

		case 102: //==Defines milli-second delays between commands
			if (rbcmd[cmd1] > 50)
				rbcmd[cmd1] = DataByte[0] * globals.Bspacing;
			delay_ms(rbcmd[cmd1]);
			break;

		case 103: //== Wait for 'm' Macro to complete before continuing MAcro 'n'
		{
			unsigned char dontDelay = 0;
			c = rbcmd[cmd2];
			// 103 39 "U" 200 or 	103 39 4 or 103 5 'G' 120	Place in 39, waits till 4 is
			// complete 103 3 'G' 30
			if (rbcmd[cmd2] < NUM_MACROS) // If pointing to a MAcro
			{
				MacroCflag[c] = rbcmd[cmd1]; // reset delay when 4 is done
				c = rbcmd[cmd1];
				delay[c] = 0xffff; // treat wait command like a long delay
			} else // Must be a conditional delay
			{
				switch (rbcmd[cmd2]) {
				case 'A':
					n = NUM_MACROS + 2;
					break; // Delay macro until Associated

				case 'U':
					n = NUM_MACROS;
					break; // Delay macro until Upload is complete
				default:
					break;
				}

				if (!dontDelay) {
					c = rbcmd[cmd1];
					delay[c] = rbcmd[cmd3] * 10; // Backup timer. Maximum delay (255sec or less)
					MacroCflag[n] = rbcmd[cmd1]; // The flag is set to the Macro being delayed
				}
			}
		} break;
#endif // __MACROS__

		case 105: // Load macro event mask
			/*
						{
							macroEventInfo_t eventInfo;
							if (rbcmd[cmd2] == 0) {
								memset(&eventInfo, 0, sizeof(macroEventInfo_t));
							} else {
								rcpy((unsigned char*)&(eventInfo.eventMask), rbcmd + cmd3, 4);
								eventInfo.len = 4;
							}

							loadMacroEventMask(rbcmd[cmd1], &eventInfo);
						}
			*/
			break;

			// set the global system event mask
		case 106:
			//            rcpy((unsigned char*)&(globals.systemEventMask), rbcmd + cmd1, 4);
			break;

		case 107: // Init Orbit
// This command used to be included under orbitCmd in case 108, but the PIC
// has trouble going so many function levels deep and then passing float64s.
// Not sure why -- might be a compiler thing.  Either way, it works here.  SO
// here it stays.
//
// Format: r 107 "<space-deliminated ascii numbers describing the Position (X Y Z) and Velocity (X Y
// Z) in the Earth-Centric Inertia Frame of Reference>" Example: r 107 "421.53 6820999.95 649.0
// 4215.3 -0.856 6490.0"
//					XPOS   YPOS      ZPOS    Vx     Vy     Vz
// To schedule an orbit init, use r 253
#ifdef __ORBIT__
			float orb_elements[6];
			// Get the floats from the text
			if (getFloats(orb_elements, &rbcmd[cmd1], 6) != SUCCESS)
				return -1;
			initOrbit(time, &orb_elements[0], &orb_elements[3],
					  time); // Position is first 3 elements, Velocity is 2nd 3.

			// Also set the globals
			globals.orbitInit[0] = orb_elements[0];
			globals.orbitInit[1] = orb_elements[1];
			globals.orbitInit[2] = orb_elements[2];
			globals.orbitInit[3] = orb_elements[3];
			globals.orbitInit[4] = orb_elements[4];
			globals.orbitInit[5] = orb_elements[5];
			globals.orbitInitTime[0] = time[0];
			globals.orbitInitTime[1] = time[1];
			globals.orbitInitTime[2] = time[2];
			globals.orbitInitTime[3] = time[3];
			globals.orbitInitTime[4] = time[4];
			globals.orbitInitTime[5] = time[5];
			globals.orbitInitTime[6] = time[6];

			globals.orbitEnable = 1;

			sendDBGALL(USER_PORT, "\r\nOrbit Initialized.");
#endif
			break;

		case 108: // Orbit Stuff
#ifdef __ORBIT__
			orbitCmd(&rbcmd[cmd1]); // Pass the string to the orbit Command Processor.
#endif
			break;

		case 109: // geomag model
#ifdef __GEOMAG__
			float tempfl;
			unsigned char* next;
			switch (rbcmd[cmd1]) {
			case 'E': // Enables Geomagnetic model
				globals.Geo_Enable = 1;
				sendDBGALL(USER_PORT, "\r\nGeomag Initialized.");
				break;
			case 'D': //
				globals.Geo_Enable = 0;
				sendDBGALL(USER_PORT, "\r\nGeomag Disabled.");
				break;
			case 'C': // This loads static coefs into flash (only need to do once)
					  // format r 109 "C" 0 1 0 " -29619.4 13.3 2.0 2.0 "  (x y bytes , 4 byte + 4
					  // bytes)
				memcpy(buf, rbcmd + cmd3, 2);
				next = rbcmd + cmd5 + 1; // Beginning of floating point data.
				for (n = 0; n < 4; n++) { // convert to float
					tempfl = strtod(next, &next);
					memcpy(buf + 2 + (n * 4), &tempfl, 4);
				}
				getCRC(buf, 18, buf + 18); // append CRC to end of coeff
				flashWrite((long)rbcmd[cmd2] * (long)32 + MAG_COEFF_ADDR, 20, buf);
				//						flash('w',(long)rbcmd[cmd2]*(long)32 + MAG_COEFF_ADDR, 20,
				// buf);
				break;

			case 'T': // Test command to print test B field vector
				// Format? (Mike Assumed): r 109 T "(pos eci) (gha)"
				next = rbcmd + cmd2;
				for (n = 0; n < 3; n++) {
					orbitData[n] = 0.0; // this zeros out thrust and drag
					orbitData[6 + n] = strtod(next, &next); // Position and velocity
				}
				orbitData[15] = strtod(next, &next); // This is GHA

				n = get_timer1();
				geomag(B_field_ECI, &orbitData[6], orbitData[15], 3900,
					   globals.ReadGeomagFromFlash);
				n = get_timer1() - n;
				sprintf(Lbuf, "%d ", n);
				placeString(0, 5, 50, Lbuf);

				for (m = 0; m < 3; m++) {
					sprintf(Lbuf, "%d", (long)B_field_ECI[m]);
					placeString(0, 1 + m, 65, Lbuf);
				}
				break;
			case 'F': // Read Coefficients from Flash
				globals.ReadGeomagFromFlash = 1;
				sendDBGALL(USER_PORT, "\r\nReading geomagcoefs from Flash.");
				break;
			case 'R': // Read Coefficients from ROM
				globals.ReadGeomagFromFlash = 0;
				sendDBGALL(USER_PORT, "\r\nReading geomag coefs from ROM.");
				break;

			case 'X':
			case 'x':
				sprintf(dbgbuf, "\r\nGeomag CRC Errors: %d", globalCRCErrorCounter);
				sendDBGALL(USER_PORT, dbgbuf);
				break;

			default:
				break;
			}
#endif
			break;

		case 110: // Flight Software COmmands
		{
#ifdef __AENEAS__
			switch (rbcmd[cmd1]) {
			case 'E': // Enable
			case 'e':
				globals.GNC_Enable = 1;
				globals.GNC_Enable ? sprintf(dbgbuf, "\r\nGNC On") : sprintf(dbgbuf, "\r\nGNC Off");
				sendDBGALL(USER_PORT, dbgbuf);
				break;

			case 'D': // Disable
				globals.GNC_Enable = 0;
				globals.GNC_Enable ? sprintf(dbgbuf, "\r\nGNC On") : sprintf(dbgbuf, "\r\nGNC Off");
				sendDBGALL(USER_PORT, dbgbuf);
				break;

			case 'S': // Store current Configuration in Flash
			case 's':
				commitExtInToFlash();
				sendDBGALL(USER_PORT, "\r\nExtIn saved.");
				break;

			case 'R': // Restore from flash the ExtIn configuration
			case 'r':
				if (restoreExtIn(ExtIn) != SUCCESS) {
					sendDBGALL(USER_PORT, "\r\nExtIn restore failed, using defaults.");
					loadExtInDefaults();
				} else {
					sendDBGALL(USER_PORT, "\r\nExtIn restored.");
				}

				break;

			case 'L': // Load a float into the ExtIn array
			case 'l':
				// Format: r 110 L (1-byte index) (ASCII float in quotes)
				// Example: r 110 L 0 "5.0" // Forces Mission Mode to 5
				c = rbcmd[cmd2];
				getFloats(&ExtIn[c], &rbcmd[cmd3], 1);
				break;

			case 'Z': // Load defaults (both D and R for defaults or Restore were taken, so how
					  // about Z)
				loadExtInDefaults();
				break;

			case 'Q': // Reset
			case 'q':
				initModel();
				break;

			case 'X': // Show/Hide the GNC screen
			case 'x':
				showGNCscreen = !showGNCscreen;
				break;

			default:
				break;
			}
#endif
			break;
		}

#ifdef __AENEAS__
		case 111: // Enable/disable the loading of ExtIn values
			switch (rbcmd[cmd1]) {
			case 'E': // Enable
				globals.load_Enable = 1;
				sendDBGALL(USER_PORT, "\r\nLoading ExtIn values enabled.");
				break;
			case 'D': // Disable
				globals.load_Enable = 0;
				sendDBGALL(USER_PORT, "\r\nLoading ExtIn values disabled.");
				break;
			default:
				break;
			}
			break;
#endif

#ifdef __GYRO_H__
		case 112: // Gyro test commands
		{
			int t;
			int r;
			int discard;

			switch (rbcmd[cmd1]) {
			case 'T': // These are tests to dump raw data results to display
				GyroFlag = collectAllGyroData(&GyroData);
				if (GyroFlag != SUCCESS)
					sendDBGALL(USER_PORT, "\r\nGyroscope failed.");
				else {
					sendDBGALL(USER_PORT, "\r\nGyroscope data aquired.");
#ifdef DEBUG
					printGyroStruct(&GyroData);
#endif
				}
				break;
			case 'P': // Gyro power on only (no initializing).
				sendDBGALL(USER_PORT, "\r\nGyro power on.");
				gyro_on();
				break;
			case 'E': // This enables the Gyro power and inits hardware
				sendDBGALL(USER_PORT, "\r\nGyro power on and initialized.");
				gyro_init();
				break;
			case 'D': // Disable
				sendDBGALL(USER_PORT, "\r\nGyro power off.");
				gyro_off();
				break;
			case 'C': // Command
					  // This sends a certain number of bytes to the gyro over SPI.  Result is
					  // HexPrinted to the screen. Note that because of full duplex, the result from
					  // one request will show on the next.  Send 0xffff to see the response.
					  // 	Format: r 112 C (gyro number 0-2) (2 bytes to clock in)
					  //	Examples:
					  //  r 112 C 0 *5600* 	// reads product ID from Gyro 0
					  //  r 112 C 0 *ffff*	// gets the result from the previous request
					  //	r 112 C 0 *0400*	// reads the GYRO_OUT rotation rate.
					  //  r 112 C 1 *3800*	// Reads the SENS_AVG register from Gyro 1
				t = make16(rbcmd[cmd3], rbcmd[cmd4]); // Create the 16-bit message to send

				gyroTalk((int)rbcmd[cmd2], t, &r);

#ifdef DEBUG
				sendDBGALL(USER_PORT, "\r\nSPI Sent:     ");
				HexPrint(USER_PORT, &rbcmd[cmd3], 2);
				sendDBGALL(USER_PORT, "\r\nSPI Returned (Reversed Order): ");
				HexPrint(USER_PORT, &r, 2);
				sendDBGALL(USER_PORT, "\r\n");
#endif // Debug
				break;

			case 'B': // Bias Optimization Test (per the datasheet)
				// Calibrates the gyro offset by recording 150 seconds of data at 256 samples per
				// second. The results are averaged, rounding to the nearest int, and multiplied by
				// negative 1. This number is then clocked in to GYRO_OFF to set the offset, and the
				// result is saved to the gyro flash.
				//
				// Format: r 112 B (gyro number 0-2) (Fix Offset 1 or 0)
				// Example: r 112 B 0 0		// Only calculates the offset.  Does not change anything
				// Must be stationary for 160 seconds.
				//			r 112 B 0 1		// Calibrates Gyro 0 Offset.  Must be stationary for 160
				// seconds.
				float r_avg = 0;
				unsigned int64 timeout;
				//					int test_samples[5]={0x1110,0x0001,0x0000,0x3FFF,0x2EF0};
				////Should represent max, min positive, zero, small negative, max negative.
				sendDBGALL(USER_PORT, "\r\n	       Gyro Calibration Test");
				sendDBGALL(USER_PORT, "\r\n------------------------------------------");
				sendDBGALL(USER_PORT, "\r\nThis test will take approx 160 seconds...");
				setup_wdt(WDT_OFF);
				gyro_off();
				delay_ms(100);
				gyro_on();
				delay_ms(10000); // 10 seconds

				// If the command is to overwrite the calibration, then we will set the sensor range
				// and perform a factory reset. Otherwise, skip this (keeping the old offset)
				if (rbcmd[cmd3]) {
					t = 0xB901; // Sets SENS_AVG to 80 deg/sec
					gyroTalk((int)rbcmd[cmd2], t, &r);
					// Perform a factory restore to clear any previous GYRO_OFFSET settings
					t = 0xBE02; // Sets the factory restore bit on GLOB_CMD
					gyroTalk((int)rbcmd[cmd2], t, &r);
				}

				t = 0x0400; // Reads GYRO_OUT
				gyroTalk((int)rbcmd[cmd2], t, &r);
				r_avg = 0.0;
				for (dbg_long = 0; dbg_long < 256 * 150; dbg_long++) {
					timeout =
						MSEC + 4; // At 256 samples per second, each sample should take only 3.90625
								  // ms.  So we make sure to take 4 ms between each read
					gyroTalk((int)rbcmd[cmd2], t, &r);
					//						r = test_samples[dbg_long];
					//						byteMe(r);
					//						sendDBGALL(USER_PORT,"\t");
					signExtend14to16(&r);
					//						byteMe(r);
					dbg_float = gyro_counts2DPS(r);
					//						sendDBGALL(USER_PORT,"\t");
					//						printFloat(dbgbuf,dbg_float,6);
					//						sendDBGALL(USER_PORT,dbgbuf);
					if (dbg_long == 0)
						r_avg = dbg_float;
					else
						r_avg = ((r_avg * dbg_long) + dbg_float) / (dbg_long + 1);
					//						sendDBGALL(USER_PORT,"\t");
					//						printFloat(dbgbuf,r_avg,6);
					//						sendDBGALL(USER_PORT,dbgbuf);
					//						sendDBGALL(USER_PORT,"\r\n");
					// delay_ms(100);
					if (dbg_long % (256 * 5) == 0) {
						// Every 5 seconds, print something
						sendDBGALL(USER_PORT, ".");
					}
					while (MSEC < timeout) {
					};
				}
				// Obtaining the offset to write
				sendDBGALL(USER_PORT, "\r\nCalculated Average = ");
				printFloat(dbgbuf, r_avg, 6);
				sendDBGALL(USER_PORT, dbgbuf);
				sendDBGALL(USER_PORT, " deg/sec");
				sendDBGALL(USER_PORT, "\r\nOffset Needed = ");
				r_avg = -1.0 * r_avg;
				printFloat(dbgbuf, r_avg, 6);
				sendDBGALL(USER_PORT, dbgbuf);
				sendDBGALL(USER_PORT, " deg/sec");
				sendDBGALL(USER_PORT, "\r\nConversion Back to Counts = ");
				r = gyro_DPS2counts(r_avg);
				byteMe(r);
				sendDBGALL(USER_PORT, " = ");
				HexPrint(USER_PORT, &r, 2);
				sendDBGALL(USER_PORT, " = ");
				sprintf(dbgbuf, "%d", r);
				sendDBGALL(USER_PORT, dbgbuf);

				// Write the results in to GYRO_OFF
				if (rbcmd[cmd3]) {
					c = make8(r, 0);
					t = make16(0x94, c);
					gyroTalk((int)rbcmd[cmd2], t, &discard);
					sendDBGALL(USER_PORT, "\r\nSPI Send = ");
					HexPrint(USER_PORT, &t, 2);
					c = make8(r, 1);
					t = make16(0x95, c);
					gyroTalk((int)rbcmd[cmd2], t, &discard);
					sendDBGALL(USER_PORT, "\r\nSPI Send = ");
					HexPrint(USER_PORT, &t, 2);
					sendDBGALL(USER_PORT, "\r\nWriting Gyro Flash");
					t = 0xBE08; // Write flash
					gyroTalk((int)rbcmd[cmd2], t, &discard);
				}
				delay_ms(50);
				t = 0x1400; // Reading what we wrote
				gyroTalk((int)rbcmd[cmd2], t, &discard);
				t = 0x1400; // Reading what we wrote
				gyroTalk((int)rbcmd[cmd2], t, &r);
				sendDBGALL(USER_PORT, "\r\nConfirmation = ");
				HexPrint(USER_PORT, &r, 2);
				sendDBGALL(USER_PORT, "\r\nTest Complete.");
				setup_wdt(WDT_ON);
				break;

			default:
				break;
			}
			break;
		}
#endif // Gyro

		case 113: // Sets port handshaking
			// Format: r 113 (port) (1/0)
			// Example: r 113 1 1 // Turns on Handshaking for Port 1
			setPortHandshaking(rbcmd[cmd1], rbcmd[cmd2]);
			break;

		case 115:
			break;

#ifdef __SUNSENSOR__
#ifdef DEBUG
		case 116: // Get sun sensor version.
		{
			if (pingSunSensor(SUN_PORT) != SUCCESS)
				sendDBGALL(USER_PORT, "\r\nSun Sensor ping failed.");
			break;
		}
#endif
#endif

		case 117: // Debug ADC Test
			break;

#ifdef LOWER_PPM
		case 118: // Enable or disable atttiude sensor collection
		{
			switch (rbcmd[cmd1]) {
			case 'E': // Enable
				globals.attitudeSensorsEnable = 1; // Update flag
				attitudeSensors4HzTask(); // Reset sensor task state
				sendDBGALL(USER_PORT, "\r\nSensor aquisition enabled.");
				break;

			case 'D': // Disable
				globals.attitudeSensorsEnable = 0;
				sendDBGALL(USER_PORT, "\r\nSensor aquisition disabled.");
				break;
			default:
				break;
			}
			break;
		}
#endif

#ifdef __IMI100_H__
		case 119: // Generic Commanding for IMI
			// Format: r 119 (cmd) (options)
			// Example: r 119 1 "990.0 0.0 300.0"	// Sets RW speeds to 990, 0, and 300
			// respectively for x,y,z Example: r 119 2 "0.01 0.0 0.0"		// Sets Torque Coil X to
			// 0.01 Am2 cmd list 0: Sets IMI to Test Mode (no options)
			//			1: Set IMI Wheel Speeds in Deg/Sec (3 options: x,y,z)
			//			2: Set IMI Torque Coil Diploe Strength in Am2 (3 options: x,y,z)
			//			3: Set IMI Wheel Torque in mNm (3 options: x,y,z).
			// options - All options are floats.

			// Get the floats from the text for anything other than option 1
			//			sprintf(dbgbuf,"\r\nCommand = %d",rbcmd[cmd1]);
			//			sendDBGALL(USER_PORT,dbgbuf);

			float imi_options[3];

			if (rbcmd[cmd1] != 0) {
				if (getFloats(imi_options, &rbcmd[cmd2], 3) != SUCCESS) {
					//					HexPrint(USER_PORT,&rbcmd[cmd2],cmd_length-7);
					sendDBGALL(USER_PORT, "\r\nUnable to extract floats.");
					return -1;
				} else {
					//					sendDBGALL(USER_PORT,"\r\nGot floats.");
				}
			}
			switch (rbcmd[cmd1]) {
			case 0:
				sendDBGALL(USER_PORT, "\r\nTest Mode Enabled.");
				setIMItoTestMode(IMI_PORT);
				break;

			case 1:
				sendDBGALL(USER_PORT, "\r\nCommanding Wheel Speeds.");
				setIMIReactionWheelSpeeds_DegPerSec(IMI_PORT, imi_options[0], imi_options[1],
													imi_options[2]);
				break;

			case 2:
				sendDBGALL(USER_PORT, "\r\nCommanding Torque Coils.");
				setIMICoilDipole_Am2(IMI_PORT, imi_options[0], imi_options[1], imi_options[2]);
				break;

			case 3:
				sendDBGALL(USER_PORT, "\r\nCommanding Wheel Torques.");
				setIMIReactionWheelTorque_mNm(IMI_PORT, imi_options[0], imi_options[1],
											  imi_options[2]);
				break;

			case 'E': // Enable IMI
				sendDBGALL(USER_PORT, "\r\nReal IMI Output Enabled.");
				globals.IMIEnable = 1;
				break;

			case 'D': // Disable IMI
				sendDBGALL(USER_PORT, "\r\nReal IMI Output Disabled.");
				globals.IMIEnable = 0;
				break;

			default:
				break;
			}

			break;

#endif

		case 120: // DEBUG: Testing iTag
			break;

#ifdef __MAESTRO__
			// TODO: Should any of these feedback messages go to the
			// ground over the radio?
		case 121: {
			// Power on the MAESTRO payload and control the UART.
			// Format: r 121 (action)
			// action 0 -- hard initializes the MAESTRO bootstrapper.
			// action 1 -- power on the MAESTRO payload.
			// action 2 -- power off the MAESTRO payload.
			// action 3 -- drive the MAESTRO reset line low.
			// action 4 -- drive the MAESTRO reset line high.
			// action 5 -- configure the UART.
			// action 6 -- restore the UART.
			// action 7 -- set the UART parity and stop bits.
			// action 8 -- set the UART nonblocking output mode.
			// action 9 -- send a character to the UART.

			if (cmd_length < cmd1) {
				goto wrong_length_r_121;
			}
			switch (rbcmd[cmd1]) {
			case 0: {
				if (cmd_length != cmd1) {
					goto wrong_length_r_121;
				}
				initialize_maestro_command(1); // hard initialization
				break;
			}
			case 1: {
				if (cmd_length != cmd1) {
					goto wrong_length_r_121;
				}
				power_on_maestro();
				break;
			}

			case 2: {
				if (cmd_length != cmd1) {
					goto wrong_length_r_121;
				}
				power_off_maestro();
				break;
			}

			case 3: {
				if (cmd_length != cmd1) {
					goto wrong_length_r_121;
				}
				reset_maestro(TRUE);
				break;
			}

			case 4: {
				if (cmd_length != cmd1) {
					goto wrong_length_r_121;
				}
				reset_maestro(FALSE);
				break;
			}

			case 5: {
				// Configure the UART for low or high speed operation.
				// Format: r 121 5 (0/1)
				// value: 0 ==> low speed operation
				//        1 ==> high speed operation
				// Example: r 121 5 1
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 122 5 (0/1)\r\n");
					break;
				}
				configure_maestro_uart(rbcmd[cmd2]);
				break;
			}

			case 6: {
				if (cmd_length != cmd1) {
					goto wrong_length_r_121;
				}
				restore_maestro_uart();
				break;
			}

			case 7: {
				// Set the UART parity and stop bits.
				// Format: r 121 7 (value)
				// value: 0 ==> no parity, 1 stop bit
				//        1 ==> no parity, 2 stop bits
				//        2 ==> even parity, 1 stop bit
				//        3 ==> even parity, 2 stop bits
				//        4 ==> odd parity, 1 stop bit
				//        5 ==> odd parity, 2 stop bits
				// Example: r 121 7 1
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 122 4 (value)\r\n");
					break;
				}
				set_maestro_uart_mode(rbcmd[cmd2]);
				break;
			}

			case 8: {
				// Set the UART nonblocking output mode.
				// Format: r 121 8 (0/1)
				// value: 0 ==> disable nonblocking output
				//        1 ==> enable nonblocking output
				// Example: r 121 8 1
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 122 5 (value)\r\n");
					break;
				}
				set_maestro_nonblocking_uart_output_mode((int)rbcmd[cmd2]);
				break;
			}

			case 9: {
				// Send a character to the MAESTRO via the UART.
				// Format: r 121 9 (char)
				// Example: r 121 9 1
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 121 9 (char)\r\n");
					break;
				}
				send_to_maestro_uart(rbcmd[cmd2]);
				break;
			}

			default:
				sendDBGALL(USER_PORT, "\r\nUnknown MAESTRO control action: r 121 (action)\r\n");
			}
			break;

		wrong_length_r_121 : {
			sendDBGALL(USER_PORT, "\r\nWrong length: r 121 (action)\r\n");
			break;
		}
		}

		case 122: {
			// Control the execution of the MAESTRO bootstrapper.
			// Format: r 122 (action)
			// action -- 0 soft initializes the MAESTRO botostrapper and
			//             starts a test program.
			// action -- 1 soft initializes the MAESTRO bootstrapper.
			// action -- 2 starts the MAESTRO test program.
			// action -- 3 stops the MAESTRO test program.
			// action -- 4 resumes the MAESTRO test program.

			if (cmd_length < cmd1) {
				goto wrong_length_r_122;
			}
			switch (rbcmd[cmd1]) {
			case 0: {
				// Soft initialize the MAESTRO bootstrapper and start a
				// MAESTRO test program.  This command takes an optional
				// argument, which is the program number.
				// Example: r 122 0
				// Example: r 122 0 3
				if (cmd_length == cmd2) {
					set_maestro_program_number(rbcmd[cmd2]);
				} else if (cmd_length != cmd1) {
					sendDBGALL(USER_PORT, "\r\nWrong length: r 123 4 [(program number)] ...\r\n");
					break;
				}
				initialize_maestro_command(0); // soft initialization
				start_maestro_test();
				break;
			}

			case 1: {
				// Soft initialize the MAESTRO bootstrapper.
				// This command takes an optional argument, which is the program number.
				// Example: r 122 1
				// Example: r 122 1 3
				if (cmd_length == cmd2) {
					set_maestro_program_number(rbcmd[cmd2]);
				} else if (cmd_length != cmd1) {
					sendDBGALL(USER_PORT, "\r\nWrong length: r 123 0 [(program number)] ...\r\n");
					break;
				}
				initialize_maestro_command(0); // soft initialization
				break;
			}

			case 2: {
				// Start a MAESTRO test program.
				// This command takes an optional argument, which is the program number.
				// Example: r 122 2
				// Example: r 122 2 3
				if (cmd_length == cmd2) {
					set_maestro_program_number(rbcmd[cmd2]);
				} else if (cmd_length != cmd1) {
					sendDBGALL(USER_PORT, "\r\nWrong length: r 123 1 [(program number)] ...\r\n");
					break;
				}
				start_maestro_test();
				break;
			}

			case 3: {
				// Stop the MAESTRO test program.
				// Example: r 122 3
				if (cmd_length != cmd1) {
					goto wrong_length_r_122;
				}
				stop_maestro_test();
				break;
			}

			case 4: {
				// Resume a MAESTRO test program.
				// Example: r 122 4
				if (cmd_length != cmd1) {
					goto wrong_length_r_122;
				}
				resume_maestro_test();
				break;
			}

			default:
				sendDBGALL(USER_PORT, "\r\nUnknown MAESTRO execution action: r 122 (action)\r\n");
			}
			break;

		wrong_length_r_122 : {
			sendDBGALL(USER_PORT, "\r\nWrong length: r 122 (action)\r\n");
			break;
		}
		}

		case 123: {
			// Set certain MAESTRO bootstrap parameters.
			if (cmd_length < cmd1) {
				sendDBGALL(USER_PORT, "\r\nWrong length: r 123 (action) ...\r\n");
				break;
			}
			switch (rbcmd[cmd1]) {
			case 'd':
			case 'D': {
				// Set the program timeout
				// Format: r 123 D (4-digit hex seconds)
				// Example: r 123 D *0100*
				// This value will be reset to a default value when the MAESTRO is hard initialized.
				if (cmd_length != cmd3) {
					sendDBGALL(USER_PORT,
							   "\r\nWrong command length: r 123 D (4-digit hex seconds)\r\n");
					break;
				}
				unsigned int value;
				value = make16(rbcmd[cmd2], rbcmd[cmd3]);
				set_maestro_program_timeout(value);
				break;
			}

			case 'e':
			case 'E': {
				// Set the program timeout enabled
				// Format: r 123 E (0/1)
				// Example: r 123 E 0
				// This value will be reset to a default value when the MAESTRO is hard initialized.
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 E (0/1)\r\n");
					break;
				}
				set_maestro_enable_program_timeout((int)rbcmd[cmd2]);
				break;
			}

			case 'f':
			case 'F': {
				// Set the show-program-fetches flag.
				// Format: r 123 F (0/1)
				// Example: r 123 F 1
				//
				// 0 ==> do not show program fetches
				// 1 ==> show program fetches
				//
				// This value will be reset to FALSE (0) when the MAESTRO is hard initialized.
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 F (0/1)\r\n");
					break;
				}
				set_maestro_show_program_fetches((int)rbcmd[cmd2]);
				break;
			}

			case 'g':
			case 'G': {
				// Set the program start time
				// Format: r 123 G (8-digit hex value)
				// Example: r 123 E *00000000*
				// This value will be reset to a default value when the MAESTRO is soft initialized.
				if (cmd_length != cmd5) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 G *00000000*\r\n");
					break;
				}
				unsigned long value;
				value = make32(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4], rbcmd[cmd5]);
				set_maestro_program_start_time(value);
				break;
			}

			case 'h':
			case 'H': {
				// Set the program timeout override flag
				// Format: r 123 H (0/1)
				// Example: r 123 H 0
				// This value will be reset to a default value when the MAESTRO is hard initialized.
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 H (0/1)\r\n");
					break;
				}
				set_maestro_override_program_timeout((int)rbcmd[cmd2]);
				break;
			}

			case 'm':
			case 'M': {
				// Set the minimum message severity level for debug printing.
				// Format: r 123 M (0/1/2)
				// Example: r 123 M 1
				//
				// 0 ==> show all messages (DEBUG, MESSAGE, FAIL)
				// 1 ==> do not show debug messages
				// 2 ==> show only FAIL messages
				//
				// This value will be reset to a default value when the MAESTRO is hard initialized.
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 M (0/1/2)\r\n");
					break;
				}
				set_maestro_shout_severity_senddbgall_filter((unsigned int)rbcmd[cmd2]);
				break;
			}

			case 'o':
			case 'O': {
				// Set the current program offset (program counter).  You must supply all 6 hex
				// digits, or the unexpected may happen.
				// Format:  r 123 O (6-digit hex value)
				// Example: r 123 O *000100*
				//
				// This value will be reset to 0 when the MAESTRO is initialized, hard or soft.
				//
				// TODO: Perhaps this control should be part of "r 122"?
				if (cmd_length != cmd4) {
					sendDBGALL(USER_PORT,
							   "\r\nWrong command length: r 123 O (6-digit hex value)\r\n");
					break;
				}
				unsigned long value;
				value = make32(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				set_maestro_program_offset(value);
				break;
			}

			case 'p':
			case 'P': {
				// Set the test program number.
				// Format: r 123 P (program number)
				// Example: r 123 P 1
				//
				// Note:  Program number 0 is the flash memory.
				//        All other program numbers refer to ROM.
				//
				// This value will be reset to a default value when the MAESTRO is hard initialized.
				// To change the program number being executed, you should perform a soft
				// initialization after the change.
				//
				// TODO: Perhaps this control should be part of "r 122"?
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 P (program number)\r\n");
					break;
				}
				set_maestro_program_number(rbcmd[cmd2]);
				break;
			}

			case 'q':
			case 'Q': {
				// Set the minimum message severity level for packetization and queuing
				// for transmission to the ground.
				// Format: r 123 Q (0/1/2)
				// Example: r 123 Q 1
				//
				// 0 ==> packetize and queue all messages (DEBUG, MESSAGE, FAIL)
				// 1 ==> do not packetize and queue debug messages
				// 2 ==> packetize and queue only FAIL messages
				//
				// This value will be reset to a default value when the MAESTRO is hard initialized.
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 Q (0/1/2)\r\n");
					break;
				}
				set_maestro_shout_severity_packetize_filter((unsigned int)rbcmd[cmd2]);
				break;
			}

			case 'r':
			case 'R': {
				// Set the show-received-chars flag.
				// Format: r 123 R (0/1)
				// Example: r 123 R 1
				//
				// 0 ==> do not show characters received on the UART from MAESTRO
				// 1 ==> show characters received on the UART from MAESTRO
				//
				// This value will be reset to FALSE (0) when the MAESTRO is hard initialized.
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 R (0/1)\r\n");
					break;
				}
				set_maestro_show_received_chars((int)rbcmd[cmd2]);
				break;
			}

			case 't':
			case 'T': {
				// Set the trace flag.
				// Format: r 123 T (0/1)
				// Example: r 123 T 1
				//
				// 0 ==> no tracing
				// 1 ==> tracing
				//
				// This value will be reset to FALSE (0) when the MAESTRO is initialized, hard or
				// soft.
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 T (0/1)\r\n");
					break;
				}
				set_maestro_trace((int)rbcmd[cmd2]);
				break;
			}

			case 'w':
			case 'W': {
				// Set a wreg to a value
				// Format:  r 123 W (reg) (8-digit hex value)
				// Example: r 123 W 1 *00000001*
				//
				// The wreg's will be reset to 0 when the MAESTRO is initialized, hard or soft.
				if (cmd_length != cmd6) {
					sendDBGALL(USER_PORT,
							   "\r\nWrong command length: r 123 W (reg) (8-digit hex value)\r\n");
					break;
				}
				unsigned int wreg_index = rbcmd[cmd2];
				unsigned long value = make32(rbcmd[cmd3], rbcmd[cmd4], rbcmd[cmd5], rbcmd[cmd6]);
				set_maestro_wreg(wreg_index, value);
				break;
			}

			case 'y':
			case 'Y': {
				// Set the command execution yield limit.
				// Format:  r 123 Y (4-digit hex value)
				// Example: r 123 Y *0064*
				//
				// This value will be reset to a default value when the MAESTRO is initialized, hard
				// or soft.
				if (cmd_length != cmd3) {
					sendDBGALL(USER_PORT,
							   "\r\nWrong command length: r 123 Y (4-digit hex value)\r\n");
					break;
				}
				unsigned int value = make16(rbcmd[cmd2], rbcmd[cmd3]);
				set_maestro_yield_limit(value);
				break;
			}

			case 'z':
			case 'Z': {
				// Set the MAESTRO system power thresholds.
				// Format: r 123 Z (B/C/S) (pwr)
				// Example: r 123 b 1 # Set battery level
				// Example: r 123 c 1 # Set threshold to continue running
				// Example: r 123 s 2 # Set threshold to start running
				if (cmd_length != cmd3) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 123 Z  (B/C/S) (pwr)\r\n");
					break;
				}
				unsigned char threshold_type = rbcmd[cmd2];
				int power_level = rbcmd[cmd3];
				if (threshold_type == 's' || threshold_type == 'S') {
					set_maestro_system_power_threshold_to_start_running(power_level);
				} else if (threshold_type == 'c' || threshold_type == 'C') {
					set_maestro_system_power_threshold_to_continue_running(power_level);
				} else if (threshold_type == 'b' || threshold_type == 'B') {
					BatteryState_g = power_level;
					sprintf(dbgbuf, "\r\nBattery state set to %u.\r\n", getBatteryState());
					sendDBGALL(USER_PORT, dbgbuf);
				} else {
					sendDBGALL(USER_PORT, "\r\nWrong threshold type: r 123 Z  (B/C/S) (pwr)\r\n");
				}
				break;
			}

			default:
				sendDBGALL(USER_PORT,
						   "\r\nUnknown MAESTRO setup command action: r 123 (action).\r\n");
			}
			break;
		}

		case 124: {
			if (cmd_length < cmd1) {
				sendDBGALL(USER_PORT, "\r\nWrong command length: r 124 (action) ...\r\n");
				break;
			}
			switch (rbcmd[cmd1]) {
			case 'b':
			case 'B': {
				// Report the status of the MAESTRO beacons.
				// Format: r 124 B
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				view_maestro_beacons();
				break;
			}

			case 'c':
			case 'C': {
				// Check the CRC of the current program, which may be in
				// flash or ROM.  If you append a program number, it will
				// be used instead of the current program number.
				//
				// Format: r 124 C
				// Format: r 124 C (program)
				if (cmd_length == cmd1) {
					check_current_maestro_program_crc();
				} else if (cmd_length == cmd2) {
					check_maestro_program_crc(rbcmd[cmd2]);
				} else {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 124 C [(program)] ...\r\n");
				}
				break;
			}

			case 'd':
			case 'D': {
				// Report the status of the MAESTRO program timeout
				// Format: r 124 D
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_program_timeout();
				break;
			}

			case 'e':
			case 'E': {
				// Report the status of the MAESTRO program timeout enabler
				// Format: r 124 E
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_enable_program_timeout();
				break;
			}

			case 'f':
			case 'F': {
				// Show the show-program-fetches flag.
				// Format: r 124 F
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_show_program_fetches();
				break;
			}

			case 'g':
			case 'G': {
				// Report the status of the MAESTRO program start time
				// Format: r 124 G
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_program_start_time();
				break;
			}

			case 'h':
			case 'H': {
				// Report the status of the MAESTRO program timeout overrider
				// Format: r 124 H
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_override_program_timeout();
				break;
			}

			case 'i':
			case 'I': {
				// List fascinating information about the current program,
				// which may be in flash or ROM.  If you append a program
				// number, it will be used instead of the current program
				// number.
				//
				// Format: r 124 I
				// Format: r 124 I (program)
				if (cmd_length == cmd1) {
					list_current_maestro_program_info();
				} else if (cmd_length == cmd2) {
					list_maestro_program_info(rbcmd[cmd2]);
				} else {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 124 I [(program)] ...\r\n");
				}
				break;
			}

			case 'j':
			case 'J': {
				// List fascinating information about the test programs.
				// Format: r 124 J
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				list_maestro_programs_full();
				break;
			}

			case 'l':
			case 'L': {
				// Briefly list the test names of the available MAESTRO test programs.
				// Format: r 124 l
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				list_maestro_programs_brief();
				break;
			}

			case 'm':
			case 'M': {
				// Show the MAESTRO shout severity sendDBGALL filter level.
				// Format: r 124 M
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_shout_severity_senddbgall_filter();
				break;
			}

			case 'n':
			case 'N': {
				// Report the number of test programs available:
				// Format: r 124 N
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				report_number_of_test_programs();
				break;
			}

			case 'o':
			case 'O': {
				// Show the MAESTRO program offset.
				// Format: r 124 O
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_program_offset();
				break;
			}

			case 'p':
			case 'P': {
				// Show the test program number.
				// Format: r 124 P
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_program_number();
				break;
			}

			case 'q':
			case 'Q': {
				// Show the MAESTRO shout severity packetize and queue filter level.
				// Format: r 124 Q
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_shout_severity_packetize_filter();
				break;
			}

			case 'r':
			case 'R': {
				// Show the show-received-chars flag.
				// Format: r 124 R
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_show_received_chars();
				break;
			}

			case 's':
			case 'S': {
				// Report the status of the MAESTRO bootstrapper.
				// Format: r 124 S
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				report_maestro_status();
				break;
			}

			case 't':
			case 'T': {
				// Show the MAESTRO trace flag.
				// Format: r 124 T
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_trace();
				break;
			}

			case 'u':
			case 'U': {
				// Report the MAESTRO UART I/O counters, if available.
				// Format: r 124 U
				//
				// Note: These counters will be reset whenever the
				// MAESTRO is hard reset.
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				view_uart_io_counters();
				break;
			}

			case 'v':
			case 'V': {
				// Print the program bytes starting at the current
				// program offset, without changing the program offset.
				// Format: r 124 V
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				examine_maestro_program(FALSE);
				break;
			}

			case 'w':
			case 'W': {
				// Show the value of a wreg.
				// Format:  r 124 W (reg)
				// Example: r 124 W 1
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 124 W (reg)\r\n");
					break;
				}
				show_maestro_wreg((int)rbcmd[cmd2]);
				break;
			}

			case 'x':
			case 'X': {
				// Print some program bytes starting at the current
				// program offset, auto-incrementing the program offset.
				// Format: r 124 X
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				examine_maestro_program(TRUE);
				break;
			}

			case 'y':
			case 'Y': {
				// Show the MAESTRO yield limit.
				// Format: r 124 Y
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_yield_limit();
				break;
			}

			case 'z':
			case 'Z': {
				// Show the MAESTRO system power level and thresholds.
				// Format: r 124 Z
				if (cmd_length != cmd1) {
					goto wrong_length_r_124;
				}
				show_maestro_system_power();
				break;
			}

			default:
				sendDBGALL(USER_PORT,
						   "\r\nUnknown MAESTRO report command action: R 124 (action)\r\n");
			}
			break;

		wrong_length_r_124 : {
			sendDBGALL(USER_PORT, "\r\nWrong command length: r 124 (action)\r\n");
			break;
		}
		}

		case 125: {
#ifdef MAESTRO_PROGRAMS_IN_FLASH
			// Manage MAESTRO programs in flash.
			switch (rbcmd[cmd1]) {
			case 'b':
			case 'B': {
				// Set the flash execution base offset.
				// Format: r 125 B (6-digit hex offset)
				//
				// Note:  This value is reset to an invalid value in a hard initialization.
				if (cmd_length != cmd4) {
					sendDBGALL(USER_PORT,
							   "\r\nERROR: Wrong command length: r 125 B (6-digit hex offset)\r\n");
					break;
				}
				unsigned long offset;
				offset = make32(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				set_maestro_flash_execution_base(offset);
				break;
			}

			case 'c':
			case 'C': {
				// Clear part of the flash program area.
				// Format: r 125 C (6-digit hex offset) (6-digit hex size)
				// For safety, this code insists that the region to be cleared
				// must align with flash block boundaries.
				if (cmd_length != cmd7) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 125 C (6-digit hex offset) "
										  "(6-digit hex size)\r\n");
					break;
				}
				unsigned long offset;
				offset = make32(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				unsigned long erase_len;
				erase_len = make32(rbcmd[cmd5], rbcmd[cmd6], rbcmd[cmd7]);
				clear_maestro_flash(offset, erase_len);
				break;
			}

#ifdef MAESTRO_SFFS
			case 'd':
			case 'D': {
				// Delete the specified program from the flash directory (sffs).
				// Format: r 125 D (program number)
				// Example: r 125 D 11
				if (cmd_length == cmd2) {
					delete_maestro_program(rbcmd[cmd2]);
				} else {
					sendDBGALL(USER_PORT,
							   "\r\nERROR: Wrong command length: r 125 D (program number)\r\n");
					break;
				}
				break;
			}

			case 'f':
			case 'F': {
				// Show the free space from the flash directory (sffs).
				// Format: r 125 F
				// Example: r 125 F
				if (cmd_length != cmd1) {
					delete_maestro_program(rbcmd[cmd2]);
					sendDBGALL(USER_PORT, "\r\nERROR: Wrong command length: r 125 F\r\n");
					break;
				}
				report_maestro_sffs_free_space();
				break;
			}
#endif // MAESTRO_SFFS

			case 'h':
			case 'H': {
				// Load new hex file
				// Loads a new hex file into flash.  You have 30 seconds to begin
				// the transfer, and 2 seconds after each line to continue sending.
				// Format: r 254 H (6-digit hex offset) (carriage return) (File contents...)
				// Example:  r 254 H *000100* (CR) (Hex File bytes)
				if (cmd_length != cmd4) {
					sendDBGALL(USER_PORT,
							   "\r\NERROR: Wrong command length: r 125 H (6-digit hex offset)\r\n");
					break;
				}
				unsigned long offset;
				offset = make32(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				sprintf(dbgbuf, "\r\nStarting hex upload to MAESTRO flash at offset 0x%Lx\r\n",
						offset);
				sendDBGALL(USER_PORT, dbgbuf);
				ReturnErr_t result;
				result = uploadHexFile(port, port, offset, PAYLOAD_ADDR, PAYLOAD_ALLOC_SIZE);
				if (result != SUCCESS) {
					sprintf(dbgbuf, "\r\nFAILURE DETECTED ON FILE UPLOAD! err=%d\r\n", result);
					sendDBGALL(USER_PORT, dbgbuf);
				} else {
					sendDBGALL(USER_PORT, "\r\nHex upload complete.\r\n");
				}
				break;
			}

			case 'l':
			case 'L': {
				// Load part of the flash program area.
				// Format: r 125 L (6-digit hex offset) (data)
				if (cmd_length != cmd4) {
					sendDBGALL(
						USER_PORT,
						"\r\nERROR: Wrong command length: r 125 L (6-digit hex offset) (data)\r\n");
					break;
				}
				unsigned long offset;
				offset = make32(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				sprintf(dbgbuf, "\r\nLoading MAESTRO flash at offset 0x%Lx\r\n", offset);
				sendDBGALL(USER_PORT, dbgbuf);
				int data_length;
				data_length = cmd_length - cmd4;
				ReturnErr_t result;
				result = flashWriteSafe(PAYLOAD_ADDR + offset, data_length, &(rbcmd[cmd5]),
										PAYLOAD_ADDR, PAYLOAD_ADDR + PAYLOAD_ALLOC_SIZE - 1);
				if (result == SUCCESS) {
					sendDBGALL(USER_PORT, "\r\nOK.\r\n");
				} else {
					sprintf(dbgbuf, "\r\nERROR: PAYLOAD data load failed with error %d.\r\n",
							result);
					sendDBGALL(USER_PORT, dbgbuf);
				}

				break;
			}

			case 'p':
			case 'P': {
				// Copy the current program (ROM or flash) into the
				// flash program area (without sffs).
				// Format: r 125 P (6-digit hex offset)
				if (cmd_length != cmd4) {
					sendDBGALL(USER_PORT,
							   "\r\nERROR: Wrong command length: r 125 P (6-digit hex offset)\r\n");
					break;
				}
				unsigned long offset;
				offset = make32(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				copy_current_maestro_program_to_flash(offset);
				break;
			}

			case 'w':
			case 'W': {
				// Wipe the entire flash program area.  For safety, the
				// command has a peculiar syntax.
				// Format: r 125 W W W W W W
				if (cmd_length != cmd6) {
					sendDBGALL(USER_PORT, "\r\nWrong command length: r 125 W W W W W W\r\n");
					break;
				}
				unsigned long offset;
				offset = 0;
				unsigned long erase_len;
				erase_len = PAYLOAD_ALLOC_SIZE;
				clear_maestro_flash(offset, erase_len);
				break;
			}

#ifdef MAESTRO_SFFS
			case 'x':
			case 'X': {
				// Copy the current program or specified (ROM or flash) into the
				// flash program area (with sffs).
				// Format: r 125 X [(program number)]
				// Example: r 125 X
				// Example: r 125 X 3
				if (cmd_length == cmd1) {
					maestro_sffs_copy_current_program_to_flash();
				} else if (cmd_length == cmd2) {
					maestro_sffs_copy_program_to_flash(rbcmd[cmd2]);
				} else {
					sendDBGALL(USER_PORT,
							   "\r\nERROR: Wrong command length: r 125 X [(program number)]\r\n");
					break;
				}
				break;
			}
#endif // MAESTRO_SFFS

				// TODO: I wanted to use checkHexFile(...), but it appears to be
				// too specific to the format of the data in flash.

			default:
				sendDBGALL(USER_PORT,
						   "\r\nUnknown MAESTRO flash program action: r 125 (action)\r\n");
			}

			break;
#else // MAESTRO_PROGRAMS_IN_FLASH
			sendDBGALL(USER_PORT, "\r\nMAESTRO flash is not supported\r\n");
#endif // MAESTRO_PROGRAMS_IN_FLASH
		}

#endif // __MAESTRO__

		case 126: // Create a telemetry packet, broadcast and/or store it:
				  // Format: r 126 (packet type) (beacon/storage) (2-byte list of elements)
				  // (packet type) -- 0 for MISC.  2 for Plymouth(not used).  Other numbers for
				  // other packet types as defined in the MySql database. (beacon/storage) -- 0 for
				  // storage only. 2 for beacon only. 1 for both beacon and storage. For each of the
				  // 2-byte elements, it will be an int in standard form (1st byte) is multiples of
				  // 256 (2nd byte) is LSB, and adds to the first one. Example:  To represent 255 --
				  // 0 and 255 (0 units of 256 and 255 units of 1)
				  ///          To represent 257 -- 1 and 1  (1 unit of 256 and 1 unit of 1)
				  // You may also use the windows calculator (or any other convenient program) to
				  // convert the number into a 4-digit hex number, and enter that into the command
				  // using the stars (*) for HexMode.

			// Examples:
			// r 126 0 0 0 150 			// Creates a MISC_packet of element 150
			// r 126 1 0 0 100 2 0 0 250  // Creates a TLM_packet of elements (100, 512, and 250)
			// r 126 1 0 *012c* 			// Creates a packet of element 300
			// r 126 1 0 *012c012d012e012f0130013101320133* // Creates a packet of elements 300-307
			// r 126 1 0 0 9 0 10 0 11 0 12 0 13 0 14 // Create a packet of 6 orbit data elements (3
			// pos eci, 3 vel eci) r 126 1 1 *0157* *0158* *0159* *015A* *015B* *015C* *015D* //
			// Elements 343 through 349 f 126 13 2 *0151* *0152* *0153* r 126 1 1 *0151* *0152*
			// *0153* // Beacon the gyro Data (elements 337,338,339 commanded from lower PPM). f 126
			// 1 1 *01C2* f 126 13 2 *01BB* *01BC*

			// A note on packet types:
			// Packet types 0,1,and 2 are special.  0 for MISC, 1 for TLM, and 2 for Plymouth (no
			// longer used, so avoid it). Other packet numbers are used to match a packet mapping in
			// the ground software and MySQL database.
			//

			// Examples for Beacon vs. Storage:
			// r 126 1 0 *012c* 			// Creates a packet of element 300 and stores it.
			// r 126 1 2 *012c* 			// Creates a packet of element 300 and beacons it.
			// r 126 1 1 *012c* 			// Creates a packet of element 300 and stores it and
			// beacons it. r 126 2 2 *0157* *0158* *0159* *015A* *015B* *015C* *015D* // Elements
			// 343 through 349 and beacon!
			{
				// sendDBGALL(USER_PORT,"\r\nBuilding telemetry packet.");

				// Note: command ("126") is the 10th byte.  So, the packet type
				// is the 11th byte (cmd1), the beacon/storage switch is 12th byte (cmd2)
				//	and the element indicies begin at the 13th byte (cmd3).

				// Print out the telemetry
				// printTelemetryElements(cmd_length,rbcmd,cmd3);

				// Try to build a telemetry data field using Lbuf (maximum size is 6 less than
				// buffer because we may need those below to pass along to the upper ppm.
				wrPtr = buildTelemtryDataField(cmd_length, rbcmd, cmd3, ProcessCMDBuf,
											   GENERAL_BUFFER_SIZE - 6);

				// Debug
				// sendDBGALL(USER_PORT,"\r\nTLM Packet: ");
				// HexPrint(USER_PORT,ProcessCMDBuf,wrPtr);

				// memcpy(ProcessCMDBuf,Lbuf,GENERAL_BUFFER_SIZE-6);

				// If we succeeded, send it out!
				if (0 != wrPtr) {
// If we're on the upper PPM
#if defined(UPPER_PPM)
					// DEBUG--------------
					/*	sendDBGALL(USER_PORT,"\r\n");
						HexPrint(USER_PORT,&wrPtr,sizeof(wrPtr));
						sendDBGALL(USER_PORT,"\r\n");
						HexPrint(USER_PORT,Lbuf,wrPtr);
						sendDBGALL(USER_PORT,"\r\n");
						HexPrint(USER_PORT,rbcmd,cmd_length);
						sprintf(dbgbuf,"\r\nSending Pkt Type %i...",rbcmd[cmd1]);
						sendDBGALL(USER_PORT,dbgbuf);
					*/	// ----------------------

					// Store the telemetry (by placing on to the beacon stack, which will store it
					// and send it over MHX later)
					if (rbcmd[cmd2] == STORE_ONLY || rbcmd[cmd2] == STORE_AND_BEACON)
						// Parameters are: data,length,stack,max length, packet type, page address
						PacketizeAndPush(ProcessCMDBuf, wrPtr, BEA, GENERAL_BUFFER_SIZE,
										 rbcmd[cmd1], PPM_NUM);
					else {
						// sendDBGALL(USER_PORT,"\r\nNo telemetry storage requested.");
					}

					// Test: Send out telemetry raw:
					// sendMSG(MHX_PORT,Lbuf,wrPtr);

#ifdef __BEACON__
					// Beacon the telemetry
					if (rbcmd[cmd2] == BEACON_ONLY || rbcmd[cmd2] == STORE_AND_BEACON) {
						if (!globals.BeaconOnFlag) {
							sendDBGALL(USER_PORT, "\r\nBeacon disabled.  Beacon skipped.");
						} else {
							// Reset the data
							wrPtr = buildTelemtryDataField(cmd_length, rbcmd, cmd3, ProcessCMDBuf,
														   GENERAL_BUFFER_SIZE - 6);

							// Packetize the data (Note: wrPtr gets updated with complte length
							getPACKETIZED(ProcessCMDBuf, &wrPtr, rbcmd[cmd1], PPM_NUM,
										  GENERAL_BUFFER_SIZE);

							// Send out packetized data
							transmitData(&gLinkManager, ProcessCMDBuf, wrPtr, HEALTH_AND_STATUS, 0);

							sendDBGALL(USER_PORT, "\r\nBeacon sent.");
						}
					} else {
						sendDBGALL(USER_PORT, "\r\nNo telemetry beaconing requested.");
					}
#endif // __BEACON__

#elif defined(LOWER_PPM)
					// If we're on the lower PPM, send to the upper PPM with a command to packetize,
					// and also send with a command to beacon.

					// Store the telemetry (by sending to upper with packetize and push command)
					if (rbcmd[cmd2] == STORE_ONLY || rbcmd[cmd2] == STORE_AND_BEACON) {
						// Copy data to local buffer 6 places in (check for overflow!)
						if (wrPtr + 6 > GENERAL_BUFFER_SIZE) {
							sendDBGALL(USER_PORT, "\r\nToo big!");
							break;
						}

						memmove(&ProcessCMDBuf[6], ProcessCMDBuf, wrPtr);

						// Assign command and arguments
						ProcessCMDBuf[0] = 133; // Command to packetize and push
						ProcessCMDBuf[1] = 0; // Stack Number
						ProcessCMDBuf[2] = rbcmd[cmd1]; // Packet Type Number
						ProcessCMDBuf[3] = 0; // MSB of PPM Number
						ProcessCMDBuf[4] = PPM_NUM; // LSB of PPM Number
						ProcessCMDBuf[5] = wrPtr; // Data Length

						// Debug
						// sendDBGALL(USER_PORT,"\r\nSending: ");
						// HexPrint(USER_PORT,ProcessCMDBuf,wrPtr+6);

						// Send it to the other PPM (Length is data length plus six for above cmd
						// and arguments)
						sendPPM(OTHER_PPM_PORT, ProcessCMDBuf, wrPtr + 6);
					} else {
						// sendDBGALL(USER_PORT,"\r\nNo telemetry storage requested.");
					}

					// Next: Beaconing
					if (rbcmd[cmd2] == BEACON_ONLY || rbcmd[cmd2] == STORE_AND_BEACON) {
						// Packetize the data (Note: wrPtr gets updated with complete length
						getPACKETIZED(ProcessCMDBuf, &wrPtr, rbcmd[cmd1], PPM_NUM,
									  GENERAL_BUFFER_SIZE);

						// Move the data to the right by 3 spaces to make room for command
						memmove(&ProcessCMDBuf[3], ProcessCMDBuf, wrPtr);

						// Send out packetized data by using the Data command (127)
						// Assign command and arguments
						ProcessCMDBuf[0] = 127; // Command to packetize and push
						ProcessCMDBuf[1] = make8(wrPtr, 1); // Upper byte of size
						ProcessCMDBuf[2] = make8(wrPtr, 0); // Lower byte of size

						// Send it to the other PPM (Length is data length plus three for above cmd
						// and arguments)
						sendPPM(OTHER_PPM_PORT, ProcessCMDBuf, wrPtr + 3);
					}
// sendDBGALL(USER_PORT,"\r\nSent telemetry to other PPM: ");
// HexPrint(USER_PORT,Lbuf,wrPtr+6);
#endif // defined(LOWER_PPM)
				} else {
					sendDBGALL(USER_PORT, "\r\nTelemetry Build Failed.");
				}

				break;
			}

		case 127: // Beacon Data
			// Format: r 127 (size of beacon data -- 2 bytes) (beacon data)
			//
			{
#ifdef UPPER_PPM
				if (!globals.BeaconOnFlag) {
					sendDBGALL(USER_PORT, "\r\nBeacon disabled.  Beacon skipped.");
					break;
				}
				wrPtr = make16(rbcmd[cmd1], rbcmd[cmd2]);

				transmitData(&gLinkManager, (char*)&rbcmd[cmd3], wrPtr, HEALTH_AND_STATUS, 0);

				sendDBGALL(USER_PORT, "\r\nBeacon sent.");
#endif
			}
			break;

		case 128: // iTag Functional Checkout
#ifdef __ITAG_H__
			if (itag_functional_check(ITAG_PORT, 5) == SUCCESS)
				sendDBGALL(USER_PORT, "\r\niTag Command Success.");
			else
				sendDBGALL(USER_PORT, "\r\niTag Command Fail.");
#endif
			break;

		case 129: // Ping iTag (it will respond with ACK)
		{
#ifdef __ITAG_H__
			itag_ping(ITAG_PORT);
#endif
			break;
		}

		case 130: // Load a float into the global ExtIn[] array
				  // BINARY VERSION: 0x78 (1-byte index) (4-byte float)
				  // Example: r 130 32 *04fe*
#ifdef __AENEAS__
			c = rbcmd[cmd1];
			memcpy(&ExtIn[c], &rbcmd[cmd2], sizeof(float));
//			printFloat(dbgbuf,ExtIn[rbcmd[cmd1]],6);
//			locateString(USER_PORT,rbcmd[cmd1],40,dbgbuf); // Prints to screen
#endif
			break;

		case 131: // Forward the received command to the other processor.
				  // Eg: r 131 151 // Forwards (131) the reset command (151) to the other processer.
			memcpy(&rbcmd[0], &rbcmd[cmd1], cmd_length - cmdBaseAddr);
			// sendDBGALL(USER_PORT,"\r\nCommand: ");
			// HexPrint(USER_PORT,rbcmd,(unsigned char)(cmd_length-cmdBaseAddr));
			sendPPM(OTHER_PPM_PORT, rbcmd, (unsigned char)(cmd_length - cmdBaseAddr));
			break;

		case 132: // ECHO COMMAND -- connect ports
				  // Eg: r 132 (port to connect) (port to connect) (port to listen for terminator)
				  // Eg: r 132 1 2 1
			echoPorts((int)rbcmd[cmd1], (int)rbcmd[cmd2], (int)rbcmd[cmd3]);
			break;

		case 133: // Packetize and push (only if stacks enabled)
#ifdef __MYSTACK__
				  // Eg: r 133 (stack number) (pkt type) (reserved space byte 1) (reserved space
				  // byte 2) (len) (data) Eh           cmd1           cmd2            cmd3 cmd4 cmd5
				  // cmd6
			int temp_;
			ReturnErr_t response;
			temp_ = make16(rbcmd[cmd3], rbcmd[cmd4]);

#ifdef UPPER_PPM
			response = PacketizeAndPush(&rbcmd[cmd6], (int)rbcmd[cmd5], BEA,
										GENERAL_BUFFER_SIZE - cmd6, (int)rbcmd[cmd2], temp_);
#else
			response = PacketizeAndPush(&rbcmd[cmd6], (int)rbcmd[cmd5], MISC,
										GENERAL_BUFFER_SIZE - cmd6, (int)rbcmd[cmd2], temp_);
#endif

			if (response == SUCCESS) {
				sprintf(dbgbuf, "\r\nPacket from PPM#%d:", PPM_NUM);
				sendDBGALL(USER_PORT, dbgbuf);
			} else
				PrintDbgErr(USER_PORT, response);
#else
			sendDBGALL(USER_PORT, "\r\nStacks not enabled.");
#endif
			break;

		case 134: // Just push onto a stack
#ifdef __MYSTACK__
				  // Eg: r 134 (stack number) (len) (data)
				  // Eg:          cmd1         cmd2  cmd3
			// HexPrint(USER_PORT,&rbcmd[cmd3],rbcmd[cmd2]);
			stack_push(MISC, &rbcmd[cmd3], rbcmd[cmd2]);
#endif
			break;

		case 135: // ProcessCmd Debugging
			HexPrint(USER_PORT, rbcmd,
					 cmd_length + 1); // Prints to the screen the received command.
			break;

		case 136: // Print Comma Seperated Value (CVS) Telemetry
			// printTelemetryElements(cmd_length,rbcmd,cmd3);
			break;

		case 137: // IMI-100 Communications Power Control
#ifdef __IMI100_H__
			if (rbcmd[cmd1]) {
				sendDBGALL(USER_PORT, "\r\nIMI-100 comm on.");
				imi100_init(IMI_PORT, 1);
			} else {
				imi100_init(IMI_PORT, 0);
				sendDBGALL(USER_PORT, "\r\nIMI-100 comm off.");
			}
#endif
			break;
		// Toggle payload power
		case 140: {
#ifdef UPPER_PPM
			processPayloadPowerCmd(&rbcmd[cmd1]);
#endif
			break;
		}
		// Forward data to payload
		case 141: {
#ifdef UPPER_PPM
			processForwardPayloadCommandCmd(&rbcmd[cmd1], cmd_length - cmdBaseAddr);
#endif
			break;
		}
		case 142:
#ifdef UPPER_PPM
			processResetPayloadDataCmd(&rbcmd[cmd1]);
#endif
			break;
		case 143:
#ifdef UPPER_PPM
#ifdef DEBUG
			char dm[100] = {0};
			sprintf(dm, "\r\nNum packets received: %u, Num packets sent: %u",
					gPayloadManager.numPayloadPacketsStored, gPayloadManager.numPayloadPacketsSent);
			sendDBGALL(USER_PORT, dm);
#endif
#endif
			break;
		case 144: {
#ifdef UPPER_PPM
			processResendPayloadDataPacketsCmd(&rbcmd[cmd1], cmd_length - cmdBaseAddr);
#endif
			break;
		}
		case 149: // Cycle the Master Clear Pin on the Other Process
			// format: r 149 (0/1)
			// (0/1) -- Choose whether (1) or not (0) to float the line after cycling the pin.
			output_low(RESET_PPM); // Reset the other processor
			delay_ms(100); // wait
			output_high(RESET_PPM); // Bring it out of reset
			if (rbcmd[cmd1])
				output_float(RESET_PPM);
			break;

		case 150:
#ifdef UPPER_PPM
			processRadioModeCmd(&rbcmd[cmd1]);
#endif
			break;

		case 151: //== Perform Reset via software cpu restart
#ifdef __MHX__
			if (ack)
				sendACK(port, I_ACK, RX_Seq_Num++, Command_ID, source, retVal,
						encrypt); // Send an ack before reset
#endif
			delay_ms(500);
			reset_cpu();
			break;

		case 152: // Flash Testing Functions
#ifdef __FLASH_DRIVER__
				  // sendDBGALL(USER_PORT,"\r\nRunning 152...");
			unsigned char temp[8];
			unsigned int fblock;
			memcpy(temp, 0xff, 8);
			switch (rbcmd[cmd1]) {
			case 1:
				// Check the version of flash and prints it to the userport
				// Inputs are 'v' -- version
				// 0x10000 (address, unused)
				// 0 -- length, unused
				// dbgbuf -- the buffer to fill with version info

				flashVersion(&temp[0], &temp[1]);
				sprintf(dbgbuf, "\r\nManufacturer: 0x%2x\r\nDevice ID: 0x%2x", temp[0], temp[1]);
				sendDBGALL(port, dbgbuf);
				break;

			case 2:
				// Checks the status bytes of the flash
				flashStatus(temp);
				HexPrint(port, temp + 1, 2);
				statusReport(port, temp[1], temp[2]);
				break;

			case 3: // Toggle the write protect pin.  This should be readable in the status.
				// TODO: Replace with function.
				output_toggle(FLASH_WRITE_PROTECT);
				break;

			case 4: // Write enable the flash device
				flashWriteEnable();
				break;

			case 5: // Write disable the flash device
				flashWriteDisable();
				break;

			case 6: // Perform a reading test in flash
				flashReadTest(CharsToFlashAddr(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]), port);
				break;

			case 7: // Disables sector protection on a specific sector (Eg: 0x010000 or 0x020000 or
					// 0x7E0000)
				addr = CharsToFlashAddr(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				flashSectorProtectDisable(addr);
				break;

			case 8: // Checks if a certain section in flash is empty
				// Inputs to CheckFlashEmpty are (address byte 1, 2, 3, length to check);
				unsigned char test;
				test = CheckFlashEmpty(CharsToFlashAddr(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]),
									   (unsigned int)rbcmd[cmd5]);
				sprintf(dbgbuf, "\r\nEmpty=%i", test);
				sendDBGALL(port, dbgbuf);
				break;

			case 9: // Erases the chosen block
				// Format: r 152 9 (2-byte block number)
				//
				// TODO: Test erasing blocks past 255
				if (cmd_length != cmd2) {
					sendDBGALL(USER_PORT, "Incorrect Size");
					break; // bail for incorrect command size
				}
				fblock = make16(rbcmd[cmd2], rbcmd[cmd3]);
				flashEraseBlockByNumber(fblock);
				break;

			case 10: // Checks Sector Protection on a specific sector
				test = flashReadSectorProtection(
					CharsToFlashAddr(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]));
				sprintf(dbgbuf, "\r\nSector=%01u", test);
				sendDBGALL(port, dbgbuf);
				break;

			case 11: // Single Write Test.  Write's a 0x00 at the chosen location.
				memcpy(temp, 0x00, 8);
				flashWrite(CharsToFlashAddr(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]), 1, temp);
				break;

			case 12: // Reset Reboot Counter
				ResetRebootCounter();
				break;

			case 13: // ERASE THE WHOLE CHIP
#ifdef __MHX__
				if (ack)
					sendACK(port, I_ACK, RX_Seq_Num++, Command_ID, source, retVal,
							encrypt); // Send an ack first
				ack = 0;
#endif
				sendDBGALL(port, "\r\nErasing Flash.  This can take up to 2 minutes.");
				flashEraseChip(); // Tihs function can take up to 2 minutes to complete.
				sendDBGALL(port, "\r\nDone.");
				break;

			case 14: // Display Memory Map
#ifdef __MHX__
				if (ack)
					sendACK(port, I_ACK, RX_Seq_Num++, Command_ID, source, retVal,
							encrypt); // Send an ack first
				ack = 0;
#endif
				flashPrintOccupiedBlocks(rbcmd[cmd2],
										 rbcmd[cmd3]); // flashPrintOccupiedBlocks(port,ascii)
				break;

			case 15: // Manually Set Telemetry Write Address
				TelemWriteAddr = CharsToFlashAddr(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				break;

			case 16: // Manually Set Plymouth Write Address
				FlashWriteAddr = CharsToFlashAddr(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4]);
				break;

			case 17: // Send Write Addresses to a port
					 // 152 17 (port)
				sprintf(dbgbuf, "\r\nTelem:%Lx ", TelemWriteAddr);
				sendDBGALL(rbcmd[cmd2], dbgbuf);
				sprintf(dbgbuf, "\r\nPly:%Lx ", FlashWriteAddr);
				sendDBGALL(rbcmd[cmd2], dbgbuf);
				sprintf(dbgbuf, "\r\nMisc:%Lx ", MiscWriteAddr);
				sendDBGALL(rbcmd[cmd2], dbgbuf);
				break;

			case 18: // Forces a start or stop of transmitting data
#ifdef __TXMANAGE__
				TX_data.NewData = rbcmd[cmd2];
				BEA_data.NewData = rbcmd[cmd2];
				MISC_data.NewData = rbcmd[cmd2];
#endif
				break;

			case 19: // Unprotects all sectors in flash.
				flashGlobalUnprotect();
				break;

			case 20:
				flashPrintSectorProtection(rbcmd[cmd2]);
				break;

			case 21: // Generic flash command?  [UNTESTED] TODO: Create generic flash command.
					 // NOTE THAT COMMAND 33 IS A GENERIC READ/WRITE COMMAND FOR FLASH!
					 // Format:  r 152 21 (length of read) (length of write) (cmd bytes)
					 // Example: r 152 21 128 4 *03000000* // Writes the command 0x03000000 to flash
					 // and reads 128 bytes of response.
			{
				unsigned int len;

				output_low(FLASH_CHIP_SELECT); // enable flash - lower chip select
				SPIWrRd(&rbcmd[cmd4], rbcmd[cmd3], dbgbuf,
						rbcmd[cmd2]); // Execute the command and read the response
				output_high(FLASH_CHIP_SELECT); // disable flash - raise chip select
#ifdef __MYSTACK__
				PacketizeAndPush(dbgbuf, len, MISC, DBG_BUF_SIZE, MISC_PACKET,
								 PPM_NUM); // Send the results to the ground.
#endif
			} break;

			case 22:
				break;

			case 23: // Sets the swap block.  Can use any of the last 16 blocks in flash
#ifdef __FLASH_DRIVER__
					 // Format: r 152 23 (int MSB) (int LSB)
				// Example: r 152 23 7 253 // Sets swap block to 2045.  (7*256 + 253)
				FLASH_SWAP_BLOCK = C2I(&rbcmd[cmd2]);
				// Make sure the flash block is within a range of 2032-->2047
				if (FLASH_SWAP_BLOCK < 2032 || FLASH_SWAP_BLOCK > 2047)
					FLASH_SWAP_BLOCK = 2047; // Default to 2047
#endif
				break;

			default:
				sendDBGALL(USER_PORT, "\r\nInvalid Command.");
				break;
			} // inner switch
#endif // __FLASH_DRIVER__
			break;

		case 153: // Schedule a command using DELTA time
			// Format: r 153 <Schedule #> <Delta Time (7 bytes)> <command>
			// Example: r 153 0 0 0 0 0 0 0 60 155   // Schedule Command 155 10 seconds from
			// whatever the current time is Example: r 153 0 1 0 255 0 0 0 155    // Schedules
			// Command 155 to run exactly 1 day from now. (The weekday is ignored in schedule
			// matching, so the number there doesn't matter) Example: r 153 255 255 255 255 1 0 0
			// 155 // Schedules Command 155 to runn 1 hour from now, and every day on that hour
			// afterwards. Note: Negative numbers are not allowed
			cmd_length -= 10; // Remove address,cmdID and index
			c = rbcmd[cmd1];
			if (cmd_length > 7 &&
				cmd_length < MAX_STORED_CMD_SIZE -
								 2) // command size validation.  Too big or too small, and we ignore
									// it.  Saving 2 spots off the max size for CRC
			{
				globals.StoredCmds[c][0] = cmd_length;
				// check for wildcards
				globals.StoredCmds[c][1] = 0;
				for (i = (cmd2 + 1); i < (cmd2 + 1 + cmd_length); i++) {
					if (rbcmd[i] == 0xff) {
						globals.StoredCmds[c][1] = 1;
						// sendDBGALL(USER_PORT, "\r\nDetected a wildcard!!!\r\n");
						break;
					}
				}
				// Adjust the time.
				unsigned char temptime[7];
				DeltaTime(time, rbcmd + cmd2, temptime);
				// Store the time
				memcpy(rbcmd + cmd2, temptime, 7);
				// Store the command
				memcpy(&globals.StoredCmds[c][2], rbcmd + cmd2, cmd_length);
				getCRC(
					&globals.StoredCmds[c][2], cmd_length,
					&globals.StoredCmds[c][cmd_length + 2]); // Compute CRC and save with commands
			} else
				globals.StoredCmds[c][0] = 0; // Clears command

			sendDBGALL(USER_PORT, "\r\nSchedule Stored.");

			break;

		case 154: // Synchronize time on the other PPM using your time
// Sync time
#ifdef LOWER_PPM
			sprintf(Lbuf, "10 %d %d %d %d %d %d %d", time[0], time[1], time[2], time[3], time[4],
					time[5], time[6]);
			sendPPMCmd(OTHER_PPM_PORT, Lbuf);
#endif
			break;

		case 155: // Get CRC of message
			getCRC(&rbcmd[cmd2], rbcmd[cmd1], ProcessCMDBuf);
			HexPrint(USER_PORT, ProcessCMDBuf, 2);
			break;

		case 156: // Configure the Beacon to default settings
#ifdef __BEACON__
			configureBeacon();
#endif
			break;

		case 157: // Send data out to the iTag in a packet
#ifdef __ITAG_H__
			sendVIAiTag(ITAG_PORT, rbcmd + cmd2, rbcmd[cmd1], time);
#endif
			break;

		case 158: // Forward a message to a port, packetizing if necessary.
// Example:  r 158 (port) (length of message) (msg...)
// 			 r 158 1 3 "ATA"
//			 r 158 0 11 "Hello World"
#ifdef __MHX__
			if (rbcmd[cmd1] == MHX_PORT)
				sendPACKETIZED(MHX_PORT, rbcmd + cmd3, rbcmd[cmd2],
							   0); // Send it packetized to MHX if that's the request
			else
#endif
				sendMSG(rbcmd[cmd1], rbcmd + cmd3,
						rbcmd[cmd2]); // Otherwise send it to whatever port
			break;

		case 159: // Adjust Beacon Callsign
// Format:  r 159 "ABC123" // new callsign of ABC123
// Example: r 159 "KJ6FIX"
// Note: You must also call configureBeacon() to reconfigure
#ifdef __BEACON__
			memcpy(globals.BeaconCallsign, &rbcmd[cmd1], 6);
#endif
			break;

		case 160: // Inspect memory (RAM DUMP)
			// Command format:
			// r 160 (memory location) (length)
			// For PIC24, memory locations are 2 bytes.  Length is also 2 bytes.
			// Range from 0000 to 4800
			// MultiPrint(port,C2I(rbcmd+cmd1),C2I(rbcmd+cmd3),rbcmd[cmd5]);
			break;

		case 161: // Change Debug Level
			DEBUG_LEVEL = rbcmd[cmd1];
			break;

		case 162: // Deprecated: Beacon with override
			break;

		case 163: // Set Delta Time
			// Example: r 163 (time to add)
			// Example: r 163 0 0 0 0 2 1 10 // Adds 2 hours, 1 minute, 10 seconds to clock
			if (cmd_length == 16) // A valid command is exactly 16 bytes long.  Why?
			// Command: Length   MSG Source/Dest   Time
			// Length:     1     +      8      +     7    =    16
			{
				// Test Adjusting the time.
				HexPrint(USER_PORT, rbcmd + cmd2, 7);
				DeltaTime(time, rbcmd + cmd2, temptime);
				// Store the time
				if (I2C_Clock_OK) // If I2c is working, use that
				{
#ifdef __I2C__
					setI2CTime(temptime); // Sets time
#endif
				} else { // Otherwise, use the RTC
					setTime(temptime); // Set the RTC
				}
			}
			break;

		case 164:
			break;

		case 165: // Various Data Displays
			// Format: 165 (what to dispalay)
			// Example: 165 1 // Sends reboot counter to whatever port called processcmd
			switch (rbcmd[cmd1]) {
			case 1: // Show Reboot Counter
				sprintf(dbgbuf, "\r\nReboot Counter = %i", num_reboots);
				sendDBGALL(port, dbgbuf);
#ifdef __MYSTACK__
				PacketizeAndPush(dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
								 PPM_NUM); // Parameters are: data,length,stack,max length, packet
										   // type, page address
#endif
				break;

			case 2: // Show Time
				sprintf(dbgbuf, "\r\nTime = ");
				getStringTime(time, &dbgbuf[strlen(dbgbuf)]); // Creates a stringed time in Lbuf
				sendDBGALL(port, dbgbuf);
#ifdef __MYSTACK__
				PacketizeAndPush(dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
								 PPM_NUM); // Parameters are: data,length,stack,max length, packet
										   // type, page address
#endif
				break;

			case 3: // Show version of software
				sprintf(dbgbuf, __VERSION__);
				// sprintf(dbgbuf,"TEST-TEST");
				sendDBGALL(port, dbgbuf);
#ifdef __MYSTACK__
				PacketizeAndPush(dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
								 PPM_NUM); // Parameters are: data,length,stack,max length, packet
										   // type, page address
#endif
				break;

			case 4: //
				break;

			case 5: //
				break;

			case 6: // Show a memory register
				// TODO: Copy from command 6
				//#ifdef __MYSTACK__
				// PacketizeAndPush(dbgbuf,strlen(dbgbuf),MISC,DBG_BUF_SIZE,MISC_PACKET,PPM_NUM);
				// // Parameters are: data,length,stack,max length, packet type, page address #endif
				break;

			case 7: // Show Macros
				showMacroPacketized();
				break;

			case 8: // Show Schedule
				showSchedulePacketized();
				break;

			case 9: // Show Macros on selected port
				// Typical use: f 165 9 (command from bottom PPM to top to show on bottom's screen)
				showMacro(port, 1); // page 1
				showMacro(port, 2); // page 2
				break;

			case 10: // Show Macros on selected port
				// Typical use: f 165 10 (command from bottom PPM to top to show on bottom's screen)
				showSchedule(port, 1); // page 1
				// showSchedule(port,2);	// page 2
				break;

			default:
				break;
			}
			break;

		case 166: // Vanity Command.  Send anything you want out the beacon port and on the MISC
				  // channel
			// Example: r 166 (len) (msg)
			// Example: r 166 11 Hello World
			// Ex: f 166 11 Hello World //from the other ppm
			// Ex: f 166 43 "It\'s too late to be working on a satellite!"
			// Ex: f 166 30 "The Test: Is Emin truly a man?"
			// Ex: f 166 247
			// ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFG---JTB
			// Ex: f 166 249
			// ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJK----
			// Ex: f 166 129
			// +++DEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRS---JTB
			// Ex: f 166 240
			// +++DEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ---JTB
			// // Doesn't get stuck, but doesn't go through. Ex: f 166 230
			// +++DEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOP---XXX
			// // Works, but got stuck Ex: f 166 233
			// +++DEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRS---JTB
			// // Doesn't get stuck, but doesn't go through. Ex: f 166 234
			// +++DEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRS----JTB
			// // Doesn't get stuck, but doesn't go through.

#ifdef __BEACON__
			// BeaconSendMessage(&rbcmd[cmd2], rbcmd[cmd1]);
#endif
#ifdef __MYSTACK__
			PacketizeAndPush(
				&rbcmd[cmd2], rbcmd[cmd1], MISC, DBG_BUF_SIZE, MISC_PACKET,
				PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address
#endif
			break;

		case 167: // Beacon On/Off
// Format: r 167 (0/1)
// Example: r 167 0 // Turns Beacon Off
// Example: r 167 1 // Turns Beacon On
#ifdef __BEACON__
			if (rbcmd[cmd1]) {
				BeaconPower(BEACON_ON);
				globals.BeaconOnFlag = 1;
				sendDBGALL(USER_PORT, "\r\nBeacon On");
			} else {
				BeaconPower(BEACON_OFF);
				globals.BeaconOnFlag = 0;
				sendDBGALL(USER_PORT, "\r\nBeacon Off");
			}
#endif
			break;

		case 172: // BURN WIRE!!!!

			// Format: r 172 (burn driver)
			// Example: r 172 1 // Burn driver 1 (of 2)  (ASCII 0x01 and "1" will both work)
			// Example: r 172 2 // Burn driver 2 (of 2)
			switch (rbcmd[cmd1]) {
			case 1:
			case '1':
#ifdef __MHX__
				MHXOff();
#endif
#ifdef __BURNWIRE__
				BurnBabyBurn(1);
				sprintf(dbgbuf, "Burn Wire A activated.");
#ifdef __MYSTACK__
				PacketizeAndPush(dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
								 PPM_NUM); // Parameters are: data,length,stack,max length, packet
										   // type, page address
#endif
#endif
#ifdef __MHX__
				MHXOn();
				delay_ms(150);
#endif
				break;

			case 2:
			case '2':
#ifdef __MHX__
				MHXOff();
#endif
#ifdef __BURNWIRE__
				BurnBabyBurn(2);
				sprintf(dbgbuf, "Burn Wire B activated.");
#ifdef __MYSTACK__
				PacketizeAndPush(dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
								 PPM_NUM); // Parameters are: data,length,stack,max length, packet
										   // type, page address
#endif
#endif
#ifdef __MHX__
				MHXOn();
				delay_ms(150);
#endif
				break;

			default:
				return -1;
				break;
			}
			break;
		case 200: //== Store Received Text message
			/*
						switch(rbcmd[cmd1])
						{
						case '#': // This loads a iTAG UnitID
						  {
							  memset(globals.UnitID, 0, MAX_UNIT_ID);
							  num2cpy = cmd_length - 10 > (MAX_UNIT_ID - 1) ? (MAX_UNIT_ID - 1) :
			   (cmd_length - 10); memcpy(globals.UnitID, rbcmd + cmd2, num2cpy);
						  }
						  break;

						default:
							cmd_length = rbcmd[0] - 10;
							flash('p', (nTextMsg * 256) + TextMsg_Addr, cmd_length + 2, rbcmd + 10);
			   // This should save a NULL byte at the end nTextMsg++; break;
						}
			*/
			break;

		case 202: //== Reset Protected Variables
			// Eg: r 202 5 // Clear Macros
			//     r 202 6 // Clear Schedules
			clearBuf(rbcmd[cmd1]);
			break;

		case 215: //== Start EPS Aquisition Cycle
		{
#ifdef __EPS_H__

			// Aquire Status (2ms each):
			EPSData.EPS_Status = getEPSStatus(EPS_I2C_ADDR);
			EPSData.BAT_Status = getEPSStatus(BAT_I2C_ADDR);
			EPSData.RBB_Status = getEPSStatus(RBB_I2C_ADDR);

			// Aquire Heater Status (2ms each):
			EPSData.BAT_HeaterStatus = getEPSHeaterStatus(BAT_I2C_ADDR);
			EPSData.RBB_HeaterStatus = getEPSHeaterStatus(RBB_I2C_ADDR);

			// Aquire Everything Else (ADC Channels) (Non-blocking):
			StartEPSAquireFlag = 1;
			EPSBackgroundTask(1);
#endif
			break;
		}
		case 216: //== Print EPS Data
#ifdef __EPS_H__
#ifdef DEBUG
			printEPSTelemetry(&EPSData, 1);
#endif
#endif
			break;

		case 217: //== Get EPS Version
#ifdef __EPS_H__
			getEPSStatus(BAT_I2C_ADDR);
			getEPSStatus(EPS_I2C_ADDR);
			getEPSStatus(RBB_I2C_ADDR);
#endif
			break;

		case 237: // Battery State Commands
				  // Format:   r 237 1 (new battery state -- 1 byte)		// Sets new battery
				  // state Format:   r 237 2 (2-byte Low/Med Threshold Counts) (2-byte Med/High
				  // Threshold Counts) // Sets new thresholds Example:  r 237 2 *0109* *00C8* // 265
				  // and 200, respectively Example:  r 237 2 *0109* *00B4* // 265 and 180,
				  // respectively NOTE: Battery counts move in reverse order to voltages.  A high
				  // count indicates low voltage, and vice versa.
		{
			switch (rbcmd[cmd1]) {
			case 1:
				BatteryState_g = rbcmd[cmd2];
				// sendDBGALL(USER_PORT,"\r\nBattery state adjusted.");
				break;

#ifdef __EPS_H__
			case 2:
				if (cmd_length == cmd5) {
					globals.BatteryThreshold[0] = make16(rbcmd[cmd2], rbcmd[cmd3]);
					globals.BatteryThreshold[1] = make16(rbcmd[cmd4], rbcmd[cmd5]);
					// sendDBGALL(USER_PORT,"\r\nThresholds adjusted.");
				} else {
					// sendDBGALL(USER_PORT,"\r\nIncorrect command length.");
				}
				break;
#endif

			default:
				// sendDBGALL(USER_PORT,"\r\nInvalid Command.");
				break;
			}
		} break;

		case 238: //== Upload new scales and rotations (only for FSW PPM)
				  // Format:  r 238 (R/W) (G/M/S/W/T) (S/R) (floats in quotations)
				  // Format:  r 238 Read/Write (Gyro/Mag/Sunsensor/Wheels (Input and Output!)/Torque
				  // Coils
				  //                 cmd1  cmd2       cmd3   cmd4-->?
				  // Example: r 238 W G S "1.0 1.0 3.0"  // multiplies the gyro Z factor by 3
#ifdef __AENEAS__
			ChangeScaleRotation(rbcmd[cmd1], rbcmd[cmd2], rbcmd[cmd3], &rbcmd[cmd4]);
#endif
			break;

		case 239:
#ifdef LOWER_PPM
			float coefficients[9];
			if (getFloats(coefficients, &rbcmd[cmd1], 9) == SUCCESS) {
				memcpy(BiasCoefficients, coefficients, sizeof(BiasCoefficients));
			}
#endif
			break;

		case 240: //== Commit globals to flash
			commitGlobalsToFlash();
			sendDBGALL(USER_PORT, "\r\nGlobals saved.");
			break;

		case 241: //== Load globals with defaults
			loadDefaults(&globals);
			sendDBGALL(USER_PORT, "\r\nDefaults loaded.");
			break;

		case 242: //== Load globals with Flash variables
			if (restoreGlobalConfig(&globals) == SUCCESS) {
				sendDBGALL(USER_PORT, "\r\nGlobals restored from flash.");
			} else {
				sendDBGALL(USER_PORT, "\r\nRestore failed, loading defaults.");
				loadDefaults(&globals);
			}
			break;

		case 249: //== Set,increment or decrement Data Byte values
			c = rbcmd[cmd1];
			switch (rbcmd[cmd2]) {
			case 's':
				memcpy(DataByte + rbcmd[cmd1], rbcmd + cmd3, cmd_length - cmd2);
				break;
			default:
				DataByte[c] = rbcmd[cmd2]; // Set DataByte (default for backward compatibility)
				break;
			}
			break;

		case 253: //== Load scheduled commands
			// Format: r 253 (schedule #) (time) (command)
			// Time format is 7 bytes: MM DD YY WKday HH MM SS
			// "255" (0xff) can be used as a wildcard in time
			// Examples:
			//    r 253 1 255 255 255 255 255 255 10 155 // Programs schedule 1 to run command 155
			//    every minute at 10 seconds past the minute. r 253 0 8 12 10 255 17 56 15 152 6
			//    *000000* // Programs schedule 0 to run command 152 (with parameters 6 and
			//    0x000000) on 8/12/10, wildcard weekday, 5:56:15pm (military time 17:56:15)
			cmd_length -= 10; // Remove address,cmdID and index
			c = rbcmd[cmd1];
			if (cmd_length > 7 && cmd_length < MAX_STORED_CMD_SIZE - 3) // minimum valid command
			{
				setSchedule(&globals.StoredCmds[c][0], MAX_STORED_CMD_SIZE, &rbcmd[cmd2 + 7],
							cmd_length - 7, &rbcmd[cmd2]);
				// HexPrint(USER_PORT,&rbcmd[cmd2+7],cmd_length);
			} else
				clearSchedule(&globals.StoredCmds[c][0]);
			break;
		case 254: // Bootloader Command
#ifdef __BOOTLOADER_H__
			switch (rbcmd[cmd1]) {
			case 'S': // ACTIVATE BOOTLOADER.  Make SURE you have verified the hex file first.
				// Format: r 254 SUDO
				// Example: r 254 SUDO
				if (rbcmd[cmd2] == 'U')
					if (rbcmd[cmd3] == 'D')
						if (rbcmd[cmd4] == 'O') {
							if (ack)
								sendACK(port, I_NAK, TX_Code, Command_ID, source, 0, encrypt);
							sendDBGALL(port, "BOOTLOADER ACTIVATED!");
#asm
							goto LOADER_ADDR
#endasm
						}
				break;

			case 'C': // Clear flash file
				// Clears a specified number of bytes from the Hexfile area of flash, rounding up to
				// the next highest flash block. Format: r 254 C (4-byte size) Example:  r 254 C
				// *0006C800*  // Command to clears a 434kB space, which is 108.5 blocks.  It will
				// clear 109 blocks, which is 436kB.

				// Validate exact command length
				if (cmd_length != cmd5) {
					sendDBGALL(port, "\r\nIncorrect command size. Format: \"r 254 C *0006C800*\"");
				} else {
					unsigned long total;
					total = make32(rbcmd[cmd2], rbcmd[cmd3], rbcmd[cmd4], rbcmd[cmd5]);
					flashEraseBetweenBlocks(FlashAddrToBlock(HEXFILE_ADDR),
											FlashAddrToBlock(HEXFILE_ADDR + total));
					sendDBGALL(port, "\r\nBlocks erased.");
				}
				break;

			case 'L': // Load new hex file
				// Loads a new hex file into flash.  You have 30 seconds to begin
				// the transfer, and 2 seconds after each line to continue sending.
				// Format: r 254 L (listen port) (report port) (carriage return) (File contents...)
				// Example:  r 254 L 2 99 (CR) (Hex File bytes)
				if (uploadBootableFile(rbcmd[cmd2], rbcmd[cmd3]) != SUCCESS) {
					sendDBGALL(port, "\r\nFAILURE DETECTED ON FILE UPLOAD!");
				}
				break;

			case 'V': // Verify file
				// Reads a hex file out of flash and compares the checksum to the given checksum
				// Format: r 254 V (2-byte crc)
				// Example:  r 254 V *4F06*

				// TODO: Add a PacketizeAndPush command to the output of this.
				checkHexFile(port, ProcessCMDBuf);
				if (ProcessCMDBuf[0] == rbcmd[cmd2] && ProcessCMDBuf[1] == rbcmd[cmd3]) {
					sprintf(dbgbuf, "GREAT SUCCESS!");
					sendDBGALL(USER_PORT, dbgbuf);
				} else {
					sprintf(dbgbuf, "CRC CHECK FAILED! Calculated CRC: %02x%02x", ProcessCMDBuf[0],
							ProcessCMDBuf[1]);
					sendDBGALL(USER_PORT, dbgbuf);
				}
#ifdef __MYSTACK__
				PacketizeAndPush(dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
								 PPM_NUM); // Parameters are: data,length,stack,max length, packet
										   // type, page address
#endif
				break;

			default:
				break;
			}
#endif
			break;

		case 255: //== Perform Reset via Watchdog Timer
			if (ack)
				sendACK(port, I_NAK, TX_Code, Command_ID, source, 0, encrypt);
			delay_ms(500);
			disable_interrupts(INTR_GLOBAL);
			for (;;)
				;
		default:
			if (ack)
				sendACK(port, I_NAK, TX_Code, Command_ID, source, 0, encrypt);
			ack = 0;
			retVal = -1;
			break;
		} // switch... Command_ID processing

#ifdef __MHX__
		if (ack)
			sendACK(port, I_ACK, RX_Seq_Num++, Command_ID, source, retVal,
					encrypt); // Return TX_Code of Zero in ACK..we are ACKing a command
#endif
		ncmds--;
	} // while ncmds>0
	return retVal;
} // Process commands

/*
void dataScreen(int port, unsigned char type) {
unsigned char i;
	clrscrn(port);
	debugNoisy = 0;

	if (type=='s' || type=='b'){
		sprintf(Lbuf,"Aeneas Data Display");
		placeString(port, 2, 14, Lbuf);
		sprintf(Lbuf, "BrdAddr:%d, ExtAddr:%d", BrdAddr, ExtAddr);
		placeString(port, 4, 4, Lbuf);
	}

	switch (type) // Is this data binary or string data?
	{
	case 's': // This is string data labels
		display[port] = 's'; // display option equals true
		break;

	case 'N':
		debugNoisy = 1;
	case 'B': // This for debug Display option
		display[port] = 'F';    // inhibit telemetry display
		menu[port] = 'F';       // inhibit menu
		send_Data[port] = 0;
//        enableDebugMSG();
		break;

	case 'b': // Normal display functions
		display[port] = 'b';
		for(i=0;i<12;i++)placeString(port,i+7, 2,globals.label[i]);
		break;
	default:
		break;
	}

sprintf(Lbuf,"Current Time");
placeString(port, 22, 4, Lbuf);
}

*/

#ifdef __MAESTRO__
// Goes into a loop where it reads bytes
// After each line ('/r'), it will write the line to flash.
// If too much time passes before the first line or between lines, aborts
// If too many characters come before a line return, abort.
// If any flash error, abort.
ReturnErr_t uploadHexFile(int listenPort, int reportPort, unsigned long initial_offset,
						  unsigned long base_addr, unsigned long alloc_size) {
	// Declarations
	unsigned long bytes_read = 0;
	int index = 0;
	unsigned char c, finished = 0;
	unsigned long timeout;
	unsigned long addr;
	unsigned int line = 0;
	unsigned char crc[2];
	unsigned char prev_handshake;

	// Write the file into flash, calculating a CRC as we go
	sprintf(dbgbuf, "\r\nBegin file transfer...\r\n");
	sendDBGALL(reportPort, dbgbuf);
	timeout = SEC_TIMER + 30; // 30 seconds allowed for first line
	bytes_read = 0;
	index = 0;
	line = 0;
	addr = base_addr + initial_offset;
	prev_handshake = getPortHandshaking(listenPort);
	setPortHandshaking(listenPort, HANDSHAKE_ON);
	while (!finished) {
		restart_wdt(); // kick dog
		if (getByte(listenPort, &c)) {
			Lbuf[index++] = c;
			bytes_read++;
			if (c == '\r') {
				timeout = SEC_TIMER + 2; // seconds allowed per line after that
				if (line % 50 == 0) {
					sprintf(dbgbuf, "\rGot Line %05u: ", line);
					sendDBGALL(reportPort, dbgbuf);
				}
				if (line == 0)
					getCRCCont(CRC_RESTART, Lbuf, index, crc);
				else
					getCRCCont(CRC_CONTINUE_PREVIOUS, Lbuf, index, crc);
				if (flashWriteSafe(addr, index, Lbuf, base_addr, base_addr + alloc_size - 1) !=
					SUCCESS)
					goto ERROR;
				addr += index;
				index = 0;
				line++;
			}
		}
		if (SEC_TIMER > timeout) {
			sendDBGALL(reportPort, "\r\nTIMEOUT");
			finished = 1;
		}
		if (index >= GENERAL_BUFFER_SIZE)
			goto ERROR;
	}

	// That should be it.  Display the CRC and total lines read.
	sprintf(dbgbuf, "\r\n Total Lines Read: %u", line);
	sendDBGALL(reportPort, dbgbuf);
	sprintf(dbgbuf, "\r\n Total Bytes Read: %u ", bytes_read);
	sendDBGALL(reportPort, dbgbuf);
	sprintf(dbgbuf, "\r\n CRC: %02x%02x", crc[0], crc[1]);
	sendDBGALL(reportPort, dbgbuf);
	sprintf(dbgbuf, "\r\n");
	sendDBGALL(reportPort, dbgbuf);

	return SUCCESS;

ERROR:
	setPortHandshaking(listenPort, prev_handshake);
	return FAILURE;
}
#endif

#ifdef __BOOTLOADER_H__
// Goes into a loop where it reads bytes
// After each line ('/r'), it will write the line to flash.
// If too much time passes before the first line or between lines, aborts
// If too many characters come before a line return, abort.
// If any flash error, abort.
ReturnErr_t uploadBootableFile(int listenPort, int reportPort) {
	// Declarations
	unsigned long bytes_read = 0;
	int index = 0;
	unsigned char c, finished = 0;
	unsigned long timeout;
	unsigned long addr;
	unsigned int line = 0;
	unsigned char crc[2];
	unsigned char prev_handshake;

	// Write the file into flash, calculating a CRC as we go
	sprintf(dbgbuf, "\r\nBegin file transfer...\r\n");
	sendDBGALL(reportPort, dbgbuf);
	timeout = SEC_TIMER + 30; // 30 seconds allowed for first line
	bytes_read = 0;
	index = 0;
	line = 0;
	addr = HEXFILE_ADDR;
	prev_handshake = getPortHandshaking(listenPort);
	setPortHandshaking(listenPort, HANDSHAKE_ON);
	while (!finished) {
		restart_wdt(); // kick dog
		if (getByte(listenPort, &c)) {
			Lbuf[index++] = c;
			bytes_read++;
			if (c == '\r') {
				timeout = SEC_TIMER + 2; // seconds allowed per line after that
				if (line % 50 == 0) {
					sprintf(dbgbuf, "\rGot Line %05u: ", line);
					sendDBGALL(reportPort, dbgbuf);
				}
				if (line == 0)
					getCRCCont(CRC_RESTART, Lbuf, index, crc);
				else
					getCRCCont(CRC_CONTINUE_PREVIOUS, Lbuf, index, crc);
				if (flashWriteSafe(addr, index, Lbuf, HEXFILE_ADDR,
								   HEXFILE_ADDR + HEXFILE_ALLOC_SIZE - 1) != SUCCESS)
					goto ERROR;
				addr += index;
				index = 0;
				line++;
			}
		}
		if (SEC_TIMER > timeout) {
			sendDBGALL(reportPort, "\r\nTIMEOUT");
			finished = 1;
		}
		if (index >= GENERAL_BUFFER_SIZE)
			goto ERROR;
	}

	// That should be it.  Display the CRC and total lines read.
	// TODO: PacketizeAndPush this output so it can get to the ground.
	sprintf(dbgbuf, "\r\n Total Lines Read: %u", line);
	sendDBGALL(reportPort, dbgbuf);
	sprintf(dbgbuf, "\r\n Total Bytes Read: %u ", bytes_read);
	sendDBGALL(reportPort, dbgbuf);
	sprintf(dbgbuf, "\r\n CRC: %02x%02x", crc[0], crc[1]);
	sendDBGALL(reportPort, dbgbuf);
	sprintf(dbgbuf, "\r\n");
	sendDBGALL(reportPort, dbgbuf);

	return SUCCESS;

ERROR:
	setPortHandshaking(listenPort, prev_handshake);
	return FAILURE;
}
#endif

#ifdef __BOOTLOADER_H__
ReturnErr_t checkHexFile(int port, unsigned char* crc) {
	// Declarations
	unsigned long bytes_read;
	int index = 0;
	unsigned char c, finished = 0;
	unsigned long addr;
	unsigned int line = 0;
	unsigned char trys;
	const unsigned char MAX_TRYS = 5; // The maximum number of times to attempt to read a line
	ReturnErr_t err;

	// Read the file from flash, calculating a CRC as we go
	sprintf(dbgbuf, "\r\nBegin file verify...\r\n");
	sendDBGALL(port, dbgbuf);
	index = 0;
	line = 0;
	bytes_read = 0;
	addr = HEXFILE_ADDR;
	crc[0] = 0x00;
	crc[1] = 0x00;
	for (;;) {
		restart_wdt(); // kick dog
		trys = 0;

		// Inner loop for reading a good line (in spite of spurios flash errors)
		do {
			trys++;
			flashRead(addr, GENERAL_BUFFER_SIZE, Lbuf); // Re-Read into buffer
			if (!(Lbuf[0] == ';' || Lbuf[0] == ':'))
				goto FINISHED; // Break if we're not reading a hex file anymore
			if (trys > MAX_TRYS)
				return TIMEOUT; // Check for exceeding max trys
			err = verifyHexLine(Lbuf, GENERAL_BUFFER_SIZE); // Check the line
			if (err != SUCCESS)
				PrintDbgErr(port, err); // Report any errors
		} while (err != SUCCESS);

		finished = 0; // Reset inner loop condition
		// Inner Loop for processing each line
		do {
			// This loop works by advancing an index until it hits a carriage return,
			// then processing the CRC of the line and resetting appropriate conditions
			// for the next one.
			c = Lbuf[index++]; // increment index and store in c
			if (c == '\r') // if it's an end of line
			{
				// Process the line
				if (line % 50 == 0) {
					sprintf(dbgbuf, "\rVerify Line %05u: ", line);
					sendDBGALL(port, dbgbuf);
				}
				if (line == 0)
					getCRCCont(CRC_RESTART, Lbuf, index, crc);
				else
					getCRCCont(CRC_CONTINUE_PREVIOUS, Lbuf, index, crc);
				addr += index; // This is next spot to read from flash
				bytes_read += index; // Running count of bytes read
				index = 0; // Reset index
				finished = 1; // Set inner loop bail condition
				line++;
			}
			if (index >= GENERAL_BUFFER_SIZE)
				goto ERROR; // if we run off the end of the buffer, get out!
		} while (!finished);
	}

FINISHED:
	// That should be it.  Display the CRC and total lines read.
	sprintf(dbgbuf, "\r\n Total Lines Read: %u", line);
	sendDBGALL(port, dbgbuf);
	sprintf(dbgbuf, "\r\n Total Bytes Read: %u ", bytes_read);
	sendDBGALL(port, dbgbuf);
	sprintf(dbgbuf, "\r\n CRC: %02x%02x", crc[0], crc[1]);
	sendDBGALL(port, dbgbuf);
	sprintf(dbgbuf, "\r\n");
	sendDBGALL(port, dbgbuf);
	return SUCCESS;

ERROR:
	sendDBGALL(port, "\r\nVerify failed.");
	return FAILURE;
}

unsigned int8 mike_atoi_b16(unsigned char* s) { // Convert two hex characters to a int8
	unsigned int8 result = 0;
	unsigned int8 i;

	for (i = 0; i < 2; i++, s++) {
		if (*s >= 'A')
			result = 16 * result + (*s) - 'A' + 10;
		else
			result = 16 * result + (*s) - '0';
	}

	return (result);
}

ReturnErr_t verifyHexLine(unsigned char* buffer, unsigned int MAX_SIZE) {
	unsigned int8 count = 0;
	unsigned int8 checksum, line_type;
	unsigned int16 buffidx;
	unsigned int i;

	// First things first, find the buffer index value (1 character beyond the line return)
	buffidx = 0;
	while (buffer[buffidx++] != '\r') {
		if (buffidx >= MAX_SIZE)
			return OUT_OF_BOUNDS; // Check array bounds
	}

	// At this point, we should have buffidx pointing to the character 1 past the line return
	// If it's data, it will start with a colon, so let's check that
	if (buffer[0] == ':') {
		count = make16(buffer[1], buffer[2]); // Get the number of bytes from the buffer

		// If the line type is 1, then data is done being sent
		if (line_type == 1) {
			return SUCCESS;
		} else {
			// Compute the checksum
			checksum = 0;
			for (i = 1; i < (buffidx - 3); i += 2)
				checksum += mike_atoi_b16(&buffer[i]);
			checksum = 0xFF - checksum + 1;

			// Check the checksum against what's in the line
			if (checksum != mike_atoi_b16(&buffer[buffidx - 3])) {
				return FAILURE; // Checksum failure
			} else {
				// This is a valid line
				return SUCCESS;
			}
		} // else
	} // if :

	// To get here, the first character is not a colon, indicating a comment instead
	// of data.  We will return success because we can ignore these lines.
	return SUCCESS;
}

#endif

/*=================== getIO  ========================
  PURPOSE:
  UNMODIFIED VARIABLES: Baddr, Eaddr <---
  MODIFIED VARIABLES:
  GLOBALS: time, t_sec, RTU_mode, globals structure, version, Macro_state, ScheduleMask, TCW
  RETURN VALUE: length
  NOTES: Current switch case only works for Baddr=0, Eaddr=6,23,21    //??
-----------------------------------------------------------
*/
/*
unsigned char getIO(unsigned char Baddr, unsigned char Eaddr, unsigned char *buf) {
//	int * pAdcResult;
//	int channel;
	static unsigned char len;
	static int n, m;
	static unsigned char temp;

	//stub static unsigned char pACL=0;			// This determines the change in nACL since last
reporting of network status len = buf[0]=0;                     // This is default length buf[1] =
0x40 | Baddr;              // Indicate extended addresses and that this is data (bit 3 and 6 must be
set) buf[2] = Eaddr;                     // Default Packet ID's TCW++; // Increment Frame Counter
with each data collection

	switch (Baddr) {
	  case 0: //== Motherboard IO
		switch (Eaddr) //== Use Extended address to further define IO
		{
		  case 6: //== iDAC configuration data
			len = 83;
			memcpy(buf+12,version,13);
			memcpy(buf + 25, globals.RTU_ID, 4);
			memcpy(buf + 29, globals.Dest_ID, 4);
//            memcpy(buf + 33, globals.GW_ID, 4); //
			buf[37] = 0;
			buf[38] = (unsigned char) globals.LANPort[0];
			buf[39] = (unsigned char) globals.LANPort[1];
			buf[40] = 0;
			buf[41] = 0; //unsigned char)globals.dataPort;
			for (n = 0; n <NUM_PORTS; n++)buf[42 + n] = 57600;
			getCFG(buf + 45);
			for (m = 0; m < 5; m++)                     // Compute and load status of Macros
			{
				temp = 1;
				for (n = 0; n < NUM_MACROS; n++) {
					if (Macro_state[n + (m * 8)] != MACRO_STOP)
						buf[76 - m] |= temp;
					temp = temp << 1;
				}
			}
			memcpy(buf+80,ScheduleMask,4);              // bug waiting to happen.. for now limit to
4 break;

		  case 0x17: //== Broadcast ID's (set with command 23)   0x17=23
			len = 12 + 4 * globals.nGroup;
			buf[12] = globals.nGroup;
			for (n = 0; n < globals.nGroup; n++)
				memcpy(buf + 13 + (4 * n), globals.Broadcast[n], 4);
			break;

		  case 21: //==get orbit and Attitude
			len= 49;
			//memcpy(buf+12,orbitData+13,24);		// Position
			 break;

		  default: //== Default Motherboard IO
			break;
		}//switch (Eaddr)

/*
	  case 0x01: //== Extended (TW0 byte) Analog Interfaces (this is now the default)
	  case 56: //							  (0x13)
		{
			len = 43;
			memset(buf + 12, 0, 24);
			pAdcResult = (int*)(buf + 12);
			for (channel = 0; channel < 12; channel++) {
				if (fbit(globals.ADC_Channel, channel)) {
//                    pAdcResult[channel] = readSingleAnalog(ADC_REF_AVDD, channel);
				} else {
//                    pAdcResult[channel] = 0x7fff;
				}
			}
			buf[36] = rebootCounter;
			buf[37] = 0x1f & commTimer; // Use the lower 5 bits for communication timer
			if(inRange)buf[37] |= 0x20;    // Set bit indicating we are in range
			buf[38] = 0;
			buf[39] = 0;            //  used to be DataByte[255];
			buf[40] = LQI;          // LAN Radio LQI
			buf[41] = 0;            // spare for now
			buf[42] = 0;
		}
		break;
*/
/*
	  default: //== Default IO configuration
		break;
	} //==switch(Baddr)

	buf[0] = len;
	if (len) // If valid data
	{
		memcpy(buf+3,time,7);		// Use the global version of time
//        buf[10] = TCW;              // RTU counter  	(optional data)
		buf[10] = t_sec;            // tenth of second
		buf[11] = RTU_mode;         // RTU mode   		(optional data)
		buf[len + 1] = '\0';        // Terminate String
	}
	return len;
}
*/

/*
// used in GetIO -- do not change without looking there!
void getCFG(unsigned char *buf) {
	buf[0] = 29; // Message length

	// FOR show cfg menu, put all binarys here, asciis below
	buf[1] = (unsigned char) (globals.Tpreamble / 4); // Tpreamble   //  buf[11]
	buf[2] = (unsigned char) (globals.Ttail);    // Radio transmit tail
	buf[3] = (unsigned char) Channel;            // Current Channel

	buf[4] = 0;

	buf[5] = (unsigned char) globals.wakeCount;          // Power save
	buf[6] = (unsigned char) globals.boardConfig;    // Default Power condition
	buf[7] = (unsigned char) globals.sleepMode;   // Default Sleep condition

	buf[8] = boardCFG;                  // this is the current status
//    buf[9] = RFSTATUS;
//    if (boardCFG & RADIO_POWERED)buf[9] |= 0x80; // Set the MSB if the Radio is enabled

	buf[10] = commTimer; // Last connection, lower 8 bits of timer?
	buf[11] = rebootCounter; // number of reboots
	buf[12] = (unsigned char) globals.discard; // number of retries
	buf[13] = (unsigned char) globals.RetryTime; // Seconds until retry
	buf[14] = 0;
	buf[15] = 0;

	// For show cfg menu, put all asciis here, ints above
	buf[16] = globals.Radio[0]; // Primary Radio type
	buf[17] = globals.Radio[1]; // Secondary Radio type
	buf[18] = globals.schedule; // Schedule checking enables
	buf[19] = 'N'; //stub globals.modem_type;    // modem type  (if installed)
	buf[20] = 'I'; //stub modem_state;						// Current state of modem
	buf[21] = 'F'; //stub AT_Answer;             // modem answer enable
	buf[22] = 'F'; //stub AT_Dial;              	// modem dialout enable
	buf[23] = globals.useData; // Data Port checkUser Enabled
	buf[24] = globals.CRC[0]; // iDAC bus enabled
	buf[25] = 'Z'; //stub globals.Network;				// Network_Type
	buf[26] = 'N';
	buf[27] = inRange; // In range of a radio network
	buf[28] = Associated; // Associated with Zigbee network
	buf[29] = (unsigned char) (globals.nGroup);
} // end of getCFG
*/

//====================random stuff===================
/*
void setDumpPointer(unsigned int p)
{
	dumpPtr = p;
}
*/

//================= Send ACK ==========================================
void sendACK(int port, unsigned char value, unsigned char Seq_Num, unsigned char PK_ID,
			 unsigned char* source, unsigned char ready, unsigned char encrypt) {
// MRA Additions
#ifdef __MHX__
	if (port == MHX_PORT) {
		sendPACKETIZED(port, &AckFlag, sizeof(AckFlag), 3);
		SyncTime = SEC_TIMER + SyncRetryTime;
	}
#endif
}

// Returns the nth bit in a value
int fbit(int value, int n) {
	return (int)((value >> n) & 0x01);
}

/*
unsigned char checkACK(unsigned char* source, unsigned char Seq_Num) {
	if ((memcmp(source, source, 4)==0) && TX_Seq_Num == Seq_Num)		//first source was TX_Dest
		return 1;
	else
		return 0;
}
*/

// This function copies 1 buffer to another in reverse order
// Eg: 12345 (source)  becomes  54321 (destination)
// Inputs: rcpy (destination, source, length)
// WARNING: will not work with the same buffer given as both inputs
void rcpy(unsigned char* buf1, unsigned char* buf2, unsigned char len) {
	int n;
	for (n = 0; n < len; n++)
		buf1[n] = buf2[len - n - 1];
}

// ??? I think this converts 2 characters into an equivalent unsigned integer.
// We can probably replace it with the PIC function make16()
// TODO: Replace this function
unsigned int C2I(unsigned char* buf) {
	return (unsigned int)buf[0] * 256U + (unsigned int)buf[1];
}

void clearBuf(unsigned char value) {
	int n;

	switch (value) {
	case 0: //== (0) Clear Transmit buffer
		//        clearTX(0);
		break;

	case 5: //==  (5)Clear Macros
#ifdef __MACROS__
		waitForFlash(); // wait for any write to complete
		for (n = 0; n < NUM_MACROS; n++) {
			flashErasePage(FlashAddrToPage((long)n * MAX_MACRO_SIZE + Macros_ADDR));
			waitForFlash(); // wait for any write to complete
		}
#endif
		break;

	case 6: //==  (6) Clear Schedule
		for (n = 0; n < NUM_STORED_CMDS; n++)
			globals.StoredCmds[n][0] = 0;
		break;

	case 8: // (8) Clear Stored Data
		break;

	case 9: // (9) Clear Stored Text Messages
		break;

	case 10: //== (10) Clear all Flash Packets
		break;

	default:
		break;
	}
}

// Writes the global struct to flash at the Config Address
void commitGlobalsToFlash() {
#ifdef __FLASH_DRIVER__
	int n, m;
	ReturnErr_t err;
	getCRC((unsigned char*)&globals, sizeof(TFlashProtected) - 2,
		   globals.CRC); // Gets and Stores the CRC with the globals

	n = sizeof(TFlashProtected); // Number of bytes to be written to flash
	m = 0;

	do {
		flashEraseBlockByAddr((unsigned long)(Config_ADDR + m)); // Erase the block at Config + m
		// -------------NOTE THAT A REBOOT HERE WOULD BE REALLY BAD ----------//
		m += FLASH_BLOCK_SIZE; // increment m
	} while (m < n); // if there's still more to go, loop around
	// sendDBGALL(USER_PORT,"B");

	// Now program the new stuff, handling any errors if they happen
	err = flashWriteSafe(Config_ADDR, n, &globals, Config_ADDR, Config_ADDR + n + 1);
	if (err != SUCCESS) {
		sprintf(dbgbuf, "\r\nError %i writing globals to flash", err);
		sendDBGALL(USER_PORT, dbgbuf);
#ifdef __MYSTACK__
		PacketizeAndPush(
			dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
			PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address
#endif
	}

#else
	sendDBGALL(USER_PORT, "\r\nNo Flash Driver installed.");
#endif
}

#ifdef __AENEAS__
// Writes the global struct to flash at the Config Address
void commitExtInToFlash() {
	int n, m;
	ReturnErr_t err;
	unsigned char crc[2];

	getCRC((unsigned char*)ExtIn, sizeof(float) * NUMIN, crc); // Gets crc

	n = sizeof(float) * NUMIN; // Number of bytes to be written to flash
	m = 0;
	do {
		flashEraseBlockByAddr(
			(unsigned long)(GNC_INPUTS_ADDR + m)); // Erase the block at Config + m
		// -------------NOTE THAT A REBOOT HERE WOULD BE REALLY BAD ----------//
		m += FLASH_BLOCK_SIZE; // increment m
	} while (m < n + 2); // if there's still more to go, loop around (+2 for crc)

	// Now program the new stuff, handling any errors if they happen
	err = flashWriteSafe(GNC_INPUTS_ADDR, n, (unsigned char*)ExtIn, GNC_INPUTS_ADDR,
						 GNC_INPUTS_ADDR + n + 1); // write data
	if (err != SUCCESS) {
		sprintf(dbgbuf, "\r\nError %i writing GNC to flash", err);
		sendDBGALL(USER_PORT, dbgbuf);
#ifdef __MYSTACK__
		PacketizeAndPush(
			dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
			PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address
#endif
	}
	err = flashWriteSafe(GNC_INPUTS_ADDR + n, 2, crc, GNC_INPUTS_ADDR + n,
						 GNC_INPUTS_ADDR + n + 3); // write crc
	if (err != SUCCESS) {
		sprintf(dbgbuf, "\r\nError %i writing GNC crc to flash", err);
		sendDBGALL(USER_PORT, dbgbuf);
#ifdef __MYSTACK__
		PacketizeAndPush(
			dbgbuf, strlen(dbgbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
			PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address
#endif
	}
}
#endif

#ifdef __AENEAS__
void loadExtInDefaults() {
	ExtIn[Mode_Override_IN] = 0;
	ExtIn[Mode_Limit_IN] = 9;
	ExtIn[Min_Omega_IN] = 0.035;
	ExtIn[Max_Omega_IN] = 0.8;
	ExtIn[Sun_Search_Slew_Transition_IN] = 600;
	ExtIn[Min_Sun_Line_Trans_IN] = 0.15;
	ExtIn[Min_Pitch_Trans_IN] = 0.17;
	ExtIn[SP_Time_Limit_IN] = 2700;
	ExtIn[Enable_Momemtum_Dump_IN] = 0;
	ExtIn[kpx_IN] = 0.16;
	ExtIn[kpy_IN] = 0.16;
	ExtIn[kpz_IN] = 0.16;
	ExtIn[krx_IN] = 0.8;
	ExtIn[kry_IN] = 0.8;
	ExtIn[krz_IN] = 0.8;
	ExtIn[Ixx_IN] = 0.1328;
	ExtIn[Ixy_IN] = 0;
	ExtIn[Ixz_IN] = 0;
	ExtIn[Iyy_IN] = 0.13204;
	ExtIn[Iyz_IN] = 0;
	ExtIn[Izz_IN] = 0.0351;
	ExtIn[Slew_to_PD_Trans_IN] = 0.25;
	ExtIn[Commanded_Aqsn_Slew_Rate_IN] = 0.026;
	ExtIn[Rate_Limit_IN] = 20;
	ExtIn[Q_Limit_IN] = 1;
	ExtIn[Wheel_Sat_Limit_IN] = 0.002;
	ExtIn[kMOMx_IN] = 0;
	ExtIn[kMOMy_IN] = 0;
	ExtIn[kMOMz_IN] = 0;
	ExtIn[Ixx_Wheel_IN] = 0.0000104;
	ExtIn[Ixy_Wheel_IN] = 0;
	ExtIn[Ixz_Wheel_IN] = 0;
	ExtIn[Iyy_Wheel_IN] = 0.0000104;
	ExtIn[Iyz_Wheel_IN] = 0;
	ExtIn[Izz_Wheel_IN] = 0.0000104;
	ExtIn[Wheel_Enable_IN] = 1;
	ExtIn[AD_Filter_Gain_Kf_IN] = 0.46651;
	ExtIn[AD_Filter_Pole_IN] = 0.53349;
	ExtIn[Whl_Filter_Gain_IN] = 1;
	ExtIn[Whl_Filter_Pole_IN] = 1;
	ExtIn[Reset_Estimator_IN] = 0;
	ExtIn[q1_M_LV_IN] = 0.5;
	ExtIn[q2_M_LV_IN] = 0.5;
	ExtIn[q3_M_LV_IN] = 0;
	ExtIn[q4_M_LV_IN] = 0.707107;
	ExtIn[Target_Lat_IN] = -121.98;
	ExtIn[Target_Long_IN] = 38.92;
	ExtIn[Target_Alt_IN] = 0;
	ExtIn[Slew_Angle_IN] = 2.5;
	ExtIn[SS_MisAln_x_IN] = 0;
	ExtIn[SS_MisAln_y_IN] = 0;
	ExtIn[SS_MisAln_z_IN] = 0;
	ExtIn[Gyro_MisAln_x_IN] = 0;
	ExtIn[Gyro_MisAln_y_IN] = 0;
	ExtIn[Gyro_MisAln_z_IN] = 0;
	ExtIn[Gyro_Drift_x_IN] = 0;
	ExtIn[Gyro_Drift_y_IN] = 0;
	ExtIn[Gyro_Drift_z_IN] = 0;
	ExtIn[Gyro_AutoCal_IN] = 1;
	ExtIn[MTR_Enable_Mode4_IN] = 0;
	ExtIn[Drag_Coeff_IN] = 2.2;

	// Models
	ExtIn[SUNVEC0_IN] = 0.577350269189625;
	ExtIn[SUNVEC1_IN] = 0.577350269189625;
	ExtIn[SUNVEC2_IN] = 0.577350269189625;

	ExtIn[ECI_POS_X_IN] = 421.53;
	ExtIn[ECI_POS_Y_IN] = 6820999.95;
	ExtIn[ECI_POS_Z_IN] = 649;
	ExtIn[ECI_VEL_X_IN] = 4215.3;
	ExtIn[ECI_VEL_Y_IN] = (-1.0) * 0.856;
	ExtIn[ECI_VEL_Z_IN] = 6490;
	ExtIn[GHA_IN] = 1.4;
	ExtIn[nT_ECI_x_IN] = 0.1;
	ExtIn[nT_ECI_y_IN] = 0.1;
	ExtIn[nT_ECI_z_IN] = 0.2;

	// Sensors
	ExtIn[Measured_Gyro_X_IN] = 0;
	ExtIn[Measured_Gyro_Y_IN] = 0;
	ExtIn[Measured_Gyro_Z_IN] = 0;
	ExtIn[Measured_Mag_X_IN] = 1;
	ExtIn[Measured_Mag_Y_IN] = 0.1;
	ExtIn[Measured_Mag_Z_IN] = 0.1;
	ExtIn[Measured_SS_x_IN] = 0.1;
	ExtIn[Measured_SS_y_IN] = 0.1;
	ExtIn[Measured_SS_z_IN] = 0.989949494;
	ExtIn[Sun_Present_Flag_IN] = 0;
	ExtIn[Measured_RW_omega_x_IN] = 0;
	ExtIn[Measured_RW_omega_y_IN] = 0;
	ExtIn[Measured_RW_omega_z_IN] = 0;

	// Spares
	ExtIn[Wheel_Disable_Mode5_IN] = 1; // Disables torque commands in Eclipse Mode
	ExtIn[Spare2_IN] = 0;
	ExtIn[Spare3_IN] = 0;
}
#endif

ReturnErr_t checkGlobalCRC(TFlashProtected* g) {
	unsigned char crc[2];

	// Setup
	crc[0] = 0x00;
	crc[1] = 0x00;

	// Compute crc
	getCRC((unsigned char*)g, sizeof(TFlashProtected) - 2, crc);

	// Debug message
	sprintf(dbgbuf, "\r\n   Checksum compare:0x%02x%02x to 0x%02x%02x...", g->CRC[0], g->CRC[1],
			crc[0], crc[1]);
	sendDBGALL(USER_PORT, dbgbuf);

	// Compare
	if (memcmp(globals.CRC, crc, 2) != 0) // If they are equal, it will return zero.
	{
		return FAILURE;
	} else {
		return SUCCESS;
	}
}

ReturnErr_t restoreGlobalConfig(TFlashProtected* config) {
#ifdef __FLASH_DRIVER__
	int n;
	int i;
	const int TIMES_TO_CHECK = 10;

	// Setup
	waitForFlash();
	n = sizeof(TFlashProtected); // Number of bytes to be read from flash

	// Read and check CRC, exiting successfully if the CRC matches
	for (i = 0; i < TIMES_TO_CHECK; i++) {
		flashRead(Config_ADDR, n, (unsigned char*)config);
		//	HexPrint(USER_PORT,(unsigned char*)config,n);
		if (checkGlobalCRC(config) == SUCCESS)
			return SUCCESS;
	}
#endif

	return FAILURE;
}

#ifdef __AENEAS__
ReturnErr_t restoreExtIn(float* ext) {
	const int TIMES_TO_CHECK = 10;
	int n;
	int i;
	char crc_read[2];
	char crc_computed[2];
	waitForFlash();

	n = NUMIN * sizeof(float);

	// Read and check CRC several times, exiting successfully if the CRC matches
	for (i = 0; i < TIMES_TO_CHECK; i++) {
		flashRead(GNC_INPUTS_ADDR, n, (unsigned char*)ext); // Read the floats
		flashRead(GNC_INPUTS_ADDR + n, 2, crc_read); // Read the CRC

		// Check the CRC
		getCRC((unsigned char*)ExtIn, NUMIN * sizeof(float),
			   crc_computed); // Computes the crc of the data
		sprintf(dbgbuf, "\r\n   Checksum compare:0x%02x%02x to 0x%02x%02x...", crc_read[0],
				crc_read[1], crc_computed[0], crc_computed[1]);
		sendDBGALL(USER_PORT, dbgbuf);

		// Compare
		if (memcmp(crc_computed, crc_read, 2) != 0) // If they are equal, it will return zero.
		{
			// Do Nothing is it fails, to loop again
		} else {
			return SUCCESS; // Return success if it checks out
		}
	}

	return FAILURE;
}
#endif

/*
int getFormattedAddress(unsigned char * buf, unsigned char * addr)
{
	return  sprintf(buf, "%02x:%02x:%02x:%02x", addr[3], addr[2], addr[1], addr[0]);
}
*/

// Connects two ports.  Terminates connection if ~ESC is detected on portTerm.
// Upgraded 7-11-11 by Michael Aherne to send the character before checking if it's a terminator
// character.
//   -- This essentially means that if a few of these are strung together, sending a termination
//   string on either end
//      will terminate the entire chain, provided it hits the monitored termination port.
// Example: In the curent config, the MHX is conencted to the top PIC on port 1.  Each PIC talks to
// the other on Port 2.  And the lower PIC's userport is Port 1.
//   Therefore, to tie Lower Userport <--> OtherPPM <--> MHX, we would say:
//   f 132 1 2 2 // Tells upper PPM to connect MHX to Lower PPM, listening for a termination on the
//   lower ppm side. r 132 1 2 1 // Tells lower PPM to connect Userport to Upper PPM, listening for
//   a termination on the userport side.
//      To end this chain, ~(ESC) can be sent to the Userport, which will be forwarded to the upper
//      PPM (and sent out the MHX) before being checked and terminated.
void echoPorts(int portA, int portB, int portTerm = USER_PORT) {
	unsigned char data = 0;
	unsigned char Pdata = 0;

	// Validate
	if (portA == portB)
		return; // Can't echo to same port
	if (portA > NUM_PORTS || portB > NUM_PORTS || portTerm > NUM_PORTS)
		return; // Can't be greater than number of ports
	if (portA < 0 || portB < 0 || portTerm < 0)
		return; // Can't be less than zero.

	// Spin forever
	for (;;) {
		restart_wdt();

		// Get a byte from A
		if (getByte(portA, &data)) {
			// Send it to B
			sendByte(portB, data);

			// If the terminator port is A
			if (portTerm == portA) {
				// Check Termination Code
				if (data == 27 && Pdata == '~')
					break; // 27 is the escape character.  So to break use ~(esc)

				// Otherwise Save past value
				Pdata = data;
			}
		}

		// Get a byte from B
		if (getByte(portB, &data)) // sendByte(USER_PORT,data);
		{
			// Send it to A
			sendByte(portA, data);

			// If the terminator port is B
			if (portTerm == portB) {
				// Check Termination Code
				if (data == 27 && Pdata == '~')
					break; // 27 is the escape character.  So to break use ~(esc)

				// Otherwise Save past value
				Pdata = data;
			}
		}

		// If the termination port is neither A nor B
		if (portTerm != portA && portTerm != portB) {
			// Get a byte on the Termination port
			if (getByte(portTerm, &data)) {
				// Check Termination Code
				if (data == 27 && Pdata == '~')
					break; // 27 is the escape character.  So to break use ~(esc)

				// Otherwise Save past value
				Pdata = data;
			}
		}
	} // for

	return;
}

void echo(void) // Echo serial data from port to UserPort
{
	unsigned char RXdata, TXdata, Pdata, TXp, mode;
	int len, port;

	TXp = 0;
	clrscrn(USER_PORT); // clear screen we plan to use
	sprintf(Lbuf, "Entering Echo mode");
	placeString(USER_PORT, 2, 2, Lbuf);
	sprintf(Lbuf, "Type port(1-3),mode(a/b)");
	placeString(USER_PORT, 4, 2, Lbuf);
	locxy(USER_PORT, 5, 2);
	len = 0;
	TXdata = 0;
	while (TXdata != CR) {
		restart_wdt();
		if (getByte(USER_PORT, &TXdata) && TXdata != CR) {
			Lbuf[len] = TXdata;
			sendByte(USER_PORT, TXdata);
			len++;
		}
	}
	port = Lbuf[0] - '0';
	mode = Lbuf[1];
	if (port < 4 && (mode == 'a' || mode == 'b')) {
		clrscrn(USER_PORT); // clear screen we plan to use
		TXdata = 0;
		sendDBGALL(USER_PORT, "Ports connected.  Press ~(ESC)mm to quit to main menu.");
		for (;;) {
			restart_wdt();
			Pdata = TXdata; // save past value
			if (getByte(USER_PORT, &TXdata)) {
				if (TXdata == 27 && Pdata == '~')
					break; // 27 is the escape character.  So to break use ~(esc)
				sendByte(port, TXdata); // If no terminating character is used, send now
			}

			if (getByte(port, &RXdata)) // sendByte(USER_PORT,data);
			{
				if (mode == 'b') // Convert binary results in ASCII
				{
					sprintf(Lbuf, "%d ", RXdata);
					sendMSG(USER_PORT, Lbuf, strlen(Lbuf));
				} else
					sendByte(USER_PORT, RXdata); // Retain ASCII results in ASCII
			}
		}
	}
} // echo()

/*
unsigned char for_iDAC(unsigned char *addr1, unsigned char *ack) {
	static int n;

	*ack = (unsigned char) 0; // assume no ACK

	if (checkAddr(addr1, globals.RTU_ID) || (memcmp(addr1, globals.MAC_ADDR, 8)==0)) //== If
specifically for this iDAC
	{
		*ack = (unsigned char) 1; // Set ACK flag to enable ACK
		return 1;
	}

	for (n = 0; n < globals.nGroup; n++) // Check to see if this a Group command (beacons, beacon
requests, and broadcasts)
	{
	if (checkAddr(addr1, globals.Broadcast[n]))
			return 2;
	}

	return 0;
}
*/

#ifdef DEBUG
void showMainMenu(int port) {
	//	unsigned char arr[50];

	clrscrn(port);

	sprintf(Lbuf, showText1);
	placeString(port, 2, 1, Lbuf);
	sprintf(Lbuf, showText2);
	placeString(port, 3, 1, Lbuf);
	sprintf(Lbuf, showText3);
	placeString(port, 4, 1, Lbuf);
	sprintf(Lbuf, " ");
	placeString(port, 5, 1, Lbuf);
	sprintf(Lbuf, showText4);
	placeString(port, 6, 1, Lbuf);
	sprintf(Lbuf, showText5);
	placeString(port, 7, 1, Lbuf);
	sprintf(Lbuf, showText6);
	placeString(port, 8, 1, Lbuf);
	sprintf(Lbuf, showText7);
	placeString(port, 9, 1, Lbuf);
	sprintf(Lbuf, showText8);
	placeString(port, 10, 1, Lbuf);
	sprintf(Lbuf, showText9);
	placeString(port, 11, 1, Lbuf);
	sprintf(Lbuf, showText10);
	placeString(port, 12, 1, Lbuf);
	sprintf(Lbuf, showText11);
	placeString(port, 13, 1, Lbuf);
	sprintf(Lbuf, showText12);
	placeString(port, 14, 1, Lbuf);
	sprintf(Lbuf, showText13);
	placeString(port, 15, 1, Lbuf);
	sprintf(Lbuf, showText14);
	placeString(port, 16, 1, Lbuf);
	sprintf(Lbuf, " ");
	placeString(port, 18, 1, Lbuf);
	//	sprintf(Lbuf,showText16);
	//	placeString(port,19,1,Lbuf);

	sprintf(Lbuf, "Current Time");
	placeString(port, 22, 4, Lbuf);
}
#endif

/*
unsigned char checkAddr(unsigned char *addr1, unsigned char *addr2) {
	return memcmp(addr1, addr2, 4)==0;
}
*/

/*=================== UnParse ===================
  PURPOSE: Take a length-specified binary command and convert back to ASCII input
  UNMODIFIED VARIABLES: original
  MODIFIED VARIABLES: processed
  GLOBALS: none
  RETURN VALUE:  ReturnErr_t type, specifying errors or not.
  NOTES:   The returned string is null-terminated, and should not contain any embedded nulls.

	If processed is not big enough to handle the result, bad juju happens!

-----------------------------------------------------------*/
/*
ReturnErr_t UnParse(unsigned char* original, unsigned char* processed)
{
	// ==== NOT YET WORKING ===== //

	const int MAX_SIZE = 128;					// Maximum size of array we can process.

	int j = 0;
	int type = 0;
	unsigned char i = 0;
	int k = 0;
	unsigned char c;
	signed long clong;
	unsigned char temp[MAX_SIZE] = "";								// temporary buffer to avoid
overwriting if passed identical pointers unsigned char temp_num_convert[8] = "";
	//unsigned char debug[80] = "";

	// Validate input
	if(original == NULL) return INVALID_ARG;			// Null pointer
	if(processed == NULL) return INVALID_ARG;	     	// Null pointer
//	if(original[0]<0) return INVALID_ARG;				// Negative Command Length

	// Loop over the entire length of the command
	for(i=0;i<original[0];i++)
	{
		c = original[i+1];				// Assign temporary unsigned char
		// Classify the unsigned char
		if(c >= 65 && c <= 126) type = 1;	// All letters a-z,A-Z
		else if(0 == c) type = 3;			// False null termination
		else type = 2;						// anything else

		switch(type)
		{
			case 1:  // printable characters
				// copy it directly to the output
				// Be sure to increment the output counter
				temp[j]=c;
				j++;
				break;
			case 2: // non-printable
				// convert the unsigned char code to a string
				// Same process in reverse -- unsigned char --> signed long --> string
				// Make sure it's not <0 or >255.   (Is zero bad?  We're creating a null-terminated
string...?)
				// Be sure to update the location of ptr.
				clong = (signed long) c;
				sprintf(temp_num_convert,"%i",clong);			// convert to a number string
				k=0;
				//copy the number string to the output
				//sprintf(debug,"\r\n#= %s",temp_num_convert);
				//sendMSG(USER_PORT,debug,strlen(debug));
				while(temp_num_convert[k]!='\0')
				{
					temp[j] = temp_num_convert[k];
					k++;
					j++;
				}
				// Add a space after the number
				temp[j] = 32;
				j++;
				break;
			case 3: // False Null termination.  Doesn't convert correctly so has to be handled
separately.
				// Write a zero, then a space
				//sprintf(debug,"\r\nFound Zero.");
				//sendMSG(USER_PORT,debug,strlen(debug));
				temp[j] = '0';
				j++;
				temp[j] = 32;
				j++;
				break;
			default:  // serious problems
				return UNREACHABLE;
		}

	// Check to make sure we're still within our buffer array
	if(j>MAX_SIZE) return OUT_OF_BOUNDS;
	}//for

	// Validate that we wrote a string (that the writer pointer has moved)
	if(0==j) return FAILURE;

	// Set the null termination
	temp[j] = '\0';

	// Finally assign to return array (strcopy ok because it's a standard string)
	strcopy(processed,temp);		//!!! Compiler was having trouble with just
strcopy(processed,temp);, so I set j equal to the return.  Not sure why this works...

	//sprintf(debug,"\r\nReturned %s",processed);
	//sendMSG(USER_PORT,debug,strlen(debug));

	return SUCCESS;
}
*/

// This will be the interPPM checker.
// It's whole purpose is to find a packet, then process the command.
// Nonblocking, so comms can be really slow (300 baud) and it will still work.
ReturnErr_t check_PPM(int port) {
	// A packet is the following:
	// Sync, Length, Data (Command -- with no msg source or destination of its own), CRC

	static unsigned char state = 0; // Maintains the state of this function
	static unsigned char wr = 0; // Write Pointer
	static unsigned char pData = 0; // previous character found
	static unsigned char msg[MAX_PPM_PKT_SIZE]; // Place to hold messages as they build up
	static unsigned char len = 0;
	unsigned char crc[2];
	unsigned char data = 0; // current character found
	const unsigned char sync[2] = {0x4e, 0xff}; // The sync to look for
	static unsigned long timeout;

	// Possible States:
	// 0 -- Uninitialized (Action: Initialize)
	// 1 -- Initialized, looking for sync (Action: Find sync)
	// 2 -- Sync found, looking for length
	// 3 -- Length found, looking for end of packet
	// 4 -- Found everything, Check CRC and process the command

	switch (state) {
	// Unitialized
	case 0:
		//			sendDBGALL(USER_PORT,"\r\nInitializing...");
		wr = 0; // Init write pointer
		data = 0; // Init data
		pData = 0; // Init pData
		len = 0; // Reset length
		state = 1; // Set state to next

	// Initialized, no sync found
	case 1:
		while (getByte(port, &data) && state == 1) {
			//				HexPrint(USER_PORT,&data,1);
			//				sendByte(USER_PORT,'1');
			//				sprintf(dbgbuf,"%02x,%02x == %02x,%02x?",data,pData,sync[1],sync[0]);
			if (data == sync[1] && pData == sync[0]) {
				state = 2; // set to next state
				timeout = SEC_TIMER + 3; // begin timeout timer
				break; // break from while loop? (re-evaluating may grab another byte and discard
					   // the length!)
			}
			//				else if(data==27 && pData=='~') // 27 is the escape character.  So to
			// break use ~(esc)
			//				{
			//					echomode=0;
			//				}
			else {
				pData = data;
#ifdef DEBUG
// if(isprint(data))
//	sendByte(USER_PORT,data);	// Echo anything ascii to the local USER_PORT
#endif
			}
			if (state == 2)
				break; // really break from while loop
		}
		if (state != 2)
			return BUSY; // don't continue without finding sync first

	// Initialized, sync found, no length found
	case 2:
		//			sendDBGALL(USER_PORT,"\r\nLooking for Length...");
		if (getByte(port, &len)) {
			//				sendByte(USER_PORT,'2');
			msg[0] = len; // write it
			wr = 1; // increment counter
			state = 3; // move on
			//				HexPrint(USER_PORT,&len,1);
			//				sprintf(dbgbuf,"%i",(unsigned int)len);
			//				sendDBGALL(USER_PORT,dbgbuf);

			// Verify length is not too large. MAX_SIZE includes CRC and Length.  These three bytes
			// are not include in the length field.
			if (len > MAX_PPM_PKT_SIZE - 3) {
				sendDBGALL(USER_PORT, "\r\nRecieved too large of packet from other PPM.");
				state = 0; // bail on timeout
				return FAILURE;
			}
		} else if (SEC_TIMER > timeout) {
			sendDBGALL(USER_PORT, "\r\nTimeout in check_PPM (2).");
			state = 0; // bail on timeout
			return FAILURE;
		} else
			return BUSY; // or bail

	// Initialized, sync found, length found, read to end of packet (including CRC)
	case 3:
		//			sendDBGALL(USER_PORT,"\r\nLooking for Packet...");
		while (
			wr <
			len + 3) //+3 to account for CRC and Length Byte, neither of which is included in length
		{
			if (getByte(port, &msg[wr])) // if we get another byte
			{
				//					HexPrint(USER_PORT,&msg[wr],1);
				//					sendByte(USER_PORT,'3');
				wr++; // increment the local write pointer
			} else if (SEC_TIMER > timeout) {
				sendDBGALL(USER_PORT, "\r\nTimeout in check_PPM (3).");
				state = 0; // bail on timeout
				return FAILURE;
			} else
				return BUSY; // Nothing to read for now, come back later
		}
		state =
			4; // If we fall out of the while loop, we're done reading the packet.  Time to process.

	// Got a full packet.  Check the CRC and process the command
	// What should be in msg at this point is just Length and Cmd
	case 4:

		//			sendDBGALL(USER_PORT,"\r\nCheck CRC...");
		// First, get the CRC of what we've read
		getCRC(&msg[1], len, crc); // Get crc of msg (not including length byte)
		//			sprintf(dbgbuf,"%02x %02x == %02x %02x?",crc[0],crc[1],msg[len+1],msg[len+2]);
		//			sendDBGALL(USER_PORT,dbgbuf);

		// Compare CRC's
		if (msg[len + 1] != crc[0] || msg[len + 2] != crc[1]) // If either CRC doesn't match
		{
			//				sendDBGALL(USER_PORT,"\r\nInvalid CRC.");
			state = 0; // reset state
			return FAILURE; // get out of here
		}

		// We matched.  Let's process the command.
		// sprintf(dbgbuf,"\r\nCommand %u Rcvd.",msg[1]);
		// sendDBGALL(USER_PORT,dbgbuf);
		// HexPrint(USER_PORT,msg,msg[0]);

		// To process the command, we must go from the given format to the command format.
		// The current format is: Len,Cmd
		// The command format is: Len, 8 bytes source/dest, Cmd
		// To convert from one to the other, we will shift everything to the right by 8 bytes,
		// update the length and add source/destination info.
		PadCommand(msg);
		//			sendDBGALL(USER_PORT,"\r\nFull command in hex format: ");
		//			sendDBGALL(USER_PORT,"\r\nLEN|Source|Destination|Command... | CRC\r\n");
		//			HexPrint(USER_PORT,msg,msg[0]+3);
		processCmd(USER_PORT, globals.RTU_ID, msg, 1, 0); // send the cmd
		state = 0;
		return SUCCESS;

	default:
		state = 0;
		return FAILURE;
	} // switch
	return UNREACHABLE;
}

#ifdef __ITAG_H__
// Simple function to print out any bytes received from iTag.
ReturnErr_t check_ITAG(int port) {
	unsigned char temp;
	static unsigned char msg[GENERAL_BUFFER_SIZE];

	//	while(getByte(port,&temp))
	//	{
	//		HexPrint(USER_PORT,&temp,1);
	//	}

	// Let's fill up the buffer, and when it hits a certain size (or time?)
	// we send it to the ground.
	static unsigned char state = 0; // Maintains the state of this function
	static unsigned char wr = 0; // Write Pointer
	static unsigned char pData = 0; // previous character found
	static unsigned char len = 0;
	unsigned char data = 0; // current character found
	static unsigned long timeout;
	unsigned char header[GENERAL_BUFFER_SIZE];
	int headersize;

	// Possible States:
	// 0 -- Uninitialized (Action: Initialize)
	// 1 -- Initialized, looking for bytes (Action: Find first byte)
	// 2 -- Save and reset (Action: Packetize and Push, then reset)

	switch (state) {
	// Unitialized
	case 0:
		//			sendDBGALL(USER_PORT,"\r\nInitializing...");
		wr = 0; // Init write pointer
		data = 0; // Init data
		pData = 0; // Init pData
		len = 0; // Reset length
		state = 1; // Set state to next

	// Initialized, no bytes found
	case 1:
		if (getByte(port, &data)) // if we find something
		{
			//				HexPrint(USER_PORT,&data,1);
			//				sendDBGALL(USER_PORT,"Chk1");
			//				sendByte(USER_PORT,'1');
			//				sprintf(dbgbuf,"%02x,%02x == %02x,%02x?",data,pData,sync[1],sync[0]);
			msg[wr] = data; // set first character
			wr++;
			state = 2; // set to next state
			timeout = SEC_TIMER + 3; // begin timeout timer
		}
		break;

		// Initialized
	case 2:
		// Create the Header
		sprintf(header, "iTag ");
		getStringTime(time, &header[strlen(header)]);
		headersize = strlen(header);
		sprintf(&header[headersize], ">");
		headersize = strlen(header);

		while (wr < GENERAL_BUFFER_SIZE - headersize - 15 &&
			   SEC_TIMER < timeout) //-15 to account for packetizing (which should take 13 bytes in
									// current version of getPacketized())
		{
			if (getByte(port, &msg[wr])) // if we get another byte
			{
				//					sendDBGALL(USER_PORT,"Chk2");
				//					HexPrint(USER_PORT,&msg[wr],1);
				//					sendByte(USER_PORT,'3');
				wr++; // increment the local write pointer
			} else
				return BUSY; // Nothing to read for now, come back later
		}
		state =
			3; // If we fall out of the while loop, we're done reading the packet.  Time to process.

	// Got enough to go.  PacketizeAndPush as Misc!
	case 3:
		//			sendDBGALL(USER_PORT,"Chk3");
		memmove(&msg[headersize], &msg[0],
				GENERAL_BUFFER_SIZE - headersize); // Make room for header
		memcpy(msg, header, headersize); // Copy header
		sprintf(dbgbuf, "iTag: %u Bytes ", wr);
		sendDBGALL(USER_PORT, dbgbuf);
		//			HexPrint(USER_PORT,msg,wr+5);
		PacketizeAndPush(msg, wr + headersize, MISC, GENERAL_BUFFER_SIZE, MISC_PACKET, PPM_NUM);
		state = 0;
		return SUCCESS;

	default:
		state = 0;
		return FAILURE;
	} // switch
	return UNREACHABLE;

	return SUCCESS;
}

#endif

/*
#ifdef __STENSAT__
// Simple function to print out any bytes received from Beacon.
ReturnErr_t check_Stensat(int port)
{
	unsigned char temp;

	while(getByte(port,&temp))
	{
		HexPrint(USER_PORT,&temp,1);
	}

	return SUCCESS;
}
#endif
*/

/*
// Check the itag for packets (non-blocking)
// It's whole purpose is to find a packet, then do something with that packet.
// Nonblocking, so comms can be really slow (300 baud) and it will still work.
ReturnErr_t check_ITAG(int port)
{
// According to Fred (on 10/21/2010) the max complete packet size is 128 bytes.

// A packet is the following:
// Sync 		- 2 bytes 	- 0xff 0x5e
// Tx counter 	- 1 byte	- Continually increasing
// Address?		- 4 bytes	- Always 0x00 0x00 0x00 0x00
// Length		- 1 byte	- Maximum 128 per Fred on Telecon.  This is the length of the data field
// Data			- variable	- Contains all data bytes.  The Length packet field refers to the length
of this data.
// CRC			- 2 bytes	- Assume uses the same CRC algo as everything else.

	#define MAX_ITAG_PACKET_SIZE 		128
	static unsigned char state=0;						// Maintains the state of this function
	static unsigned char wr=0;							// Write Pointer
	static unsigned char pData=0;						// previous character found
	static unsigned char msg[MAX_ITAG_PACKET_SIZE];		// Place to hold messages as they build up
	static unsigned char len=0;
	unsigned char crc[2];
	unsigned char data=0;								// current character found
	const unsigned char sync[2]={0xff,0x5e};				// The sync to look for
	static unsigned long timeout;
	#undef MAX_ITAG_PACKET_SIZE

	// Possible States:
	// 0 -- Uninitialized (Action: Initialize)
	// 1 -- Initialized, looking for sync (Action: Find sync)
	// 2 -- Sync found, looking for length
	// 3 -- Length found, looking for end of packet
	// 4 -- Found everything, Check CRC and process the command

	switch(state)
	{
		// Unitialized
		case 0:
			//sendDBGALL(USER_PORT,"\r\nInitializing...");
			wr=0;		// Init write pointer
			data=0;		// Init data
			pData=0;	// Init pData
			len=0;		// Reset length
			state=1;	// Set state to next

		// Initialized, no sync found
		case 1:
			while(getByte(port,&data) && state==1)
			{
				HexPrint(USER_PORT,&data,1);
				//sendByte(USER_PORT,'1');
				//sprintf(dbgbuf,"%02x,%02x == %02x,%02x?",data,pData,sync[1],sync[0]);
				if(data==sync[1] && pData==sync[0])
				{
					//sendDBGALL(USER_PORT,"\r\nFound sync!");
					state=2;	// set to next state
					timeout=SEC_TIMER+3;	// begin timeout timer
					break;		// break from while loop? (re-evaluating may grab another byte and
discard the length!)
				}
//				else if(data==27 && pData=='~') // 27 is the escape character.  So to break use
~(esc)
//				{
//					echomode=0;
//				}
				else
				{
					pData=data;
					#ifdef DEBUG
					//if(isprint(data))
					//	sendByte(USER_PORT,data);	// Echo anything ascii to the local USER_PORT
					#endif
				}
				if(state==2) break;		// really break from while loop
			}
			if(state!=2) return BUSY;	// don't continue without finding sync first

		// Initialized, sync found, no length found
		case 2:
			// If we are still looking for Source Address bytes and TX Counter (5 bytes)
			if(wr<5)
			{
				//sendDBGALL(USER_PORT,"\r\nLooking for Source Address and TX Code...");

				// Try to get one byte of the Source Address
				if(getByte(port,&data))
				{
					//HexPrint(USER_PORT,&data,1);

					// Increment the counter.
					wr++;

					return BUSY;
				}
				else if(SEC_TIMER>timeout)
				{
					sendDBGALL(USER_PORT,"\r\nTimeout waiting for iTag length.");
					state=0;			// bail on timeout
					return FAILURE;
				}
				else
					return BUSY;				// or bail
			}
			else
			{
				//sendDBGALL(USER_PORT,"\r\nLooking for Length...");
				if(getByte(port,&len))
				{
					//sendByte(USER_PORT,'2');
					msg[0]=len;		// write it
					wr=1;				// increment counter
					state=3;			// move on
					//HexPrint(USER_PORT,&len,1);
					//sprintf(dbgbuf,"%i",(unsigned int)len);
					//sendDBGALL(USER_PORT,dbgbuf);
				}
				else if(SEC_TIMER>timeout)
				{
					sendDBGALL(USER_PORT,"\r\nTimeout waiting for iTag length.");
					state=0;			// bail on timeout
					return FAILURE;
				}
				else
					return BUSY;				// or bail
			}

		// Initialized, sync found, length found, read to end of packet (including CRC)
		case 3:
			//sendDBGALL(USER_PORT,"\r\nLooking for Packet...");
			while(wr<len+3) //+3 to account for CRC and Length Byte, neither of which is included in
length
			{
				if(getByte(port,&msg[wr]))		// if we get another byte
				{
					//HexPrint(USER_PORT,&msg[wr],1);
//					sendByte(USER_PORT,'3');
					wr++;						// increment the local write pointer
				}
				else if(SEC_TIMER>timeout)
				{
					sendDBGALL(USER_PORT,"\r\nTimeout waiting for rest of iTag packet.");
					state=0;			// bail on timeout
					return FAILURE;
				}
				else
					return BUSY;						// Nothing to read for now, come back later
			}
			state=4;							// If we fall out of the while loop, we're done
reading the packet.  Time to process.

		// Got a full packet.  Check the CRC and process the command
		// What should be in msg at this point is just Length and Cmd
		case 4:

			//sendDBGALL(USER_PORT,"\r\nCheck CRC...");
			// First, get the CRC of what we've read
			getCRC(&msg[1],len,crc);			// Get crc of msg (not including length byte)
			//sprintf(dbgbuf,"%02x %02x == %02x %02x?",crc[0],crc[1],msg[len+1],msg[len+2]);
			//sendDBGALL(USER_PORT,dbgbuf);

			//Compare CRC's
			if(msg[len+1]!=crc[0] || msg[len+2]!=crc[1])	// If either CRC doesn't match
			{
				sendDBGALL(USER_PORT,"\r\nInvalid CRC of iTag packet.");
				state=0;	// reset state
				return FAILURE;		// get out of here
			}

			// We matched.  Let's process the command.
			//sendDBGALL(USER_PORT,"\r\nValid Packet from iTag Rcvd: ");
			//HexPrint(USER_PORT,msg,msg[0]);

			// Place full data field onto MISC stack and label with packet type 1 (PAYLOAD)
			PacketizeAndPush(msg,msg[0],MISC,DBG_BUF_SIZE,PLY_PACKET,PPM_NUM);

			//sendDBGALL(USER_PORT,"\r\nI (((");

			state=0;
			return SUCCESS;

		default: state=0; return FAILURE;
	}//switch
	return UNREACHABLE;
}
*/

#ifdef UPPER_PPM
void getAndStorePayloadData() {
	char payloadData[MAX_PAYLOAD_DATA_SIZE] = {0};
	uint16_t payloadDataSize =
		getAvailablePayloadData(&gPayloadManager, payloadData, sizeof(payloadData));

	if (payloadDataSize > 0) {
		savePayloadData(&gPayloadManager, payloadData, payloadDataSize);
	}
	return;
}

void getAndProcessIncomingCommand() {
	char command[300] = {0};
	uint16_t commandSize = getAvailableCommand(&gLinkManager, command, sizeof(command));

	if (commandSize > 0) {
		if (ParseCommandString((unsigned char*)command, (unsigned char*)command) == SUCCESS) {
			PadCommand(command);
			if (command[0] >= 9) {
				processCmd(USER_PORT, globals.Dest_ID, command, 1, 0);
				return;
			}
		}
	}
	return;
}

void sendPayloadDataWhenReady() {
	sendAvailablePayloadDataWhenReady(&gPayloadManager);
	return;
}
#endif

#ifdef __MHX__
void check_LAN(int port) {
	// int n;
	const int MAX_SIZE = 256;
	int CRClen;
	int len;
	unsigned char pData, data, protocol, ACK;
	unsigned char valid;
	unsigned char TX_Code;
	unsigned char msg[MAX_SIZE];
	// unsigned int wait;

	unsigned char msgSource[8], msgDest[8], encrypt; // MRA added to allow it to compile
	unsigned char PScnt; // MRA added to allow it to compile

	// wait=globals.Ttail;
	// wait=400;

	if (getByte(port, &pData)) //== if data is detected in receive buffer
	{
		// sendMSG(USER_PORT,&pData,1);  // DEBUG
		// sendDBGALL(USER_PORT,"Incoming Data on LAN");

		PScnt = 0; // reset PowerSave
		memset(msg, 0, MAX_SIZE);

		//		if(wait)delay_ms(wait);		//== If CD is not used, wait for rest of message
		CRClen = 0; // Reset CRC length
		valid = 0; // assume bad telemetry to begin with
		protocol = 0; // don't assume a particular protocol
		len = 1;
		msg[0] = 252; // Assume large initial message length to encourage persistant scan

		while (len < msg[0] + 3 && len >= 0 &&
			   len < MAX_SIZE) //== Look for start of frame to determine protocol (include CRC)
		{
			// sendDBGALL(USER_PORT,":");
			if (getByte(port, &data)) //== If data is in buffer
			{
				// sendMSG(USER_PORT,&pData,1);  // DEBUG

				if (protocol) //== If sync'ed to a protocol
				{
					msg[len] = data; // Begin saving message
					len++;
				} else //== If not sync'ed to a protocol, keep trying
				{
					// Received Acknowledge Packet
					if (pData == 0xff && data == 0x5e) {
						protocol = 'I';
						getByte(port, &msg[0]);
					} // Received iControl Start of frame
					pData = data; // Save previous byte
				}
			} else
				break; //== Bail out if no data
		} //== scanning for message

		// if(debugMSGFlag)showMSG('R',port,protocol,msg);		// Used for debugging
		if (len < 8 || protocol == 0 || len >= MAX_SIZE)
			return; // Too short a message to be valid

		encrypt = protocol & 0x80; // MRA using same code from other areas -- hope it works

		switch (protocol) //== Process Message based on Network or detected protocol
		{
		case 'I': //== iControl Network Protocol
			CRClen = msg[0];
			memcpy(msgSource, msg + 1, 4); // Save message source
			memcpy(msgDest, msg + 5, 4); // Save message destintaion
			TX_Code = msg[9];

			// if(for_iDAC(msgDest,&ACK))			//== For this iDAC
			//{
			ACK = 1; // Always ack anything received.

			// Data Quality Tester
			// When quality = 0, always true
			// When quality = 100, never true
			//	if(!(rand()<=up_connection_quality*RAND_MAX/100)) return;

			if (checkData(msg)) // Check CRC, remove encryption,and time pads
			{
				// if(TX_Code==0x00 || TX_Code==64 && msg[10]==2) 	// Received ACK or Command
				// request from an iDAC

				if (TX_Code == AckFlag) // Did we receive the correct AckFlag?
				{
					int temp;
					temp = msg[0] - 8;
					if (temp >= LAST_CMD_SIZE)
						temp = LAST_CMD_SIZE;
					memset(g_last_cmd, 0xff, LAST_CMD_SIZE); // Clear the last command
					memcpy(g_last_cmd, msg + 9, temp); // Make a copy of the command for logging
					if (AckFlag == 0x01)
						AckFlag = 0x00;
					else
						AckFlag = 0x01; // Flips the AckFlag
					processCmd(port, msgSource, msg, 1, ACK); // Now process requested command
					return;
				} else {
					sendDBGALL(USER_PORT, "\r\nNot in Sync. No Command Executed.");
					// HexPrint(USER_PORT,&AckFlag,sizeof(AckFlag));
					// HexPrint(USER_PORT,&TX_Code,sizeof(TX_Code));
				}
			} // Valid checkData

			//}			// If for this iDAC
			break;

		default: //== Let User know there is a problem
			return;
			break;
		} // Protocol Type
	} // if getByte
	else //==if no data to receive
	{
	}
} // end of monitoring RF interface
#endif // __MHX__

//=============================================================================
unsigned char checkData(unsigned char* buf) {
	unsigned char retval;
	unsigned char CRC[2];
	int len;

	retval = 0;
	len = buf[0];

	// A command looks like this:
	// Name:  Length Source Destination TX_CODE Time    CMD   CRC
	// Size:    1      4         4         1      7   Varies   2

	getCRC(buf + 1, len, CRC);

	// This section sends the data being checked to the screen
	// sendDBGALL(USER_PORT,"Cmd Rcv\r\n");
	/*
	{
	//clrscrn(USER_PORT);
	//locxy(USER_PORT,1,1);
	sendDBGALL(USER_PORT,"\r\n\r\n\r\n");
	sendDBGALL(USER_PORT,"Command Received at ");
	sprintf(Lbuf,"%i/%i/%i %i:%i:%i",time[0],time[1],time[2],time[4],time[5],time[6]);
	sendDBGALL(USER_PORT,Lbuf);
	sendDBGALL(USER_PORT,"\r\nLen | ---- Source ----  |  ---Destination-- | TX |\r\n");
		int i;
		for(i=0;i<10;i++)
		{
			sprintf(Lbuf,"0x%2x ",buf[i]);
			sendDBGALL(USER_PORT,Lbuf);
			//if((i+1)%16==0) sendDBGALL(USER_PORT,"\r\n");  //return cursor every ith time.
		}
	sendDBGALL(USER_PORT,"\r\n");
	sendDBGALL(USER_PORT,"\r\n|-------------- Time ------------ | Command & CRC ---> \r\n");
		for(i=10;i<len+3;i++)
		{
			sprintf(Lbuf,"0x%2x ",buf[i]);
			sendDBGALL(USER_PORT,Lbuf);
			//if((i+1)%16==0) sendDBGALL(USER_PORT,"\r\n");  //return cursor every ith time.
		}

	sprintf(dbgbuf,"\r\n\r\nCRC should be 0x%2x 0x%2x \r\n",CRC[0],CRC[1]);
	sendDBGALL(USER_PORT,dbgbuf);
	}
	//------------------ End CheckData Display Section -----------------------
	*/

	if (CRC[0] == buf[len + 1] && CRC[1] == buf[len + 2])
		retval = 1; //==If valid
	else {
		placeString(USER_PORT, 24, 2, "RX: Bad command or checksum           ");
		return retval;
	}

	switch (buf[9]) // TX_code
	{
	case 0x00: // TX_Code 0x00 is a command
	case 0x01: // TX_Code 0x01 is a command too
		memcpy(buf + 9, buf + 17, buf[0] - 16); // remove TX_Code and time pad (shift left)
		buf[0] -= 8; // Now 8 bytes shorter
		break;
	default: // Telemetry or ACK, do nothing
		break;
	}
	return retval;
}

// Display data from flash onto a port.
void flashReadTest(unsigned long address, int port) {
#ifdef __FLASH_DRIVER__
	unsigned char tempad[3];
	int i;
	unsigned int sector, block, page;

	// Clear Screen
	if (port == USER_PORT) {
		clrscrn(USER_PORT);
		locxy(USER_PORT, 1, 1); // put the cursor in the upper left
	}

	// Determine Addresses, Sectors, etc and print header
	LongTo3Chars(address, tempad);
	sector = FlashAddrToSector(address);
	block = FlashAddrToBlock(address);
	page = FlashAddrToPage(address);
	sprintf(Lbuf, "Data from Flash address %li (0x%x%x%x) S%01u/B%01u/P%01u:\r\n", address,
			tempad[0], tempad[1], tempad[2], sector, block, page);
	sendDBGALL(port, Lbuf);
	unsigned long begins, ends, beginb, endb, beginp, endp;
	SectorToFlashAddr(sector, &begins, &ends);
	BlockToFlashAddr(block, &beginb, &endb);
	PageToFlashAddr(page, &beginp, &endp);
	sprintf(Lbuf, "Sector %01u %01u-->%01u / Block %01u %01u-->%01u / Page %01u %01u-->%01u \r\n",
			sector, begins, ends, block, beginb, endb, page, beginp, endp);
	sendDBGALL(port, Lbuf);

	// Read data from flash
	flashRead(address, DBG_BUF_SIZE, dbgbuf);

	// Print it to screen
	for (i = 0; i < DBG_BUF_SIZE; i++) {
		sprintf(Lbuf, "0x%2x ", dbgbuf[i]);
		sendDBGALL(port, Lbuf);
	}
#endif
}

// This reads the PICs native stack size.
/*
void ReadStackSz()
{
	sprintf(dbgbuf,"\r\nStack = %u",getenv("STACK"));
	sendDBGALL(USER_PORT,dbgbuf);
	delay_ms(500);
}
*/

void Init_Handlers() {
	// ReturnErr_t response;

#define FUNCTION_DBG_LEVEL 101

	sendDBGALL(USER_PORT, "\r\n\tFunction: Init Handlers", FUNCTION_DBG_LEVEL);

// Initialize Beacon Data Handlers
#ifdef __MYSTACK__
#ifdef UPPER_PPM
	stack_init(BEA);
#endif
#endif

// Set up the Beacon Stack TX/Flash Handler
#ifdef __TXMANAGE__
	initNBHandler(&BEA_nb, (unsigned int)&BEA_data);
	TX_initStruct(&BEA_data, (unsigned int)BEA, NULL, &TelemWriteAddr, 3, 1, PAYLOAD_DATA_ADDR,
				  PAYLOAD_DATA_ALLOC_SIZE);
#endif

// Initialize Aeneas Stack
#ifdef __MYSTACK__
//	stack_init(AEN);
#endif

	// Initialize Miscellaneous Stack Handler
#ifdef __MYSTACK__
	stack_init(MISC);
#endif

// Set up the Misc Stack TX/Flash Handler
#ifdef __TXMANAGE__
	initNBHandler(&MISC_nb, (unsigned int)&MISC_data);
	TX_initStruct(&MISC_data,
				  (unsigned int)MISC, // table pointer
				  NULL, // read pointer
				  &MiscWriteAddr, // flash r/w pointer
				  3, 1, // wait, stackmode
				  MISC_ADDR, // Flash start address
				  MISC_ALLOC_SIZE); // Flash Size allocation
#endif

#undef FUNCTION_DBG_LEVEL

	// Setup the transfer handler for connecting lower and upper stacks
#ifdef __LOWER_TX__
	initTX_Properties(&LOWER_TX_HANDLER, MISC, PACKETIZING_OFF, MISC_PACKET, 0,
					  0); // Last 2 arguments are the reserved bytes
#endif

#ifdef UPPER_PPM
	LinkManager__init(&gLinkManager);
	FlashManager__init(&gFlashManager);
	PayloadManager__init(&gPayloadManager);
#endif
}

// Initializes the Flash Memory between lower and upper bounds.
// Does this by finding the first empty page within the bounds.
void Init_Flash(unsigned long* RWptr, unsigned long Lower_Boundary, unsigned long Upper_Boundary,
				unsigned char findedge = 0) {
#ifdef __FLASH_DRIVER__
#define FUNCTION_DBG_LEVEL 101

	// Variables for storing addresses
	unsigned long temp, templow;

	// sprintf(dbgbuf,"\r\nLower Boundary = %u",temp);
	// sendDBGALL(USER_PORT,dbgbuf,FUNCTION_DBG_LEVEL);

	sendDBGALL(USER_PORT, "\r\n\tFunction: Init_Flash()", FUNCTION_DBG_LEVEL);
	sendDBGALL(USER_PORT, ".", FUNCTION_DBG_LEVEL);

	// Loops across all sectors within the bounds and unprotects them 1 at a time.
	while (temp < Upper_Boundary && temp >= Lower_Boundary) {
		flashSectorProtectDisable(temp);
		temp += FLASH_SECTOR_SIZE;
	}

	sendDBGALL(USER_PORT, ".", FUNCTION_DBG_LEVEL);

	// Find the first empty page and set the Read/Write Pointer to it.
	// Initialize the Read/Write Pointer for Plymouth Data
	if (findedge) {
		unsigned int page;
		if (flashFindEmptyPage(FlashAddrToPage(Lower_Boundary), FlashAddrToPage(Upper_Boundary),
							   &page) != SUCCESS) {
			// if no empty page fonud, put the read/write pointer at the very end of the allocation
			*RWptr = Upper_Boundary - 1;
			// sendDBGALL(USER_PORT,"\r\nFailed to find empty page");
		} else {
			// If you find an emptypage, put the read/write pointer at the beginning of it.
			PageToFlashAddr(page, &templow, &temp);
			*RWptr = templow;
			// sprintf(dbgbuf,"\r\nFirst Empty Page In Plymouth Allocation = %u,%u",templow,*RWptr);
			// sendDBGALL(USER_PORT,dbgbuf);
		}
	} // if findedge
	else
		*RWptr = Lower_Boundary;

#undef FUNCTION_DBG_LEVEL
#endif //__FLASH_DRIVER__
}

// Communications Function
// Sends a given message in packetized form
void sendPACKETIZED(int port, unsigned char* msg, int len, unsigned char pkt_type = 1) {
	const unsigned char sync[] = "CAERUS";
	unsigned char* cpy;
	unsigned char wr[12];
	int t_len;
	unsigned char crc[2];
	unsigned int pageaddr = 0x4D41;
	//	int i;

	t_len = len + 1 + 2; // Len + type + page

	// This is what a packet looks like
	// [----------- HEADER ----------------][-- DATA --][-- FOOTER --]
	// (sync) (t_length) (type) (page addr)    (data)       (CRC)
	//   (6)     (2)     (1)       (2)         (len)         (2)

	// Type is a format specifier
	// Length includes everything after it except CRC (the type, page addr, and DATA)

	//----- Build CRC ----------
	// CRC the type (restart high)
	getCRCCont(CRC_RESTART, &pkt_type, 1, crc);

	// CRC the page address (continuing)
	getCRCCont(CRC_CONTINUE_PREVIOUS, &pageaddr, 2, crc);

	// CRC the data (continuing)
	getCRCCont(CRC_CONTINUE_PREVIOUS, msg, len, crc);

	//----- Build Header--------

	// Copy the sync
	cpy = wr;
	memcpy(cpy, sync, 6);
	cpy += 6; // increment pointer

	// Copy the length
	memcpy(cpy, &t_len, 2); // copies length
	cpy += 2; // increment pointer

	// Copy the type
	memcpy(cpy, &pkt_type, 1); // copies type
	cpy += 1; // increment pointer

	// Copy the page address
	memcpy(cpy, &pageaddr, 2); // copies page address
	cpy += 2; // increment pointer

	//===== TRANSMIT ===============
	// Send the header
	sendMSG(port, wr, 11);

	// Send the data
	sendMSG(port, msg, len);

	// Send the crc
	sendMSG(port, crc, 2);
}

// getPACKETIZED(Lbuf,wrPtr,rbcmd[cmd1],PPM_NUM,GENERAL_BUFFER_SIZE);

// Converts a given message to packetized form
// Inputs: The msg.  The length of the message in len (which is modified to be
// the length of the returned message.  The type of packet. The page address to include
// in the header.  The maximum size allowed to be written by the function.
void getPACKETIZED(unsigned char* msg, int* len, unsigned char pkt_type = 1,
				   unsigned int pageaddr = 0x574D, int MAX_SIZE = 0) {
	const unsigned char sync[] = "USCLAJ";
	unsigned char* cpy;
	int t_len;
	unsigned char crc[2];
	//	signed int i;

	t_len = *len + 1 + 2; // Len + type + page

	if ((6 + 2 + 1 + 2 + *len + 2) >= MAX_SIZE)
		return; // Validate the array size
	// This is what a packet looks like
	// [----------- HEADER ----------------][-- DATA --][-- FOOTER --]
	// (sync) (t_length) (type) (page addr)    (data)       (CRC)
	//   (6)     (2)     (1)       (2)         (len)         (2)

	// Type is a format specifier
	// Length includes everything after it except CRC (the type, page addr, and DATA)

	// Shift the data to the right by 11 characters (to make room for the header)
	memmove(&msg[11], &msg[0], *len);
	/*
	for(i = *len;i>=0;i--)
	{
		msg[i+11]=msg[i];
	}
	*/

	// Copy the sync
	cpy = msg;
	memcpy(cpy, sync, 6);

	// Copy the length
	cpy = cpy + 6; // increment pointer
	memcpy(cpy, &t_len, 2); // copies length

	// Copy the type
	cpy = cpy + 2; // increment pointer
	memcpy(cpy, &pkt_type, 1); // copies packet type

	// Copy the page address (empty for now)
	cpy = cpy + 1; // increment pointer
	memcpy(cpy, &pageaddr, 2); // copies page address

	// OLD: Copy the data
	// cpy = cpy + 2;			// increment pointer
	// memcpy(cpy,msg,*len);	// copies data

	// Get the CRC
	cpy = msg + 8; // Sets the pointer to just after the t_length byte
	getCRC(cpy, t_len, crc); // Gets the CRC

	// Copy the CRC
	cpy = msg + 8 + t_len; // Sets the pointer to where the CRC goes
	memcpy(cpy, crc, 2); // Copies the crc

	// Adjust length
	*len = (8 + t_len + 2);
	// memcpy(msg,Lbuf,*len);

	return;
}

// Will print 1's and 0's for each block. 2048 total. 1 is empty, 0 is non-empty.
void flashPrintOccupiedBlocks(
	int port = 0,
	unsigned char ascii =
		1) // Sends a listing of occupied blocks (in either ascii or binary) to a specified port
{
	/*
		signed int32 i,j;
		unsigned char c;
		unsigned long addr;
		unsigned char tmp[81];

		memset(tmp,0x41,sizeof(tmp));
		addr=0;

		if(ascii)
		{
			for (i=0;i<FLASH_MAX_BLOCKS;i++)
			{
				restart_wdt();
				j=i%80;
				c=CheckFlashEmpty(addr,(unsigned int)FLASH_BLOCK_SIZE);
				sprintf(tmp+j,"%i",c);
				if(j>=79 || i==FLASH_MAX_BLOCKS-1)
				{
					if(port==MHX_PORT) sendPACKETIZED(port,tmp,sizeof(tmp),0);
					else sendMSG(port,tmp,sizeof(tmp));
				}
				if(i+1==FLASH_MAX_BLOCKS-1) addr+=FLASH_BLOCK_SIZE-1;  // Correction for the very
	   last block else addr+=FLASH_BLOCK_SIZE;

				if(addr>MAX_FLASH_ADDR) return;  	// Safety validation
			}
		}
		else //non-ascii
		{
			unsigned char hx;
			unsigned char shift;

			for (i=0;i<FLASH_MAX_BLOCKS;i++)
			{
				c=CheckFlashEmpty(addr,(unsigned int)FLASH_BLOCK_SIZE);
				shift=7-(i%8);
				if(shift==7) hx = 0x00;
				hx=hx|(c<<shift);
				if(shift==0)
				{
					sprintf(tmp,"%2x ",hx);
					sendDBGALL(port,tmp);
				}
				if(i+1==FLASH_MAX_BLOCKS-1) addr+=FLASH_BLOCK_SIZE-1;	// Small correction for the
	   last block so we don't loop around else addr+=FLASH_BLOCK_SIZE; if(addr>MAX_FLASH_ADDR)
	   return;
			}
		}
	*/
	return;
}

ReturnErr_t IncrementRebootCounter() {
#ifdef __FLASH_DRIVER__
	unsigned long i;
	unsigned long addy;

	// This loops over the block, looking for a set of 2 empty bytes.
	for (i = 0; i < FLASH_BLOCK_SIZE - 1; i = i + 2) {
		//		sprintf(dbgbuf,"\r\n%i",i); sendDBGALL(USER_PORT,dbgbuf);
		if (CheckFlashEmpty(RESERVED_ADDR + i, 2))
			break;
	}
	// i now contains the location (offset from RESERVED_ADDR) of the first empty set of bytes in
	// flash, or is >= FLASH_BLOCK_SIZE if it could not find an empty spot.
	addy = RESERVED_ADDR + i - 2;

	// Read the previous value
	if (i >= 2)
		flashRead(addy, sizeof(num_reboots),
				  &num_reboots); // For values of i greater than first iteration
	else
		num_reboots = 0; // For first iteration.

	// Validate no data corruption
	if (num_reboots < 0)
		num_reboots = 0;

	// Increment the counter
	num_reboots++;

	// Now increment the value in flash

	// First disable protection
	flashSectorProtectDisable(RESERVED_ADDR);

	// Next check if we are at the end of the block, because we'll need to erase the block and start
	// over in that case
	if (i >= FLASH_BLOCK_SIZE - 1) {
		flashEraseBlockByAddr(RESERVED_ADDR);
		i = 0;
	}

	// Finally, write the new value
	ReturnErr_t err;
	err = flashWriteSafe(RESERVED_ADDR + i, sizeof(num_reboots), &num_reboots, RESERVED_ADDR,
						 RESERVED_ADDR + FLASH_BLOCK_SIZE - 1);
	return err;
#else
	return FAILURE;
#endif //__FLASH_DRIVER__
}

void ResetRebootCounter() {
#ifdef __FLASH_DRIVER__
	num_reboots = 0;
	flashSectorProtectDisable(RESERVED_ADDR);
	flashEraseBlockByAddr(RESERVED_ADDR);
	flashWriteSafe(RESERVED_ADDR, sizeof(num_reboots), &num_reboots, RESERVED_ADDR,
				   RESERVED_ADDR + FLASH_BLOCK_SIZE - 1);
#endif
}

#ifdef __MYSTACK__
// Takes a given data, length, pkt_type and page_addy and will packetize data and push it onto a
// stack.  Also takes in max_len, which is used during packetization to make sure the data buffer
// can handle the header and footer being added to it.
// MODIFIES THE DATA GIVEN TO IT!
ReturnErr_t PacketizeAndPush(unsigned char* msg, int len, stack_t* S, int max_len = 0,
							 int pkt_type = 0, int page_addy = 0) {
	int templen;
	if (len > max_len)
		return FAILURE;

	templen = len; // Use a temporary variable for length
	getPACKETIZED(msg, &templen, pkt_type, page_addy, max_len); // Packetize the data
	// DEBUG ------------
	//	sprintf(dbgbuf,"\r\nPacket Type = %i",pkt_type);
	//	sendDBGALL(USER_PORT,dbgbuf);
	//	HexPrint(USER_PORT,msg,templen);
	// --------------
	if (stack_push(S, msg, templen) != SUCCESS) // Push the data onto the stack
		return SUBFUNCTION_ERR;

	return SUCCESS;
}
#endif

#ifdef __MAESTRO__
#ifdef __MYSTACK__
void MaestroPacketizeAndPush(unsigned char* msg, int len, int max_len) {
	PacketizeAndPush(msg, len, MISC, max_len, MISC_PACKET, PPM_NUM);
}
#endif // __MYSTACK__
#endif // __MAESTRO __

#ifdef __MYSTACK__
void stack_print(int port, stack_t* S) {
	int i;
	unsigned char dbg[4];

	for (i = 0; i < S->num_items; i++) {
		// Numbering
		sprintf(dbg, "\r\n%i.  ", i);
		sendDBGALL(port, dbg);

		// Data
		if (*(S->data + (i * S->itemsize)) == 0) {
			sendDBGALL(port, "-=Empty=-");
		} else {
			HexPrint(port, (S->data + (i * S->itemsize) + 1), *(S->data + (i * S->itemsize) + 0));
			// sendMSG(USER_PORT,(S->data+(i*S->itemsize)+1),*(S->data+(i*S->itemsize)+0));
		}
	}
}
#endif

/*
// Repeated calls will concatenate a string onto a base pointer, until the total length
// exceeds a specified amount, at which point it will send out the message
int stringCatAndSend(int port, unsigned char * base, unsigned char * add, int * i, int len, short
force_send=0)
{
	if(force_send)
	{
		i=0;
		i+=strlen(add);

		// If the first string to add is too big, just send it off
		if(i>len)
		{
			sendPACKETIZED(port,msg,strlen(msg),MISC_PACKET,PPM_NUM);
			i=0;
			return 1;	// if trying to
		}

		// If the first string to add is small, then add it.
		sprintf(&base[0],add);
		return;
	}

	// Repeat calls
	else
	{
		i+=strlen(add);

		// If the string to add overflows the buffer, send the buffer off
		if(i>len)
		{
			sendPACKETIZED(port,msg,strlen(msg),MISC_PACKET,PPM_NUM);
			i=0;
			return 1;	// if trying to
		}

		if(i!=0) sprintf(&base[i-1],add);
	}
}
*/

void showSchedulePacketized() {
	unsigned char mask = 'x';
	int m;
	int i, len;
	unsigned char ran = 'x';
	unsigned char* p;

#ifdef __MYSTACK__
#ifdef UPPER_PPM
	sprintf(Lbuf, "UPPER PPM Stored Schedule:");
#else
	sprintf(Lbuf, "LOWER PPM Stored Schedule:");
#endif
	//	sendDBGALL(USER_PORT,Lbuf);
	PacketizeAndPush(
		Lbuf, strlen(Lbuf), MISC, DBG_BUF_SIZE, MISC_PACKET,
		PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address

	// Now service the stack a few times:
	for (i = 0; i < 10; i++) {
#ifdef __LOWER_TX__
		CheckAndSend(&LOWER_TX_HANDLER); // Manages the lower PPM MISC stack (sends anything on it
										 // to the upper)
#endif
#ifdef __TXMANAGE__
		TX_nonBlockProcess(&MISC_nb); // Manages all Misc data transmission
#endif
	} // Servicing stack loop

	for (m = 0; m < NUM_STORED_CMDS; m++) {
		p = &globals.StoredCmds[m][0];

		if (fbit(ScheduleMask[m / 8], m % 8))
			mask = 'E'; // Mark schedule if it is enabled
		else
			mask = 'D';

		len = p[0];

		if (p[1] == 1)
			ran = 'R';
		else if (p[1] == 2)
			ran = 'W';
		else
			ran = '0';

		if (len > 7 && len < MAX_STORED_CMD_SIZE) {
			sprintf(Lbuf, "\r\n%02u| %c%c |%03u|", m, mask, ran, len);
			for (i = 2; i < 4; i++) {
				if (p[i] == 255)
					sprintf(&Lbuf[strlen(Lbuf)], "**/");
				else
					sprintf(&Lbuf[strlen(Lbuf)], "%02u/", p[i]);
			}

			if (p[4] == 255)
				sprintf(&Lbuf[strlen(Lbuf)], "** ");
			else
				sprintf(&Lbuf[strlen(Lbuf)], "%02u ", p[4]);

			if (p[5] == 255)
				sprintf(&Lbuf[strlen(Lbuf)], "** ");
			else
				sprintf(&Lbuf[strlen(Lbuf)], "%02u ", p[5]);

			for (i = 6; i < 8; i++) {
				if (p[i] == 255)
					sprintf(&Lbuf[strlen(Lbuf)], "**:");
				else
					sprintf(&Lbuf[strlen(Lbuf)], "%02u:", p[i]);
			}

			if (p[8] == 255)
				sprintf(&Lbuf[strlen(Lbuf)], "**|");
			else
				sprintf(&Lbuf[strlen(Lbuf)], "%02u|", p[8]);

			for (i = 9; i < len + 2; i++) {
				sprintf(&Lbuf[strlen(Lbuf)], "%02u ", p[i]);
			}

			//			sendDBGALL(USER_PORT,Lbuf);
			PacketizeAndPush(Lbuf, strlen(Lbuf), MISC, DBG_BUF_SIZE, MISC_PACKET, PPM_NUM);

			// Now service the stack a few times:
			for (i = 0; i < 10; i++) {
#ifdef __LOWER_TX__
				CheckAndSend(&LOWER_TX_HANDLER); // Manages the lower PPM MISC stack (sends anything
												 // on it to the upper)
#endif
#ifdef __TXMANAGE__
				TX_nonBlockProcess(&MISC_nb); // Manages all Misc data transmission
#endif
			}
		} else {
			//			sprintf(Lbuf,"\r\n%02u| %c | 0",m,mask);
			//			sendDBGALL(port,Lbuf);
		}
	} // for
}

//================== Show Macros ===================================
void showMacroPacketized() {
#ifdef __MACROS__
	int i, m, n, len, slen;
	unsigned char* buf;
	unsigned char* macro;

	macro = Lbuf;
	buf = dbgbuf;

#ifdef __MYSTACK__
#ifdef UPPER_PPM
	sprintf(buf, "UPPER PPM Stored Macros:");
#else
	sprintf(buf, "LOWER PPM Stored Macros:");
#endif
	//	sendDBGALL(USER_PORT,buf);
	PacketizeAndPush(
		buf, strlen(buf), MISC, DBG_BUF_SIZE, MISC_PACKET,
		PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address

	// Now service the stack a few times:
	for (i = 0; i < 10; i++) {
#ifdef __LOWER_TX__
		CheckAndSend(&LOWER_TX_HANDLER); // Manages the lower PPM MISC stack (sends anything on it
										 // to the upper)
#endif
#ifdef __TXMANAGE__
		TX_nonBlockProcess(&MISC_nb); // Manages all Misc data transmission
#endif
	} // Servicing stack loop

	for (m = 0; m < NUM_MACROS; m++) {
		len = readMacro('M', m, macro);
		if (len != 0) // Only do this for written macros to save bandwidth.
		{
			sprintf(buf, "%2d %c %3d:", m, Macro_state[m], len);
			for (n = 1; n < len + 1; n++) {
				slen = strlen(buf);
				sprintf(&buf[slen], "%u ", macro[n]);
				slen = strlen(buf);
				if (slen >
					DBG_BUF_SIZE - 10) // If we're going to overrun, just add ellipses and send
				{
					sprintf(&buf[slen], "...");
					n = len + 5; // Set to something that will fail the for loop check
				}
			}
			//			sendDBGALL(USER_PORT,buf);
			PacketizeAndPush(buf, strlen(buf), MISC, DBG_BUF_SIZE, MISC_PACKET, PPM_NUM);

			// Now service the stack a few times:
			for (i = 0; i < 10; i++) {
#ifdef __LOWER_TX__
				CheckAndSend(&LOWER_TX_HANDLER); // Manages the lower PPM MISC stack (sends anything
												 // on it to the upper)
#endif
#ifdef __TXMANAGE__
				TX_nonBlockProcess(&MISC_nb); // Manages all Misc data transmission
#endif
			} // Servicing stack loop
		}
	}
#endif //__MYSTACK
#endif //__MACROS__
}

// A Test function to show protection on each sector
void flashPrintSectorProtection(int port) {
#ifdef __FLASH_DRIVER__
	unsigned int i;
	int temp;
	unsigned long s_addr, e_addr;

	for (i = 0; i < FLASH_MAX_SECTORS; i++) {
		SectorToFlashAddr(i, &s_addr, &e_addr);
		temp = flashReadSectorProtection(s_addr);
		sprintf(dbgbuf, "\r\n%i\t%i", i, temp);
		sendDBGALL(port, dbgbuf);
	}
#endif
}

// A quick function for converting a string into a series of floats
ReturnErr_t getFloats(float* floatarray, unsigned char* a, int num_floats) {
	int i;
	/*   DEBUG
		sendMSGALL(USER_PORT,"\r\n\n");
		sendMSG(USER_PORT,a,45);
		sendMSGALL(USER_PORT,"\r\n\n");
		delay_ms(1000);
	*/
	for (i = 0; i < num_floats; i++) {
		floatarray[i] = strtod(a, &a);
		/* DEBUG STUFF
		sendMSGALL(USER_PORT,"\r\nA: ");
		sendMSG(USER_PORT,a,45);
		sendMSGALL(USER_PORT,"\r\nFloat:");
		printFloat(dbgbuf,floatarray[i],6);
		sendMSGALL(USER_PORT,dbgbuf);
		sendMSGALL(USER_PORT,"\r\n");
		*/
	}

	return SUCCESS; // If you make it here, success.
}

// Handles some orbit commands.  Case 0 was moved because PIC had trouble with floats after so many
// function calls
void orbitCmd(unsigned char* cmd) {
	switch (cmd[0]) {
	//
	case 0:
		break;

	case 1:
// Display Orbit Data
// r 108 1 (format)
// Example: r 108 1 1
// Example: r 108 1 0
// f 108 1 0
#ifdef __ORBIT__
		orbitPrintData(cmd[1]);
#endif
		break;
#ifdef __ORBIT__
	case 'E':
	case 'e': // Enable
		globals.orbitEnable = 1;
		break;

	case 'D':
	case 'd': // Disable
		globals.orbitEnable = 0;
		break;
#endif
	default:
		break;
	} // switch
}

#ifdef __ADC__
#ifndef FLIGHT
// Reads the ADC for the given channel to the given port.
void readADC(int port, unsigned char channel) {
	unsigned char value;

	set_adc_channel(channel);
	delay_us(20);
	read_adc(ADC_START_ONLY);
	while (!adc_done()) {
	}; // wait for ADC to be done
	value = read_adc(ADC_READ_ONLY);

	sprintf(dbgbuf, "\r\nADC%02i = %02x --- ", channel, value);
	sendDBGALL(port, dbgbuf);
	byteMe(value);

	return;
}
#endif
#endif

#ifdef __ORBIT__
/*
void orbitTest()
{
	// Declarations
	float pos[3]={421.53,6820999.95,649.0};
	float vel[3]={4215.3,-0.856,6490.0};
	long i;
	unsigned char temp;

	// Statements
	sendDBGALL(USER_PORT,"\r\nOrbit Test");
	sendDBGALL(USER_PORT,"\r\nEpoch: ");
	PrintTime(time,USER_PORT);
	sendDBGALL(USER_PORT,"\r\nStarting POS and VEL (ECI): ");
	for(i=0;i<3;i++) { printFloat(dbgbuf,pos[i],6); sendDBGALL(USER_PORT,dbgbuf);
sendDBGALL(USER_PORT,",");} for(i=0;i<3;i++) { printFloat(dbgbuf,vel[i],6);
sendDBGALL(USER_PORT,dbgbuf); sendDBGALL(USER_PORT,",");} sendDBGALL(USER_PORT,"\r\n\n");
	sendDBGALL(USER_PORT,"Time(seconds from epoch),ECI Pos X,ECI Pos Y,ECI Pos Z,Vel X,Vel Y,Vel
Z,Acc X,Acc Y,Acc Z,GHA\r\n"); initOrbit(time, pos,vel); for(i=0;i<259200;i++) // Divide total by 5
to get # seconds.  259200=72 hours
	{
		restart_wdt();
//		orbit(0.2);
		// Display every x iterations.  Remember that 5 iterations = 1 second.
		// 50 = 10 seconds
		// 300 = 1 minute
		if(i%300==0){
			sprintf(dbgbuf,"%Ld,",i*5);
			sendDBGALL(USER_PORT,dbgbuf);
			orbitPrintData(1);
		}
		if(getByte(USER_PORT,temp)) break;
	}
}
*/
#endif

// Sends a simple message to a Port
// Format of this sending function is:
// Sync + Len + Data + CRC
ReturnErr_t sendPPM(int port, unsigned char* data, unsigned char len) {
	unsigned char crc[2];
	unsigned char sync[2] = {0x4e, 0xff};

	// Check if message is too long (3 for CRC(2) and Length(1)):
	if (len + 3 > MAX_PPM_PKT_SIZE) {
		// Note: Can't debug since we use this to send messages between PPMS (Compiler does not
		// allow recursion!)
		// sendDBGALL(USER_PORT,"\r\nTrying to send too large of packet to other PPM.  Fail.");
		return FAILURE;
	}

	// Get the CRC
	getCRC(data, (int)len, crc);

	// Send off the message
	sendMSG(port, sync, 2);
	sendByte(port, len);
	sendMSG(port, data, (int)len);
	sendMSG(port, crc, 2);

	// Debug
	/*
		sprintf(dbgbuf,"\r\n sendPPM: ");
		sendMSG(USER_PORT,dbgbuf,strlen(dbgbuf));
		HexPrint(USER_PORT,sync,2);
		HexPrint(USER_PORT,&len,1);
		HexPrint(USER_PORT,data,(int)len);
		HexPrint(USER_PORT,crc,2);
	*/
	// Print out to screen
	sendMSG(USER_PORT, "\r\nP --- ",
			8); // Have to use this instead of sendDBGALL because of recursion.

	return SUCCESS;
}

// Sends a command to the iTag
// Format of the input is ASCII (like you would use in CheckUser)
// For example, if you wanted to run the following command on the iTag:
// r 39 0 1 "Test Command" (enter)
// You should be able to run it by typing
// i 39 0 1 "Test Command" (enter)
ReturnErr_t sendiTagCmd(unsigned char* data) {
	unsigned char len;
	unsigned char count;
#ifdef LOWER_PPM
	unsigned char i;
	unsigned char j;
	unsigned char temp[GENERAL_BUFFER_SIZE];
#endif

	len = strlen(data);
	count = 0;

	// The data contains the command (everything after the 'i')
	// For example, if the command was:
	// i 39 0 1 "Test Command" (enter)
	// data will contain {' ','3','9',' ','0',' ','1',...}
	// We don't want to parse this data, because the iTag will handle the parsing.
	// So we just want to forward the string directly to the iTag, with an 'r' in
	// front.  Let's prepend the r.

	memmove(data + 1, data, len); // Slide the command to the right by 1 char.
	data[0] = 'r'; // Append the run command
	len++; // Update the length
	sendDBGALL(USER_PORT, "\r\nData: ");
	sendMSG(USER_PORT, data, len);

	// We have two scenarios:
	// The first is that this command is run on the bottom processor.
	// In this case, we need to PREPEND the command with a "Forward to iTag" command.
	// We also need to parse the command to run it across the PPMs, but we want to
	// preserve the original structure.  So we have to append quotes to the front and
	// back of the the command.  We also have to add a return inside the quotes.

#ifdef LOWER_PPM

	// First, let's find out how many internal quotes we'll have to escape.
	for (i = 0; i <= len; i++) {
		if (data[i] == '"' || data[i] == '\'')
			count++;
	}

	//	sprintf(dbgbuf,"\r\nCount=%d",count);
	//	sendDBGALL(USER_PORT,dbgbuf);
	memmove(data + 2 + count + 1, data,
			len); // slide the data to the right to make space for quotes(2), escaped quotes(count),
				  // and the final return character(1)
	len = len + 2 + count + 1; // update length

	// Next, let's move down the data, escaping as necessary
	data[0] = '"';
	j = 1;
	for (i = 2 + count; i <= len; i++) {
		if (data[i] == '"') {
			data[j] = '\\';
			j++;
			data[j] = '"';
		} else if (data[i] == '\'') {
			data[j] = '\\';
			j++;
			data[j] = '\'';
		} else {
			data[j] = data[i];
		}
		j++;

		//		sendDBGALL(USER_PORT,"\r\n:");
		//		sendMSG(USER_PORT,data,len);
	}
	data[len - 1] = '"'; // final quote
	data[len] = '\r'; // return character

	//	sendDBGALL(USER_PORT,"\r\n:");
	//	sendMSG(USER_PORT,data,len);

	//	Next, we forward it to the iTag
	/*
		// Forward a menu command to the iTag
		sprintf(temp,"158 3 3 \"mm\" \r");
		sendDBGALL(USER_PORT,"\r\nCMD: ");
		sendMSG(USER_PORT,temp,strlen(temp));
		sendPPMCmd(OTHER_PPM_PORT,temp);

		delay_ms(100);

		// Forward a menu command to the iTag
		sprintf(temp,"158 3 3 \"mm\" \r");
		sendDBGALL(USER_PORT,"\r\nCMD: ");
		sendMSG(USER_PORT,temp,strlen(temp));
		sendPPMCmd(OTHER_PPM_PORT,temp);

		delay_ms(100);
	*/
	// Forward the forward command
	// 131 158 (port) (len) (msg)
	sprintf(temp, "158 3 %d %s\r", len, data);
	sendDBGALL(USER_PORT, "\r\nCMD: ");
	sendMSG(USER_PORT, temp, strlen(temp));
	sendPPMCmd(OTHER_PPM_PORT, temp);
	// RunCommand(temp);

#endif

	return SUCCESS;
}

// Sends a command to the other PPM
// Format of the input is ASCII (like you would use in CHeckUser)
ReturnErr_t sendPPMCmd(int port, unsigned char* data) {
	// echomode=1;
	ParseCommandString(data, data); // Process into actual command

	//	HexPrint(USER_PORT,data,(int)(data[0]+1));
	//	sendPPM(port,data,(unsigned char)(data[0]+1)); // Send it off

	// Note, the first byte of data[] is the length of the command.  However,
	// sendPPM() adds a length field in front of the data field.  Therefore
	// we need to do two things:
	//		1. Feed in data to sendPPM() starting at the 2nd byte (&data[1])
	//      2. Extract data[0] as the length and place it into the length argument
	sendPPM(port, &data[1], (unsigned char)(data[0])); // Send it off

	return SUCCESS;
}

#ifdef __MHX__
void CheckSync() {
	unsigned char c;
	c = 0x00;

	if (MHXConnected() && SEC_TIMER > SyncTime) {
		sendPACKETIZED(MHX_PORT, &c, 1, 4);
		AckFlag = c;
		SyncTime = SEC_TIMER + SyncRetryTime;
		sendDBGALL(USER_PORT, "Sync )))\r\n");
	}
}
#endif

// This function sends orbit data to the other PPM, activating the command to place the data in the
// appropriate slot in ExtIn[] for GNC to process.
#ifdef __ORBIT__
#ifdef __AENEAS__
/*
void orbitSendPPM()
{
	// Use Lbuf for the command
//	float temp;
	int i;
	int
index[]={ECI_POS_X_IN,ECI_POS_Y_IN,ECI_POS_Z_IN,ECI_VEL_X_IN,ECI_VEL_Y_IN,ECI_VEL_Z_IN,GHA_IN}; //
Index values for recieving the floats int
o_data_index[]={ORBIT_POS_X_INDEX,ORBIT_POS_Y_INDEX,ORBIT_POS_Z_INDEX,
						ORBIT_VEL_X_INDEX,ORBIT_VEL_Y_INDEX,ORBIT_VEL_Z_INDEX,
						ORBIT_GHA_INDEX}; // Position (3), Velocity(3), and GHA
	#define COMMAND		130

	Lbuf[0]=sizeof(unsigned char)+sizeof(unsigned char)+sizeof(unsigned char)+sizeof(float); //
Length, Command, Index, 4-byte float Lbuf[1]=COMMAND; // ProcessCmd() Command for processing float
data

	for(i=0;i<7;i++)
	{
//		// Send Orbit Data
//		Lbuf[2]=index[i]; 							// Store Index
//		memcpy(&Lbuf[3],&orbitData[o_data_index[i]],sizeof(float));		// Store float
//		sendPPM(OTHER_PPM_PORT, Lbuf, Lbuf[0]);		// Send it off
	}

	#undef COMMAND
}
*/
#endif
#endif

#ifdef __AENEAS__
/*
void PrintExt(float * data, int start, int stop)
{
	int i;
	//int c=1;

	for(i=start+1;i<=stop;i++)
	{
		printFloat(dbgbuf,data[i-1],6);
		sendDBGALL(USER_PORT,dbgbuf);
		sendDBGALL(USER_PORT,"\r\n");
		//if(c<=20) locateString(USER_PORT,c,0,dbgbuf);
		//else if(c<=40)locateString(USER_PORT,c-20,20,dbgbuf);
		//else if(c<=60)locateString(USER_PORT,c-40,40,dbgbuf);
		//else locateString(USER_PORT,c-60,60,dbgbuf);
		//c++;
	}
}
*/
#endif

#ifdef LOWER_PPM
void attitudeSensors4HzTask() {
#ifdef DEBUG
	static int i = 0;
	int j = 0;
	if (displaySensors) {
		// Print Sensor Debug Info as needed
		//	printGyroStruct(&GyroData);

		// Line Number (based on iterated "i")
		sprintf(dbgbuf, "\r\n%Ld", i++);
		sendDBGALL(USER_PORT, dbgbuf);

#ifdef __SUNSENSOR__
		// Process and Write Data From last cycle
		if (SunFlag == SUCCESS) {
			sprintf(dbgbuf, ",[S]");
			sendDBGALL(USER_PORT, dbgbuf);

			// Sun Vector
			for (j = 0; j < 3; j++) {
				// sprintf(dbgbuf,",%d",SunData.Vector_int[j]);
				printFloat(dbgbuf, SunData.Vector[j], 6);
				sendDBGALL(USER_PORT, ",");
				sendDBGALL(USER_PORT, dbgbuf);
			}

			// Sun Temperature
			// sprintf(dbgbuf,",%d",SunData.Temperature);
			// sendDBGALL(USER_PORT,dbgbuf);
		} else
			sendDBGALL(USER_PORT, ",[S] XXX");
#endif

#ifdef __GYRO_H__
		if (GyroFlag == SUCCESS) {
			sprintf(dbgbuf, ",[G]");
			sendDBGALL(USER_PORT, dbgbuf);

			// Rates
			for (j = 0; j < NUM_OF_GYROS; j++) {
				sprintf(dbgbuf, ",%d", GyroData.rate[j]);
				sendDBGALL(USER_PORT, dbgbuf);
			}

			// Temps
			for (j = 0; j < NUM_OF_GYROS; j++) {
				sprintf(dbgbuf, ",%d", GyroData.temp[j]);
				sendDBGALL(USER_PORT, dbgbuf);
			}
		} else
			sendDBGALL(USER_PORT, ",[G] XXX");
#endif

#ifdef __IMI100_H__
		if (IMIFlag == SUCCESS) {
			sprintf(dbgbuf, ",[I]");
			sendDBGALL(USER_PORT, dbgbuf);

			// IMI Coils
			for (j = 0; j < 3; j++) {
				sprintf(dbgbuf, ",%d", IMIData.gc_torqueCoilCmd[j]);
				sendDBGALL(USER_PORT, dbgbuf);
			}

			// IMI speeds
			for (j = 0; j < 3; j++) {
				sprintf(dbgbuf, ",%d", IMIData.gs_rwsSpeedTach[j]);
				sendDBGALL(USER_PORT, dbgbuf);
			}

			// IMI Telem
			sprintf(dbgbuf, ",%d,%d,%d,%d", IMIData.gus_cmdValidCntr, IMIData.gus_cmdInValidCntr,
					IMIData.gus_cmdInvalidChksumCntr, IMIData.guc_LastCommand);
			sendDBGALL(USER_PORT, dbgbuf);
		} else
			sendDBGALL(USER_PORT, ",[I] XXX");
#endif

#ifdef __MAG_H__
		if (MagFlag == SUCCESS) {
			sprintf(dbgbuf, ",[M]");
			sendDBGALL(USER_PORT, dbgbuf);

			// Mag data
			for (j = 0; j < 3; j++) {
				sprintf(dbgbuf, ",%d", MagData[j]);
				sendDBGALL(USER_PORT, dbgbuf);
			}
		} else
			sendDBGALL(USER_PORT, ",[M] XXX");
#endif
	} // if
#endif // ifdef DEBUG

	//------------------------------------- Reset all sensors and actuators

#ifdef __IMI100_H__
	// Reset Flags for IMI and Mag (Sun and Gyro get set just below).
	IMIFlag = BUSY;
#endif

#ifdef __MAG_H__
	MagFlag = BUSY;

	// Start a new mag cycle
	MagFlag = readAllMagAxis(MagData, 1);
#endif

#ifdef __SUNSENSOR__
	// Flip the Sun Sensor Sync bit
	SunSync = !SunSync;

	// Start a new sun sensor cycle (or at least try :-)
	SunFlag = getSunSensorData(SUN_PORT, sunChannels, sunNumChannels, &SunData, 1,
							   SunSync); // Note the 2nd to last argument is a reset
#endif

#ifdef __GYRO_H__
	// Read the gyroscope data and reset
	GyroFlag = collectAllGyroData(&GyroData);
	AverageGyroData(&GyroData, &GyroData, 1); // Resets the averager.
#endif

	return;
}
#endif // LOWER_PPM

#ifdef LOWER_PPM
void attitudeSensorsBackgroundTask() {
	//	ReturnErr_t temp;

#ifdef __SUNSENSOR__
	// Read the Sun Sensor
	if (getSunSensorData(SUN_PORT, sunChannels, sunNumChannels, &SunData, 0, SunSync) == SUCCESS) {
		SunFlag = SUCCESS;
	}
#endif

#ifdef __IMI100_H__
	// Read the IMI always
	if (getIMITelemetry(IMI_PORT, &IMIData) == SUCCESS) {
		IMIFlag = SUCCESS;
		// attitudeSensors4HzTask();
	}
#endif

#ifdef __MAG_H__
	// Read the Mag
	if (readAllMagAxis(MagData, 0) == SUCCESS) {
		MagFlag = SUCCESS;
	}
#endif

#ifdef __GYRO_H__
	// Read the gyroscope data continuously
	if (collectAllGyroData(&GyroAveragingData) == SUCCESS) {
		AverageGyroData(&GyroData, &GyroAveragingData,
						0); // Averages the just-read data into GyroData.
	}
#endif

	return;
}
#endif

// This is a simple function to print a Telemetry Element that is part
// of the GNC system.  This function could later be expanded to print
// any of the indexed Telemetry Elements, but that would require
// dealing with the different types (int, float) and the different
// textual presentations (floating point, hexadeicmal, decimal numbers
// etc).
/*
void printTelemetryElements(int cmd_length, unsigned char * rbcmd, unsigned char startIndex)
{
	int i=0, index=0;

	sendDBGALL(USER_PORT,"\r\n");

	for (i=0;i<(cmd_length-startIndex);i=i+2)
	{
	  index = make16(rbcmd[startIndex+i], rbcmd[startIndex+i+1]);   // Left-shift the first 8 bits
and copy the second 8 bits

		#ifdef __AENEAS__
		// If the index is below 200, it is an ExtIn[] item
		if(index<200)
		{
			printFloat(dbgbuf,ExtIn[index],6);
			sendDBGALL(USER_PORT,dbgbuf);
			//sendDBGALL(USER_PORT,"\r\n");
			//HexPrint(USER_PORT,&(ExtIn[index]),4);
		}
		// Otherwise, check if the index is below 300, then its ExtOut[]
		else if(index<300)
		{
			index = index-200;
			printFloat(dbgbuf,ExtOut[index],6);
			sendDBGALL(USER_PORT,dbgbuf);
			//endDBGALL(USER_PORT,"\r\n");
			//HexPrint(USER_PORT,&(ExtOut[index]),4);
		}
		// Finally, this is not a GNC telemetry element so ignore it.
		else
		{
			sendDBGALL(USER_PORT,"\r\nTelemetry Element not supported yet.");
		}
		#endif

		// Add a comma for CVS format.
		sendDBGALL(USER_PORT,",");
	}

	return;
}
*/

int addTelmetryElement(unsigned char* data, int index) {
	switch (index) {
#ifdef __AENEAS__
	case 0:
		memcpy(data, &(ExtIn[Mode_Override_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 1:
		memcpy(data, &(ExtIn[Mode_Limit_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 2:
		memcpy(data, &(ExtIn[Min_Omega_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 3:
		memcpy(data, &(ExtIn[Max_Omega_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 4:
		memcpy(data, &(ExtIn[Sun_Search_Slew_Transition_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 5:
		memcpy(data, &(ExtIn[Min_Sun_Line_Trans_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 6:
		memcpy(data, &(ExtIn[Min_Pitch_Trans_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 7:
		memcpy(data, &(ExtIn[SP_Time_Limit_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 8:
		memcpy(data, &(ExtIn[Enable_Momemtum_Dump_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 9:
		memcpy(data, &(ExtIn[kpx_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 10:
		memcpy(data, &(ExtIn[kpy_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 11:
		memcpy(data, &(ExtIn[kpz_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 12:
		memcpy(data, &(ExtIn[krx_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 13:
		memcpy(data, &(ExtIn[kry_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 14:
		memcpy(data, &(ExtIn[krz_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 15:
		memcpy(data, &(ExtIn[Ixx_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 16:
		memcpy(data, &(ExtIn[Ixy_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 17:
		memcpy(data, &(ExtIn[Ixz_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 18:
		memcpy(data, &(ExtIn[Iyy_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 19:
		memcpy(data, &(ExtIn[Iyz_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 20:
		memcpy(data, &(ExtIn[Izz_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 21:
		memcpy(data, &(ExtIn[Slew_to_PD_Trans_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 22:
		memcpy(data, &(ExtIn[Commanded_Aqsn_Slew_Rate_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 23:
		memcpy(data, &(ExtIn[Rate_Limit_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 24:
		memcpy(data, &(ExtIn[Q_Limit_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 25:
		memcpy(data, &(ExtIn[Wheel_Sat_Limit_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 26:
		memcpy(data, &(ExtIn[kMOMx_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 27:
		memcpy(data, &(ExtIn[kMOMy_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 28:
		memcpy(data, &(ExtIn[kMOMz_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 29:
		memcpy(data, &(ExtIn[Ixx_Wheel_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 30:
		memcpy(data, &(ExtIn[Ixy_Wheel_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 31:
		memcpy(data, &(ExtIn[Ixz_Wheel_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 32:
		memcpy(data, &(ExtIn[Iyy_Wheel_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 33:
		memcpy(data, &(ExtIn[Iyz_Wheel_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 34:
		memcpy(data, &(ExtIn[Izz_Wheel_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 35:
		memcpy(data, &(ExtIn[Wheel_Enable_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 36:
		memcpy(data, &(ExtIn[AD_Filter_Gain_Kf_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 37:
		memcpy(data, &(ExtIn[AD_Filter_Pole_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 38:
		memcpy(data, &(ExtIn[Whl_Filter_Gain_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 39:
		memcpy(data, &(ExtIn[Whl_Filter_Pole_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 40:
		memcpy(data, &(ExtIn[Reset_Estimator_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 41:
		memcpy(data, &(ExtIn[q1_M_LV_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 42:
		memcpy(data, &(ExtIn[q2_M_LV_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 43:
		memcpy(data, &(ExtIn[q3_M_LV_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 44:
		memcpy(data, &(ExtIn[q4_M_LV_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 45:
		memcpy(data, &(ExtIn[Target_Lat_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 46:
		memcpy(data, &(ExtIn[Target_Long_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 47:
		memcpy(data, &(ExtIn[Target_Alt_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 48:
		memcpy(data, &(ExtIn[Slew_Angle_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 49:
		memcpy(data, &(ExtIn[SS_MisAln_x_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 50:
		memcpy(data, &(ExtIn[SS_MisAln_y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 51:
		memcpy(data, &(ExtIn[SS_MisAln_z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 52:
		memcpy(data, &(ExtIn[Gyro_MisAln_x_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 53:
		memcpy(data, &(ExtIn[Gyro_MisAln_y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 54:
		memcpy(data, &(ExtIn[Gyro_MisAln_z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 55:
		memcpy(data, &(ExtIn[Gyro_Drift_x_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 56:
		memcpy(data, &(ExtIn[Gyro_Drift_y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 57:
		memcpy(data, &(ExtIn[Gyro_Drift_z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 58:
		memcpy(data, &(ExtIn[Gyro_AutoCal_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 59:
		memcpy(data, &(ExtIn[MTR_Enable_Mode4_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 60:
		memcpy(data, &(ExtIn[Drag_Coeff_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 61:
		memcpy(data, &(ExtIn[SUNVEC0_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 62:
		memcpy(data, &(ExtIn[SUNVEC1_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 63:
		memcpy(data, &(ExtIn[SUNVEC2_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 64:
		memcpy(data, &(ExtIn[ECI_POS_X_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 65:
		memcpy(data, &(ExtIn[ECI_POS_Y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 66:
		memcpy(data, &(ExtIn[ECI_POS_Z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 67:
		memcpy(data, &(ExtIn[ECI_VEL_X_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 68:
		memcpy(data, &(ExtIn[ECI_VEL_Y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 69:
		memcpy(data, &(ExtIn[ECI_VEL_Z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 70:
		memcpy(data, &(ExtIn[GHA_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 71:
		memcpy(data, &(ExtIn[nT_ECI_x_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 72:
		memcpy(data, &(ExtIn[nT_ECI_y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 73:
		memcpy(data, &(ExtIn[nT_ECI_z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 74:
		memcpy(data, &(ExtIn[Measured_Gyro_X_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 75:
		memcpy(data, &(ExtIn[Measured_Gyro_Y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 76:
		memcpy(data, &(ExtIn[Measured_Gyro_Z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 77:
		memcpy(data, &(ExtIn[Measured_Mag_X_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 78:
		memcpy(data, &(ExtIn[Measured_Mag_Y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 79:
		memcpy(data, &(ExtIn[Measured_Mag_Z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 80:
		memcpy(data, &(ExtIn[Measured_SS_x_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 81:
		memcpy(data, &(ExtIn[Measured_SS_y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 82:
		memcpy(data, &(ExtIn[Measured_SS_z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 83:
		memcpy(data, &(ExtIn[Sun_Present_Flag_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 84:
		memcpy(data, &(ExtIn[Measured_RW_omega_x_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 85:
		memcpy(data, &(ExtIn[Measured_RW_omega_y_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 86:
		memcpy(data, &(ExtIn[Measured_RW_omega_z_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 87:
		memcpy(data, &(ExtIn[Wheel_Disable_Mode5_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 88:
		memcpy(data, &(ExtIn[Spare2_IN]), sizeof(float));
		return sizeof(float);
		break;
	case 89:
		memcpy(data, &(ExtIn[Spare3_IN]), sizeof(float));
		return sizeof(float);
		break;

	case 200:
		memcpy(data, &(ExtOut[Meas_Sun_Vector_B_x_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 201:
		memcpy(data, &(ExtOut[Meas_Sun_Vector_B_y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 202:
		memcpy(data, &(ExtOut[Meas_Sun_Vector_B_z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 203:
		memcpy(data, &(ExtOut[Sun_Sensor_Processing_7_1_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 204:
		memcpy(data, &(ExtOut[Filt_Omega_B_N_B_x_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 205:
		memcpy(data, &(ExtOut[Filt_Omega_B_N_B_y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 206:
		memcpy(data, &(ExtOut[Filt_Omega_B_N_B_z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 207:
		memcpy(data, &(ExtOut[meas_mag_body_x_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 208:
		memcpy(data, &(ExtOut[meas_mag_body_y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 209:
		memcpy(data, &(ExtOut[meas_mag_body_z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 210:
		memcpy(data, &(ExtOut[Meas_Wheel_omega_rad_s_x_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 211:
		memcpy(data, &(ExtOut[Meas_Wheel_omega_rad_s_y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 212:
		memcpy(data, &(ExtOut[Meas_Wheel_omega_rad_s_z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 213:
		memcpy(data, &(ExtOut[SP_check_failed_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 214:
		memcpy(data, &(ExtOut[Rate_too_high_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 215:
		memcpy(data, &(ExtOut[True_Last_3_Samples_2_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 216:
		memcpy(data, &(ExtOut[True_Last_3_Samples_1_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 217:
		memcpy(data, &(ExtOut[Wheel_Hx_Body_Frame_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 218:
		memcpy(data, &(ExtOut[Wheel_Hy_Body_Frame_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 219:
		memcpy(data, &(ExtOut[Wheel_Hz_Body_Frame_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 220:
		memcpy(data, &(ExtOut[Mode_1_Idle_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 221:
		memcpy(data, &(ExtOut[Mode_2_Inertial_Capture_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 222:
		memcpy(data, &(ExtOut[Mode_3_Sun_Search_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 223:
		memcpy(data, &(ExtOut[Mode_4_Sun_Pointing_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 224:
		memcpy(data, &(ExtOut[Mode_5_Eclipse_Hold_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 225:
		memcpy(data, &(ExtOut[Mode_6_Quat_Hold_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 226:
		memcpy(data, &(ExtOut[Mode_7_Surface_Track_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 227:
		memcpy(data, &(ExtOut[Mode_8_Test_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 228:
		memcpy(data, &(ExtOut[Mode_9_Momentum_Dump_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 229:
		memcpy(data, &(ExtOut[True_Last_3_Samples_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 230:
		memcpy(data, &(ExtOut[q1_err_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 231:
		memcpy(data, &(ExtOut[q2_err_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 232:
		memcpy(data, &(ExtOut[q3_err_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 233:
		memcpy(data, &(ExtOut[q4_err_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 234:
		memcpy(data, &(ExtOut[wx_err_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 235:
		memcpy(data, &(ExtOut[wy_err_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 236:
		memcpy(data, &(ExtOut[wz_err_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 237:
		memcpy(data, &(ExtOut[Torque_Rod_Enable_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 238:
		memcpy(data, &(ExtOut[Reset_Estimator_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 239:
		memcpy(data, &(ExtOut[Latch_2_1_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 240:
		memcpy(data, &(ExtOut[Save_Sun_Quaternion_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 241:
		memcpy(data, &(ExtOut[Approx_Angular_Err_frm_Sun_Lin_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 242:
		memcpy(data, &(ExtOut[q1_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 243:
		memcpy(data, &(ExtOut[q2_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 244:
		memcpy(data, &(ExtOut[q3_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 245:
		memcpy(data, &(ExtOut[q4_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 246:
		memcpy(data, &(ExtOut[nadir_vect_x_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 247:
		memcpy(data, &(ExtOut[nadir_vect_y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 248:
		memcpy(data, &(ExtOut[nadir_vect_z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 249:
		memcpy(data, &(ExtOut[Body_Hx_ECI_Frame_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 250:
		memcpy(data, &(ExtOut[Body_Hy_ECI_Frame_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 251:
		memcpy(data, &(ExtOut[Body_Hz_ECI_Frame_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 252:
		memcpy(data, &(ExtOut[Mode_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 253:
		memcpy(data, &(ExtOut[dzero_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 254:
		memcpy(data, &(ExtOut[dzero_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 255:
		memcpy(data, &(ExtOut[dzero_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 256:
		memcpy(data, &(ExtOut[gyro_offset_x_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 257:
		memcpy(data, &(ExtOut[gyro_offset_y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 258:
		memcpy(data, &(ExtOut[gyro_offset_z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 259:
		memcpy(data, &(ExtOut[overwritepulse_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 260:
		memcpy(data, &(ExtOut[Requested_Torque_B_x_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 261:
		memcpy(data, &(ExtOut[Requested_Torque_B_y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 262:
		memcpy(data, &(ExtOut[Requested_Torque_B_z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 263:
		memcpy(data, &(ExtOut[PD_Control_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 264:
		memcpy(data, &(ExtOut[Mode_5_Wheel_Enable_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 265:
		memcpy(data, &(ExtOut[dzero_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 266:
		memcpy(data, &(ExtOut[dzero_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 267:
		memcpy(data, &(ExtOut[dzero_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 268:
		memcpy(data, &(ExtOut[dzero_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 269:
		memcpy(data, &(ExtOut[Cmded_Whl_Torque_x_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 270:
		memcpy(data, &(ExtOut[Cmded_Whl_Torque_y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 271:
		memcpy(data, &(ExtOut[Cmded_Whl_Torque_z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 272:
		memcpy(data, &(ExtOut[Cmd_Dipole_X_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 273:
		memcpy(data, &(ExtOut[Cmd_Dipole_Y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 274:
		memcpy(data, &(ExtOut[Cmd_Dipole_Z_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 275:
		memcpy(data, &(ExtOut[Total_Hx_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 276:
		memcpy(data, &(ExtOut[Total_Hy_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 277:
		memcpy(data, &(ExtOut[Total_Hz_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 278:
		memcpy(data, &(ExtOut[Cmd_MTR_counts_X_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 279:
		memcpy(data, &(ExtOut[Cmd_MTR_counts_Y_OUT]), sizeof(float));
		return sizeof(float);
		break;
	case 280:
		memcpy(data, &(ExtOut[Cmd_MTR_counts_Z_OUT]), sizeof(float));
		return sizeof(float);
		break;
#endif
#ifdef __IMI100_H__
	case 300:
		memcpy(data, &(IMIData.sync), 2 * sizeof(char));
		return 2 * sizeof(char);
		break;
	case 301:
		memcpy(data, &(IMIData.gus_cmdValidCntr), sizeof(int));
		return sizeof(int);
		break;
	case 302:
		memcpy(data, &(IMIData.gus_cmdInValidCntr), sizeof(int));
		return sizeof(int);
		break;
	case 303:
		memcpy(data, &(IMIData.gus_cmdInvalidChksumCntr), sizeof(int));
		return sizeof(int);
		break;
	case 304:
		memcpy(data, &(IMIData.guc_LastCommand), 1 * sizeof(char));
		return 1 * sizeof(char);
		break;
	case 305:
		memcpy(data, &(IMIData.guc_acsMode), 1 * sizeof(char));
		return 1 * sizeof(char);
		break;
	case 306:
		memcpy(data, &(IMIData.gs_rwsSpeedCmd[0]), sizeof(int));
		return sizeof(int);
		break;
	case 307:
		memcpy(data, &(IMIData.gs_rwsSpeedCmd[1]), sizeof(int));
		return sizeof(int);
		break;
	case 308:
		memcpy(data, &(IMIData.gs_rwsSpeedCmd[2]), sizeof(int));
		return sizeof(int);
		break;
	case 309:
		memcpy(data, &(IMIData.gs_rwsSpeedTach[0]), sizeof(int));
		return sizeof(int);
		break;
	case 310:
		memcpy(data, &(IMIData.gs_rwsSpeedTach[1]), sizeof(int));
		return sizeof(int);
		break;
	case 311:
		memcpy(data, &(IMIData.gs_rwsSpeedTach[2]), sizeof(int));
		return sizeof(int);
		break;
	case 312:
		memcpy(data, &(IMIData.gf_rwaTorqueCmd[0]), sizeof(float));
		return sizeof(float);
		break;
	case 313:
		memcpy(data, &(IMIData.gf_rwaTorqueCmd[1]), sizeof(float));
		return sizeof(float);
		break;
	case 314:
		memcpy(data, &(IMIData.gf_rwaTorqueCmd[2]), sizeof(float));
		return sizeof(float);
		break;
	case 315:
		memcpy(data, &(IMIData.gc_rwaTorqueCmd[0]), sizeof(char));
		return sizeof(char);
		break;
	case 316:
		memcpy(data, &(IMIData.gc_rwaTorqueCmd[1]), sizeof(char));
		return sizeof(char);
		break;
	case 317:
		memcpy(data, &(IMIData.gc_rwaTorqueCmd[2]), sizeof(char));
		return sizeof(char);
		break;
	case 318:
		memcpy(data, &(IMIData.gf_torqueCoilCmd[0]), sizeof(float));
		return sizeof(float);
		break;
	case 319:
		memcpy(data, &(IMIData.gf_torqueCoilCmd[1]), sizeof(float));
		return sizeof(float);
		break;
	case 320:
		memcpy(data, &(IMIData.gf_torqueCoilCmd[2]), sizeof(float));
		return sizeof(float);
		break;
	case 321:
		memcpy(data, &(IMIData.gc_torqueCoilCmd[0]), sizeof(char));
		return sizeof(char);
		break;
	case 322:
		memcpy(data, &(IMIData.gc_torqueCoilCmd[1]), sizeof(char));
		return sizeof(char);
		break;
	case 323:
		memcpy(data, &(IMIData.gc_torqueCoilCmd[2]), sizeof(char));
		return sizeof(char);
		break;
	case 324:
		memcpy(data, &(IMIData.g_RotatingVariable_A[0]), 4 * sizeof(char));
		return 4 * sizeof(char);
		break;
	case 325:
		memcpy(data, &(IMIData.g_RotatingVariable_B[0]), 4 * sizeof(char));
		return 4 * sizeof(char);
		break;
	case 326:
		memcpy(data, &(IMIData.g_RotatingVariable_C[0]), 4 * sizeof(char));
		return 4 * sizeof(char);
		break;
	case 327:
		memcpy(data, &(IMIData.crc), 2 * sizeof(char));
		return 2 * sizeof(char);
		break;
#endif
#ifdef __GYRO_H__
	case 328:
		memcpy(data, &(GyroData.rawrate[0]), sizeof(int));
		return sizeof(int);
		break;
	case 329:
		memcpy(data, &(GyroData.rawrate[1]), sizeof(int));
		return sizeof(int);
		break;
	case 330:
		memcpy(data, &(GyroData.rawrate[2]), sizeof(int));
		return sizeof(int);
		break;
	case 331:
		memcpy(data, &(GyroData.rawtemp[0]), sizeof(int));
		return sizeof(int);
		break;
	case 332:
		memcpy(data, &(GyroData.rawtemp[1]), sizeof(int));
		return sizeof(int);
		break;
	case 333:
		memcpy(data, &(GyroData.rawtemp[2]), sizeof(int));
		return sizeof(int);
		break;
	case 334:
		memcpy(data, &(GyroData.rawerror[0]), sizeof(int));
		return sizeof(int);
		break;
	case 335:
		memcpy(data, &(GyroData.rawerror[1]), sizeof(int));
		return sizeof(int);
		break;
	case 336:
		memcpy(data, &(GyroData.rawerror[2]), sizeof(int));
		return sizeof(int);
		break;
	case 337:
		memcpy(data, &(GyroData.rate[0]), sizeof(int));
		return sizeof(int);
		break;
	case 338:
		memcpy(data, &(GyroData.rate[1]), sizeof(int));
		return sizeof(int);
		break;
	case 339:
		memcpy(data, &(GyroData.rate[2]), sizeof(int));
		return sizeof(int);
		break;
	case 340:
		memcpy(data, &(GyroData.temp[0]), sizeof(int));
		return sizeof(int);
		break;
	case 341:
		memcpy(data, &(GyroData.temp[1]), sizeof(int));
		return sizeof(int);
		break;
	case 342:
		memcpy(data, &(GyroData.temp[2]), sizeof(int));
		return sizeof(int);
		break;
	case 343:
		memcpy(data, &(GyroData.error[0]), sizeof(int));
		return sizeof(int);
		break;
	case 344:
		memcpy(data, &(GyroData.error[1]), sizeof(int));
		return sizeof(int);
		break;
	case 345:
		memcpy(data, &(GyroData.error[2]), sizeof(int));
		return sizeof(int);
		break;
	case 346:
		memcpy(data, &(GyroData.rateND[0]), sizeof(char));
		return sizeof(char);
		break;
	case 347:
		memcpy(data, &(GyroData.rateND[1]), sizeof(char));
		return sizeof(char);
		break;
	case 348:
		memcpy(data, &(GyroData.rateND[2]), sizeof(char));
		return sizeof(char);
		break;
	case 349:
		memcpy(data, &(GyroData.tempND[0]), sizeof(char));
		return sizeof(char);
		break;
	case 350:
		memcpy(data, &(GyroData.tempND[1]), sizeof(char));
		return sizeof(char);
		break;
	case 351:
		memcpy(data, &(GyroData.tempND[2]), sizeof(char));
		return sizeof(char);
		break;
	case 352:
		memcpy(data, &(GyroData.rateEA[0]), sizeof(char));
		return sizeof(char);
		break;
	case 353:
		memcpy(data, &(GyroData.rateEA[1]), sizeof(char));
		return sizeof(char);
		break;
	case 354:
		memcpy(data, &(GyroData.rateEA[2]), sizeof(char));
		return sizeof(char);
		break;
	case 355:
		memcpy(data, &(GyroData.tempEA[0]), sizeof(char));
		return sizeof(char);
		break;
	case 356:
		memcpy(data, &(GyroData.tempEA[1]), sizeof(char));
		return sizeof(char);
		break;
	case 357:
		memcpy(data, &(GyroData.tempEA[2]), sizeof(char));
		return sizeof(char);
		break;
#endif
#ifdef __SUNSENSOR__
	case 358:
		memcpy(data, &(SunData.GoCode), 1 * sizeof(char));
		return 1 * sizeof(char);
		break;
	case 359:
		memcpy(data, &(SunData.MinExposure), sizeof(long));
		return sizeof(long);
		break;
	case 360:
		memcpy(data, &(SunData.MaxExposure), sizeof(long));
		return sizeof(long);
		break;
	case 361:
		memcpy(data, &(SunData.TargetBrightness), sizeof(int));
		return sizeof(int);
		break;
	case 362:
		memcpy(data, &(SunData.MinBrightness), sizeof(int));
		return sizeof(int);
		break;
	case 363:
		memcpy(data, &(SunData.MaxBrightness), sizeof(int));
		return sizeof(int);
		break;
	case 364:
		memcpy(data, &(SunData.IntegratedBrightness), sizeof(long));
		return sizeof(long);
		break;
	case 365:
		memcpy(data, &(SunData.ExposureLength), sizeof(long));
		return sizeof(long);
		break;
	case 366:
		memcpy(data, &(SunData.Temperature), sizeof(int));
		return sizeof(int);
		break;
	case 367:
		memcpy(data, &(SunData.Vector[0]), sizeof(float));
		return sizeof(float);
		break;
	case 368:
		memcpy(data, &(SunData.Vector[1]), sizeof(float));
		return sizeof(float);
		break;
	case 369:
		memcpy(data, &(SunData.Vector[2]), sizeof(float));
		return sizeof(float);
		break;
	case 370:
		memcpy(data, &(SunData.FitQuality), sizeof(char));
		return sizeof(char);
		break;
	case 371:
		memcpy(data, &(SunData.GeometryQuality), sizeof(char));
		return sizeof(char);
		break;
	case 372:
		memcpy(data, &(SunData.Iterations), sizeof(char));
		return sizeof(char);
		break;
	case 373:
		memcpy(data, &(SunData.ProcessingTime), sizeof(long));
		return sizeof(long);
		break;
	case 374:
		memcpy(data, &(SunData.Vector_int[0]), sizeof(int));
		return sizeof(int);
		break;
	case 375:
		memcpy(data, &(SunData.Vector_int[1]), sizeof(int));
		return sizeof(int);
		break;
	case 376:
		memcpy(data, &(SunData.Vector_int[2]), sizeof(int));
		return sizeof(int);
		break;
	case 377:
		memcpy(data, &(SunData.FitQuality_int), sizeof(char));
		return sizeof(char);
		break;
	case 378:
		memcpy(data, &(SunData.GeometryQuality_int), sizeof(char));
		return sizeof(char);
		break;
	case 379:
		memcpy(data, &(SunData.Diagnostic[0]), sizeof(int));
		return sizeof(int);
		break;
	case 380:
		memcpy(data, &(SunData.Diagnostic[1]), sizeof(int));
		return sizeof(int);
		break;
	case 381:
		memcpy(data, &(SunData.Diagnostic[2]), sizeof(int));
		return sizeof(int);
		break;
	case 382:
		memcpy(data, &(SunData.CelsiusTemperature), sizeof(float));
		return sizeof(float);
		break;
#endif
#ifdef __MAG_H__
	case 383:
		memcpy(data, &(MagData[0]), sizeof(int));
		return sizeof(int);
		break;
	case 384:
		memcpy(data, &(MagData[1]), sizeof(int));
		return sizeof(int);
		break;
	case 385:
		memcpy(data, &(MagData[2]), sizeof(int));
		return sizeof(int);
		break;
#endif
#ifdef __EPS_H__
	case 386:
		memcpy(data, &(EPSData.EPS_Status), sizeof(int));
		return sizeof(int);
		break;
	case 387:
		memcpy(data, &(EPSData.EPS_Voltage_PLUS[0]), sizeof(int));
		return sizeof(int);
		break;
	case 388:
		memcpy(data, &(EPSData.EPS_Voltage_PLUS[1]), sizeof(int));
		return sizeof(int);
		break;
	case 389:
		memcpy(data, &(EPSData.EPS_Voltage_PLUS[2]), sizeof(int));
		return sizeof(int);
		break;
	case 390:
		memcpy(data, &(EPSData.EPS_Voltage_PLUS[3]), sizeof(int));
		return sizeof(int);
		break;
	case 391:
		memcpy(data, &(EPSData.EPS_Voltage_PLUS[4]), sizeof(int));
		return sizeof(int);
		break;
	case 392:
		memcpy(data, &(EPSData.EPS_Current_PLUS[0]), sizeof(int));
		return sizeof(int);
		break;
	case 393:
		memcpy(data, &(EPSData.EPS_Current_PLUS[1]), sizeof(int));
		return sizeof(int);
		break;
	case 394:
		memcpy(data, &(EPSData.EPS_Current_PLUS[2]), sizeof(int));
		return sizeof(int);
		break;
	case 395:
		memcpy(data, &(EPSData.EPS_Current_PLUS[3]), sizeof(int));
		return sizeof(int);
		break;
	case 396:
		memcpy(data, &(EPSData.EPS_Current_PLUS[4]), sizeof(int));
		return sizeof(int);
		break;
	case 397:
		memcpy(data, &(EPSData.EPS_Temperature_PLUS[0]), sizeof(int));
		return sizeof(int);
		break;
	case 398:
		memcpy(data, &(EPSData.EPS_Temperature_PLUS[1]), sizeof(int));
		return sizeof(int);
		break;
	case 399:
		memcpy(data, &(EPSData.EPS_Temperature_PLUS[2]), sizeof(int));
		return sizeof(int);
		break;
	case 400:
		memcpy(data, &(EPSData.EPS_Temperature_PLUS[3]), sizeof(int));
		return sizeof(int);
		break;
	case 401:
		memcpy(data, &(EPSData.EPS_Temperature_PLUS[4]), sizeof(int));
		return sizeof(int);
		break;
	case 402:
		memcpy(data, &(EPSData.EPS_Voltage_MINUS[0]), sizeof(int));
		return sizeof(int);
		break;
	case 403:
		memcpy(data, &(EPSData.EPS_Voltage_MINUS[1]), sizeof(int));
		return sizeof(int);
		break;
	case 404:
		memcpy(data, &(EPSData.EPS_Voltage_MINUS[2]), sizeof(int));
		return sizeof(int);
		break;
	case 405:
		memcpy(data, &(EPSData.EPS_Voltage_MINUS[3]), sizeof(int));
		return sizeof(int);
		break;
	case 406:
		memcpy(data, &(EPSData.EPS_Voltage_MINUS[4]), sizeof(int));
		return sizeof(int);
		break;
	case 407:
		memcpy(data, &(EPSData.EPS_Current_MINUS[0]), sizeof(int));
		return sizeof(int);
		break;
	case 408:
		memcpy(data, &(EPSData.EPS_Current_MINUS[1]), sizeof(int));
		return sizeof(int);
		break;
	case 409:
		memcpy(data, &(EPSData.EPS_Current_MINUS[2]), sizeof(int));
		return sizeof(int);
		break;
	case 410:
		memcpy(data, &(EPSData.EPS_Current_MINUS[3]), sizeof(int));
		return sizeof(int);
		break;
	case 411:
		memcpy(data, &(EPSData.EPS_Current_MINUS[4]), sizeof(int));
		return sizeof(int);
		break;
	case 412:
		memcpy(data, &(EPSData.EPS_Temperature_MINUS[0]), sizeof(int));
		return sizeof(int);
		break;
	case 413:
		memcpy(data, &(EPSData.EPS_Temperature_MINUS[1]), sizeof(int));
		return sizeof(int);
		break;
	case 414:
		memcpy(data, &(EPSData.EPS_Temperature_MINUS[2]), sizeof(int));
		return sizeof(int);
		break;
	case 415:
		memcpy(data, &(EPSData.EPS_Temperature_MINUS[3]), sizeof(int));
		return sizeof(int);
		break;
	case 416:
		memcpy(data, &(EPSData.EPS_Temperature_MINUS[4]), sizeof(int));
		return sizeof(int);
		break;
	case 417:
		memcpy(data, &(EPSData.EPS_ThreePointThreeCurrentSense), sizeof(int));
		return sizeof(int);
		break;
	case 418:
		memcpy(data, &(EPSData.EPS_FiveCurrentSense), sizeof(int));
		return sizeof(int);
		break;
	case 419:
		memcpy(data, &(EPSData.EPS_BatteryBusCurrentSense), sizeof(int));
		return sizeof(int);
		break;
	case 420:
		memcpy(data, &(EPSData.BAT_Status), sizeof(int));
		return sizeof(int);
		break;
	case 421:
		memcpy(data, &(EPSData.BAT_HeaterStatus), sizeof(int));
		return sizeof(int);
		break;
	case 422:
		memcpy(data, &(EPSData.BAT_BatteryCurrentDirection[0]), sizeof(int));
		return sizeof(int);
		break;
	case 423:
		memcpy(data, &(EPSData.BAT_BatteryCurrentDirection[1]), sizeof(int));
		return sizeof(int);
		break;
	case 424:
		memcpy(data, &(EPSData.BAT_BatteryCurrentDirection[2]), sizeof(int));
		return sizeof(int);
		break;
	case 425:
		memcpy(data, &(EPSData.BAT_BatteryCurrent[0]), sizeof(int));
		return sizeof(int);
		break;
	case 426:
		memcpy(data, &(EPSData.BAT_BatteryCurrent[1]), sizeof(int));
		return sizeof(int);
		break;
	case 427:
		memcpy(data, &(EPSData.BAT_BatteryCurrent[2]), sizeof(int));
		return sizeof(int);
		break;
	case 428:
		memcpy(data, &(EPSData.BAT_CellOneVoltage[0]), sizeof(int));
		return sizeof(int);
		break;
	case 429:
		memcpy(data, &(EPSData.BAT_CellOneVoltage[1]), sizeof(int));
		return sizeof(int);
		break;
	case 430:
		memcpy(data, &(EPSData.BAT_CellOneVoltage[2]), sizeof(int));
		return sizeof(int);
		break;
	case 431:
		memcpy(data, &(EPSData.BAT_BatteryVoltage[0]), sizeof(int));
		return sizeof(int);
		break;
	case 432:
		memcpy(data, &(EPSData.BAT_BatteryVoltage[1]), sizeof(int));
		return sizeof(int);
		break;
	case 433:
		memcpy(data, &(EPSData.BAT_BatteryVoltage[2]), sizeof(int));
		return sizeof(int);
		break;
	case 434:
		memcpy(data, &(EPSData.BAT_BatteryTemp[0]), sizeof(int));
		return sizeof(int);
		break;
	case 435:
		memcpy(data, &(EPSData.BAT_BatteryTemp[1]), sizeof(int));
		return sizeof(int);
		break;
	case 436:
		memcpy(data, &(EPSData.BAT_BatteryTemp[2]), sizeof(int));
		return sizeof(int);
		break;
	case 437:
		memcpy(data, &(EPSData.RBB_Status), sizeof(int));
		return sizeof(int);
		break;
	case 438:
		memcpy(data, &(EPSData.RBB_HeaterStatus), sizeof(int));
		return sizeof(int);
		break;
	case 439:
		memcpy(data, &(EPSData.RBB_BatteryVoltage), sizeof(int));
		return sizeof(int);
		break;
	case 440:
		memcpy(data, &(EPSData.RBB_CellVoltage), sizeof(int));
		return sizeof(int);
		break;
	case 441:
		memcpy(data, &(EPSData.RBB_Current), sizeof(int));
		return sizeof(int);
		break;
	case 442:
		memcpy(data, &(EPSData.RBB_Temperature), sizeof(int));
		return sizeof(int);
		break;
#endif
	case 443:
		memcpy(data, &(time[0]), 7 * sizeof(unsigned char));
		return 7 * sizeof(unsigned char);
		break;
	case 444:
		memcpy(data, &(num_reboots), sizeof(int));
		return sizeof(int);
		break;
	case 445:
		memcpy(data, &(prev_restart_reason), sizeof(int));
		return sizeof(int);
		break;
	case 446:
		memcpy(data, &(flash_status[0]), 2 * sizeof(unsigned char));
		return 2 * sizeof(unsigned char);
		break;
	case 447:
		memcpy(data, &(g_last_cmd[0]), 15 * sizeof(unsigned char));
		return LAST_CMD_SIZE * sizeof(unsigned char);
		break;
	case 448:
		memcpy(data, &(FlashWriteAddr), sizeof(long));
		return sizeof(long);
		break;
	case 449:
		memcpy(data, &(TelemWriteAddr), sizeof(long));
		return sizeof(long);
		break;
#ifdef __MHX__
	case 450:
		memcpy(data, &(mhx_status), 1 * sizeof(unsigned char));
		return 1 * sizeof(unsigned char);
		break;
#endif
#ifdef LOWER_PPM
	case 451:
		memcpy(data, &(globals.Geo_Enable), sizeof(char));
		return sizeof(char);
		break;
	case 452:
		memcpy(data, &(globals.ReadGeomagFromFlash), sizeof(char));
		return sizeof(char);
		break;
	case 453:
		memcpy(data, &(globals.orbitEnable), sizeof(char));
		return sizeof(char);
		break;
	case 454:
		memcpy(data, &(globals.GNC_Enable), sizeof(char));
		return sizeof(char);
		break;
	case 455:
		memcpy(data, &(globals.load_Enable), sizeof(char));
		return sizeof(char);
		break;
	case 456:
		memcpy(data, &(globals.attitudeSensorsEnable), sizeof(char));
		return sizeof(char);
		break;
	case 457:
		memcpy(data, &(globals.IMIEnable), sizeof(char));
		return sizeof(char);
		break;
#endif
#ifdef __ADC__
	case 458:
		memcpy(data, &(temperatureSensors[0]), sizeof(float));
		return sizeof(float);
		break;
	case 459:
		memcpy(data, &(temperatureSensors[1]), sizeof(float));
		return sizeof(float);
		break;
	case 460:
		memcpy(data, &(temperatureSensors[2]), sizeof(float));
		return sizeof(float);
		break;
	case 461:
		memcpy(data, &(temperatureSensors[3]), sizeof(float));
		return sizeof(float);
		break;
#endif
	case 462:
		memcpy(data, &(MiscWriteAddr), sizeof(long));
		return sizeof(long);
		break;
#ifdef __MAESTRO__
	case 463:
		memcpy(data, &(maestro_beacon1), sizeof(int));
		return sizeof(int);
		break;
	case 464:
		memcpy(data, &(maestro_beacon2), sizeof(int));
		return sizeof(int);
		break;
	case 465:
		memcpy(data, &(maestro_beacon3), sizeof(int));
		return sizeof(int);
		break;
	case 466:
		memcpy(data, &(maestro_beacon4), sizeof(int));
		return sizeof(int);
		break;
	case 467:
		memcpy(data, &(maestro_beacon5), sizeof(long));
		return sizeof(long);
		break;
	case 468:
		memcpy(data, &(maestro_beacon6), sizeof(long));
		return sizeof(long);
		break;
#endif
#ifdef UPPER_PPM
	case 512: {
		data[0] = (unsigned char)getPayloadPowerState(&gPayloadManager);
		return sizeof(unsigned char);
	}
	case 513: {
		memcpy(data, &gPayloadManager.numPayloadPacketsStored, sizeof(uint16_t));
		return sizeof(uint16_t);
	}
	case 514: {
		memcpy(data, &gLinkManager.numCommandsReceived, sizeof(uint16_t));
		return sizeof(uint16_t);
	}
#endif
	default: {
		sprintf(dbgbuf, "\r\nERROR - Attempted to add telemetry from unknown index %u", index);
		sendDBGALL(USER_PORT, dbgbuf);
		return 0;
	}
	} // switch

	return 0;
}

int16 buildTelemtryDataField(int cmd_length, unsigned char* rbcmd, unsigned char startIndex,
							 unsigned char* buf, int max_length) {
	// "i" counts the number of elements, "wrPtr" contains the
	// next position to write to (also the length of the buffer)
	int i = 0, wrPtr = 0, index = 0, elementSize = 0;

	// Flag indicates a telemetry data field was created successfully.
	short successflag = 1;

	// Note: command ("126") is the 10th byte.  So, the packet type
	// is the 11th byte (cmd1), and the element indicies begin at the 12th
	// byte (cmd2).

	// sendDBGALL(USER_PORT,"\r\nStarted recording telemetry elements.");

	for (i = 0; i <= (cmd_length - startIndex); i = i + 2) {
		index = make16(
			rbcmd[startIndex + i],
			rbcmd[startIndex + i + 1]); // Left-shift the first 8 bits and copy the second 8 bits

		// Debug------------------
		/*		sprintf(dbgbuf,"\r\nI=%u, Length=%u, Index=%u, startIndex=%u,
		   startIndex+1=%u",i,(cmd_length-11), index,rbcmd[startIndex+i],rbcmd[startIndex+i+1]);
				sendDBGALL(USER_PORT,dbgbuf);

				sprintf(dbgbuf,"\r\nAdding telemetry element %d at index %d.",index,wrPtr);
				sendDBGALL(USER_PORT,dbgbuf);
		*/		//------------------------

		elementSize = addTelmetryElement(&(buf[wrPtr]), index);
		if (elementSize == 0) {
			successflag = 0;
			sendDBGALL(USER_PORT, "\r\nInvalid telemetry element byte length.  Try requesting "
								  "valid telemetry elements.");
			break;
		}
		wrPtr += elementSize;

		// Assume that no single Telemetry Element is greater than 20 bytes in length.
		if (wrPtr >= max_length - 20) {
			successflag = 0;
			sendDBGALL(
				USER_PORT,
				"\r\nToo close to end of buffer.  Requested telemetry packet size is too big.");

			break; // If we get too close to the end of the buffer, bail.
		}
	}

	// sendDBGALL(USER_PORT,"\r\nFinished recording telemetry elements:");
	// HexPrint(USER_PORT,buf,wrPtr);

	if (successflag)
		return wrPtr;
	else
		return 0;
}

#ifdef __EPS_H__
void EPSBackgroundTask(short reset) {
	ReturnErr_t response;

	if (StartEPSAquireFlag) {
		// if(reset) sendDBGALL(USER_PORT,"\r\nStarting EPS aquisition cycle.");

		response = getAllEPSDevices(&EPSData, reset);

		if (response == SUCCESS) {
			// sendDBGALL(USER_PORT,"\r\nFinished aquiring EPS data.");

			// Indicate new data!
			EPSFlag = 1;

			// Reset the Run Flag
			StartEPSAquireFlag = 0;
		} else if (response == BUSY) {
			// sendDBGALL(USER_PORT,".");

			// Ensure continued running
			StartEPSAquireFlag = 1;
		} else {
			sendDBGALL(USER_PORT, "\r\nFailed aquiring EPS data");

			// Give up.
			StartEPSAquireFlag = 0;
		}
	}

	return;
}
#endif

#ifdef __AENEAS__
/*
LOAD EXTIN
This function loads the GNC's ExtIn[] array with various sensor, actuator and modelled data.

Note that ALL items should be in standard metric units in the vehicle's BODY frame.  Conversions to
this frame should happen just before loadExtIn() is called.

*/
ReturnErr_t loadExtIn() {
	ReturnErr_t retval = SUCCESS;

//------------------------------------------------------- Orbit and Sun Models
#ifdef __ORBIT__
	if (globals.orbitEnable) {
		// Orbit
		ExtIn[ECI_POS_X_IN] = orbitData[ORBIT_POS_X_INDEX];
		ExtIn[ECI_POS_Y_IN] = orbitData[ORBIT_POS_Y_INDEX];
		ExtIn[ECI_POS_Z_IN] = orbitData[ORBIT_POS_Z_INDEX];
		ExtIn[ECI_VEL_X_IN] = orbitData[ORBIT_VEL_X_INDEX];
		ExtIn[ECI_VEL_Y_IN] = orbitData[ORBIT_VEL_Y_INDEX];
		ExtIn[ECI_VEL_Z_IN] = orbitData[ORBIT_VEL_Z_INDEX];
		ExtIn[GHA_IN] = orbitData[ORBIT_GHA_INDEX];

		// Sun Model
		ExtIn[SUNVEC0_IN] = Sun_ECI[0];
		ExtIn[SUNVEC1_IN] = Sun_ECI[1];
		ExtIn[SUNVEC2_IN] = Sun_ECI[2];
	} else {
		sendDBGALL(USER_PORT, "\r\nOrbit Fail!");
		retval = FAILURE;
	}
#endif

//------------------------------------------------------- Geomagnetic Model
#ifdef __GEOMAG__
	if (globals.Geo_Enable) {
		// Geomag
		ExtIn[nT_ECI_x_IN] = B_field_ECI[0];
		ExtIn[nT_ECI_y_IN] = B_field_ECI[1];
		ExtIn[nT_ECI_z_IN] = B_field_ECI[2];
	} else {
		sendDBGALL(USER_PORT, "\r\nGeoMag Fail!");
		retval = FAILURE;
	}
#endif

//------------------------------------------------------- Magnetometer
#ifdef __MAG_H__
	if (MagFlag == SUCCESS) {
		// Check for a garbage reading (all F's)
		if (MagData[0] == 0xFFFF && MagData[1] == 0xFFFF && MagData[2] == 0xFFFF) {
			sendDBGALL(USER_PORT, "\r\nMag Garbage!");
			retval = FAILURE;
		}

		// Given to Flight Software in microTesla in the Body Frame.
		ExtIn[Measured_Mag_X_IN] = MagConvertedData.microTesla[0];
		ExtIn[Measured_Mag_Y_IN] = MagConvertedData.microTesla[1];
		ExtIn[Measured_Mag_Z_IN] = MagConvertedData.microTesla[2];
	} else {
		// sendDBGALL(USER_PORT, "M! ");
		retval = FAILURE;
	}
#endif //__MAG_H__

//------------------------------------------------------- Sun Sensor
#ifdef __SUNSENSOR__
	if (SunFlag == SUCCESS) {
		// Check the fit quality
		if (SunData.FitQuality < 0) {
			//			sendDBGALL(USER_PORT,"\r\nSS Fail Fit check.");
			ExtIn[Sun_Present_Flag_IN] = 0.0;
		} else if (SunData.GeometryQuality < 0) {
			//			sendDBGALL(USER_PORT,"\r\nSS Fail Geometry check.");
			ExtIn[Sun_Present_Flag_IN] = 0.0;
		} else {
			// Unit vector to the sun in the body coordinates.
			ExtIn[Measured_SS_x_IN] = SunData.Vector[0];
			ExtIn[Measured_SS_y_IN] = SunData.Vector[1];
			ExtIn[Measured_SS_z_IN] = SunData.Vector[2];
			ExtIn[Sun_Present_Flag_IN] = 1.0;
		}
	} else {
		ExtIn[Sun_Present_Flag_IN] = 0.0;
		//		sendDBGALL(USER_PORT,"\r\nNo Sun.");
	}
#endif //__SUNSENSOR__

//------------------------------------------------------- IMI-100 Wheel Speeds
#ifdef __IMI100_H__
	if (IMIFlag == SUCCESS) {
		// IMI is given to the FSW in rad/sec and body coordinates.  The conversion is done
		// elsewhere.
		ExtIn[Measured_RW_omega_x_IN] = IMIConvertedData.rwa_SpeedRPSBodyFrame[0];
		ExtIn[Measured_RW_omega_y_IN] = IMIConvertedData.rwa_SpeedRPSBodyFrame[1];
		ExtIn[Measured_RW_omega_z_IN] = IMIConvertedData.rwa_SpeedRPSBodyFrame[2];
	} else {
		// sendDBGALL(USER_PORT, "I! ");
	}
#endif

//------------------------------------------------------- Gyros
#ifdef __GYRO_H__
	if (GyroFlag == SUCCESS) {
		// Gyros are given to FSW in the body axis in rad/sec.  This conversion is done elsewhere.
		ExtIn[Measured_Gyro_X_IN] = GyroData.f_rateRPS[0];
		ExtIn[Measured_Gyro_Y_IN] = GyroData.f_rateRPS[1];
		ExtIn[Measured_Gyro_Z_IN] = GyroData.f_rateRPS[2];
	} else {
		sendDBGALL(USER_PORT, "G! ");
		retval = FAILURE;
	}
#endif

	return retval;
}
#endif //__AENEAS__

void RunCommand(unsigned char* ascii_cmd) {
	unsigned char RCbuf[GENERAL_BUFFER_SIZE];

	ParseCommandString(ascii_cmd, RCbuf);
	PadCommand(RCbuf);
	processCmd(USER_PORT, globals.RTU_ID, RCbuf, 1, 0);
}

// Takes a command in the form:
// (length) (cmd) (parameters)
// And shifts it to the right to make it
// (length) (source) (destination) (cmd) (parameters)
// Relies on the global "globals" variable.
// DOES NOT CHECK ARRAY BOUNDS!
void PadCommand(unsigned char* rcmd) {
	int n;

	for (n = rcmd[0]; n > 0; n--)
		rcmd[n + 8] = rcmd[n]; // Shift commands to the right
	rcmd[0] += 8; // increment length bit by 8

	memcpy(&rcmd[1], globals.Dest_ID, 4); // Use default destination
	memcpy(&rcmd[5], globals.RTU_ID, 4); // Use this unit's source
}

#ifdef __EPS_H__
// Checks the EPSData for the presence of sun (indicated by certain current counts, or maybe
// voltage.  I can't decide...) Returns 1 for True (Sun Found) and 0 for False.
int checkSun() {
	int threshold = 700; // Counts Below which there is probably Sun.
	//	int i;
	static int trigger = 0;
	unsigned char triggerflag = 0;
	unsigned char sunbuf[GENERAL_BUFFER_SIZE];

	// Check Deployables against threshold
	// The deployable panels are hooked up to:
	// -- BCRs 0,1, and 2 on the plus side
	// -- BCR 3 on the minus side

	// Checking +Y Side
	if (EPSData.EPS_Voltage_PLUS[0] < threshold && EPSData.EPS_Current_PLUS[0] < threshold) {
		triggerflag = 1;
		sprintf(sunbuf, "+Y Side triggered at ");
		getStringTime(time, &sunbuf[strlen(sunbuf)]);
	}

	// Checking +X Side
	if (EPSData.EPS_Voltage_PLUS[1] < threshold && EPSData.EPS_Current_PLUS[1] < threshold) {
		triggerflag = 1;
		sprintf(sunbuf, "+X Side triggered at ");
		getStringTime(time, &sunbuf[strlen(sunbuf)]);
	}

	// Checking -X Side
	if (EPSData.EPS_Voltage_PLUS[2] < threshold && EPSData.EPS_Current_PLUS[2] < threshold) {
		triggerflag = 1;
		sprintf(sunbuf, "-X Side triggered at ");
		getStringTime(time, &sunbuf[strlen(sunbuf)]);
	}

	// Checking -Y Side
	if (EPSData.EPS_Voltage_MINUS[3] < threshold && EPSData.EPS_Current_MINUS[3] < threshold) {
		triggerflag = 1;
		sprintf(sunbuf, "-Y Side triggered at ");
		getStringTime(time, &sunbuf[strlen(sunbuf)]);
	}

	if (triggerflag) {
		// sprintf(dbgbuf,"\r\nEPS=%d,%d.  Threshold is
		// %d",EPSData.EPS_Current_PLUS[i],EPSData.EPS_Current_MINUS[i],threshold[i]);
		// sendDBGALL(USER_PORT,dbgbuf);
		// if(EPSData.EPS_Current_PLUS[i]<threshold[i]) return 1;
		// if(EPSData.EPS_Current_MINUS[i]<threshold[i]) return 1;
		trigger++; // Increases the trigger point
		sendDBGALL(USER_PORT, sunbuf);
#ifdef __MYSTACK__
		PacketizeAndPush(
			sunbuf, strlen(sunbuf), MISC, GENERAL_BUFFER_SIZE, MISC_PACKET,
			PPM_NUM); // Parameters are: data,length,stack,max length, packet type, page address
#endif
	}

	// Once we are in the sun for a full 5 triggers, return true.
	if (trigger > 5) {
		trigger = 0;
		return 1;
	}

	return 0;
}
#endif //__EPS_H__

#ifdef __AENEAS__
// Quick function for debugging
int checkMode() {
	int mode;
	int i;
	mode = 99; // Set to something absurd to start

	for (i = Mode_1_Idle_OUT; i <= Mode_9_Momentum_Dump_OUT; i++) {
		if (ExtOut[i] > 0.5) {
			mode = i - Mode_1_Idle_OUT + 1;
			break; // break from for loop
		}
	}

	return mode;
}

//#ifndef FLIGHT
// A big debug function to watch GNC
void showGNCvalues(int port, int page) {
	//		int i;
	int mode;
	//		const int OUTCOL=40;

	// TEMPORARILY DISABLE THE MSEC TIMER TO USE SPRINTF %f
	disable_interrupts(INT_TIMER1);

	// Page 1
	switch (page) {
	case 1:
		// Clear the screen
		clrscrn(USER_PORT);

		// Header
		sprintf(dbgbuf, "--- GNC Screen ---  (Enable/Disable with "
						"r 110 X"
						")");
		locateString(USER_PORT, 1, 1, dbgbuf);

		// Possible Warning:
		if (!globals.load_Enable) {
			sendDBGALL(USER_PORT, "\r\nWARNING: EXTIN LOADING DISABLED!  DATA WILL NOT REFRESH!");
		}

		// Time
		sprintf(dbgbuf, "\r\nTime: %06.2f", ELAPSED_TIME);
		sendDBGALL(USER_PORT, dbgbuf);

		// Mode and certain enables
		mode = checkMode();
		sprintf(dbgbuf, "    Mode: %d [Override: %d, Limit: %d MDump: %d, MTR@4: %d]", mode,
				(int)ExtIn[Mode_Override_IN], (int)ExtIn[Mode_Limit_IN],
				(int)ExtIn[Enable_Momemtum_Dump_IN], (int)ExtIn[MTR_Enable_Mode4_IN]);
		sendDBGALL(USER_PORT, dbgbuf);

		// OTher FSW info
		/*			sprintf(dbgbuf,"\r\n    TSSNP: %f   Mode3 Time: %f",
						Time_since_Sun_not_present, Time_since_Mode_3_Start);
					sendDBGALL(USER_PORT,dbgbuf);
		*/
		// Attitude
		sprintf(dbgbuf, "\r\nAttitude: [%5.3f,%5.3f,%5.3f,%5.3f]  Err  : [%5.3f,%5.3f,%5.3f,%5.3f]",
				ExtOut[q1_OUT], ExtOut[q2_OUT], ExtOut[q3_OUT], ExtOut[q4_OUT], ExtOut[q1_err_OUT],
				ExtOut[q2_err_OUT], ExtOut[q3_err_OUT], ExtOut[q4_err_OUT]);
		sendDBGALL(USER_PORT, dbgbuf);
		/*				sprintf(dbgbuf,"\r\nMAttitude:[%5.3f,%5.3f,%5.3f,%5.3f]  WxErr:
		[%5.3f,%5.3f,%5.3f]", mike_q[0],mike_q[1],mike_q[2],mike_q[3],
						ExtOut[wx_err_OUT],ExtOut[wy_err_OUT],ExtOut[wz_err_OUT]);
					sendDBGALL(USER_PORT,dbgbuf);
					sprintf(dbgbuf,"\r\nCosine Matrix:
		[%5.3f,%5.3f,%5.3f]\r\n\t\t[%5.3f,%5.3f,%5.3f]\r\n\t\t[%5.3f,%5.3f,%5.3f]",
						mike_cmat[0],mike_cmat[1],mike_cmat[2],
						mike_cmat[3],mike_cmat[4],mike_cmat[5],
						mike_cmat[6],mike_cmat[7],mike_cmat[8]);
					sendDBGALL(USER_PORT,dbgbuf);
					sprintf(dbgbuf,"\r\nLOGIC: [%5.3f,%5.3f,%5.3f,%5.3f]",
						mike_logic[0],mike_logic[1],mike_logic[2],mike_logic[3]);
					sendDBGALL(USER_PORT,dbgbuf);
					sprintf(dbgbuf,"\r\nQs: [%5.3f,%5.3f,%5.3f,%5.3f]\r\n
		[%5.3f,%5.3f,%5.3f,%5.3f]", mike_noflip[0],mike_noflip[1],mike_noflip[2],mike_noflip[3],
						mike_noflip[4],mike_noflip[5],mike_noflip[6],mike_noflip[7]);
					sendDBGALL(USER_PORT,dbgbuf);
					sprintf(dbgbuf,"\r\n    [%5.3f,%5.3f,%5.3f,%5.3f]\r\n
		[%5.3f,%5.3f,%5.3f,%5.3f]", mike_noflip[8],mike_noflip[9],mike_noflip[10],mike_noflip[11],
						mike_noflip[12],mike_noflip[13],mike_noflip[14],mike_noflip[15]
		);
					sendDBGALL(USER_PORT,dbgbuf);
		*/

		// Gains
		sprintf(dbgbuf,
				"\r\nGains: POS[%2.4f,%2.4f,%2.4f] VEL[%2.4f,%2.4f,%2.4f] MOM[%2.2f,%2.2f,%2.2f]",
				ExtIn[kpx_IN], ExtIn[kpy_IN], ExtIn[kpz_IN], ExtIn[krx_IN], ExtIn[kry_IN],
				ExtIn[krz_IN], ExtIn[kMOMx_IN], ExtIn[kMOMy_IN], ExtIn[kMOMz_IN]);
		sendDBGALL(USER_PORT, dbgbuf);

		// Wheels
		sprintf(dbgbuf, "\r\nWheels:\r\n  CMDED [%5.3f,%5.3f,%5.3f]\r\n  SPD  [%5.0f,%5.0f,%5.0f]",
				IMIConvertedData.rwa_TorqueCmdIMIFrame[0],
				IMIConvertedData.rwa_TorqueCmdIMIFrame[1],
				IMIConvertedData.rwa_TorqueCmdIMIFrame[2], ExtIn[Measured_RW_omega_x_IN],
				ExtIn[Measured_RW_omega_y_IN], ExtIn[Measured_RW_omega_z_IN]);
		sendDBGALL(USER_PORT, dbgbuf);

		// Coils
		sprintf(dbgbuf, "\r\nCoils:\r\n  CMDED [%5.6f,%5.6f,%5.6f]\r\n  RCVD  [%5.6f,%5.6f,%5.6f]",
				ExtOut[Cmd_Dipole_X_OUT], ExtOut[Cmd_Dipole_Y_OUT], ExtOut[Cmd_Dipole_Z_OUT],
				IMIData.gf_torqueCoilCmd[0], IMIData.gf_torqueCoilCmd[1],
				IMIData.gf_torqueCoilCmd[2]);
		sendDBGALL(USER_PORT, dbgbuf);

		// Sensors
		if ((int)ExtIn[Sun_Present_Flag_IN]) {
			sprintf(dbgbuf,
					"\r\nSensors: \r\n  SUN  [%5.3f,%5.3f,%5.3f]\r\n  MAG  [%5.3f,%5.3f,%5.3f]\r\n "
					" GYRO [%5.5f,%5.5f,%5.5f]",
					ExtIn[Measured_SS_x_IN], ExtIn[Measured_SS_y_IN], ExtIn[Measured_SS_z_IN],
					ExtIn[Measured_Mag_X_IN], ExtIn[Measured_Mag_Y_IN], ExtIn[Measured_Mag_Z_IN],
					ExtIn[Measured_Gyro_X_IN], ExtIn[Measured_Gyro_Y_IN],
					ExtIn[Measured_Gyro_Z_IN]);
		} else {
			sprintf(dbgbuf,
					"\r\nSensors: \r\n  SUN  [xxxxx,xxxxx,xxxxx]\r\n  MAG  [%5.3f,%5.3f,%5.3f]\r\n "
					" GYRO [%5.5f,%5.5f,%5.5f]",
					ExtIn[Measured_Mag_X_IN], ExtIn[Measured_Mag_Y_IN], ExtIn[Measured_Mag_Z_IN],
					ExtIn[Measured_Gyro_X_IN], ExtIn[Measured_Gyro_Y_IN],
					ExtIn[Measured_Gyro_Z_IN]);
		}
		sendDBGALL(USER_PORT, dbgbuf);

		// Models
		sprintf(dbgbuf, "\r\nModels: \r\n   ORBIT\t\tGEOMAG\t\tSUN");
		sendDBGALL(USER_PORT, dbgbuf);
		sprintf(dbgbuf, "\r\nX: %7.0f\t\t%05.1f\t\t%3.1f", ExtIn[ECI_POS_X_IN], ExtIn[nT_ECI_x_IN],
				ExtIn[SUNVEC0_IN]);
		sendDBGALL(USER_PORT, dbgbuf);
		sprintf(dbgbuf, "\r\nY: %7.0f\t\t%05.1f\t\t%3.1f", ExtIn[ECI_POS_Y_IN], ExtIn[nT_ECI_y_IN],
				ExtIn[SUNVEC1_IN]);
		sendDBGALL(USER_PORT, dbgbuf);
		sprintf(dbgbuf, "\r\nZ: %7.0f\t\t%05.1f\t\t%3.1f", ExtIn[ECI_POS_Z_IN], ExtIn[nT_ECI_z_IN],
				ExtIn[SUNVEC2_IN]);
		sendDBGALL(USER_PORT, dbgbuf);

		// Settings
		sprintf(dbgbuf, "\r\nSettings:");
		sendDBGALL(USER_PORT, dbgbuf);
		sprintf(dbgbuf, "\r\n  Omega      : MIN=%3.2f , MAX=%3.2f", ExtIn[Min_Omega_IN],
				ExtIn[Max_Omega_IN]);
		sendDBGALL(USER_PORT, dbgbuf);
		sprintf(dbgbuf, "\r\n  Time Limits: SUN SEARCH=%3.0f , ECLIPSE_MODE=%3.0f",
				ExtIn[Sun_Search_Slew_Transition_IN], ExtIn[SP_Time_Limit_IN]);
		sendDBGALL(USER_PORT, dbgbuf);

		break;

	default:
		break;
	}

	// REENABLE TIMER
	enable_interrupts(INT_TIMER1);
}
//#endif

// A function to change the scale and rotation settings in the globals variable.
void ChangeScaleRotation(char ReadWrite, char SensorChoice, char ScaleRot, char* newnums) {
	switch (ReadWrite) {
	case 'r':
	case 'R':
		// read code here
		break;

	case 'w':
	case 'W':
		// write code
		if (ScaleRot == 'S' || ScaleRot == 's')
			WriteNewScale(SensorChoice, newnums);
		if (ScaleRot == 'R' || ScaleRot == 'r')
			WriteNewRot(SensorChoice, newnums);
		break;

	default:
		break;
	}
}

void WriteNewScale(char SC, char* newnums) {
	float fa[3];

	if (getFloats(fa, newnums, 3) != SUCCESS)
		return;

	switch (SC) {
	case 'G':
	case 'g':
		globals.GyroScale[0] = fa[0];
		globals.GyroScale[1] = fa[1];
		globals.GyroScale[2] = fa[2];
		break;

	case 'M':
	case 'm':
		globals.MagScale[0] = fa[0];
		globals.MagScale[1] = fa[1];
		globals.MagScale[2] = fa[2];
		break;

	case 'S':
	case 's':
		globals.SSScale[0] = fa[0];
		globals.SSScale[1] = fa[1];
		globals.SSScale[2] = fa[2];
		break;

	case 'W':
	case 'w':
		globals.RWAScale[0] = fa[0];
		globals.RWAScale[1] = fa[1];
		globals.RWAScale[2] = fa[2];
		break;

	case 'T':
	case 't':
		globals.MTRScale[0] = fa[0];
		globals.MTRScale[1] = fa[1];
		globals.MTRScale[2] = fa[2];
		break;

	default:
		break;
	}
}

void WriteNewRot(char SC, char* newnums) {
	float fa[9];

	if (getFloats(fa, newnums, 9) != SUCCESS)
		return;

	switch (SC) {
	case 'G':
	case 'g':
		globals.GyroRot[0] = fa[0];
		globals.GyroRot[1] = fa[1];
		globals.GyroRot[2] = fa[2];
		globals.GyroRot[3] = fa[3];
		globals.GyroRot[4] = fa[4];
		globals.GyroRot[5] = fa[5];
		globals.GyroRot[6] = fa[6];
		globals.GyroRot[7] = fa[7];
		globals.GyroRot[8] = fa[8];
		break;

	case 'M':
	case 'm':
		globals.MagRot[0] = fa[0];
		globals.MagRot[1] = fa[1];
		globals.MagRot[2] = fa[2];
		globals.MagRot[3] = fa[3];
		globals.MagRot[4] = fa[4];
		globals.MagRot[5] = fa[5];
		globals.MagRot[6] = fa[6];
		globals.MagRot[7] = fa[7];
		globals.MagRot[8] = fa[8];
		break;

	case 'S':
	case 's':
		globals.SSRot[0] = fa[0];
		globals.SSRot[1] = fa[1];
		globals.SSRot[2] = fa[2];
		globals.SSRot[3] = fa[3];
		globals.SSRot[4] = fa[4];
		globals.SSRot[5] = fa[5];
		globals.SSRot[6] = fa[6];
		globals.SSRot[7] = fa[7];
		globals.SSRot[8] = fa[8];
		break;

	case 'W':
	case 'w':
		globals.RWARot[0] = fa[0];
		globals.RWARot[1] = fa[1];
		globals.RWARot[2] = fa[2];
		globals.RWARot[3] = fa[3];
		globals.RWARot[4] = fa[4];
		globals.RWARot[5] = fa[5];
		globals.RWARot[6] = fa[6];
		globals.RWARot[7] = fa[7];
		globals.RWARot[8] = fa[8];
		break;

	case 'T':
	case 't':
		globals.MTRRot[0] = fa[0];
		globals.MTRRot[1] = fa[1];
		globals.MTRRot[2] = fa[2];
		globals.MTRRot[3] = fa[3];
		globals.MTRRot[4] = fa[4];
		globals.MTRRot[5] = fa[5];
		globals.MTRRot[6] = fa[6];
		globals.MTRRot[7] = fa[7];
		globals.MTRRot[8] = fa[8];
		break;

	default:
		break;
	}
	return;
}
#endif // __AENEAS__

/*

Regular Expression Search Testing

unsigned char* test = {0,0,0};
unsigned char * test = {0,0,0};
unsigned char * test[] = {0,0,0};
unsigned char test[] = {0,0,0};

float xxxx = 324235.4533;

chara any = any;
unsigned char any = any;

*/

#ifdef __MYSTACK__
// Disable TX_nonBlockProcess(&MISC_nb); and put this instead
/*
int CheckStackAndPrint()
{
	int len;
	unsigned char * data_ptr;
	stack_t * S;

	S = MISC;	// Hard-coded to MISC stack for this debug function

	// Check the stack
	stack_toplength(S,&len);				// Acquire the length
	if(len==0) return 0;					// Get out quickly if there's nothing on the stack

	// Send it to the debug port
	stack_pop_ptr(S, &data_ptr,&len);
	sendMSG(USER_PORT,data_ptr,len);
	return 1;
}
*/
#endif // __MYSTACK__

#ifdef __EPS_H__
// This function checks the various batteries for a low battery condition,
// and sets the battery pin appropriately.
// Returns the battery state as an int (see defines)

int checkBatteryState(eps_s* E) {
	int average_battery_voltage_counts;

	average_battery_voltage_counts =
		(E->BAT_BatteryVoltage[0] + E->BAT_BatteryVoltage[1] + E->BAT_BatteryVoltage[2]) / 3;

#ifdef DEBUG
	sprintf(dbgbuf, "\r\nLow Threshold:%u ", globals.BatteryThreshold[0]);
	sprintf(dbgbuf, "\r\nHigh Threshold:%u ", globals.BatteryThreshold[1]);
#endif // debug

	if (average_battery_voltage_counts < globals.BatteryThreshold[1])
		return BATTERY_STATE_HIGH;
	else if (average_battery_voltage_counts < globals.BatteryThreshold[0])
		return BATTERY_STATE_MEDIUM;
	else
		return BATTERY_STATE_LOW;
}

// Sends a command with the current battery state to the other processor
void updateOtherProcessBatteryState() {
	// Load Defaults on Upper (this clears the beaconflag)
	sprintf(Lbuf, "237 1 %d", BatteryState_g);
	sendPPMCmd(OTHER_PPM_PORT, Lbuf);
}

#endif // __EPS_H__

unsigned char getBatteryState() {
	return BatteryState_g;
}