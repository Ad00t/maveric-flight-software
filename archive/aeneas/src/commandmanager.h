#ifndef __COMMAND_MANAGER_H__
#define __COMMAND_MANAGER_H__

/***************************************************************************

								commandmanager.h

Company: USC/ISI
License: Proprietary
Author(s): Stephen Hunt

Purpose: Provides functionality for each command

***************************************************************************/

#include "config.h"
#include "errors.h"

ReturnErr_t ParseCommandString(unsigned char* commandString, unsigned char* commandBuffer);

#ifdef UPPER_PPM
/*
  Command:

  Function: Deletes all paylaod data stored in flash and resets payload data counters
*/
void processResetPayloadDataCmd(unsigned char* command);

/*
  Command: <Power Option>

  Power options: 0 = off, 1 = on
*/
void processPayloadPowerCmd(unsigned char* command);

/*
  Command: <Beacon Enabled> <Transmit Payload Data Enabled>

  Beacon Enabled: 0 = false, 1 = true
  Transmit Payload Data Enabled: 0 = false, 1 = true
*/
void processRadioModeCmd(unsigned char* command);

/*
  Command: <Payload Command>

  Sends a payload command, represented as a char buffer, to the payload
*/
void processForwardPayloadCommandCmd(unsigned char* payloadCommand, int payloadCommandLength);
#endif

#ifdef LOWER_PPM
#endif

#endif // __COMMAND_MANAGER_H__