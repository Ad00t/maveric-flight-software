// sboot_execution.c  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

/*
 * This is a serial bootstrapper for the Maestro chip. It is intended to be
 * incorporated into the code running on the upper processor of the Aeneas
 * satellite.  This imposes certain stylistic constraints.  For example,
 * the satellite's code imcorporates ".c" files directly, rather than
 * using seperate compilation.
 *
 * TODO: Consider an overall execution timeout.
 * TODO: Consider a timeout on sending data to the UART.
 * TODO: Consider limitations on the number or frequency of "shout" messages
 * that may be issued.
 * TODO: Byte manipulations are implemented with shifts.  Is this efficient,
 * expecially for uint32's on the PIC24?
 */

#if defined(__PCD__) && !defined(__24FJ256GA110_H__)
#include "../../common.h"
#define __24FJ256GA110_H__
#endif // defined(__PCD__)

#include "../includes/sboot_types.h"
#include "../includes/sboot_shout.h"
#include "../includes/sboot_wreg.h"
#include "../includes/sboot_program.h"
#include "../includes/sboot_op.h"
#include "../includes/sboot_features.h"
#include "../includes/sboot_execution.h"
#include "../includes/sboot_callbacks.h"
#include "../includes/sboot_workspace.h"
#include "../includes/sboot_execution_state.h"

// make16(...) and make32(...) are helper functions provided
// by the PCD compiler.  The copies that follow provided a
// subset of the functionality of the originals.
//
// TODO: These routines should be moved into a separate file.
#ifndef __PCD__
static uint16 make16(uint8 high_byte, uint8 low_byte) {
  return ((uint16)low_byte) | (((uint16)high_byte) << 8);
}

static uint32 make32(uint8 high_byte, uint8 upper_middle_byte,
		     uint8 lower_middle_byte, uint8 low_byte) {
  uint32 value;
  value = low_byte;
  value |= ((uint32)lower_middle_byte) << 8;
  value |= ((uint32)upper_middle_byte) << 16;
  value |= ((uint32)high_byte) << 24;
  return value;
}
#endif // __PCD__

// make24(...) is a helper routine that provides access to some
// of the functionality of make32(...), avoiding problematic
// polymorphism.
#define make24(a,b,c) make32(0,a,b,c)

#ifdef SBOOT_TRACE
#ifndef __PCD__
#include <stdio.h>
#endif // __PCD__

// If SBOOT_TRACE is defined, code will be compiled that prints execution
// traces.  The sboot_trace variable is defined outside the workspace to
// facilitate debugging activities.
static bool sboot_trace_flag = FALSE;

#endif // SBOOT_TRACE

void sboot_set_trace(bool value) {
#ifdef SBOOT_TRACE
  sboot_trace_flag = value;
#endif // SBOOT_TRACE
}
bool sboot_get_trace() {
#ifdef SBOOT_TRACE
  return sboot_trace_flag;
#else 
  return FALSE;
#endif // SBOOT_TRACE
}

#ifdef SBOOT_TRACE
#if defined(__PCD__) && defined(__MAESTRO__)
void maestro_msg(char *msg);
void maestro_msg_dbgbuf();
#endif // __PCD__
#endif // SBOOT_TRACE

// Here's the global state:
static sboot_workspace_t sboot_workspace;

// TODO: could use memset(...) here.
void sboot_initialize(sboot_program_number_t program_number) {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_initialize_workspace(workspace)\n");
    printf(" sizeof(sboot_workspace_t) = %d\n", sizeof(sboot_workspace_t));
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  sboot_workspace.program_number = program_number;
  sboot_workspace.input_offset = 0;
  sboot_workspace.current_command_offset = 0;

  sboot_workspace.read_timeout = SBOOT_READ_TIMEOUT_DEFAULT;

  // Timeout workspace share between reset, delay, and reading
  // from the MAESTRO.
  sboot_workspace.duration_until_timeout = 0;
  sboot_workspace.start_of_timeout = 0;

  sboot_workspace.program_timeout = SBOOT_PROGRAM_TIMEOUT_DEFAULT;
  sboot_workspace.enable_program_timeout = SBOOT_ENABLE_PROGRAM_TIMEOUT_DEFAULT;
  sboot_workspace.override_program_timeout = SBOOT_OVERRIDE_PROGRAM_TIMEOUT_DEFAULT;
  sboot_workspace.program_start_time = 0;

  sboot_workspace.state = SBOOT_STATE_INITIALIZED;
#ifdef SBOOT_TRACE
  sboot_workspace.old_state = SBOOT_STATE_INITIALIZED;
#endif // SBOOT_TRACE
  sboot_workspace.next_state = SBOOT_STATE_UNINITIALIZED;
  sboot_workspace.state_before_stop = SBOOT_STATE_UNINITIALIZED;

  sboot_workspace.bytes_to_send = 0;

  {
    wreg_t wreg;
    for (wreg = 0; wreg < SBOOT_NUMBER_OF_WORD_REGISTERS; wreg++) {
      sboot_workspace.wreg[wreg] = 0;
    }
  }

  sboot_workspace.call_stack_len = 0;
  sboot_workspace.string_msg_buf_len = 0;

  sboot_workspace.execute_max_commands_before_yield =
    SBOOT_EXECUTE_MAX_COMMANDS_BEFORE_YIELD_DEFAULT;

  sboot_workspace.maestro_is_enabled = FALSE;

  sboot_workspace.boot_pass_succeeded = FALSE;
  sboot_workspace.boot_pass_failed = FALSE;
  sboot_workspace.boot_pass_vector0 = 0;
  sboot_workspace.boot_pass_vector1 = 0;
  sboot_workspace.boot_done = FALSE;
}

//  Get the current program number.
sboot_program_number_t sboot_get_program_number() {
  return sboot_workspace.program_number;
}
void sboot_set_program_number(sboot_program_number_t program_number) {
  sboot_workspace.program_number = program_number;
}

//  Get/set the current program offset.
sboot_size_t sboot_get_program_offset() {
  return sboot_workspace.input_offset;
}
void sboot_set_program_offset(sboot_size_t offset) {
  sboot_workspace.input_offset = offset;
}


// Get the offset of the start of the current command, for debugging.
sboot_size_t sboot_get_current_command_offset() {
  return sboot_workspace.current_command_offset;
}
void sboot_set_current_command_offset(sboot_size_t offset) {
  sboot_workspace.current_command_offset = offset;
}

// Get/set a WREG value.  This can be used for debugging, or for invoking
// programs with arguments.
bool sboot_get_wreg(wreg_t wreg_index, uint32 *valuep) {
  if (wreg_index >= SBOOT_NUMBER_OF_WORD_REGISTERS) {
    return FALSE;
  }
  *valuep = sboot_workspace.wreg[wreg_index];
  return TRUE;
}
bool sboot_set_wreg(wreg_t wreg_index, uint32 value) {
  if (wreg_index >= SBOOT_NUMBER_OF_WORD_REGISTERS) {
    return FALSE;
  }
  sboot_workspace.wreg[wreg_index] = value;
  return TRUE;
}

// Get/set the program timeout.
void sboot_set_program_timeout(uint16 value) {
  sboot_workspace.program_timeout = value;
}
uint16 sboot_get_program_timeout() {
  return sboot_workspace.program_timeout;
}

// Is the program timeout enabled?
void sboot_set_enable_program_timeout(bool value) {
  sboot_workspace.enable_program_timeout = value;
}
bool sboot_get_enable_program_timeout() {
  return sboot_workspace.enable_program_timeout;
}

// Is the program timeout overidden?
void sboot_set_override_program_timeout(bool value) {
  sboot_workspace.override_program_timeout = value;
}
bool sboot_get_override_program_timeout() {
  return sboot_workspace.override_program_timeout;
}

// Get/set the program start time.
void sboot_set_program_start_time(sboot_timer_t value) {
  sboot_workspace.program_start_time = value;
}
sboot_timer_t sboot_get_program_start_time() {
  return sboot_workspace.program_start_time;
}

// This routine exists to provide convenient tracing.
static sboot_timer_t sboot_fetch_current_seconds() {
  sboot_timer_t current_seconds;
  current_seconds = sboot_current_seconds();
#ifdef SBOOT_TRACE
  static sboot_timer_t previous_seconds;
  previous_seconds = 0;
  if (sboot_trace_flag && current_seconds != previous_seconds) {
#ifndef __PCD__
    printf("+++ time=%d\n", (sboot_timer_t) current_seconds);
    previous_seconds = current_seconds;
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  return current_seconds;
}

// sboot_start() should start running, but only if in SBOOT_INITIALIZED state.
// If currently in SBOOT_INITIALIZED state, it advances to the running state.
// Otherwise, it is ignored (idempotency).  Should it produce feedback messages
// as confirmation?
void sboot_start() {
  switch (sboot_workspace.state) {
  case SBOOT_STATE_INITIALIZED: {
    sboot_workspace.state = SBOOT_STATE_RUNNING;
    sboot_workspace.program_start_time = sboot_fetch_current_seconds();
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_start(): entering the running state.\n");
#endif // __PCD__
    }
#endif // SBOOT_TRACE
    break;
  }

  default: {
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_start(): unknown or incorrect state %d\n", sboot_workspace.state);
#endif // __PCD__
    }
#endif // SBOOT_TRACE
  }
  }
}

// sboot_stop() should change state to STOPPED, saving the prior
// state, but not if the prior state was itself STOPPED (idempotency)
// or certain other initial or terminal states.
void sboot_stop() {
  switch (sboot_workspace.state) {
  case SBOOT_STATE_UNINITIALIZED:
  case SBOOT_STATE_INITIALIZED:
  case SBOOT_STATE_STOPPED:
  case SBOOT_STATE_FAILED:
  case SBOOT_STATE_DONE: {
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_stop(): staying in state %d.\n", sboot_workspace.state);
#endif // __PCD__
    }
#endif // SBOOT_TRACE
    break; // Do nothing.
  }

  case SBOOT_STATE_RUNNING:
  case SBOOT_STATE_WAITING:
  case SBOOT_STATE_RESETTING_MAESTRO:
  case SBOOT_STATE_SENDING_DATA_TO_MAESTRO:
  case SBOOT_STATE_READING_DATA_FROM_MAESTRO: {
    sboot_workspace.state_before_stop = sboot_workspace.state;
    sboot_workspace.state = SBOOT_STATE_STOPPED;
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_start(): entering the stopped state from state %d.\n", sboot_workspace.state);
#endif // __PCD__
    }
#endif // SBOOT_TRACE
    break;
  }

  default: {
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_stop(): unknown state %d\n", sboot_workspace.state);
#endif // __PCD__
    }
#endif // SBOOT_TRACE
  }
  }
}

// sboot_resume() is similar to sboot_start(), except that it it will
// operate only if the current state is SBOOT_STATE_STOPPED.  It restores
// the state before the sboot_stop(). You can use
// sboot_stop()/sboot_resume() as a single-level lock.
void sboot_resume() {
  switch (sboot_workspace.state) {
  case SBOOT_STATE_STOPPED: {
    sboot_workspace.state = sboot_workspace.state_before_stop;
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_resume(): resuming state %d.\n", sboot_workspace.state);
#endif // __PCD__
    }
#endif // SBOOT_TRACE
    break;
  }

  default: {
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_resume(): unknown or incorrect state %d\n", sboot_workspace.state);
#endif // __PCD__
    }
#endif // SBOOT_TRACE
  }
  }
}

