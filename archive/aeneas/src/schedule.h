/***************************************************************************

								schedule.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne

Purpose: Header file for the scheduling system

Usage: See the descriptions of each function below.

Changelog:

Date 	 |  Au.  |	Notes
07-19-11	mra		Initial revision.		
****************************************************************************
*/



#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

//========================================
//  			Dependencies
//========================================
#include "common.h"							// Global Definitions, Clock Speeds, Interrupt Registers and Port Settings
#include "debug.h"							// sendDBGALL() functionality
#include "crc.h"							// getCRC() functionality
#include "fredtime.h"						// Time vector definitions

extern TFlashProtected globals;				// defined in main
extern unsigned char time[7];						// defined in main
extern int processCmd(int port, unsigned char *source, unsigned char *rbcmd, int ncmds, unsigned char ack); // defined in main


//========================================
//  			DEFINES
//========================================
#define SCHEDULED_PORT		USER_PORT			// When a command is activated from the schedule, this is the port used for any response


//========================================
// 				Data structures
//========================================


//========================================
//  		Driver Functions
//========================================
void checkSchedule(void);
ReturnErr_t setSchedule(unsigned char * Srow, 
						int MaxSize,				// Maximum size of the Srow array, so we don't overflow
						unsigned char * command,
						unsigned char CmdSize,		// Size of the Command and parameters (does not include the length byte or CRC)
						unsigned char * time);
ReturnErr_t clearSchedule(unsigned char * Srow);
//========================================
// 		Development Functions
//========================================

#ifdef DEBUG

#endif // DEBUG

#endif // __GYRO_H__
