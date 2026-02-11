// ccsc_test.c  30-Nov-2011  Craig Milo Rogers <rogers@isi.edu?
//
// Set up a test for compiling code with the CCSC compiler.

#include <24FJ256GA110.h>
#define __24FJ256GA110_H__

#CASE

#include "../includes/sboot_types.h"
#include "../includes/sboot_enable_maestro_phase.h"
#include "../execution/sboot_execution.c"

// **************** SBOOT Callbacks **********************
//
// sboot_execute(...) will call these routines.
//

// Fetch a byte from the virtual machine input buffer.
// Return SBOOT_FETCH_ERROR if the offset is too large.
uint16 sboot_fetch_program_byte(sboot_program_number_t program_number,
				sboot_size_t offset) {
  return 0;
}

void sboot_reset_maestro(bool value) {
}

void sboot_enable_maestro(sboot_enable_maestro_phase_t phase) {
}

void sboot_disable_maestro() {
}

sboot_timer_t sboot_current_seconds() {
  return (sboot_timer_t) 0;
}

bool sboot_uart_change_speed(bool high_speed) {
  return FALSE;
}

bool sboot_uart_send(uint8 byteval) {
  return FALSE;
}

uint16 sboot_uart_receive() {
  return SBOOT_UART_NOT_READY;
}

bool sboot_get_shout_buf(uchar **bufp, uint16 *buf_lenp) {
  return FALSE;
}

void sboot_send_shout(sboot_shout_severity_t severity,
		      sboot_shout_reason_t reason,
		      uint16 len) {
}

// Return TRUE when sboot should yield ASAP.
bool sboot_should_yield_now() {
  return FALSE;
}

// Return TRUE when sboot should shut down ASAP, e.g., due to an anomoly
// in the power system.
bool sboot_should_shut_down() {
  return FALSE;
}

// Get the number of programs.
sboot_program_number_t sboot_get_number_of_programs() {
  return 1;
}

// Get the first program number.
sboot_program_number_t sboot_get_first_program_number() {
  return 1;
}

// Validate the program number.
// Returns TRUE if the program number is valid, else FALSE.
bool sboot_validate_program_number(sboot_program_number_t program_number) {
  return TRUE;
}

// ***************************************************************
// Embedded programs get a simpler main(...);
void main() {
  sboot_initialize((sboot_program_number_t)0);
  sboot_start();
  while (sboot_execute()) {
  }
}