// Get/set the max number of commands to execute before yielding.
//
// Note: Any value set here gets reset to the default when
// sboot_initialize() is called.
uint16 sboot_get_max_commands_before_yield() {
  return sboot_workspace.execute_max_commands_before_yield;
}

void sboot_set_max_commands_before_yield(uint16 value) {
  sboot_workspace.execute_max_commands_before_yield = value;
}

// Get the current execution state as a uint8.  This will be
// helpful for debugging or status messages.
uint8 sboot_get_execution_state_byte() {
  return (uint8)(sboot_workspace.state);
}

uint8 sboot_get_execution_next_state_byte() {
  return (uint8)(sboot_workspace.next_state);
}

uint8 sboot_get_execution_state_before_stop_byte() {
  return (uint8)(sboot_workspace.state_before_stop);
}

// Do we think we've enabled the MAESTRO?
bool sboot_is_maestro_enabled() {
  return sboot_workspace.maestro_is_enabled;
}

// Have we seen a BOOT_PASS message from the MAESTRO level 1 test
// code, and if so, did it indicate success or failure?
bool sboot_get_boot_pass_succeeded() {
  return sboot_workspace.boot_pass_succeeded;
}
bool sboot_get_boot_pass_failed() {
  return sboot_workspace.boot_pass_failed;
}

// Return the two 32-bit BOOT_PASS result vectors.
uint32 sboot_get_boot_pass_vector0() {
  return sboot_workspace.boot_pass_vector0;
}
uint32 sboot_get_boot_pass_vector1() {
  return sboot_workspace.boot_pass_vector1;
}

// Have we seen a BOOT_DONE message from the MAESTRO level 2 test
// code?
bool sboot_get_boot_done() {
  return sboot_workspace.boot_done;
}

// Complete the shout message and send it off.
//
// Add a terminating null character in case the callback prefers it.
void sboot_send_shout_done() {
  sboot_workspace.shout_buf[sboot_workspace.shout_len] = '\0';
  sboot_send_shout(sboot_workspace.shout_severity,
		   sboot_workspace.shout_reason,
		   sboot_workspace.shout_len);
}

// Transfer a character into the shout buffer, truncating on overflow.
// Returns TRUE if there was space in the buffer, else returns FALSE.
bool sboot_shout_uchar(uchar c) {
  if (sboot_workspace.shout_len < sboot_workspace.shout_buf_len) {
    sboot_workspace.shout_buf[sboot_workspace.shout_len++] = c;
    return TRUE;
  } else {
    return FALSE;
  }
}

// Build and send messages (shouts).
void sboot_start_shouting(sboot_shout_severity_t severity,
			  sboot_shout_reason_t reason) {
  uint16 buf_len;
  if (sboot_get_shout_buf(&sboot_workspace.shout_buf, &buf_len)) {
    sboot_workspace.shout_buf_len = buf_len - 1;
  } else {
    sboot_workspace.shout_buf = 0;
    sboot_workspace.shout_buf_len = 0;
  }
  sboot_workspace.shout_len = 0;
  sboot_workspace.shout_severity = severity;
  sboot_workspace.shout_reason = reason;
}

// Transfer a C string *constant* into the shout buffer, truncating on
// overflow.
void sboot_shout_string(string_literal msg) {
  for (;;) {
    uchar c;
    c = (uchar)(*msg);
    if (c == 0) {
      break;
    }
    if (!sboot_shout_uchar(c)) {
      break;
    }
    msg = msg + 1;// Avoid possible compiler bug.
  }
}

// Transfer a string from the input program into the shout buffer,
// truncating on overflow.  Die if program exhaustion occurs.
static void sboot_shout_stream(uint8 len) {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_shout_stream(%d)\n", len);
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  while (len--) {
    uint16 result = sboot_fetch_program_byte(sboot_workspace.program_number,
					     sboot_workspace.input_offset);
    if (result == SBOOT_FETCH_ERROR) {
      sboot_workspace.state = SBOOT_STATE_FAILED;
      sboot_shout_string("|sboot program exhausted");
      return;
    }
    sboot_workspace.input_offset = sboot_workspace.input_offset + 1; // Avoid possible compiler bug

    if (!sboot_shout_uchar((uchar)result)) {
      // The shout buffer is full.  Skip fetching the rest of the
      // string, but don't let the input offset get out of sync.
      sboot_workspace.input_offset =
	sboot_workspace.input_offset + len; // Avoid possible compiler bug
      return;
    }
  }
}

// Append a hex value into the shout buffer, truncating on overflow.
// Surround the value in parentheses, in part to guard against
// misinterpretation if buffer overflow does occur.
void sboot_shout_hex_value(uint32 value) {
  // Special case: zero value is formatted without leading "0x":
  if (value == 0) {
    sboot_shout_string("(0)");
    return;
  }

  sboot_shout_string("(0x");

  char stack[8]; // Embedded assumption about uint32!
  uint8 len;
  len = 0;

  // Break the value into nybbles, convert them to digits, and push them
  // on the stack.
  while (value != 0) {
    uint8 nybble;
    nybble = value & 0x0f;
    if (nybble < 10) {
      stack[len++] = '0' + nybble;
    } else {
      stack[len++] = 'A' + nybble - 10;
    }
    value = value >> 4;
  }

  // Pop the digits off the stack and append them to the shout buffer.
  while (len > 0) {
    sboot_shout_uchar((uchar)stack[--len]);
  }

  sboot_shout_uchar((uchar)')');
}

// Shout the offset of the current command.
static void sboot_shout_command_offset() {
  sboot_shout_string(" [pc=");
  sboot_shout_hex_value(sboot_workspace.current_command_offset);
  uint32 diff;
  diff = sboot_workspace.input_offset - sboot_workspace.current_command_offset;
  sboot_shout_string("+");
  sboot_shout_hex_value(diff);
  sboot_shout_string("]");
}

// Shout about a failure, using a normal "C" string as a message.
static void sboot_shout_failure(sboot_shout_reason_t reason, string_literal msg) {
  sboot_start_shouting(SBOOT_SHOUT_FAILURE, reason);
  sboot_shout_string(msg);
  sboot_shout_command_offset();
  sboot_send_shout_done();

  sboot_workspace.state = SBOOT_STATE_FAILED; // Might already be set to this value.
}

// Shout about a failure, using a normal "C" string as a message, and
// appending a value (in hex).
static void sboot_shout_failure_with_value(sboot_shout_reason_t reason,
					   string_literal msg,
					   uint32 value) {
  sboot_start_shouting(SBOOT_SHOUT_FAILURE, reason);
  sboot_shout_string(msg);
  sboot_shout_hex_value(value);
  sboot_shout_command_offset();
  sboot_send_shout_done();

  sboot_workspace.state = SBOOT_STATE_FAILED; // Might already be set to this value.
}


// returns TRUE if the timeout is still in progress, otherwise FALSE.
static bool sboot_timeout_still_running(sboot_timer_t start_of_timeout,
					sboot_timer_t duration_until_timeout) {
  sboot_timer_t end_of_timeout;
  end_of_timeout = start_of_timeout + duration_until_timeout; // may roll over
  sboot_timer_t current_seconds;
  current_seconds = sboot_fetch_current_seconds();
  if (current_seconds < end_of_timeout) {
    return TRUE;
  }
  if (end_of_timeout < start_of_timeout &&
      current_seconds >= start_of_timeout) {
    return TRUE;
  }
  return FALSE;
}

// returns TRUE if the timeout is complete, otherwise FALSE.
static bool sboot_timeout_is_complete(sboot_execution_state_t new_state) {
  // If sboot_workspace.duration_until_timeout is 0, treat it as a request
  // for an immediate timeout (rather than no timeout).
  uint8 duration_until_timeout;
  duration_until_timeout = sboot_workspace.duration_until_timeout;
  if (duration_until_timeout == 0) {
    sboot_workspace.state = new_state;

#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_timeout_is_complete(...): no timeout delay, state=%d\n",
	     new_state);
#endif // __PCD__
    }
#endif // SBOOT_TRACE

    return TRUE;
  }

  if (sboot_timeout_still_running(sboot_workspace.start_of_timeout,
				  duration_until_timeout)) {
    return FALSE;
  }
  sboot_workspace.state = new_state;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_timeout_is_complete(...): timeout complete, state=%d\n",
	   new_state);
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  return TRUE;
}

// Returns a uint8 byte from the command program, or the special value
// SBOOT_FETCH_ERROR.
static uint16 sboot_fetch_input_byte() {
  uint16 result;
  result = sboot_fetch_program_byte(sboot_workspace.program_number,
				    sboot_workspace.input_offset);
  if (result == SBOOT_FETCH_ERROR) {
    sboot_shout_failure(SBOOT_SHOUT_FETCH_INPUT_BYTE_ERROR,
			"Ran off end of sboot program");
  }
  sboot_workspace.input_offset = sboot_workspace.input_offset + 1; // Avoid possible compiler bug
  return result;
}

