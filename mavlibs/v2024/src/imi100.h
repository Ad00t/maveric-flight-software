/***************************************************************************

								imi100.h

Company: USC/ISI
License: Proprietary
Author(s): John Doe (jd), Jane Mary Dilbert (jmd)

Purpose: Header file for IMI-100 Driver

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
07-15-10  Will	Initial revision.		

****************************************************************************
*/

#ifndef __IMI100_H__
#define __IMI100_H__

//========================================
// 				Dependency
//========================================
#include "i2c.h"


//========================================
// 				Data structures
//========================================

// This is the structure of the telemetry returned from the IMI-100
// Dummy bytes indicate field of data within the IMI-100 data packet that 
// are not useful to this mission.  They can be added at a later date if required.
typedef struct 
{
 	unsigned int sync;									// Always 0xEB90
//	unsigned char dummyBytes1[6]; 						// Dummy bytes
	unsigned int gus_cmdValidCntr;
	unsigned int gus_cmdInValidCntr;
	unsigned int gus_cmdInvalidChksumCntr;
	unsigned char guc_LastCommand;
	unsigned char guc_acsMode;
//	unsigned char dummyBytes2[37]; 						// Dummy bytes
	signed int gs_rwsSpeedCmd[3];
	signed int gs_rwsSpeedTach[3];
	float gf_rwaTorqueCmd[3];
	signed char gc_rwaTorqueCmd[3];
	float gf_torqueCoilCmd[3];
	signed char gc_torqueCoilCmd[3];
//	unsigned char dummyBytes3[40]; 								// Dummy bytes
	unsigned char g_RotatingVariable_A[4];
	unsigned char g_RotatingVariable_B[4];
	unsigned char g_RotatingVariable_C[4];
	unsigned int crc;
} imi_s;


// This is an AENEAS-created structure used for holding converted IMI data.
typedef struct
{
	float rwa_SpeedRPSBodyFrame[3];			// RW speeds from the IMI, converted to radians/sec in the Body Frame
	float rwa_TorqueCmdIMIFrame[3];			// RW Torque Commands from the FSW, converted to N-m in the IMI Frame. 
	float gc_torqueCoilCmd[3];				// Torque Coil commands from the FSW, converted to Am^2 in the IMI Frame.
} imi_converted_s;

//========================================
//  		Driver Functions
//========================================

// Initialization
void imi100_init(int port,short on);
void imiDataInit(imi_s * iData);

// Non-blocking Functions
ReturnErr_t getIMITelemetry(int port, imi_s * telemetry);

// Blocking Commanding Functions:
ReturnErr_t setIMItoTestMode(int port);
ReturnErr_t setIMIReactionWheelSpeeds_DegPerSec(int port, float X, float Y, float Z);
ReturnErr_t setIMICoilDipole_Am2(int port, float X, float Y, float Z);
ReturnErr_t setIMIReactionWheelTorque_mNm(int port, float X, float Y, float Z);

// Interface to FSW Functions:
ReturnErr_t	convertFrameIMIToBody(imi_s * Original, imi_converted_s * Converted);		// IMI frame and unit conversion
ReturnErr_t	convertFrameBodyToIMI(imi_converted_s * Converted, 
									float X, float Y, float Z,					// Reaction Wheel Commands
									float MTRx, float MTRy, float MTRz);		// Torque Coil Commands

//========================================
// 		Development Functions
//========================================

#ifdef DEBUG
void printIMIData(imi_s * telemetry);
void printIMICnvData(imi_converted_s * data);
void imiAutomaticDataCollection(int port, long iterations, short continuous, short human);
ReturnErr_t blockForIMITelemetry(int port, imi_s * Data,unsigned int time_to_block);
#endif //DEBUG

#endif // __IMI100_H__
