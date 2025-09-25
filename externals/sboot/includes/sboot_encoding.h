// sboot_encoding.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

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

#ifndef __SBOOT_ENCODING_H
#define __SBOOT_ENCODING_H

#include <stdio.h>
#include <time.h>

#include "sboot_types.h"
#include "sboot_wreg.h"
#include "sboot_op.h"
#include "sboot_enable_maestro_phase.h"
#include "sboot_shout.h"

void init_sboot_encoding();
void set_sboot_encoding_verbose(bool value);

uint8 *get_sboot_buffer();
uint32 get_sboot_buffer_len();

void set_read_timeout(uint8 seconds);
void set_program_timeout(uint16 seconds);

void enable_maestro(sboot_enable_maestro_phase_t phase);
void disable_maestro();
void reset_maestro(uint8 seconds);

void send_byte(address_t address, channel_t channel, uint8 byte);
void send_word(address_t address, channel_t channel, uint32 word);
void send_bytes(address_t address, channel_t channel,
		uint16 len, uint8 *bytes);

void stn_push(uint32 value);

void wait(uint8 seconds);

// Read into a WREG (clearing high-order bits).
void read_byte(wreg_t wreg, address_t address, channel_t channel);

// Insert a byte into a position in a word
void insert_byte_into_word(wreg_t dest_wreg, uint8 dest_byte_index, wreg_t src_wreg);

void pack_bytes_into_word(wreg_t dest_wreg, wreg_t high_wreg, wreg_t upper_mid_wreg,
			  wreg_t lower_mid_wreg, wreg_t low_wreg);

// Reads a word from 4 successive addresses, little-endian
void read_word(wreg_t wreg, address_t address, channel_t channel);

void set_word(wreg_t dest, uint32 value);
void copy_word(wreg_t dest, wreg_t src);

void op_ri(wreg_t dest, wreg_t src, sboot_op_t op, uint32 value);

void op_rl(wreg_t dest, wreg_t src1, sboot_op_t op,
	   uint32 *list_data, list_len_t list_len);
void op_rr(wreg_t dest, wreg_t src1, sboot_op_t op, wreg_t src2);

void increment(wreg_t dest);
void decrement(wreg_t dest);

// In "_field" versions, the masked field is extracted and right-shifted.

// Debug messages:
void debug(char *msg);
void debug2(char *msg1, char *msg2);
void debug_value(char *msg, uint32 value);
void debug_wreg(char *msg, wreg_t wreg);
void debug_wreg_field(char *msg, wreg_t wreg, uint32 mask);
void debug_wreg_wreg(char *msg, wreg_t wreg1, wreg_t wreg2);
void debug_if_zero(char *msg, wreg_t test_wreg, wreg_t show_wreg);
void debug_if_nonzero(char *msg, wreg_t test_wreg, wreg_t show_wreg);
void debug_if_false(char *msg, wreg_t test_wreg, wreg_t show_wreg);
void debug_if_true(char *msg, wreg_t test_wreg, wreg_t show_wreg);

// Normal messages (perhaps revealing test results):
void shout(sboot_shout_reason_t reason, char *msg);
void shout_value(sboot_shout_reason_t reason, char *msg, uint32 value);
void shout_wreg(sboot_shout_reason_t reason, char *msg, wreg_t wreg);
void shout_wreg_field(sboot_shout_reason_t reason, char *msg, wreg_t wreg, uint32 mask);
void shout_wreg_wreg(sboot_shout_reason_t reason, char *msg, wreg_t wreg1, wreg_t wreg2);
void shout_if_zero(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg);
void shout_if_nonzero(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg);
void shout_if_false(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg);
void shout_if_true(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg);

// Failure messages:
void fail(sboot_shout_reason_t reason, char *msg);
void fail_value(sboot_shout_reason_t reason, char *msg, uint32 value);
void fail_wreg(sboot_shout_reason_t reason, char *msg, wreg_t wreg);
void fail_wreg_field(sboot_shout_reason_t reason, char *msg, wreg_t wreg, uint32 mask);
void fail_wreg_wreg(sboot_shout_reason_t reason, char *msg, wreg_t wreg1, wreg_t wreg2);
void fail_if_zero(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg);
void fail_if_nonzero(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg);
void fail_if_false(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg);
void fail_if_true(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg);

void all_done();

void note_boot_pass_succeeded();
void note_boot_pass_failed(wreg_t result_vector0_wreg, wreg_t result_vector1_wreg);
void note_boot_done();

