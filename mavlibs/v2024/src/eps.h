/***************************************************************************

								eps.h

Company: USC/ISI
License: Proprietary
Author(s): Will Bezouska

Purpose: Header file for ClydeSpace EPS Board

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
08-30-10  Will	Initial revision.		

****************************************************************************
*/

#ifndef __EPS_H__
#define __EPS_H__



//========================================
// 				Dependency
//========================================
#include "i2c.h"

//========================================
// 				Defines
//========================================

#define BAT_I2C_ADDR 		0x2A
#define EPS_I2C_ADDR 		0x2B
#define RBB_I2C_ADDR 		0x2C

#define EPS_NUM_CHANNELS	33
#define BAT_NUM_CHANNELS	15
#define RBB_NUM_CHANNELS	4

//========================================
// 				Data structures
//========================================

typedef struct
{
	// EPS (or DEPS, whatever)
	int EPS_Status;
	int EPS_Voltage_PLUS[5];
	int EPS_Current_PLUS[5];
	int EPS_Temperature_PLUS[5];
	int EPS_Voltage_MINUS[5];
	int EPS_Current_MINUS[5];
	int EPS_Temperature_MINUS[5];
	int EPS_ThreePointThreeCurrentSense;
	int EPS_FiveCurrentSense;
	int EPS_BatteryBusCurrentSense;

	// Battery Board
	int BAT_Status;
	int BAT_HeaterStatus;
	int BAT_BatteryCurrentDirection[3];
	int BAT_BatteryCurrent[3];
	int BAT_CellOneVoltage[3];
	int BAT_BatteryVoltage[3];
	int BAT_BatteryTemp[3];

	// Remote Battry Board
	int RBB_Status;
	int RBB_HeaterStatus;
	int RBB_BatteryVoltage;
	int RBB_CellVoltage;
	int RBB_Current;
	int RBB_Temperature;

} eps_s;
	

//========================================
//  		Driver Functions
//========================================

// Initialization
void eps_init();

// Non-blocking Functions
ReturnErr_t getEPSDeviceTelemetry(unsigned char addr, unsigned char num_channels, eps_s * Data, short reset);
ReturnErr_t getAllEPSDevices(eps_s * Data, short reset);
void requestEPSChannel(unsigned char addr, unsigned char channel);
ReturnErr_t readEPSResponse(unsigned char addr, unsigned char * response);

// Decode and Store Data
void decodeEPSStatus(unsigned char addr, eps_s * Data, unsigned char * response, unsigned char channel);
void decodeEPSHeaterStatus(unsigned char addr, eps_s * Data, unsigned char * response, unsigned char channel);
float convertEPSChannel(unsigned char addr, unsigned char channel, int ADCreading);

// Command Functions (also Non-blocking)
void sendEPSCommand(unsigned char addr, unsigned char command, unsigned char value);
void epsFlicker(short ThreePointThree, short Five, short bus);
void epsTriggerWatchdog(unsigned char addr);
void epsTurnHeaterOn(unsigned char addr);
void epsTurnHeaterOff(unsigned char addr);

// Blocking Functions
int getEPSStatus(unsigned char addr);
int getEPSHeaterStatus(unsigned char addr);
ReturnErr_t blockForEPSChannel(unsigned char addr, unsigned char channel, int * value);
ReturnErr_t blockForEPSTelemetry(eps_s * Data, unsigned int max_time_seconds);
ReturnErr_t eps_functional_check(eps_s * Data, unsigned int max_time_seconds);

//========================================
// 		Development Functions
//========================================

#ifdef DEBUG

// Blocking Functions for Testing
void epsAutomaticDataCollection(long iterations, short continuous, short human);
int getEPSVersionNumber(unsigned char addr);
void testbytes2intTEN();

// Print to user port
void printEPSTelemetry(eps_s * Data, unsigned char counts);
void printEPSChannel(unsigned char addr, eps_s * Data, unsigned char channel);
void printEPSStatus(unsigned char addr, eps_s * Data);
void printEPSHeaterStatus(unsigned char addr, eps_s * Data);
void eps_show_batteries(eps_s * Data);

#endif //DEBUG

#endif // __EPS_H__
