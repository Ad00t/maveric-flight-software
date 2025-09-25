// sboot_program.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

// This file contains definitions used for encoding test programs for
// execution on the Maestro chip in the Aenas satellite.

/*
 * This file contains names and concepts related to the Tilera bootstrap
 * message interface used in the Maestro (Tilera49) chip.  Some of these names
 * and concepts were extracted from the files containing the following
 * copyright notice:
 *
 * Copyright 2009 Tilera Corporation. All Rights Reserved.
 *
 *   The source code contained or described herein and all documents
 *   related to the source code ("Material") are owned by Tilera
 *   Corporation or its suppliers or licensors.  Title to the Material
 *   remains with Tilera Corporation or its suppliers and licensors. The
 *   software is licensed under the Tilera MDE License.
 *
 *   Unless otherwise agreed by Tilera in writing, you may not remove or
 *   alter this notice or any other notice embedded in Materials by Tilera
 *   or Tilera's suppliers or licensors in any way.
 */

#ifndef __SBOOT_PROGRAM_H
#define __SBOOT_PROGRAM_H

#include "sboot_features.h"

// TODO: Create a more generalized timeout system?

// TODO:  Create shorter forward JUMPSs (HOPs?) with uint8 and perhaps
// uint16 offsets.  There should be a savings in the sboot program, but
// it will add complexity to the generator.

// TODO: COMMAND_OP_WREG_LIST_TO_WREG has a uint8 list length.
// Do we need a uint32 version for generality?

// Some thoughts:
//
// 1) The conditional backward jumps are not frequently used.

enum sboot_program_command {
  // Output messages:
  COMMAND_SHOUT,              // 0
  COMMAND_SHOUT_VALUE,        // 1
  COMMAND_SHOUT_WREG,         // 2
  COMMAND_SHOUT_WREG_FIELD,   // 3
  COMMAND_SHOUT_WREG_WREG,    // 4
  COMMAND_SHOUT_IF_ZERO,      // 5
  COMMAND_SHOUT_IF_NONZERO,   // 6

  // Time management:
  COMMAND_SET_READ_TIMEOUT,       // 7
  COMMAND_WAIT,                   // 8
  COMMAND_GET_CURRENT_SECONDS,    // 9

  // MAESTRO interface:
  COMMAND_ENABLE_MAESTRO,         // 10
  COMMAND_DISABLE_MAESTRO,        // 11
  COMMAND_RESET_MAESTRO,          // 12
  COMMAND_SEND_BYTE_TO_MAESTRO,   // 13
  COMMAND_SEND_WORD_TO_MAESTRO,   // 14
  COMMAND_SEND_BYTES_TO_MAESTRO,  // 15
  COMMAND_READ_BYTE_FROM_MAESTRO, // 16

  // Data copying:
  COMMAND_INSERT_BYTE_INTO_WORD,  // 17
  COMMAND_PACK_BYTES_INTO_WORD,   // 18
  COMMAND_ZERO_WORD,              // 19
  COMMAND_SET_WORD,               // 20
  COMMAND_COPY_WORD,              // 21

  // Arithmetic and logical operations:
  COMMAND_OP_WREG_WREG_TO_WREG,   // 22
#ifdef SBOOT_OPTIMIZE_IMMEDIATES
  COMMAND_OP_WREG_ZERO_TO_WREG,   // 23
  COMMAND_OP_WREG_UINT8_TO_WREG,  // 24
  COMMAND_OP_WREG_UINT16_TO_WREG, // 25
  COMMAND_OP_WREG_UINT24_TO_WREG, // 26
#endif // SBOOT_OPTIMIZE_IMMEDIATES
  COMMAND_OP_WREG_UINT32_TO_WREG, // 27
  COMMAND_OP_WREG_LIST_TO_WREG,   // 28

  // Forward JUMPs use a uint32 offset:
  COMMAND_JUMP_FORWARD,                    // 29
  COMMAND_JUMP_FORWARD_IF_ZERO,            // 30
  COMMAND_JUMP_FORWARD_IF_NONZERO,         // 31

#ifdef SBOOT_OPTIMIZE_JUMPS
  // Backward JUMPs may use a compressed offset:
  COMMAND_JUMP_BACKWARD_UINT8,             // 32
  COMMAND_JUMP_BACKWARD_IF_ZERO_UINT8,     // 33
  COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT8,  // 34

  COMMAND_JUMP_BACKWARD_UINT16,            // 35
  COMMAND_JUMP_BACKWARD_IF_ZERO_UINT16,    // 36
  COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT16, // 37

  COMMAND_JUMP_BACKWARD_UINT24,            // 38
  COMMAND_JUMP_BACKWARD_IF_ZERO_UINT24,    // 39
  COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT24, // 40
#endif // SBOOT_OPTIMIZE_JUMPS

  COMMAND_JUMP_BACKWARD_UINT32,            // 41
  COMMAND_JUMP_BACKWARD_IF_ZERO_UINT32,    // 42
  COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT32, // 43

  // Subroutine calls are backward only.
#ifdef SBOOT_OPTIMIZE_CALLS
  // Offsets may be compressed:
  COMMAND_CALL_UINT8,                      // 44
  COMMAND_CALL_UINT16,                     // 45
  COMMAND_CALL_UINT24,                     // 46
#endif // SBOOT_OPTIMIZE_CALLS
  COMMAND_CALL_UINT32,                     // 47

  // Subroutine returns:
  COMMAND_RETURN,                          // 48

  // Specialized code for reassembling a string message from the MAESTRO:
  COMMAND_ASSEMBLE_STRING_MSG,             // 49

  // Successful program execution:
  COMMAND_DONE,                            // 50

  // Capture certain success/failure indications from the MAESTRO:
  COMMAND_NOTE_BOOT_PASS_SUCCEEDED,        // 51
  COMMAND_NOTE_BOOT_PASS_FAILED,           // 52
  COMMAND_NOTE_BOOT_DONE,                  // 53

  // Metadata about the program:
  COMMAND_PROGRAM_INFO,                    // 54

  // Read a byte from a program in memory, for use by
  // satellite-safe CRC checks, etc.
  COMMAND_READ_PROGRAM_BYTE,		   // 55
  COMMAND_GET_NUMBER_OF_PROGRAMS,          // 56
  COMMAND_GET_FIRST_PROGRAM_NUMBER,        // 57
  COMMAND_VALIDATE_PROGRAM_NUMBER,         // 58
  COMMAND_CRC_KERNEL,                      // 59

  // Reconfigure the UART to the MAESTRO.
  COMMAND_CHANGE_UART_SPEED,		   // 60

  // This ought to go under time management:
  COMMAND_SET_PROGRAM_TIMEOUT              // 61

};

typedef enum sboot_program_command sboot_program_command_t;

// This is the length of the UART data header for a SEND or READ command.
//
// TODO: Find a better home for this definition?
#define UART_DATA_HEADER_LEN 4

#endif // __SBOOT_PROGRAM_H
