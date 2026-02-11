#ifndef __SBOOT_EXECUTION_H
#define __SBOOT_EXECUTION_H

#include "sboot_types.h"
#include "sboot_wreg.h"
#include "sboot_program_info.h"

// Call this routine to initialize it sboot's workspace and execution
// state, including setting the pc to 0.  This is intentionally the only
// way to change the current program number.
void sboot_initialize(sboot_program_number_t program_number);

// Get/set the current program number.
sboot_program_number_t sboot_get_program_number();
void sboot_set_program_number(sboot_program_number_t program_number);

// Get/set the current program number.
sboot_size_t sboot_get_program_offset();
void sboot_set_program_offset(sboot_size_t offset);

// Get/set the offset of the start of the current command, for debugging.
sboot_size_t sboot_get_current_command_offset();
void sboot_set_current_command_offset(sboot_size_t offset);

// Get/set a WREG value.  This can be used for debugging, or for invoking
// programs with arguments.  These routines return FALSE when the wreg_index
// is out of range, else TRUE;
//
// TODO: Reconsider the call signature here, it'st odd for a getter/setter
// pair.
//
// TODO: Rename "wreg_t" to "sboot_wreg_t" here and elsewhere.
bool sboot_get_wreg(wreg_t wreg_index, uint32 *valuep);
bool sboot_set_wreg(wreg_t wreg_index, uint32 value);

// sboot_start() should start running, but only if in SBOOT_INITIALIZED state.
// If currently in SBOOT_INITIALIZED state, it advances to the running state.
// Otherwise, it is ignored (idempotency).  Should it produce feedback messages
// as confirmation?
void sboot_start();

// sboot_stop() should change state to STOPPED saving the prior
// state, but not if the prior state was itself STOPPED (idempotency)
// or certain other initial or terminal states.
void sboot_stop();

// sboot_resume() is similar to sboot_start(), except that it it will
// operate only if the current state is SBOOT_STOPPED.  It restores
// the state before the sboot_stop(). You can use
// sboot_stop()/sboot_resume() as a single-level lock.
void sboot_resume();

// Call this routine to execute some of an sboot program.  Call it
// again to execute some more.
//
// When done processing, or if waiting for I/O, it will return
// immediately.  Otherwise, it will execute at most some number of
// virtual machine instructions, then return.
//
// Returns TRUE when processing, FALSE when done.
bool sboot_execute();

// This routine provides dynamic control over sboot execution traces,
// but is effective only when SBOOT_TRACE has been defined at compile time.
void sboot_set_trace(bool value);
bool sboot_get_trace();

// Get/set the program timeout.
void sboot_set_program_timeout(uint16 value);
uint16 sboot_get_program_timeout();

// Is the program timeout enabled?
void sboot_set_enable_program_timeout(bool value);
bool sboot_get_enable_program_timeout();

// Is the program timeout overridden?
void sboot_set_override_program_timeout(bool value);
bool sboot_get_override_program_timeout();

// Get/set the program start time.
void sboot_set_program_start_time(sboot_timer_t value);
sboot_timer_t sboot_get_program_start_time();

// sboot_show_state() should generate one or more messages with important workspace info.

// Get/set the max number of commands to execute before yielding.
//
// Note: Any value set here gets reset to the default when
// sboot_initialize() is called.
uint16 sboot_get_max_commands_before_yield();
void sboot_set_max_commands_before_yield(uint16 value);

// Get the current execution state as a uint8.  This will be
// helpful for debugging or status messages.
uint8 sboot_get_execution_state_byte();
uint8 sboot_get_execution_next_state_byte();
uint8 sboot_get_execution_state_before_stop_byte();

// Do we think we've enabled the MAESTRO?
bool sboot_is_maestro_enabled();

// Have we seen a BOOT_PASS message from the MAESTRO level 1 test
// code, and if so, did it indicate success or failure?
bool sboot_get_boot_pass_succeeded();
bool sboot_get_boot_pass_failed();

// Return the two 32-bit BOOT_PASS result vectors.
uint32 sboot_get_boot_pass_vector0();
uint32 sboot_get_boot_pass_vector1();

// Have we seen a BOOT_DONE message from the MAESTRO level 2 test
// code?
bool sboot_get_boot_done();

#endif // __SBOOT_EXECUTION_H