// Jump labels.   Multiple backward jumps can use the same label.
// The labels are also used for subroutine declarations and calls.
enum label_type_enum {
  INITIAL_LABEL,
  FORWARD_LABEL,
  BACKWARD_LABEL,
  INITIAL_SUBR_LABEL,
  SUBR_LABEL
};
typedef enum label_type_enum label_type_t;

// TODO: use a fixup chain or other dynamic structure.
#define MAX_LABELS 1000
#define MAX_LABEL_FIXUPS 10

struct label_struct {
  label_type_t label_type;
  sboot_size_t fixup_location[MAX_LABEL_FIXUPS];
  uint8 fixup_count;
  sboot_size_t jump_target;
  char *name;
  struct label_struct *context;
};
typedef struct label_struct *label_t;

// Create a new jump target label tied to the current position in the
// sboot_stream:
label_t current_label(char *name);

// Create a new jump target label that, in the future, will be tied to a
// spot in the sboot stream:
label_t future_label(char *name);

// Label a jump target that was defined through future_label().  Perform
// forward reference fixup(s).  The supplied label is returned for
// convenience.
label_t label(label_t label);

void check_for_unresolved_labels();

// Forward or backward jump, depending on the target.  The
// supplied label is returned for convenience, allowing structures
// such as:
//
// label_t some_label = go_to(future_label());
label_t go_to(label_t label);
label_t go_to_if_zero(label_t label, wreg_t wreg);
label_t go_to_if_nonzero(label_t label, wreg_t wreg);
label_t go_to_if_false(label_t label, wreg_t wreg);
label_t go_to_if_true(label_t label, wreg_t wreg);

// Declare a subroutine, also creating a label.  You will want
// to save this label, because it's needed to end and call
// the subroutine.
//
// TODO: Would it be better to use the name field to end and
// call the subroutine?
label_t begin_subr(char *name);

// Return from a subroutine.
void subr_return();

// End a subroutine.  The supplied subr_label is checked against the stack
// to better detect nesting errors.  A subroutine return will be generated
// gratis for convenience.
void end_subr(label_t subr_label);

// Call a subroutine.
void call_subr(label_t label);

void get_current_seconds(wreg_t wreg);

// This is a specialized operation for processing MAESTRO bootstrap
// string messages.
void assemble_string_msg(wreg_t four_bytes, wreg_t sender_x, wreg_t sender_y);

// The following 'if' tests must be paired with an end_if(...).
// If the test fails, it jumps to the returned label.  The returned
// label might be useful to construct complex 'if/then/else'
// structures.
//
// TODO: Consider else_if_xxx(...) and otherwise(...) structures.
//
// TODO: Consider and_if_xxx(...) to build more complex conditionals.
// or_if_xxx(...) is harder to implement, and would imply some sort of
// parenthesizing structure, and "not(...)".
label_t if_ri(wreg_t wreg, sboot_op_t op, uint32 value, char *id);
label_t if_rr(wreg_t wreg1, sboot_op_t op, wreg_t wreg2, char *id);
label_t if_rl(wreg_t wreg, sboot_op_t op,
	      uint32 *list_data, list_len_t list_len, char *id);
label_t if_true(wreg_t wreg, char *id);
label_t if_false(wreg_t wreg, char *id);
label_t if_zero(wreg_t wreg, char *id);
label_t if_nonzero(wreg_t wreg, char *id);

// The id is a message to assist in debugging.
void end_if(char *id);

void begin_program_info();
void store_program_info_test_name(char *test_name);
void store_program_info_test_type(char *test_type);
void store_program_info_l1_file_name(char *filename);
void store_program_info_l1_file_time(time_t sometime);
void store_program_info_l2_file_name(char *filename);
void store_program_info_l2_file_time(time_t sometime);
void store_program_info_sboot_creation_by(char *username);
void store_program_info_sboot_creation_host(char *username);
void store_program_info_sboot_creation_time(time_t sometime);
void store_program_info_end();
void end_program_info();
void finish_program_info();

void read_program_byte(wreg_t dest_wreg,
		       wreg_t src_program_number_wreg,
		       wreg_t src_offset_wreg);
void get_number_of_programs(wreg_t dest_wreg);
void get_first_program_number(wreg_t dest_wreg);
void validate_program_number(wreg_t dest_wreg, wreg_t src_program_number_wreg);
void crc_kernel(wreg_t crc_wreg, wreg_t byte_wreg);

void change_uart_speed(bool high_speed);

#endif // __SBOOT_ENCODING_H
