// sboot_workspace.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

// This file defines the workspace structure used by the Maestro serial
// bootstrap code.

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

#ifndef __SBOOT_WORKSPACE_H
#define __SBOOT_WORKSPACE_H

#include "sboot_types.h"
#include "sboot_wreg.h"
#include "sboot_execution_state.h"
#include "sboot_shout.h"
#include "sboot_features.h"

struct sboot_workspace {
  // Which program should we execute?
  sboot_program_number_t program_number;

  // This is the program counter for the virtual machine implemented
  // by sboot:
  sboot_size_t input_offset;

  // This is the program counter at the start of the current command:
  sboot_size_t current_command_offset;

  // This timeout will be initialized to SBOOT_READ_TIMEOUT_DEFAULT, and may
  // get values from the bytestream.  The actual timeout will last at least as
  // long as the specified value.
  uint8 read_timeout; // seconds
  uint8 padding; // Avoid CCS compiler bug by adding padding.

  uint8 duration_until_timeout; //seconds
  sboot_timer_t start_of_timeout;

  // This timeout will be initialized to SBOOT_PROGRAM_TIMEOUT_DEFAULT, and may
  // get values from the bytestream.  The actual timeout will last at least as
  // long as the specified value.
  uint16 program_timeout; // seconds
  bool enable_program_timeout;
  bool override_program_timeout;
  sboot_timer_t program_start_time;

  // The current execution state:
  sboot_execution_state_t state;

#ifdef SBOOT_TRACE
  // The prior state, for tracing state changes.
  sboot_execution_state_t old_state;
#endif // SBOOT_TRACE

  // The next execution state (used to coalesce execution paths
  // when sending data to MAESTRO).
  sboot_execution_state_t next_state;

  // The execution state prior to stopping.
  sboot_execution_state_t state_before_stop;

  // Count of data to sent to MAESTRO:
  uint16 bytes_to_send;

  // The byte and word scratchpad registers.
  uint32 wreg[SBOOT_NUMBER_OF_WORD_REGISTERS];

  // The buffer used to build messages to the developer or user:
  uint16 shout_len;
  uint16 shout_buf_len; // less 1 for a trailing NUL
  uchar *shout_buf; // NULL if no buffer.
  sboot_shout_severity_t shout_severity;
  sboot_shout_reason_t shout_reason;

  // The virtual machine call stack:
  uint8 call_stack_len;
  sboot_size_t call_stack[SBOOT_CALL_STACK_SIZE];

  // This is a special buffer for assembling STRING_MSG output
  // from MAESTRO code:
  uchar string_msg_buf[SBOOT_STRING_MSG_BUF_MAX_LEN];
  uint8 string_msg_buf_len;

 // How many commands should we execute before yielding?
  uint16 execute_max_commands_before_yield;

  // Have we enabled the MAESTRO?  If so, automatically
  // disable it when entering a terminal state.
  bool maestro_is_enabled;

  // Have we seen a BOOT_PASS message from the MAESTRO level 1 test
  // code, and if so, did it indicate success or failure?
  bool boot_pass_succeeded;
  bool boot_pass_failed;

  // Have we seen a BOOT_DONE message from the MAESTRO level 2 test
  // code?
  bool boot_done;

  // Store the two 32-bit BOOT_PASS result vectors here:
  uint32 boot_pass_vector0;
  uint32 boot_pass_vector1;
};

typedef struct sboot_workspace sboot_workspace_t;

#endif // __SBOOT_WORKSPACE_H
