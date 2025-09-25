#ifndef __SBOOT_EXECUTION_SHOUT_H
#define __SBOOT_EXECUTION_SHOUT_H

// This file declares soume shout routines that are declared
// in "sboot_execution.c" and are used in "sboto_execution_info.c".

#include "sboot_types.h"
#include "sboot_shout.h"

// Build and send messages (shouts).
void sboot_start_shouting(sboot_shout_severity_t severity,
			  sboot_shout_reason_t reason);

// Transfer a character into the shout buffer, truncating on overflow.
// Returns TRUE if there was space in the buffer, else returns FALSE.
bool sboot_shout_uchar(uchar c);

// Transfer a C string *constant* into the shout buffer, truncating on
// overflow.
void sboot_shout_string(string_literal msg);

// Append a hex value into the shout buffer, truncating on overflow.
// Surround the value in parentheses, in part to guard against
// misinterpretation if buffer overflow does occur.
void sboot_shout_hex_value(uint32 value);

// Complete the shout message and send it off.
void sboot_send_shout_done();

#endif // __SBOOT_EXECUTION_SHOUT_H