// Extracts a uint16 word from the command program. The result is stored
// in a uint32.  Returns TRUE if successful, else returns FALSE.
static bool sboot_fetch_input_uint16(uint32 *result) {
  uint16 byte0;
  byte0 = sboot_fetch_input_byte();
  if (byte0 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte1;
  byte1 = sboot_fetch_input_byte();
  if (byte1 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint16 value;
  value = make16((uint8)byte1, (uint8)byte0);
  *result = (uint32)value;
 return TRUE;
}

// Extracts a uint24 word from the command program. The result is stored
// in a uint32.  Returns TRUE if successful, else returns FALSE.
static bool sboot_fetch_input_uint24(uint32 *result) {
  uint16 byte0;
  byte0 = sboot_fetch_input_byte();
  if (byte0 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte1;
  byte1 = sboot_fetch_input_byte();
  if (byte1 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte2;
  byte2 = sboot_fetch_input_byte();
  if (byte2 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  *result = make24(byte2, byte1, byte0);
  return TRUE;
}

// Extracts a uint32 word from the command program. Returns TRUE
// if successful, else returns FALSE.
static bool sboot_fetch_input_uint32(uint32 *result) {
  uint16 byte0;
  byte0 = sboot_fetch_input_byte();
  if (byte0 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte1;
  byte1 = sboot_fetch_input_byte();
  if (byte1 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte2;
  byte2 = sboot_fetch_input_byte();
  if (byte2 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte3;
  byte3 = sboot_fetch_input_byte();
  if (byte3 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  *result = make32((uint8)byte3, (uint8)byte2, (uint8)byte1, (uint8)byte0);
  return TRUE;
}

// Returns the index (not the contents) of a wreg register from the command
// program, or the special value SBOOT_FETCH_ERROR, which may indicate either a
// fetch_input error or that the wreg index was out of bounds.
static uint16 sboot_fetch_input_wreg() {
  uint16 result;
  result = sboot_fetch_input_byte();
  if (result == SBOOT_FETCH_ERROR) {
    return result;
  }
  if (result >= SBOOT_NUMBER_OF_WORD_REGISTERS) {
    sboot_shout_failure_with_value(SBOOT_SHOUT_INVALID_WREG,
				   "Invalid wreg number ",
				   (uint32)result);
    result = SBOOT_FETCH_ERROR;
  }
  return result;
}

// Returns byte index (0..3)from the command program, or the special
// value SBOOT_FETCH_ERROR, which may indicate either a
// fetch_input error or that the byte index was out of bounds.
static uint16 sboot_fetch_input_byte_index() {
  uint16 result;
  result = sboot_fetch_input_byte();
  if (result == SBOOT_FETCH_ERROR) {
    return result;
  }
  if (result > 3) { // TODO: Define a constant or an enum.
    sboot_shout_failure_with_value(SBOOT_SHOUT_INVALID_BYTE_INDEX,
				   "Invalid byte index ",
				   (uint32)result);
    result = SBOOT_FETCH_ERROR;
  }
  return result;
}

// This timeout is used by COMMAND_RESET_MAESTRO, COMMAND_WAIT,
// and COMMAND_READ_BYTE_FROM_MAESTRO.
static void sboot_start_timeout(uint16 timeout,
				sboot_execution_state_t new_state) {
  sboot_workspace.duration_until_timeout = timeout;
  sboot_workspace.start_of_timeout = sboot_fetch_current_seconds();
  sboot_workspace.state = new_state;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_start_timeout(): timeout=%d new_state=%d\n",
	   timeout, new_state);
#endif // __PCD__
  }
#endif // SBOOT_TRACE
}

// returns FALSE if an error occurs, else TRUE.
static bool sboot_enable_maestro_command() {
  uint16 phase;
  phase = sboot_fetch_input_byte();
  if (phase == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_enable_maestro_command(): phase=%d\n",
	   phase);
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  // Double check that power's OK before we change phase, which
  // might result in an increased power draw.
  if (sboot_should_shut_down()) {
    sboot_shout_failure(SBOOT_SHOUT_SHUTDOWN_REQUESTED_WHEN_ENABLING_MAESTRO,
			"sboot: shutdown requested in sboot_enable_maestro_command");
    return FALSE;
  }

  sboot_enable_maestro((sboot_enable_maestro_phase_t)phase);
  sboot_workspace.maestro_is_enabled = TRUE;
  return TRUE;
}

// returns TRUE.
static bool sboot_disable_maestro_command() {
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_disable_maestro_command()\n");
#endif // __PCD__
    }
#endif // SBOOT_TRACE

  sboot_disable_maestro();
  sboot_workspace.maestro_is_enabled = FALSE;
  return TRUE;
}

// returns TRUE if the reset has started, FALSE if an error occurs.
static bool sboot_start_reset_maestro_command() {
  uint16 timeout;
  timeout = sboot_fetch_input_byte();
  if (timeout == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_start_reset_maestro_command(): timeout=%d\n",
	     timeout);
#endif // __PCD__
    }
#endif // SBOOT_TRACE

  sboot_reset_maestro(TRUE);
  sboot_start_timeout(timeout, SBOOT_STATE_RESETTING_MAESTRO);
  return TRUE;
}

// returns TRUE if the reset if current, FALSE when it is done (not an error).
static bool sboot_resetting_maestro() {
  if (sboot_timeout_is_complete(SBOOT_STATE_RUNNING)) {
    sboot_reset_maestro(FALSE);

#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_resetting_maestro(): reset complete\n");
#endif // __PCD__
    }
#endif // SBOOT_TRACE

    return FALSE;
  }

  return TRUE;
}

// returns TRUE on success, FALSE on error.
static bool sboot_set_read_timeout_command() {
  uint16 timeout;
  timeout = sboot_fetch_input_byte();
  if (timeout == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_set_read_timeout_command(): timeout=%d\n", timeout);
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  sboot_workspace.read_timeout = timeout;
  return TRUE;
}

// returns TRUE on success, FALSE on error.
static bool sboot_set_program_timeout_command() {
  uint32 timeout;
  if (!sboot_fetch_input_uint16(&timeout)) {
    return FALSE;
  }

  if (!sboot_workspace.override_program_timeout) {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_set_program_timeout_command(): timeout=%d\n", timeout);
#endif // __PCD__
  }
#endif // SBOOT_TRACE

    sboot_workspace.program_timeout = timeout;
  } else {
#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_set_program_timeout_command(): timeout=%d is overridden\n", timeout);
#endif // __PCD__
    }
#endif // SBOOT_TRACE
  }
  return TRUE;
}

// returns TRUE if the wait has started, FALSE if an error occurs.
static bool sboot_start_wait() {
  uint16 timeout;
  timeout = sboot_fetch_input_byte();
  if (timeout == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_start_wait(): timeout=%d\n", timeout);
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  sboot_start_timeout(timeout, SBOOT_STATE_WAITING);
  return TRUE;
}

// returns TRUE if the wait is current, FALSE when it is done (not an error).
static bool sboot_waiting() {
  if (sboot_timeout_is_complete(SBOOT_STATE_RUNNING)) {
    return FALSE;
  }
  return TRUE;
}

// returns TRUE on success, FALSE on error.
static bool sboot_send_fixed_len_data_to_maestro(uint16 len) {
  sboot_workspace.bytes_to_send = UART_DATA_HEADER_LEN + len;
  sboot_workspace.state = SBOOT_STATE_SENDING_DATA_TO_MAESTRO;
  sboot_workspace.next_state = SBOOT_STATE_RUNNING;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_send_fixed_len_data_to_maestro(%d)\n", len);
#else
#ifdef __MAESTRO__
    sprintf(dbgbuf, "\r\n sboot_send_fixed_len_data_to_maestro(%d + %d)\r\n",
	    len, UART_DATA_HEADER_LEN);
    maestro_msg_dbgbuf();
#endif // __MAESTRO__
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  return TRUE;
}

// returns TRUE on success, FALSE on error.
static bool sboot_send_variable_len_data_to_maestro() {
  uint16 len_low;
  len_low = sboot_fetch_input_byte();
  if (len_low == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint16 len_high;
  len_high = sboot_fetch_input_byte();
  if (len_high == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint16 len;
  len = make16((uint8)len_high, (uint8)len_low);
  sboot_workspace.bytes_to_send = UART_DATA_HEADER_LEN + len;
  sboot_workspace.state = SBOOT_STATE_SENDING_DATA_TO_MAESTRO;
  sboot_workspace.next_state = SBOOT_STATE_RUNNING;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_send_variable_len_data_to_maestro() len=%d\n", len);
# endif // __PCD__
  }
#endif // SBOOT_TRACE

  return TRUE;
}

// TODO: Consider a low-level write timeout.
// returns TRUE when progress is made, else FALSE.
static bool sboot_sending_data_to_maestro() {
  // First, is there any data left to send?
  uint16 len;
  len = sboot_workspace.bytes_to_send;
  if (len == 0) {
    sboot_workspace.state = sboot_workspace.next_state;
    return TRUE; // Consider the change of state as progress.
  }

  // Note:  can't use sboot_fetch_input_byte(...) here
  // because we don'twant to increment input_offset until
  // we know that the UART has accepted the byte.
  uint16 data;
  data = sboot_fetch_program_byte(sboot_workspace.program_number,
				  sboot_workspace.input_offset);
  if (data == SBOOT_FETCH_ERROR) {
    sboot_shout_failure(SBOOT_SHOUT_FETCH_DATA_ERROR,
			"Ran off end of sboot program sending data to maestro");
    return TRUE; // Consider the change of state as progress.
  }

  // Try to send a byte to MAESTRO via the UART link:
  if (sboot_uart_send((uint8) data)) {
    // The UART has accepted the byte, advance to the next byte.
    sboot_workspace.input_offset = sboot_workspace.input_offset + 1; // Avoid possible compiler bug
    sboot_workspace.bytes_to_send--;
    return TRUE; // We have made progress.
  } else {
    // The UART did not accept the byte.
    return FALSE; // No progress was made.
  }
}

// We want to read a byte from MAESTRO.  First we send a fixed-length read
// request header from the sboot program to the MAESTRO.  Next, we wait for
// a response from the MAESTRO or a (fatal) timeout.
//
// returns TRUE on success, FALSE on error.
static bool sboot_start_read_byte_from_maestro() {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_start_read_byte_from_maestro(): header_len=%d\n",
	   UART_DATA_HEADER_LEN);
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  sboot_workspace.bytes_to_send = UART_DATA_HEADER_LEN;
  sboot_execution_state_t new_state = SBOOT_STATE_SENDING_DATA_TO_MAESTRO;
  sboot_start_timeout(sboot_workspace.read_timeout, new_state);
  sboot_workspace.next_state = SBOOT_STATE_READING_DATA_FROM_MAESTRO;
  return TRUE;
}

// returns TRUE if the read is waiting, FALSE when it is done (may or
// may not be an error).
static bool sboot_reading_data_from_maestro() {
  if (sboot_timeout_is_complete(SBOOT_STATE_FAILED)) {
    sboot_shout_failure(SBOOT_SHOUT_TIMEOUT_READING_UART,
			"Timeout reading UART.");
    return FALSE;
  }
  uint16 value;
  value = sboot_uart_receive();
  if (value == SBOOT_UART_NOT_READY) {
    return TRUE;
  }

  // Get the destination word register from the command program.
  uint16 wreg_index;
  wreg_index = sboot_fetch_input_wreg();
  if (wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Store the received value and process the next command.
  sboot_workspace.wreg[(wreg_t)wreg_index] = value;
  sboot_workspace.state = SBOOT_STATE_RUNNING;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_reading_data_from_maestro(): wreg%d=0x%X\n",
	   wreg_index, value);
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  return FALSE;
}

// Extract a byte from the least significant 8 bits of a
// wreg.  Insert thay byte into one of 4 byte positions
// in a destination wreg.  The byte positions are numbered
// 0 to 3, least to most significant.  The argument order
// is:
//
// source wreg index
// destination wreg index
// destination byte index
//
// returns TRUE on success, FALSE on error.
static bool sboot_insert_byte_into_word() {
  // Get the source word register from the command program.
  uint16 src_wreg_index;
  src_wreg_index = sboot_fetch_input_wreg();
  if (src_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Get the destination word register from the command program.
  uint16 dest_wreg_index;
  dest_wreg_index = sboot_fetch_input_wreg();
  if (dest_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Get the destination byte index from the command program.
  uint16 dest_byte_index;
  dest_byte_index = sboot_fetch_input_byte_index();
  if (dest_byte_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 src_byte;
  src_byte = sboot_workspace.wreg[(wreg_t)src_wreg_index] & 0xff;

  uint32 dest_word;
  dest_word = sboot_workspace.wreg[(wreg_t)dest_wreg_index];

  switch (dest_byte_index) {
  case 0: {
    dest_word = (dest_word &0xff) | src_byte;
    break;
  }

  case 1: {
    dest_word = (dest_word &0xff00) | (src_byte << 8);
    break;
  }

  case 2: {
    dest_word = (dest_word &0xff0000) | (src_byte << 16);
    break;
  }

  case 3: {
    dest_word = (dest_word &0xff000000) | (src_byte << 24);
    break;
  }
  }

  sboot_workspace.wreg[(wreg_t)dest_wreg_index] = dest_word;
  return TRUE;
}

// Pack 4 bytes into a word.  The bytes come from the
// least significant 8 bits of four source wreg's.
// The source wreg's are named from most-to-least
// significant, and are followed by the dest wreg.
//
// returns TRUE on success, FALSE on error.
static bool sboot_pack_bytes_into_word() {
  // Get the source word register from the command program.
  uint16 wreg_or_error;
  wreg_or_error = sboot_fetch_input_wreg();
  if (wreg_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint8 high_byte;
  high_byte = (uint8)sboot_workspace.wreg[(wreg_t)wreg_or_error];

  wreg_or_error = sboot_fetch_input_wreg();
  if (wreg_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint8 upper_mid_byte;
  upper_mid_byte = (uint8)sboot_workspace.wreg[(wreg_t)wreg_or_error];

  wreg_or_error = sboot_fetch_input_wreg();
  if (wreg_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint8 lower_mid_byte;
  lower_mid_byte = (uint8)sboot_workspace.wreg[(wreg_t)wreg_or_error];

  wreg_or_error = sboot_fetch_input_wreg();
  if (wreg_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint8 low_byte;
  low_byte = (uint8)sboot_workspace.wreg[(wreg_t)wreg_or_error];

  // Get the destination word register from the command program.
  wreg_or_error = sboot_fetch_input_wreg();
  if (wreg_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  wreg_t dest_wreg_index;
  dest_wreg_index = (wreg_t)wreg_or_error;
  
  uint32 value;
  value = make32(high_byte,
		 upper_mid_byte,
		 lower_mid_byte,
		 low_byte);
  sboot_workspace.wreg[dest_wreg_index] = value;
  return TRUE;
}


// returns TRUE on success, FALSE on error.
static bool sboot_zero_word() {
  uint16 wreg_index;
  wreg_index =  sboot_fetch_input_wreg();
  if (wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_workspace.wreg[(wreg_t)wreg_index] = 0;
  return TRUE;
}


// returns TRUE on success, FALSE on error.
static bool sboot_set_word() {
  uint32 value;
  if (!sboot_fetch_input_uint32(&value)) {
    return FALSE;
  }
  
  uint16 wreg_index;
  wreg_index =  sboot_fetch_input_wreg();
  if (wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_workspace.wreg[(wreg_t)wreg_index] = value;
  return TRUE;
}


// returns TRUE on success, FALSE on error.
static bool sboot_copy_word() {
  uint16 src_index;
  src_index = sboot_fetch_input_wreg();
  if (src_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  
  uint16 dest_index;
  dest_index =  sboot_fetch_input_wreg();
  if (dest_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_workspace.wreg[(wreg_t)dest_index] =
    sboot_workspace.wreg[(wreg_t)src_index];
  return TRUE;
}


static uint32 sboot_perform_op(sboot_op_t op, uint32 value1, uint32 value2) {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_perform_op(): op=%d value1=%d value2=%d\n",
	   op, value1, value2);
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  switch (op) {

    // arithmetic operations:
  case OP_ADD:
    return value1 + value2;

  case OP_SUB:
    return value1 - value2;

  case OP_SUB_REVERSED:
    return value2 - value1;

  case OP_MUL:
    return value1 * value2;

  case OP_DIV:
    return value1 / value2;

  case OP_DIV_REVERSED:
    return value2 / value1;

  case OP_MOD:
    return value1 % value2;

  case OP_MOD_REVERSED:
    return value2 % value1;

    // comparison operations:
  case OP_EQ:
  case OP_IN:
    return (value1 == value2) ? TRUE : FALSE;

  case OP_NE:
  case OP_NOT_IN:
    return (value1 != value2) ? TRUE : FALSE;

  case OP_GT:
    return (value1 > value2) ? TRUE : FALSE;

  case OP_GE:
    return (value1 >= value2) ? TRUE : FALSE;

  case OP_LT:
    return (value1 < value2) ? TRUE : FALSE;

  case OP_LE:
    return (value1 <= value2) ? TRUE : FALSE;

    // logical operations:
  case OP_AND:
    return (value1 && value2) ? TRUE : FALSE;

  case OP_OR:
    return (value1 || value2) ? TRUE : FALSE;

  case OP_XOR:
    return ((value1 && !value2) || (value2 && !value1)) ? TRUE : FALSE;

    // logical operation, ignoring second argument:
  case OP_NOT:
    return value1 ? FALSE : TRUE;

    // mask operations:
  case OP_ALL_ZEROS_UNDER_MASK:
    return ((value1 & value2) == 0) ? TRUE : FALSE;

  case OP_NOT_ALL_ZEROS_UNDER_MASK:
    return ((value1 & value2) != 0) ? TRUE : FALSE;

  case OP_ALL_ONES_UNDER_MASK:
    return ((value1 & value2) == value2) ? TRUE : FALSE;

  case OP_NOT_ALL_ONES_UNDER_MASK:
    return ((value1 & value2) != value2) ? TRUE : FALSE;

    // shift operations:
  case OP_SHIFT_RIGHT:
    return value1 >> value2;

  case OP_SHIFT_LEFT:
    return value1 << value2;

    // bitwise operations:

  case OP_AND_BITS:
    return value1 & value2;

  case OP_OR_BITS:
    return value1 | value2;

  case OP_XOR_BITS:
    return value1 ^ value2;

  case OP_NAND_BITS:
    return ~(value1 & value2);

  case OP_NOR_BITS:
    return ~(value1 | value2);

  case OP_XNOR_BITS:
    return ~(value1 ^ value2);

    // bitwise operation, ignoring second argument:
  case OP_INVERT_BITS:
    // 29-Dec-2011: The CCS PCD C compiler used for production
    // miscompiles this:
    // return ~value1;
    return value1 ^ 0xffffffff;

    // special timeout comparison operator, allowing
    // for rollover.
    // The first operand is the start of the timeout (from current_seconds).
    // The second operand is the duration of the timeout.
    // The destination gets true if the timeout has passed, else false.
  case OP_TIMEOUT: {
    if (sboot_timeout_still_running(value1, value2)) {
      return FALSE;
    }
    return TRUE;
  }

  default:
    sboot_shout_failure_with_value(SBOOT_SHOUT_UNKNOWN_OPCODE,
				   "sboot_perform_op: unknown opcode ",
				   (uint32)op);
    return FALSE; // This value shouldn't matter, we should fail.
  }
}

// Perform a data operation.  There are two data sources.  The destination
// is a word register.
//
// Format:
// op
// src_wreg
// wreg, immediate value (uint8, uint16, uint24 or uint32), or nothing
// dest_wreg
//
// returns TRUE on success, FALSE on error.
static bool sboot_execute_data_op(uint8 command) {
  // Get the operation code from the command program.
  uint16 op;
  op = sboot_fetch_input_byte();
  if (op == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Get first source value from the command program.
  uint16 src_wreg_index;
  src_wreg_index = sboot_fetch_input_wreg();
  if (src_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint32 value1;
  value1 = sboot_workspace.wreg[(wreg_t)src_wreg_index];

  uint32 value2; // The source of the second value depends on the command.
  switch (command) {

  case COMMAND_OP_WREG_WREG_TO_WREG: {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" COMMAND_OP_WREG_WREG_TO_WREG\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE
    // The second source value comes from a word register.
  uint16 src_wreg_index2;
  src_wreg_index2 = sboot_fetch_input_wreg();
    if (src_wreg_index2 == SBOOT_FETCH_ERROR) {
      return FALSE;
    }
    value2 = sboot_workspace.wreg[(wreg_t)src_wreg_index2];
    break;
  }

#ifdef SBOOT_OPTIMIZE_IMMEDIATES
  case COMMAND_OP_WREG_ZERO_TO_WREG: {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" COMMAND_OP_WREG_ZERO_TO_WREG\n");
#else
#ifdef __MAESTRO__
    maestro_msg(" COMMAND_OP_WREG_ZERO_TO_WREG");
#endif // __MAESTRO__
#endif // __PCD__
  }
#endif // SBOOT_TRACE
    // The second source value is zero.
    value2 = 0;
    break;
  }

  case COMMAND_OP_WREG_UINT8_TO_WREG: {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" COMMAND_OP_WREG_UINT8_TO_WREG\n");
#else
#ifdef __MAESTRO__
    maestro_msg(" COMMAND_OP_WREG_UINT8_TO_WREG");
#endif // __MAESTRO__
#endif // __PCD__
  }
#endif // SBOOT_TRACE
    // Get the uint8 immediate value from the command program.
  uint16 value;
  value = sboot_fetch_input_byte();
    if (value == SBOOT_FETCH_ERROR) {
      return FALSE;
    }
    value2 = (uint32)value;
    break;
  }

  case COMMAND_OP_WREG_UINT16_TO_WREG: {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" COMMAND_OP_WREG_UINT16_TO_WREG\n");
#else
#ifdef __MAESTRO__
    maestro_msg(" COMMAND_OP_WREG_UINT16_TO_WREG");
#endif // __MAESTRO__
#endif // __PCD__
  }
#endif // SBOOT_TRACE
    // Get the uint16 immediate value from the command program.
    // It is zero-extended and stored as a uint32.
    if (!sboot_fetch_input_uint16(&value2)) {
      return FALSE;
    }
    break;
  }

  case COMMAND_OP_WREG_UINT24_TO_WREG: {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" COMMAND_OP_WREG_UINT24_TO_WREG\n");
#else
#ifdef __MAESTRO__
    maestro_msg(" COMMAND_OP_WREG_UINT24_TO_WREG");
#endif // __MAESTRO__
#endif // __PCD__
  }
#endif // SBOOT_TRACE
    // Get the uint24 immediate value from the command program.
    // It is zero-extended and stored as a uint32.
    if (!sboot_fetch_input_uint24(&value2)) {
      return FALSE;
    }
    break;
  }
#endif // SBOOT_OPTIMIZE_IMMEDIATES

  case COMMAND_OP_WREG_UINT32_TO_WREG: {
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" COMMAND_OP_WREG_UINT32_TO_WREG\n");
#else
#ifdef __MAESTRO__
    maestro_msg(" COMMAND_OP_WREG_UINT32_TO_WREG");
#endif // __MAESTRO__
#endif // __PCD__
  }
#endif // SBOOT_TRACE
    // Get the uint32 immediate value from the command program.
    if (!sboot_fetch_input_uint32(&value2)) {
      return FALSE;
    }
    break;
  }

  default:
    sboot_shout_failure_with_value(SBOOT_SHOUT_DATA_OP_UNKNOWN_COMMAND,
				   "sboot_data_operations: unknown command ",
				   (uint32)command);
    return FALSE;
  }

  // Get the destination word register from the command program.
  uint16 dest_wreg_index;
  dest_wreg_index = sboot_fetch_input_wreg();
  if (dest_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Process the operation and store the result.
  uint32 result;
  result = sboot_perform_op(op, value1, value2);
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" result=%d stored in wreg=%d\n",
	   result, dest_wreg_index);
#else
#ifdef __MAESTRO__
    sprintf(dbgbuf, "\r\n op=%d value1=%Ld value2=%Ld result=%Ld --> wreg=%d\r\n",
	    op, value1, value2, result, dest_wreg_index);
    maestro_msg_dbgbuf();
#endif // __MAESTRO__
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  sboot_workspace.wreg[(wreg_t)dest_wreg_index] = result;
  return TRUE;
}

// Perform a data operation.  There are two data sources, the first of which
// is a word register, and the second of which is an immediate list of
// word (uint32) values. The destination is a word register.  Only the
// list operations, OP_IN and OP_NOT_IN, are supported.
//
// Format:
// op
// src_wreg
// list_len
// uint32 ...
// dest_wreg
//
// This routine returns TRUE on success, FALSE on error.
static bool sboot_execute_list_op() {
  // Get the operation code from the command program.
  uint16 op;
  op = sboot_fetch_input_byte();
  if (op == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Get first source value from the command program.
  uint16 src_wreg_index;
  src_wreg_index = sboot_fetch_input_wreg();
  if (src_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint32 value1;
  value1 = sboot_workspace.wreg[(wreg_t)src_wreg_index];

  // Get list length from the command program.
  // This code "knows" that list_len_t is uint8.
  uint16 list_len_value;
  list_len_value = sboot_fetch_input_byte();
  if (list_len_value == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  list_len_t list_len;
  list_len = list_len_value;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" sboot_execute_list_op(): op=%d src_wreg=%d list_len=%d\n",
	   op, src_wreg_index, list_len);
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  uint32 result;
  uint32 value2; // Declared outside the loop for presumed efficiency.

  switch (op) {
  case OP_IN:
  case OP_NOT_IN: {
    result = (op == OP_IN) ? FALSE : TRUE;
    bool not_result;
    not_result = !result;
    // If list_len is too long, this will take too long!
    // We "know" that list_len_t is uint8, so this shouldn't be an issue.
    {
      int idx;
      for (idx = 0; idx < list_len; idx++) {
	if (!sboot_fetch_input_uint32(&value2)) {
	  return FALSE;
	}
#ifdef SBOOT_TRACE
	if (sboot_trace_flag) {
#ifndef __PCD__
	  printf(" list[%d]=%d\n", idx, value2);
#endif // __PCD__
	}
#endif // SBOOT_TRACE
	if (value1 == value2) {
	  // TODO: It would be nice to shortcut here, but the we'd have to
	  // adjust the sboot program offset.
	  // result = not_result;
	  // break;
	  result = not_result;
	} 
      }
    }
    break;
  }

  default:
    sboot_shout_failure_with_value(SBOOT_SHOUT_UNKNOWN_LIST_OPCODE,
				   "sboot_execute_list_op: unknown opcode ",
				   (uint32)op);
  }

  // Get the destination word register from the command program.
  uint16 dest_wreg_index;
  dest_wreg_index = sboot_fetch_input_wreg();
  if (dest_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#ifndef __PCD__
    printf(" result=%d stored in wreg=%d\n",
	   result, dest_wreg_index);
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  sboot_workspace.wreg[(wreg_t)dest_wreg_index] = result;
  return TRUE;
}

// returns TRUE if no problem, returns FALSE on error or intended failure.
static bool sboot_shout() {
  uint16 severity;
  severity = sboot_fetch_input_byte();
  if (severity == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 reason;
  if (!sboot_fetch_input_uint16(&reason)) {
    return FALSE;
  }

  uint16 msg_len;
  msg_len = sboot_fetch_input_byte();
  if (msg_len == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_start_shouting((sboot_shout_severity_t)severity,
		       (sboot_shout_reason_t)reason);
  sboot_shout_stream(msg_len);
  sboot_send_shout_done();

  if ((sboot_shout_severity_t)severity == SBOOT_SHOUT_FAILURE) {
    sboot_workspace.state = SBOOT_STATE_FAILED;
    return FALSE;
  } else {
    return TRUE;
  }
}

// This is a convenience method.  It prints a text message and a
// constant value.  Ideally, the constant value could be converted to
// text and boot_shout(...) used, instead.
// returns TRUE if no problem, returns FALSE on error or intended failure.
static bool sboot_shout_value() {
  uint16 severity;
  severity = sboot_fetch_input_byte();
  if (severity == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 reason;
  if (!sboot_fetch_input_uint16(&reason)) {
    return FALSE;
  }

  // Get the uint32 value from the command program.
  uint32 value;
  if (!sboot_fetch_input_uint32(&value)) {
    return FALSE;
  }

  uint16 msg_len;
  msg_len = sboot_fetch_input_byte();
  if (msg_len == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_start_shouting((sboot_shout_severity_t)severity,
		       (sboot_shout_reason_t)reason);
  sboot_shout_stream(msg_len);
  sboot_shout_hex_value(value);
  sboot_send_shout_done();

  if ((sboot_shout_severity_t)severity == SBOOT_SHOUT_FAILURE) {
    sboot_workspace.state = SBOOT_STATE_FAILED;
    return FALSE;
  } else {
    return TRUE;
  }
}

// returns TRUE if no problem, returns FALSE on error or intended failure.
static bool sboot_shout_wreg() {
  uint16 severity;
  severity = sboot_fetch_input_byte();
  if (severity == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 reason;
  if (!sboot_fetch_input_uint16(&reason)) {
    return FALSE;
  }

  // Get the source word register from the command program.
  uint16 wreg_or_error;
  wreg_or_error = sboot_fetch_input_wreg();
  if (wreg_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  wreg_t wreg_index;
  wreg_index = (wreg_t)wreg_or_error;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg: got wreg index ");
    HexPrint(userPort, &wreg_index, sizeof(wreg_t));
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  uint32 value;
  value = sboot_workspace.wreg[wreg_index];
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg: got value ");
    HexPrint(userPort, &value, 4);
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  uint16 msg_len;
  msg_len = sboot_fetch_input_byte();
  if (msg_len == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    maestro_msg("sboot_shout_wreg: got a message length");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  sboot_start_shouting((sboot_shout_severity_t)severity,
		       (sboot_shout_reason_t)reason);
  sboot_shout_stream(msg_len);
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    maestro_msg("sboot_shout_wreg: about to shout hex");
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  sboot_shout_hex_value(value);
  sboot_send_shout_done();

  if ((sboot_shout_severity_t)severity == SBOOT_SHOUT_FAILURE) {
    sboot_workspace.state = SBOOT_STATE_FAILED;
    return FALSE;
  } else {
    return TRUE;
  }
}

// returns TRUE if no problem, returns FALSE on error or intended failure.
static bool sboot_shout_wreg_field() {
  uint16 severity;
  severity = sboot_fetch_input_byte();
  if (severity == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 reason;
  if (!sboot_fetch_input_uint16(&reason)) {
    return FALSE;
  }

  // Get the source word register from the command program.
  uint16 wreg_or_error;
  wreg_or_error = sboot_fetch_input_wreg();
  if (wreg_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  wreg_t wreg_index;
  wreg_index = (wreg_t)wreg_or_error;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_field: got wreg index ");
    HexPrint(userPort, &wreg_index, sizeof(wreg_t));
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  uint32 value;
  value = sboot_workspace.wreg[wreg_index];
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_field: got value ");
    HexPrint(userPort, &value, 4);
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  // Get the uint32 mask from the command program.
  // TODO: Declare an error if the mask is 0?
  uint32 mask;
  if (!sboot_fetch_input_uint32(&mask)) {
    return FALSE;
  }
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_field: got mask ");
    HexPrint(userPort, &mask, 4);
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  // Mask the field and right-align it for printing.
  value = value & mask;
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_field: masked value ");
    HexPrint(userPort, &value, 4);
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  if (mask != 0) {
    while ((mask & 1) == 0) {
      mask = mask >> 1;
      value = value >> 1;
    }
  }
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_field: shifted value ");
    HexPrint(userPort, &value, 4);
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  uint16 msg_len;
  msg_len = sboot_fetch_input_byte();
  if (msg_len == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
  maestro_msg("sboot_shout_wreg_field: got a message length");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  sboot_start_shouting((sboot_shout_severity_t)severity,
		       (sboot_shout_reason_t)reason);
  sboot_shout_stream(msg_len);
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
  maestro_msg("sboot_shout_wreg_field: about to shout hex");
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  sboot_shout_hex_value(value);
  sboot_send_shout_done();

  if ((sboot_shout_severity_t)severity == SBOOT_SHOUT_FAILURE) {
    sboot_workspace.state = SBOOT_STATE_FAILED;
    return FALSE;
  } else {
    return TRUE;
  }
}

// returns TRUE if no problem, returns FALSE on error or intended failure.
static bool sboot_shout_wreg_wreg() {
  uint16 severity;
  severity = sboot_fetch_input_byte();
  if (severity == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 reason;
  if (!sboot_fetch_input_uint16(&reason)) {
    return FALSE;
  }

  // Get the first source word register from the command program.
  uint16 wreg1_or_error;
  wreg1_or_error = sboot_fetch_input_wreg();
  if (wreg1_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  wreg_t wreg1_index;
  wreg1_index = (wreg_t)wreg1_or_error;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_wreg: got wreg1 index ");
    HexPrint(userPort, &wreg1_index, sizeof(wreg_t));
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  uint32 value1;
  value1 = sboot_workspace.wreg[wreg1_index];
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_wreg: got value1 ");
    HexPrint(userPort, &value1, 4);
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  // Get the second source word register from the command program.
  uint16 wreg2_or_error;
  wreg2_or_error = sboot_fetch_input_wreg();
  if (wreg2_or_error == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  wreg_t wreg2_index;
  wreg2_index = (wreg_t)wreg2_or_error;

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_wreg: got wreg2 index ");
    HexPrint(userPort, &wreg2_index, sizeof(wreg_t));
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  uint32 value2;
  value2 = sboot_workspace.wreg[wreg2_index];
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sendDBGALL(userPort, "\r\nsboot_shout_wreg_wreg: got value2 ");
    HexPrint(userPort, &value2, 4);
    sendDBGALL(userPort, "\r\n");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  uint16 msg_len;
  msg_len = sboot_fetch_input_byte();
  if (msg_len == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    maestro_msg("sboot_shout_wreg_wreg: got a message length");
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  sboot_start_shouting((sboot_shout_severity_t)severity,
		       (sboot_shout_reason_t)reason);
  sboot_shout_stream(msg_len);
#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    maestro_msg("sboot_shout_wreg_wreg: about to shout hex");
#endif // __PCD__
  }
#endif // SBOOT_TRACE
  sboot_shout_hex_value(value1);
  sboot_shout_hex_value(value2);
  sboot_send_shout_done();

  if ((sboot_shout_severity_t)severity == SBOOT_SHOUT_FAILURE) {
    sboot_workspace.state = SBOOT_STATE_FAILED;
    return FALSE;
  } else {
    return TRUE;
  }
}

// Returns TRUE if failure is not detected, FALSE on error
// or a detected failure.
static bool sboot_shout_maybe(bool shout_if_nonzero) {
  uint16 severity;
  severity = sboot_fetch_input_byte();
  if (severity == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 reason;
  if (!sboot_fetch_input_uint16(&reason)) {
    return FALSE;
  }

  // Get the source word register from the command program.
  uint16 test_wreg_index;
  test_wreg_index =  sboot_fetch_input_wreg();
  if (test_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint16 show_wreg_index;
  show_wreg_index = sboot_fetch_input_wreg();
  if (show_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint16 msg_len;
  msg_len = sboot_fetch_input_byte();
  if (msg_len == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 test_value;
  test_value = sboot_workspace.wreg[(wreg_t)test_wreg_index];
  if (shout_if_nonzero ? (test_value != 0) : (test_value == 0)) {
    sboot_start_shouting((sboot_shout_severity_t)severity,
			 (sboot_shout_reason_t)reason);
    sboot_shout_stream(msg_len);
    uint32 show_value;
    show_value = sboot_workspace.wreg[(wreg_t)show_wreg_index];
    sboot_shout_hex_value(show_value);
    sboot_send_shout_done();

    if ((sboot_shout_severity_t)severity == SBOOT_SHOUT_FAILURE) {
      sboot_workspace.state = SBOOT_STATE_FAILED;
      return FALSE;
    } else {
      return TRUE;
    }
  }

  // Skip the message text in the input program.
  sboot_workspace.input_offset += msg_len;
  return TRUE;
}

// Returns TRUE if failure is not detected, FALSE on error
// or a detected failure.
//
// This set of commands takes a program counter offset, followed by
// an optional register index.  The program counter offset appears first
// so it will be at a fixed location to simplify fixups.  The offset is
// relative to the location, in the program, of the offset word itself.
static bool sboot_jump(uint8 offset_bytes,
		       bool jump_forward,
		       bool always_jump,
		       bool jump_if_nonzero) {

#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_jump(): offset_bytes=%d forward=%d always=%d if_nonzero=%d\n",
	     offset_bytes, jump_forward, always_jump, jump_if_nonzero);
#else
#ifdef __MAESTRO__
      sprintf(dbgbuf, "\r\n sboot_jump(): offset_bytes=%d forward=%d always=%d if_nonzero=%d\r\n",
	      offset_bytes, jump_forward, always_jump, jump_if_nonzero);
      maestro_msg_dbgbuf();
#endif // __MAESTRO__
#endif // __PCD__
    }
#endif // SBOOT_TRACE

  // Get the program counter offset from the command program.
    sboot_size_t current_pc;
    current_pc = sboot_workspace.input_offset;
  uint32 jump_offset;
  switch (offset_bytes) {
#ifdef SBOOT_OPTIMIZE_JUMPS
  case BYTES_PER_UINT8: {
    uint16 value;
    value = sboot_fetch_input_byte();
    if (value == SBOOT_FETCH_ERROR) {
      return FALSE;
    }
    jump_offset = (uint32)value;
    break;
  }

  case BYTES_PER_UINT16: {
    if (!sboot_fetch_input_uint16(&jump_offset)) {
      return FALSE;
    }
    break;
  }

  case BYTES_PER_UINT24: {
    if (!sboot_fetch_input_uint24(&jump_offset)) {
      return FALSE;
    }
    break;
  }
#endif // SBOOT_OPTIMIZE_JUMPS

  case BYTES_PER_UINT32: {
    if (!sboot_fetch_input_uint32(&jump_offset)) {
      return FALSE;
    }
    break;
  }

  default:
    sboot_shout_failure_with_value(SBOOT_SHOUT_BAD_JUMP_OFFSET_WIDTH,
				   "sboot_jump: bad jump offset width",
                                   (uint32)offset_bytes);
    return FALSE;    
  }

  if (jump_offset == 0) {
    sboot_shout_failure(SBOOT_SHOUT_BAD_JUMP_OFFSET_0,
			"sboot_jump: bad jump offset 0");
    return FALSE;
  }

  bool do_the_jump;
  do_the_jump = TRUE;
  if (!always_jump) {
    // Get the source word register from the command program.
    uint16 test_wreg_index;
    test_wreg_index =  sboot_fetch_input_wreg();
    if (test_wreg_index == SBOOT_FETCH_ERROR) {
      return FALSE;
    }
    uint32 test_value;
    test_value = sboot_workspace.wreg[(wreg_t)test_wreg_index];
    do_the_jump = jump_if_nonzero ? (test_value != 0) : (test_value == 0);

#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_jump: test_wreg=%d test_value=%d do_the_jump=%d\n",
	     test_wreg_index, test_value, do_the_jump);
#else
#ifdef __MAESTRO__
      sprintf(dbgbuf, "\r\n sboot_jump: test_wreg=%d test_value=%Ld do_the_jump=%d\r\n",
	     test_wreg_index, test_value, do_the_jump);
      maestro_msg_dbgbuf();
#endif // __MAESTRO__
#endif // __PCD__
    }
#endif // SBOOT_TRACE
  }

#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
#ifndef __PCD__
      printf(" sboot_jump: do_the_jump=%d jump_offset=%d\n",
	     do_the_jump, jump_offset);
#else
#ifdef __MAESTRO__
      sprintf(dbgbuf, "\r\n sboot_jump: do_the_jump=%d jump_offset=%Ld\r\n",
	      do_the_jump, jump_offset);
      maestro_msg_dbgbuf();
#endif // __MAESTRO__
#endif // __PCD__
    }
#endif // SBOOT_TRACE

  if (do_the_jump) {
    if (jump_forward) {
      sboot_workspace.input_offset = current_pc + jump_offset;
    } else {
      sboot_workspace.input_offset = current_pc - jump_offset;
    }
  }
  return TRUE;
}

// Call a subroutine in the sboot input program. Calls are backwards only.
//
// Returns TRUE if failure is not detected, FALSE on error
// or a detected failure.
static bool sboot_call(bool offset_bytes) {

  // Get the uint32 program counter offset from the command program.
  sboot_size_t current_pc;
  current_pc = sboot_workspace.input_offset;
  uint32 call_offset;
  switch (offset_bytes) {
#ifdef SBOOT_OPTIMIZE_CALLS
  case BYTES_PER_UINT8: {
    uint16 value;
    value = sboot_fetch_input_byte();
    if (value == SBOOT_FETCH_ERROR) {
      return FALSE;
    }
    call_offset = (uint32)value;
    break;
  }

  case BYTES_PER_UINT16: {
    if (!sboot_fetch_input_uint16(&call_offset)) {
      return FALSE;
    }
    break;
  }

  case BYTES_PER_UINT24: {
    if (!sboot_fetch_input_uint24(&call_offset)) {
      return FALSE;
    }
    break;
  }
#endif // SBOOT_OPTIMIZE_CALLS

  case BYTES_PER_UINT32: {
    if (!sboot_fetch_input_uint32(&call_offset)) {
      return FALSE;
    }
    break;
  }

  default:
    sboot_shout_failure_with_value(SBOOT_SHOUT_BAD_CALL_OFFSET_WIDTH,
				   "sboot_call: bad call offset width ",
                                   (uint32)offset_bytes);
    return FALSE;    
  }

  if (sboot_workspace.call_stack_len >= SBOOT_CALL_STACK_SIZE) {
    sboot_shout_failure(SBOOT_SHOUT_CALL_STACK_OVERFLOW,
			"sboot_call: call stack overflow");
    return FALSE;
  }
  sboot_workspace.call_stack[sboot_workspace.call_stack_len++] =
    sboot_workspace.input_offset;
  sboot_workspace.input_offset = current_pc - call_offset;

  return TRUE;
}

// Return from a subroutine in the sboot input program.
//
// Returns TRUE if failure is not detected, FALSE on error
// or a detected failure.
static bool sboot_return() {

  if (sboot_workspace.call_stack_len == 0) {
    sboot_shout_failure(SBOOT_SHOUT_CALL_STACK_UNDERFLOW,
			"sboot_return: call stack underflow");
    return FALSE;
  }
  sboot_workspace.input_offset = 
    sboot_workspace.call_stack[--sboot_workspace.call_stack_len];
  return TRUE;
}

// returns TRUE on success, FALSE on error.
static bool sboot_get_current_seconds() {
  uint16 wreg_index;
  wreg_index =  sboot_fetch_input_wreg();
  if (wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_workspace.wreg[(wreg_t)wreg_index] = sboot_fetch_current_seconds();
  return TRUE;
}

// Send a completed boot string message with an indication of its
// origin tile.
//
// NOTE: there is an implicit assumption here, based on the python code,
// that string messages from different tiles won't be interleaved.
static void sboot_send_string_msg(uint16 sender_x, uint16 sender_y) {

  sboot_start_shouting(SBOOT_SHOUT_MESSAGE, SBOOT_SHOUT_MAESTRO_STRING_MESSSAGE);
  sboot_shout_string("Tile X=");

  // TODO: Really need decimal conversion here
  sboot_shout_hex_value(sender_x);
  sboot_shout_string(" Y=");
  sboot_shout_hex_value(sender_y);
  sboot_shout_string(": ");

  int i;
  i = 0;
  while (i < sboot_workspace.string_msg_buf_len) {
    sboot_shout_uchar(sboot_workspace.string_msg_buf[i]);
  }
  sboot_send_shout_done();

  sboot_workspace.string_msg_buf_len = 0;
}

// This routine assists in building complete boot string messages in a
// buffer.  If the string message buffer overflows, the partial message
// is sent immediately.
static void sboot_build_string_msg(uchar c,
				   uint16 sender_x, uint16 sender_y) {
  if (sboot_workspace.string_msg_buf_len >= SBOOT_STRING_MSG_BUF_MAX_LEN) {
    sboot_send_string_msg(sender_x, sender_y);
  }
  sboot_workspace.string_msg_buf[sboot_workspace.string_msg_buf_len++] = c;
}

// This routine contains special code for assembling MASESTRO boot string
// messages into complete strings, and sending the when done.
//
// returns TRUE on success, FALSE on error.
static bool sboot_assemble_string_msg() {
  uint16 four_bytes_wreg_index;
  four_bytes_wreg_index =  sboot_fetch_input_wreg();
  if (four_bytes_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint32 four_bytes;
  four_bytes = sboot_workspace.wreg[(wreg_t)four_bytes_wreg_index];

  uint16 sender_x_wreg_index;
  sender_x_wreg_index =  sboot_fetch_input_wreg();
  if (sender_x_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 sender_x;
  sender_x= sboot_workspace.wreg[(wreg_t)sender_x_wreg_index];

  uint16 sender_y_wreg_index;
  sender_y_wreg_index =  sboot_fetch_input_wreg();
  if (sender_y_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 sender_y;
  sender_y = sboot_workspace.wreg[(wreg_t)sender_y_wreg_index];

  uchar byte0;
  byte0 = (uchar)((four_bytes & 0x000000ff) >> 0);
  uchar byte1;
  byte1 = (uchar)((four_bytes & 0x0000ff00) >> 8);
  uchar byte2;
  byte2 = (uchar)((four_bytes & 0x00ff0000) >> 16);
  uchar byte3;
  byte3 = (uchar)((four_bytes & 0xff000000) >> 24);

  // Note: string messages come out 4 bytes at a time, and are terminated by
  // a 0.
  if (byte0 == 0) {
    sboot_send_string_msg(sender_x, sender_y);
    return TRUE;
  }
  sboot_build_string_msg(byte0, sender_x, sender_y);

  if (byte1 == 0) {
    sboot_send_string_msg(sender_x, sender_y);
    return TRUE;
  }
  sboot_build_string_msg(byte1, sender_x, sender_y);

  if (byte2 == 0) {
    sboot_send_string_msg(sender_x, sender_y);
    return TRUE;
  }
  sboot_build_string_msg(byte2, sender_x, sender_y);

  if (byte3 == 0) {
    sboot_send_string_msg(sender_x, sender_y);
    return TRUE;
  }
  sboot_build_string_msg(byte3, sender_x, sender_y);

  return TRUE;
}

// These routines are called to record specific success/failure indications
// from the programs running on the MAESTRO, so we can include them in
// beacon indicators.
static bool sboot_note_boot_pass_succeeded() {
  sboot_workspace.boot_pass_succeeded = TRUE;
  sboot_workspace.boot_pass_vector0 = 0;
  sboot_workspace.boot_pass_vector1 = 0;
  return TRUE;
}

// Note that we received a BOOT_PASS message from the MAESTRO
// that indicated failure by including a non-zero value for one of
// the two 32-bit result vectors.  We'll copy the result vectors
// for later reporting.
//
// Note:  An alternative design would be to reserve two WREG's
// to hold the result vectors, and have the beacons directly
// access the WREG values from the reserved WREG's.
static bool sboot_note_boot_pass_failed() {
  sboot_workspace.boot_pass_failed = TRUE;

  // Get the vector0 word register from the command program.
  uint16 vector0_wreg_index;
  vector0_wreg_index = sboot_fetch_input_wreg();
  if (vector0_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Get the vector1 word register from the command program.
  uint16 vector1_wreg_index;
  vector1_wreg_index = sboot_fetch_input_wreg();
  if (vector1_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Fetch the result vector values and store them for future use.
  sboot_workspace.boot_pass_vector0 =
    sboot_workspace.wreg[(wreg_t)vector0_wreg_index];
  sboot_workspace.boot_pass_vector1 =
    sboot_workspace.wreg[(wreg_t)vector1_wreg_index];
}

static bool sboot_note_boot_done() {
  sboot_workspace.boot_done = TRUE;
  return TRUE;
}

// This routine is called when the sboot program has reached
// a point of successful conclusion.
//
// returns TRUE on success always.
static bool sboot_done() {
  sboot_workspace.state = SBOOT_STATE_DONE;
  return TRUE;
}

// Skip program metadata (test name, length, CRC).  The metadata
// starts with a 1-byte type field and a 1-byte length field.
//
// Well, print the test name.
//
// returns TRUE on success always.
static bool sboot_process_program_info() {
  uint16 info_type;
  info_type = sboot_fetch_input_byte();
  if (info_type == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 info_length;
  info_length = sboot_fetch_input_byte();
  if (info_length == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  if (info_type == SBOOT_INFO_TEST_NAME) {
    // TODO: range check the info length.
    sboot_start_shouting(SBOOT_SHOUT_MESSAGE, SBOOT_SHOUT_TEST_NAME);
    sboot_shout_string("Running ");
    sboot_shout_stream(info_length);
    sboot_send_shout_done();

  } else {
    // Skip the embedded info:
    sboot_workspace.input_offset =
      sboot_workspace.input_offset + info_length; // Avoid possible compiler bug.
  }
  return TRUE;
}

// Fetch a byte from a specified program at a specified offset.
// The arguments are three WREG indexes:
//
// 1) The index of the WREG containing the source program number.
// 2) The index of the WREG containing the source offset.
// 3) The index of the WREG that will hold the result.
//
// The result will be SBOOT_FETCH_ERROR if the source program
// number or source offset is invalid.  
//
// returns FALSE if the current program runs off the end or
// if one of the wreg indexes is incorrect.  Otherwise, return
// TRUE, even if the source program number or source program
// offset is invalid.
static bool sboot_read_program_byte() {
  uint16 src_program_wreg_index;
  src_program_wreg_index = sboot_fetch_input_wreg();
  if (src_program_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  
  uint16 src_offset_wreg_index;
  src_offset_wreg_index = sboot_fetch_input_wreg();
  if (src_offset_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  
  uint16 dest_wreg_index;
  dest_wreg_index =  sboot_fetch_input_wreg();
  if (dest_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint32 src_program_number;
  src_program_number = sboot_workspace.wreg[(wreg_t)src_program_wreg_index];
  uint32 src_offset;
  src_offset = sboot_workspace.wreg[(wreg_t)src_offset_wreg_index];
  uint16 result;
  result = sboot_fetch_program_byte(src_program_number, src_offset);

  sboot_workspace.wreg[(wreg_t)dest_wreg_index] = result;
  return TRUE;
}

// returns FALSE if the current program runs off the end or
// if the wreg index is incorrect.  Otherwise, return TRUE.
static bool sboot_do_get_number_of_programs() {
  uint16 dest_wreg_index;
  dest_wreg_index =  sboot_fetch_input_wreg();
  if (dest_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_workspace.wreg[(wreg_t)dest_wreg_index] =
    sboot_get_number_of_programs();
  return TRUE;
}

// returns FALSE if the current program runs off the end or
// if the wreg index is incorrect.  Otherwise, return TRUE.
static bool sboot_do_get_first_program_number() {
  uint16 dest_wreg_index;
  dest_wreg_index =  sboot_fetch_input_wreg();
  if (dest_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_workspace.wreg[(wreg_t)dest_wreg_index] =
    sboot_get_first_program_number();
  return TRUE;
}

// returns FALSE if the current program runs off the end or
// if a wreg index is incorrect.  Otherwise, return TRUE.
static bool sboot_do_validate_program_number() {
  uint16 src_program_wreg_index;
  src_program_wreg_index = sboot_fetch_input_wreg();
  if (src_program_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint16 dest_wreg_index;
  dest_wreg_index =  sboot_fetch_input_wreg();
  if (dest_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  sboot_program_number_t src_program_number;
  src_program_number = sboot_workspace.wreg[(wreg_t)src_program_wreg_index];

  sboot_workspace.wreg[(wreg_t)dest_wreg_index] =
    sboot_validate_program_number(src_program_number);
  return TRUE;
}

// Apply the CRC kernel calculation to one byte of program data.
// Parameters:
//
// crc_wreg_index -- The index of the WREG that holds the current
//                   CRC value.  It is both read and written.
//
// program_byte_wreg_index
//
// returns FALSE if the current program runs off the end or
// if a wreg indexis incorrect.  Otherwise, return TRUE.
static bool sboot_crc_kernel() {
  uint16 crc_wreg_index;
  crc_wreg_index =  sboot_fetch_input_wreg();
  if (crc_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint16 program_byte_wreg_index;
  program_byte_wreg_index =  sboot_fetch_input_wreg();
  if (program_byte_wreg_index == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  uint16 crcval = sboot_workspace.wreg[crc_wreg_index];
  uint16 program_byte = sboot_workspace.wreg[program_byte_wreg_index];

  uint16 t;
  t = crcval ^ program_byte;
  t = (t ^ (t << 4)) & 0xff;
  crcval = (crcval >> 8) ^ (t << 8) ^ (t << 3) ^ (t >> 4);

  sboot_workspace.wreg[(wreg_t)crc_wreg_index] = crcval;
  return TRUE;
}

// Reconfigure the UART to the MAESTRO.
static bool sboot_change_uart_speed() {
  uint16 high_speed;
  high_speed =  sboot_fetch_input_wreg();
  if (high_speed == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  if (high_speed) {
    bool result;
    result = sboot_uart_change_speed(TRUE);
    if (!result) {
      sboot_shout_failure(SBOOT_SHOUT_FAILED_TO_CHANGE_UART_TO_HIGH_SPEED,
			  "sboot: failed to change the UART to high speed ");
      return FALSE;
    }

  } else {
    bool result;
    result = sboot_uart_change_speed(FALSE);
    if (!result) {
      sboot_shout_failure(SBOOT_SHOUT_FAILED_TO_CHANGE_UART_TO_LOW_SPEED,
			  "sboot: failed to change the UART to low speed ");
      return FALSE;
    }
  }
  return TRUE;
}


// This routine is called when a bad command is encountered in
// the sboot input program.
//
// returns FALSE on error always.
static bool sboot_report_bad_command(uint16 command) {
  sboot_shout_failure_with_value(SBOOT_SHOUT_BAD_COMMAND,
				 "sboot: bad command ",
				 (uint32)command);
  return FALSE;
}

// Get the next command from the sboot input program and dispatch
// to the command's handler.
//
// Returns TRUE when processing, FALSE when an error occurs.
static bool sboot_process_command() {
  // Save the offset of the start of the command:
  sboot_workspace.current_command_offset = sboot_workspace.input_offset;

  // Fetch the new command:
  uint16 command;
  command = sboot_fetch_input_byte();
  if (command == SBOOT_FETCH_ERROR) {
    // TODO: give a feedback message.
    return FALSE;
  }

#ifdef SBOOT_TRACE
  if (sboot_trace_flag) {
#if defined(__PCD__) && defined(__MAESTRO__)
    sprintf(dbgbuf, "\r\n*** executing command %d at offset %Ld\r\n",
	    command, sboot_workspace.current_command_offset);
    maestro_msg_dbgbuf();
#endif // __PCD__ && __MAESTRO
#if !defined(__PCD__)
    printf("*** executing command %d at offset %d\n",
	   command, sboot_workspace.current_command_offset);
#endif // __PCD__
  }
#endif // SBOOT_TRACE

  switch ((sboot_program_command_t)command) {
  case COMMAND_SET_READ_TIMEOUT:
    return sboot_set_read_timeout_command();

  case COMMAND_SET_PROGRAM_TIMEOUT:
    return sboot_set_program_timeout_command();
      
  case COMMAND_ENABLE_MAESTRO:
    return sboot_enable_maestro_command();

  case COMMAND_DISABLE_MAESTRO:
    return sboot_disable_maestro_command();

  case COMMAND_RESET_MAESTRO:
    return sboot_start_reset_maestro_command();

  case COMMAND_SEND_BYTE_TO_MAESTRO:
    return sboot_send_fixed_len_data_to_maestro(1);

  case COMMAND_SEND_WORD_TO_MAESTRO:
    return sboot_send_fixed_len_data_to_maestro(BYTES_PER_WORD);

  case COMMAND_SEND_BYTES_TO_MAESTRO:
    return sboot_send_variable_len_data_to_maestro();

  case COMMAND_WAIT:
    return sboot_start_wait();

  case COMMAND_READ_BYTE_FROM_MAESTRO:
    return sboot_start_read_byte_from_maestro();

  case COMMAND_INSERT_BYTE_INTO_WORD:
    return sboot_insert_byte_into_word();

  case COMMAND_PACK_BYTES_INTO_WORD:
    return sboot_pack_bytes_into_word();

  case COMMAND_ZERO_WORD:
    return sboot_zero_word();

  case COMMAND_SET_WORD:
    return sboot_set_word();

  case COMMAND_COPY_WORD:
    return sboot_copy_word();

  case COMMAND_OP_WREG_WREG_TO_WREG:
#ifdef SBOOT_OPTIMIZE_IMMEDIATES
  case COMMAND_OP_WREG_ZERO_TO_WREG:
  case COMMAND_OP_WREG_UINT8_TO_WREG:
  case COMMAND_OP_WREG_UINT16_TO_WREG:
  case COMMAND_OP_WREG_UINT24_TO_WREG:
#endif // SBOOT_OPTIMIZE_IMMEDIATES
  case COMMAND_OP_WREG_UINT32_TO_WREG:
    return sboot_execute_data_op(command);

  case COMMAND_OP_WREG_LIST_TO_WREG:
    return sboot_execute_list_op();

  case COMMAND_SHOUT:
    return sboot_shout();

  case COMMAND_SHOUT_VALUE:
    return sboot_shout_value();

  case COMMAND_SHOUT_WREG:
    return sboot_shout_wreg();

  case COMMAND_SHOUT_WREG_FIELD:
    return sboot_shout_wreg_field();

  case COMMAND_SHOUT_WREG_WREG:
    return sboot_shout_wreg_wreg();

  case COMMAND_SHOUT_IF_ZERO:
    return sboot_shout_maybe(FALSE);

  case COMMAND_SHOUT_IF_NONZERO:
    return sboot_shout_maybe(TRUE);

  case COMMAND_DONE:
    return sboot_done();


  case COMMAND_JUMP_FORWARD:
    return sboot_jump(BYTES_PER_UINT32, TRUE, TRUE, FALSE);

  case COMMAND_JUMP_FORWARD_IF_ZERO:
    return sboot_jump(BYTES_PER_UINT32, TRUE, FALSE, FALSE);

  case COMMAND_JUMP_FORWARD_IF_NONZERO:
    return sboot_jump(BYTES_PER_UINT32, TRUE, FALSE, TRUE);


#ifdef SBOOT_OPTIMIZE_JUMPS
  case COMMAND_JUMP_BACKWARD_UINT8:
    return sboot_jump(BYTES_PER_UINT8, FALSE, TRUE, FALSE);

  case COMMAND_JUMP_BACKWARD_IF_ZERO_UINT8:
    return sboot_jump(BYTES_PER_UINT8, FALSE, FALSE, FALSE);

  case COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT8:
    return sboot_jump(BYTES_PER_UINT8, FALSE, FALSE, TRUE);


  case COMMAND_JUMP_BACKWARD_UINT16:
    return sboot_jump(BYTES_PER_UINT16, FALSE, TRUE, FALSE);

  case COMMAND_JUMP_BACKWARD_IF_ZERO_UINT16:
    return sboot_jump(BYTES_PER_UINT16, FALSE, FALSE, FALSE);

  case COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT16:
    return sboot_jump(BYTES_PER_UINT16, FALSE, FALSE, TRUE);


  case COMMAND_JUMP_BACKWARD_UINT24:
    return sboot_jump(BYTES_PER_UINT24, FALSE, TRUE, FALSE);

  case COMMAND_JUMP_BACKWARD_IF_ZERO_UINT24:
    return sboot_jump(BYTES_PER_UINT24, FALSE, FALSE, FALSE);

  case COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT24:
    return sboot_jump(BYTES_PER_UINT24, FALSE, FALSE, TRUE);
#endif // SBOOT_OPTIMIZE_JUMPS


  case COMMAND_JUMP_BACKWARD_UINT32:
    return sboot_jump(BYTES_PER_UINT32, FALSE, TRUE, FALSE);

  case COMMAND_JUMP_BACKWARD_IF_ZERO_UINT32:
    return sboot_jump(BYTES_PER_UINT32, FALSE, FALSE, FALSE);

  case COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT32:
    return sboot_jump(BYTES_PER_UINT32, FALSE, FALSE, TRUE);


#ifdef SBOOT_OPTIMIZE_CALLS
  case COMMAND_CALL_UINT8:
    return sboot_call(BYTES_PER_UINT8);

  case COMMAND_CALL_UINT16:
    return sboot_call(BYTES_PER_UINT16);

  case COMMAND_CALL_UINT24:
    return sboot_call(BYTES_PER_UINT24);
#endif // SBOOT_OPTIMIZE_CALLS

  case COMMAND_CALL_UINT32:
    return sboot_call(BYTES_PER_UINT32);


  case COMMAND_RETURN:
    return sboot_return();

  case COMMAND_GET_CURRENT_SECONDS:
    return sboot_get_current_seconds();

  case COMMAND_ASSEMBLE_STRING_MSG:
    return sboot_assemble_string_msg();

  // Capture certain success/failure indications from the MAESTRO:
  case COMMAND_NOTE_BOOT_PASS_SUCCEEDED:
    return sboot_note_boot_pass_succeeded();

  case COMMAND_NOTE_BOOT_PASS_FAILED:
    return sboot_note_boot_pass_failed();

  case COMMAND_NOTE_BOOT_DONE:
    return sboot_note_boot_done();

  case COMMAND_PROGRAM_INFO:
    return sboot_process_program_info();

  case COMMAND_READ_PROGRAM_BYTE:
    return sboot_read_program_byte();

  case COMMAND_GET_NUMBER_OF_PROGRAMS:
    return sboot_do_get_number_of_programs();

  case COMMAND_GET_FIRST_PROGRAM_NUMBER:
    return sboot_do_get_first_program_number();

  case COMMAND_VALIDATE_PROGRAM_NUMBER:
    return sboot_do_validate_program_number();

  case COMMAND_CRC_KERNEL:
    return sboot_crc_kernel();

  case COMMAND_CHANGE_UART_SPEED:
    return sboot_change_uart_speed();

  default:
    return sboot_report_bad_command(command);
  }
}

// This routine should be called at least when entering
// a terminal state.
static void sboot_cleanup() {
  if (sboot_workspace.maestro_is_enabled) {
    sboot_disable_maestro();
    sboot_workspace.maestro_is_enabled = FALSE;
  }
}

// This is the main execution state dispatcher.
//
// Returns TRUE when processing, FALSE when done.
bool sboot_execute() {
  // If we are running, check on entry if certain conditions have
  // been met.
  sboot_execution_state_t entry_state;
  entry_state = sboot_workspace.state;

  // Check if we should sbut own the MAESTRO because we are running
  // low on power.  Because the MAESTRO-releated code runs on the
  // upper processor, and satellite power mamangement is conducted on
  // the lower processor, there's no point (in the present satellite
  // software archetecture) to making this test more frequently than
  // once per sboot_execute() entry.  If the satellite software
  // changes in this regard in the future, we may want to move this
  // test to the point marked below.
  if (entry_state != SBOOT_STATE_UNINITIALIZED) {
    if (sboot_workspace.maestro_is_enabled &&
	sboot_should_shut_down()) {
      sboot_shout_failure(SBOOT_SHOUT_SHUTDOWN_REQUESTED,
			  "sboot: shutdown requested");
      sboot_cleanup();
      return FALSE;
    }
    
    switch (entry_state) {
    case SBOOT_STATE_RUNNING:
    case SBOOT_STATE_WAITING:
    case SBOOT_STATE_RESETTING_MAESTRO:
    case SBOOT_STATE_SENDING_DATA_TO_MAESTRO:
    case SBOOT_STATE_READING_DATA_FROM_MAESTRO: {
      if (sboot_workspace.enable_program_timeout &&
	  !(sboot_timeout_still_running(sboot_workspace.program_timeout,
					sboot_workspace.program_start_time))) {
	sboot_shout_failure(SBOOT_SHOUT_PROGRAM_TIMEOUT,
			    "sboot: program timeout occured");
	sboot_cleanup();
	return FALSE;
	
      }
    }
    }
  }
    
  uint16 max_iterations;
  max_iterations= sboot_workspace.execute_max_commands_before_yield;

  bool first_iteration;
  first_iteration = TRUE;
  while (max_iterations > 0) {
    max_iterations--;

    // This is an alternative site for the sboot_should_shut_down() test.

    if (sboot_should_yield_now()) {
      return TRUE;
    }

    sboot_execution_state_t state;
    state = sboot_workspace.state;

#ifdef SBOOT_TRACE
    if (sboot_trace_flag) {
      if (state != sboot_workspace.old_state) {
#ifndef __PCD__
	printf("*** state %d --> %d\n", (int)sboot_workspace.old_state, (int)state);
#endif // __PCD__
	sboot_workspace.old_state = state;
      }
    }
#endif // SBOOT_TRACE

    switch (state) {

    case SBOOT_STATE_UNINITIALIZED: {
      // We shouldn't ever get here... and that's why we check if we do.
      sboot_shout_failure_with_value(SBOOT_SHOUT_UNINITIALIZED_EXECUTION_STATE,
				     "sboot: uninitialized execution state ",
				     (uint32)state);
      // Note:  we do *not* call sboot_cleanup() when uninitialized.
      return FALSE;
    }

    case SBOOT_STATE_INITIALIZED: {
      return TRUE; // Wait until started.
    }

    case SBOOT_STATE_STOPPED: {
      // On the one hand, I want to return TRUE here when the
      // stop was meant to be a pause.  Ont the other hand, I want
      // to return FALSE here when the stop was meant as a pause.
      //
      // TODO: Split sboot_stop() functionality.
      return TRUE; // Essentially, paused.
    }

    case SBOOT_STATE_RUNNING: {
      if (!sboot_process_command()) {
	sboot_cleanup();
	return FALSE;
      }
      break;
    }

    case SBOOT_STATE_WAITING: {
      if (sboot_waiting()) {
	return TRUE;
      }
      break;
    }

    case SBOOT_STATE_RESETTING_MAESTRO: {
      if (sboot_resetting_maestro()) {
	return TRUE;
      }
      break;
    }

    case SBOOT_STATE_SENDING_DATA_TO_MAESTRO: {
      if (sboot_sending_data_to_maestro()) {
	break; // Progress is being made, iterate
      }
      return TRUE;
    }

    case SBOOT_STATE_READING_DATA_FROM_MAESTRO: {
      if (sboot_reading_data_from_maestro()) {
	return TRUE;
      }
      break;
    }

    case SBOOT_STATE_FAILED:
    case SBOOT_STATE_DONE: {
      // Terminal states.
      sboot_cleanup();
      return FALSE;
    }

    default:
      // We shouldn't ever get here... and that's why we check if we do.
      sboot_shout_failure_with_value(SBOOT_SHOUT_UNKNOWN_EXECUTION_STATE,
				     "sboot: unknown execution state ",
				     (uint32)state);
      sboot_cleanup();
      return FALSE;
    }
    first_iteration = FALSE;
  }
  return TRUE;
}
