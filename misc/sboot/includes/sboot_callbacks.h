#ifndef __SBOOT_CALLBACKS_H
#define __SBOOT_CALLBACKS_H

#include "sboot_types.h"
#include "sboot_shout.h"
#include "sboot_enable_maestro_phase.h"

// These routines must be defined externally to sboot_execution.

// Return a pointer to an output buffer.  Returns TRUE if a buffer
// is available, else FALSE.  The buffer will be returned to
// the callback environment before the next yield.
//
// In exceptional circumstances, sboot_get_shout_buf(...) might
// be called twice in a row without an intervening call to
// sboot_send_shout(...).  It should be safe to return the same
// buffer in those circumstances.
bool sboot_get_shout_buf(uchar **bufp, uint16 *buf_lenp);

// Send a progress or failure message.  "len" is the length of the
// message, which is also guaranteed to end with a terminating null
// character, not included in "len".  The buffer is implicit;  it's
// the last one obtained through sboot_get_shout_buf(...).  This
// call returns control of the buffer to the callback environment.
void sboot_send_shout(sboot_shout_severity_t severity,
		      sboot_shout_reason_t reason,
		      uint16 len);

// Fetch a byte from the buffer containing the program to run on the Maestro.
// Return SBOOT_FETCH_ERROR if the offset is out of bounds.
uint16 sboot_fetch_program_byte(sboot_program_number_t program_number,
				sboot_size_t offset);

// Reset the Maestro chip.  Activate the reset pin when value == true.
// Deactivate it when value == false.  Alternatively, an implementation
// may perform an entire reset cycle when called with value == true, and
// ignore the value == false call, so long as this does not violate other
// timing constraints.
void sboot_reset_maestro(bool value);

// Enable the MAESTRO chip.  This may involve turning on power, saving
// the current UART configuration, changing the UART configuration,
// and changing the UART routing.  It may be necessary for this
// activity to take place in several phases, with waits inbetween.
//
// An alternative implementation would be to have seperate callbacks
// for power-on, UART reconfiguration and UART rerouting.  I opted
// for this design in an attempt to reduce the impact to the code base of
// last-minute changes in this area.
//
// TODO: Do we need to flush the UART?
void sboot_enable_maestro(sboot_enable_maestro_phase_t phase);

// Disable the MAESTRO chip.  This may involve turning off
// power, restoring the UART configuration, and rerouting the UART.
// I chose not to implement phases with possible time delays, because
// it may be necessary to call this routine during an abnormal exit
// from an sboot progam.
//
// TODO: Are phases necessary here?
// TODO: Do we need to flush the UART?
void sboot_disable_maestro();

// Return a value that increments by one each second.  The
// initial value is not defined.
sboot_timer_t sboot_current_seconds();

// Switch the UART between low speed (9600 bps) and high speed
// (115200 bps) operation.
//
// TODO: Generalize to other speeds.
//
// Returns TRUE if the change appears to have succeeded.
bool sboot_uart_change_speed(bool high_speed);

// Send a byte to the UART.  Returns true if the send appears to
// to have succeeded (although the outgoing character may be waiting
// in a buffer), else return false.
bool sboot_uart_send(uint8 byteval);

// Receive a byte from the UART.  Returns SBOOT_UART_NOT_READY if the UART's
// receive buffer is empty, otherwise returns the cbyte received.
// This call should be non-blocking.
uint16 sboot_uart_receive();

// SBOOT_UART_NOT_READY is a special value used by the UART receive
// routine.  It is a uint16 value that is "out of band" to any uint8
// uart data byte.  sboot_uart_receive() returns this value when the UART's
// receive buffer is empty.
#define SBOOT_UART_NOT_READY 0xffff

// Return TRUE when sboot should yield ASAP.
bool sboot_should_yield_now();

// Return TRUE when sboot should shut down ASAP, e.g., due to an anomoly
// in the power system.
bool sboot_should_shut_down();

// Get the number of programs.
sboot_program_number_t sboot_get_number_of_programs();

// Get the first program number.
sboot_program_number_t sboot_get_first_program_number();

// Validate the program number.
// Returns TRUE if the program number is valid, else FALSE.
bool sboot_validate_program_number(sboot_program_number_t program_number);

#endif // __SBOOT_CALLBACKS_H
