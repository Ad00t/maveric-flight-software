// sboot_encoding.c  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

// This file contains routines used for encoding test programs for
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

// TODO: check for I/O errors.
// TODO: bounds checks for arguments.
//
// TODO: unclosed labels, subroutines and (eventually) if blocks should be
// reported at the end of encoding.  If this is done in all_done(), then
// some other API call should be provided to indicate runtime completion
// without compile-time completion.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../includes/sboot_encoding.h"
#include "../includes/sboot_program.h"
#include "../includes/sboot_types.h"
#include "../includes/sboot_wreg.h"
#include "../includes/sboot_op.h"
#include "../includes/sboot_shout.h"
#include "../includes/sboot_features.h"
#include "../includes/sboot_enable_maestro_phase.h"
#include "../includes/sboot_program_info.h"

#include "../mdb/rshim.h"

// Create a fixed-length buffer for the encoded output.  We can switch to
// dynamic allocation later.
#define SBOOT_BUFFER_SIZE 100000

static uint8 sboot_buffer[SBOOT_BUFFER_SIZE];
static sboot_size_t sboot_buffer_len;

// Nested subroutine definitions are allowed.  Create a stack
// for analyzing them.  This stack can have a different depth
// from the runtime subroutine call stack in the workspace.
//
// TODO: label_t is too overloaded, split it up.
#define SBOOT_SUBR_DEFINITION_MAX_DEPTH 6
static label_t subr_definition_stack[SBOOT_SUBR_DEFINITION_MAX_DEPTH];
static int subr_definition_stack_len;
static label_t current_subr;

// Nested if statements are allowed.  Create a stack to
// simplify handling them.
#define SBOOT_IF_STACK_MAX_DEPTH 10
static label_t if_stack[SBOOT_IF_STACK_MAX_DEPTH];
static int if_stack_len;

static sboot_size_t program_info_length_target;
static sboot_size_t program_info_crc_target;

static bool verbose;

// Initialize various internal static fields.  Even if these
// fields are supposed ot initialize to these values, it is
// preferable to call this routine.
void init_sboot_encoding() {
  sboot_buffer_len = 0;
  subr_definition_stack_len = 0;
  current_subr = 0;
  if_stack_len = 0;

  program_info_length_target = 0;
  program_info_crc_target = 0;

  verbose = FALSE;
}

void set_sboot_encoding_verbose(bool value) {
  verbose = value;
}


uint8 *get_sboot_buffer() {
  return sboot_buffer;
}

sboot_size_t get_sboot_buffer_len() {
  return sboot_buffer_len;
}

static void store(uint8 byte) {
  if (sboot_buffer_len >= SBOOT_BUFFER_SIZE) {
    fprintf(stderr, "sboot_encoding:  ran out of buffer space.\n");
    exit(EXIT_FAILURE);
  }
  sboot_buffer[sboot_buffer_len++] = byte;
}

static void store_at(uint8 byte, sboot_size_t loc) {
  sboot_buffer[loc] = byte;
}

static void store_uint16(uint16 word) {
  store(word & 0xff);
  store((word >> 8) & 0xff);
}

static void store_uint16_at(uint32 word, sboot_size_t loc) {
  store_at(word & 0xff, loc);
  store_at((word >> 8) & 0xff, loc + 1);
}

// The is no uint24 type, so wa pass the argument as uint32.
static void store_uint24(uint32 word) {
  store(word & 0xff);
  store((word >> 8) & 0xff);
  store((word >> 16) & 0xff);
}

static void store_word(uint32 word) {
  store(word & 0xff);
  store((word >> 8) & 0xff);
  store((word >> 16) & 0xff);
  store((word >> 24) & 0xff);
}

static void store_word_at(uint32 word, sboot_size_t loc) {
  store_at(word & 0xff, loc);
  store_at((word >> 8) & 0xff, loc + 1);
  store_at((word >> 16) & 0xff, loc + 2);
  store_at((word >> 24) & 0xff, loc + 3);
}

void set_read_timeout(uint8 seconds) {
  store(COMMAND_SET_READ_TIMEOUT);
  store(seconds);
}

void set_program_timeout(uint16 seconds) {
  store(COMMAND_SET_PROGRAM_TIMEOUT);
  store_uint16(seconds);
}

void enable_maestro(sboot_enable_maestro_phase_t phase) {
  store(COMMAND_ENABLE_MAESTRO);
  store(phase);
}

void disable_maestro() {
  store(COMMAND_DISABLE_MAESTRO);
}

void reset_maestro(uint8 seconds) {
  store(COMMAND_RESET_MAESTRO);
  store(seconds);
}

// Encode the destination address and channel, the length of the data transfer
// in bytes, and a read/write indicator, into a 4-octet header.
//
// TODO: bounds checks for address, channel and len
static void send_header(address_t address, channel_t channel,
			bool is_write, uint16 len) {
  if (channel > RSHIM_LAST_CHANNEL) {
    fprintf(stderr,"ERROR: Channel too large, %d > %d\n", channel,
	    RSHIM_LAST_CHANNEL);
    exit(EXIT_FAILURE);
  }

  // Help the user out and check the address/len combination.
  uint8 addr_low2 = address & 3;
  if (len <= BYTES_PER_WORD) {
    // If len is 4 or less, cannot cross a 4 byte boundary
    if ((addr_low2 + len) > BYTES_PER_WORD) {
      fprintf(stderr, "ERROR: SerialPort.send_segment: Access crosses a word boundary: address=0x%X, len=%d.\n",
	      address, len);
      exit(EXIT_FAILURE);
    }
  } else {
    // If len is greater than 4, addr must be word alligned, and
    // len must be a multiple of 4:
    if (addr_low2 != 0) {
      fprintf(stderr,  "ERROR: SerialPort.send_segment: len (%d) > 4, but addr not word aligned (0x%X).\n",
	      len, address);
      exit(EXIT_FAILURE);
    }
    if ((len % BYTES_PER_WORD) != 0) {
      fprintf(stderr, "ERROR: SerialPort.send_segment: len (%d) > 4, but not a multiple of 4.\n",
	      len);
      exit(EXIT_FAILURE);
    }
  }

  uint8 wr_rd = is_write ? 0 : 1;
  
  uint8 byte0 =
    len & 0xff;
  uint8 byte1 =
    ((address << 6) & 0xc0) |
    ((len >> 8) & 0x1f);
  uint8 byte2 =
    (address >> 2) & 0xff;
  uint8 byte3 =
    ((wr_rd << 7) & 0x80) |
    ((channel << 4) & 0x70) |
    ((address >> 10) & 0xf);

  store(byte0);
  store(byte1);
  store(byte2);
  store(byte3);
}

static void send_write_header(address_t address, channel_t channel,
			      uint16 len) {
  send_header(address, channel, TRUE, len);
}

static void send_read_header(address_t address, channel_t channel) {
  // Reads send a 1-byte length in the header.
  send_header(address, channel, FALSE, 1);
}

void send_byte(address_t address, channel_t channel, uint8 byte) {
  store(COMMAND_SEND_BYTE_TO_MAESTRO);
  send_write_header(address, channel, 1);
  store(byte);
}

void send_word(address_t address, channel_t channel, uint32 word) {
  store(COMMAND_SEND_WORD_TO_MAESTRO);
  send_write_header(address, channel, BYTES_PER_WORD);
  store_word(word);
}

void send_bytes(address_t address, channel_t channel,
		uint16 len, uint8 *bytes) {
  store(COMMAND_SEND_BYTES_TO_MAESTRO);

  // Send the length for convenient use on the PIC24, even though it also
  // appears in the header.
  store(len & 0xff);
  store((len >> 8) & 0xff);

  send_write_header(address, channel, len);

  while (len-- > 0) {
    store(*bytes++);
  }
}

void stn_push(uint32 value) {
  send_word(RSHIM_STN_DATA, RSHIM_CHANNEL_RSHIM, value);
}

void wait(uint8 seconds) {
  store(COMMAND_WAIT);
  store(seconds);
}


void read_byte(wreg_t wreg, address_t address, channel_t channel) {
  store(COMMAND_READ_BYTE_FROM_MAESTRO);
  send_read_header(address, channel);
  store(wreg);
}

void insert_byte_into_word(wreg_t dest_wreg, uint8 dest_byte_index, wreg_t src_wreg) {
  store(COMMAND_INSERT_BYTE_INTO_WORD);
  store(src_wreg);
  store(dest_wreg);
  store(dest_byte_index);
}

void pack_bytes_into_word(wreg_t dest_wreg, wreg_t high_wreg, wreg_t upper_mid_wreg,
			  wreg_t lower_mid_wreg, wreg_t low_wreg) {
  store(COMMAND_PACK_BYTES_INTO_WORD);
  store(high_wreg);
  store(upper_mid_wreg);
  store(lower_mid_wreg);
  store(low_wreg);
  store(dest_wreg);
}

// Reads a word from 4 successive addresses, little-endian, using
// WREG0 throuth WREG3 as scratch registers.
//
// TODO: This implementation uses 6 bytes in the stream for each byte
// read, plus 2 bytes for the bytes_to_word conversion, for a total of 26
// bytes, when a more direct encoding would have taken only 6 bytes in
// the stream (4 bytes, if doing the (address, channel) to header conversion
// at sboot run time).
void read_word(wreg_t wreg, address_t address, channel_t channel) {
  read_byte(WREG0, /* = */ address++, channel);
  read_byte(WREG1, /* = */ address++, channel);
  read_byte(WREG2, /* = */ address++, channel);
  read_byte(WREG3, /* = */ address, channel);
  pack_bytes_into_word(wreg, /* = */ WREG3, WREG2, WREG1, WREG0);
}

void set_word(wreg_t wreg, uint32 value) {
  if (value == 0) {
    store(COMMAND_ZERO_WORD);
    store(wreg);
  } else {
    store(COMMAND_SET_WORD);
    store_word(value);
    store(wreg);
  }
}
  
void copy_word(wreg_t dest, wreg_t src) {
  store(COMMAND_COPY_WORD);
  store(src);
  store(dest);
}
  
void op_ri(wreg_t dest, wreg_t src, sboot_op_t op, uint32 value) {
#ifdef SBOOT_OPTIMIZE_IMMEDIATES
  if (value == 0) {
    store(COMMAND_OP_WREG_ZERO_TO_WREG);
  } else if (value <= UINT8_MAX) {
    store(COMMAND_OP_WREG_UINT8_TO_WREG);
  } else if (value <= UINT16_MAX) {
    store(COMMAND_OP_WREG_UINT16_TO_WREG);
  } else if (value <= UINT24_MAX) {
    store(COMMAND_OP_WREG_UINT24_TO_WREG);
  } else {
    store(COMMAND_OP_WREG_UINT32_TO_WREG);
  }
#else // SBOOT_OPTIMIZE_IMMEDIATES
  store(COMMAND_OP_WREG_UINT32_TO_WREG);
#endif // SBOOT_OPTIMIZE_IMMEDIATES

  store(op);
  store(src);

#ifdef SBOOT_OPTIMIZE_IMMEDIATES
  if (value == 0) {
  } else if (value <= UINT8_MAX) {
    store((uint8)value);
  } else if (value <= UINT16_MAX) {
    store_uint16((uint16)value);
  } else if (value <= UINT24_MAX) {
    store_uint24(value);
  } else {
    store_word(value);
  }
#else // SBOOT_OPTIMIZE_IMMEDIATES
  store_word(value);
#endif // SBOOT_OPTIMIZE_IMMEDIATES

  store(dest);
}

void op_rr(wreg_t dest, wreg_t src1, sboot_op_t op, wreg_t src2) {
  store(COMMAND_OP_WREG_WREG_TO_WREG);
  store(op);
  store(src1);
  store(src2);
  store(dest);
}

void op_rl(wreg_t dest, wreg_t src1, sboot_op_t op,
           uint32 *list_data, list_len_t list_len) {
  if (op != OP_IN && op != OP_NOT_IN) {
    fprintf(stderr, "Invalid list op %d.\n", op);
    exit(EXIT_FAILURE);
  }

  if (list_len == 0) {
    fprintf(stderr, "A empty list is invalid.\n");
    exit(EXIT_FAILURE);

  } else if (list_len > UINT8_MAX) {
    fprintf(stderr, "Tle list is too long (len=%d).\n", list_len);
    exit(EXIT_FAILURE);

  } else if (list_len == 1) {
    op_ri(op, src1, list_data[0], dest);

  } else {
    store(COMMAND_OP_WREG_LIST_TO_WREG);
    store(op);
    store(src1);
    store(list_len);
    while (list_len--) {
      store_word(*list_data++);
    }
    store(dest);
  }
}

void increment(wreg_t dest) {
  op_ri(dest, dest, OP_ADD, 1);
}

void decrement(wreg_t dest) {
  op_ri(dest, dest, OP_SUB, 1);
}

static void store_message(char *msg) {
  // TODO: bounds check len.
  size_t len = strlen(msg);
  store(len);

  while (len--) {
    store(*msg++);
  }
}

static void store_message2(char *msg1, char *msg2) {
  // TODO: bounds check len.
  size_t len1 = strlen(msg1);
  size_t len2 = strlen(msg2);
  size_t len = len1 + len2;
  store(len);

  while (len1--) {
    store(*msg1++);
  }

  while (len2--) {
    store(*msg2++);
  }
}

static void start_shout(sboot_program_command_t command,
			sboot_shout_severity_t severity,
			sboot_shout_reason_t reason) {
  store(command);
  store(severity);
  store_uint16(reason);
}

void fail(sboot_shout_reason_t reason, char *msg) {
  start_shout(COMMAND_SHOUT, SBOOT_SHOUT_FAILURE, reason);
  store_message(msg);
}

void fail_value(sboot_shout_reason_t reason, char *msg, uint32 value) {
  start_shout(COMMAND_SHOUT_VALUE, SBOOT_SHOUT_FAILURE, reason);
  store_word(value);
  store_message(msg);
}

void fail_wreg(sboot_shout_reason_t reason, char *msg, wreg_t wreg) {
  start_shout(COMMAND_SHOUT_WREG, SBOOT_SHOUT_FAILURE, reason);
  store(wreg);
  store_message(msg);
}

void fail_field(sboot_shout_reason_t reason, char *msg, wreg_t wreg, uint32 mask) { 
  start_shout(COMMAND_SHOUT_WREG_FIELD, SBOOT_SHOUT_FAILURE, reason);
  store(wreg);
  store_word(mask);
  store_message(msg);
}

void fail_wreg_wreg(sboot_shout_reason_t reason, char *msg, wreg_t wreg1, wreg_t wreg2) {
  start_shout(COMMAND_SHOUT_WREG_WREG, SBOOT_SHOUT_FAILURE, reason);
  store(wreg1);
  store(wreg2);
  store_message(msg);
}

void fail_if_zero(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  start_shout(COMMAND_SHOUT_IF_ZERO, SBOOT_SHOUT_FAILURE, reason);
  store(test_wreg);
  store(show_wreg);
  store_message(msg);
}

void fail_if_nonzero(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  start_shout(COMMAND_SHOUT_IF_NONZERO, SBOOT_SHOUT_FAILURE, reason);
  store(test_wreg);
  store(show_wreg);
  store_message(msg);
}

void fail_if_false(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  fail_if_zero(reason, msg, test_wreg, show_wreg);
}

void fail_if_true(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  fail_if_nonzero(reason, msg, test_wreg, show_wreg);
}

void debug(char *msg) {
  start_shout(COMMAND_SHOUT, SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_NONE);
  store_message(msg);
}

void debug2(char *msg1, char *msg2) {
  start_shout(COMMAND_SHOUT, SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_NONE);
  store_message2(msg1, msg2);
}

void debug_value(char *msg, uint32 value) {
  start_shout(COMMAND_SHOUT_VALUE, SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_NONE);
  store_word(value);
  store_message(msg);
}

void debug_wreg(char *msg, wreg_t wreg) {
  start_shout(COMMAND_SHOUT_WREG, SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_NONE);
  store(wreg);
  store_message(msg);
}

void debug_wreg_field(char *msg, wreg_t wreg, uint32 mask) {
  start_shout(COMMAND_SHOUT_WREG_FIELD, SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_NONE);
  store(wreg);
  store_word(mask);
  store_message(msg);
}

void debug_wreg_wreg(char *msg, wreg_t wreg1, wreg_t wreg2) {
  start_shout(COMMAND_SHOUT_WREG_WREG, SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_NONE);
  store(wreg1);
  store(wreg2);
  store_message(msg);
}

void debug_if_zero(char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  start_shout(COMMAND_SHOUT_IF_ZERO, SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_NONE);
  store(test_wreg);
  store(show_wreg);
  store_message(msg);
}

void debug_if_nonzero(char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  start_shout(COMMAND_SHOUT_IF_NONZERO, SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_NONE);
  store(test_wreg);
  store(show_wreg);
  store_message(msg);
}

void debug_if_false(char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  debug_if_zero(msg, test_wreg, show_wreg);
}

void debug_if_true(char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  debug_if_nonzero(msg, test_wreg, show_wreg);
}

void shout(sboot_shout_reason_t reason, char *msg) {
  start_shout(COMMAND_SHOUT, SBOOT_SHOUT_MESSAGE, reason);
  store_message(msg);
}

void shout_value(sboot_shout_reason_t reason, char *msg, uint32 value) {
  start_shout(COMMAND_SHOUT_VALUE, SBOOT_SHOUT_MESSAGE, reason);
  store_word(value);
  store_message(msg);
}

void shout_wreg(sboot_shout_reason_t reason, char *msg, wreg_t wreg) {
  start_shout(COMMAND_SHOUT_WREG, SBOOT_SHOUT_MESSAGE, reason);
  store(wreg);
  store_message(msg);
}

void shout_wreg_field(sboot_shout_reason_t reason, char *msg, wreg_t wreg, uint32 mask) {
  start_shout(COMMAND_SHOUT_WREG_FIELD, SBOOT_SHOUT_MESSAGE, reason);
  store(wreg);
  store_word(mask);
  store_message(msg);
}

void shout_wreg_wreg(sboot_shout_reason_t reason, char *msg, wreg_t wreg1, wreg_t wreg2) {
  start_shout(COMMAND_SHOUT_WREG_WREG, SBOOT_SHOUT_MESSAGE, reason);
  store(wreg1);
  store(wreg2);
  store_message(msg);
}

void shout_if_zero(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  start_shout(COMMAND_SHOUT_IF_ZERO, SBOOT_SHOUT_MESSAGE, reason);
  store(test_wreg);
  store(show_wreg);
  store_message(msg);
}

void shout_if_nonzero(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  start_shout(COMMAND_SHOUT_IF_NONZERO, SBOOT_SHOUT_MESSAGE, reason);
  store(test_wreg);
  store(show_wreg);
  store_message(msg);
}

void shout_if_false(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  shout_if_zero(reason, msg, test_wreg, show_wreg);
}

void shout_if_true(sboot_shout_reason_t reason, char *msg, wreg_t test_wreg, wreg_t show_wreg) {
  shout_if_nonzero(reason, msg, test_wreg, show_wreg);
}

static struct label_struct labels[MAX_LABELS];
static int label_count = 0;

// Create a new jump or subroutine label (target):
static label_t new_label(char *name) {
  if (label_count >= MAX_LABELS) {
    fprintf(stderr, "Ran out of labels.\n");
    exit(EXIT_FAILURE);
  }

  label_t label = &(labels[label_count++]);
  label->label_type = INITIAL_LABEL;
  label->name = name;
  label->context = current_subr;
  return label;
}

// Create a new lable to be defined in the future.
label_t future_label(char *name) {
  return new_label(name);
}

// Define a label.
label_t label(label_t a_label) {
  switch (a_label->label_type) {
  case INITIAL_LABEL: {
    // The label was not used for a forward jump or a subr, so just
    // save the location in the sboot stream.
    a_label->jump_target = get_sboot_buffer_len();
    a_label->label_type = BACKWARD_LABEL;
    break;
  }

  case FORWARD_LABEL: {
    // The label was used for a forward jump, so
    // save the location in the sboot stream and fix
    // the forward reference.  The label may now be used
    // as a backward label.
    a_label->jump_target = get_sboot_buffer_len();
    for (int i = 0; i < a_label->fixup_count; i++) {
      sboot_size_t fixup_location = a_label->fixup_location[i];
      uint32 offset = a_label->jump_target - fixup_location;
      store_word_at(offset, fixup_location);
    }
    a_label->label_type = BACKWARD_LABEL;
    break;
  }

  case BACKWARD_LABEL: {
    fprintf(stderr, "sboot_encoding:  May not mark a label more than once.\n");
    exit(EXIT_FAILURE);
  }

  case INITIAL_SUBR_LABEL:
  case SUBR_LABEL: {
    fprintf(stderr, "sboot_encoding:  May not mark a subroutine label.\n");
    exit(EXIT_FAILURE);
  }

  default:
    fprintf(stderr, "sboot_encoding:  Marking an unknown label type %d.\n",
	    a_label->label_type);
    exit(EXIT_FAILURE);
  }

  return a_label;
}

// Check for Unresolved labels:
void check_for_unresolved_labels() {
  for (int label_num = 0; label_num < label_count; label_num++) {
    label_t a_label = &(labels[label_num]);
    switch (a_label->label_type) {
    case INITIAL_LABEL: {
      fprintf(stderr, "Unresolved initial label %d \"%s\"\n", label_num, a_label->name);
      exit(EXIT_FAILURE);
      break;
    }
      
    case FORWARD_LABEL: {
      fprintf(stderr, "Unresolved forward label %d \"%s\"\n", label_num, a_label->name);
      exit(EXIT_FAILURE);
      break;
    }
      
    case INITIAL_SUBR_LABEL: {
      fprintf(stderr, "Unresolved initial subr label %d \"%s\"\n", label_num, a_label->name);
      exit(EXIT_FAILURE);
      break;
    }

      // TODO: itemize the OK cases and add a default to catch internal errors.
    }
  }
}

// Generate a new label and define to at the current point in the sboot stream.
label_t current_label(char *name) {
  return label(new_label(name));
}

// TODO: jumps into subroutines or out of subroutines should be
// disallowed.
static void go_to_common(label_t label,
			 sboot_program_command_t forward_command,
#ifdef SBOOT_OPTIMIZE_JUMPS
			 sboot_program_command_t backward_command_uint8,
			 sboot_program_command_t backward_command_uint16,
			 sboot_program_command_t backward_command_uint24,
#endif // SBOOT_OPTIMIZE_JUMPS
			 sboot_program_command_t backward_command_uint32) {

  if (label->context != current_subr) {
    fprintf(stderr, "Attempted to go to a label from the wrong subroutine context\n");
    exit(EXIT_FAILURE);
  }

  switch (label->label_type) {
  case INITIAL_LABEL: {
    // The label was not used for a forward jump or a subr, so
    // turn it into a foward reference:

    label->label_type = FORWARD_LABEL;
    label->fixup_count = 1;
    store(forward_command);
    label->fixup_location[0] = get_sboot_buffer_len();
    store_word(0);
    break;
  }

  case FORWARD_LABEL: {
    // Add another fixup.
    if (label->fixup_count >= MAX_LABEL_FIXUPS) {
      // TODO: Identify the label.
      // TODO: print MAX_LABEL_FIXUPS
      fprintf(stderr, "sboot_encoding:  Too many forward references for the same label.\n");
      exit(EXIT_FAILURE);
    }
    int idx = label->fixup_count++;

    store(forward_command);
    label->fixup_location[idx] = get_sboot_buffer_len();
    store_word(0);
    break;
  }

  case BACKWARD_LABEL: {
    sboot_size_t command_offset = get_sboot_buffer_len();
    store(backward_command_uint32); // Store a dummy command.
    uint32 offset = get_sboot_buffer_len() - label->jump_target;
#ifdef SBOOT_OPTIMIZE_JUMPS
    if (offset <= UINT8_MAX) {
      store_at(backward_command_uint8, command_offset);
      store((uint8)offset);
    } else if (offset <= UINT16_MAX) {
      store_at(backward_command_uint16, command_offset);
      store_uint16((uint16)offset);
    } else if (offset <= UINT24_MAX) {
      store_at(backward_command_uint24, command_offset);
      store_uint24(offset);
    } else {
      // No need to actually overwrite the command
      // store_at(backward_command_uint32, command_offset);
      store_word(offset);
    }
#else // SBOOT_OPTIMIZE_JUMPS
    store_word(offset);
#endif // SBOOT_OPTIMIZE_JUMPS
    break;
  }

  case INITIAL_SUBR_LABEL:
  case SUBR_LABEL: {
    fprintf(stderr, "sboot_encoding:  May not go to a subroutine label.\n");
    exit(EXIT_FAILURE);
  }

  default:
    fprintf(stderr, "sboot_encoding:  Going to an unknown label type %d.\n", label->label_type);
    exit(EXIT_FAILURE);
  }
}

label_t go_to(label_t label) {
  go_to_common(label, COMMAND_JUMP_FORWARD,
#ifdef SBOOT_OPTIMIZE_JUMPS
	       COMMAND_JUMP_BACKWARD_UINT8,
	       COMMAND_JUMP_BACKWARD_UINT16,
	       COMMAND_JUMP_BACKWARD_UINT24,
#endif // SBOOT_OPTIMIZE_JUMPS
	       COMMAND_JUMP_BACKWARD_UINT32);
  return label;
}

label_t go_to_if_zero(label_t label, wreg_t wreg) {
  go_to_common(label, COMMAND_JUMP_FORWARD_IF_ZERO,
#ifdef SBOOT_OPTIMIZE_JUMPS
	       COMMAND_JUMP_BACKWARD_IF_ZERO_UINT8,
	       COMMAND_JUMP_BACKWARD_IF_ZERO_UINT16,
	       COMMAND_JUMP_BACKWARD_IF_ZERO_UINT24,
#endif // SBOOT_OPTIMIZE_JUMPS
	       COMMAND_JUMP_BACKWARD_IF_ZERO_UINT32);
  store(wreg);
  return label;
}

label_t go_to_if_nonzero(label_t label, wreg_t wreg) {
  go_to_common(label, COMMAND_JUMP_FORWARD_IF_NONZERO,
#ifdef SBOOT_OPTIMIZE_JUMPS
	       COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT8,
	       COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT16,
	       COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT24,
#endif // SBOOT_OPTIMIZE_JUMPS
	       COMMAND_JUMP_BACKWARD_IF_NONZERO_UINT32);
  store(wreg);
  return label;
}

label_t go_to_if_false(label_t label, wreg_t wreg) {
  go_to_if_zero(label, wreg);
  return label;
}

label_t go_to_if_true(label_t label, wreg_t wreg) {
  go_to_if_nonzero(label, wreg);
  return label;
}

// TODO: If generating verbose messages, should track the current subroutine
// name so return can print it.
label_t begin_subr(char *name) {
  if (verbose) {
    printf("Defining subroutine %s()\n", name);
    debug2("Defining subr: ", name);
  }

  // Check if we are about to exceed the static subroutine nesting depth:
  if (subr_definition_stack_len >= SBOOT_SUBR_DEFINITION_MAX_DEPTH) {
    fprintf(stderr, "sboot_encoding: too many nested subroutines (%s).\n",
	    name);
    exit(EXIT_FAILURE);
  }

  // We're beginning a new subroutine.
  current_subr = new_label(name);
  // TODO: could remove the following check:
  if (current_subr->label_type != INITIAL_LABEL) {
    fprintf(stderr, "sboot_encoding:  May not begin a subr with a dirty label.\n");
    exit(EXIT_FAILURE);
  }
  current_subr->label_type = INITIAL_SUBR_LABEL;
  subr_definition_stack[subr_definition_stack_len++] = current_subr;

  // Generate a jump around the body of the subroutine.
  store(COMMAND_JUMP_FORWARD);
  current_subr->fixup_location[0] = get_sboot_buffer_len();
  store_word(0);

  // The subroutine proper starts here.
  current_subr->jump_target = get_sboot_buffer_len();

  if (verbose) {
    debug2("Calling subr: ", name);
  }

  return current_subr;
}

void subr_return() {
  if (current_subr == 0) {
    fprintf(stderr, "sboot_encoding:  Attempt to return from a subroutine when not in one.\n");
    exit(EXIT_FAILURE);
  }
  store(COMMAND_RETURN);
}

void end_subr(label_t subr_label) {
  if (subr_definition_stack_len == 0) {
    fprintf(stderr, "sboot_encoding:  May not end a subr when not in one (%s).\n",
	    subr_label->name);
    exit(EXIT_FAILURE);
  }
  label_t label = subr_definition_stack[--subr_definition_stack_len];
  if (label != subr_label) {
    fprintf(stderr, "sboot_encoding: Subroutine nesting error:  ending %d, expected %s\n",
	    subr_label->name, label->name);
  }

  label_type_t label_type = label->label_type;
  if (label_type == INITIAL_LABEL) {
    fprintf(stderr, "sboot_encoding:  May not end a subr before it is begun (%s).\n",
	    label->name);
    exit(EXIT_FAILURE);
  } else if (label_type == SUBR_LABEL) {
    fprintf(stderr, "sboot_encoding:  May not end a subr more than once (%s)).\n",
	    label->name);
    exit(EXIT_FAILURE);
  } else if (label_type != INITIAL_SUBR_LABEL) {
    fprintf(stderr, "sboot_encoding:  May not end a non-subr label (%s).\n",
	    label->name);
    exit(EXIT_FAILURE);
  }

  if (verbose) {
    printf("  Ending subroutine %s()\n", label->name);
  }

  // Generate a return in case we fall off the end of the subroutine.
  //
  // TODO: Need a more general returning-from-subr message.
  if (verbose) {
    debug2("Returning from subr: ", label->name);
  }
  store(COMMAND_RETURN);

  // Fix up the jump around the body of the subroutine.
  sboot_size_t fixup_location = label->fixup_location[0];
  uint32 offset = get_sboot_buffer_len() - fixup_location;
  store_word_at(offset, fixup_location);

  // The subroutine label is now complete.
  label->label_type = SUBR_LABEL;

  // Set the current subroutine context.
  if (subr_definition_stack_len > 0) {
    current_subr = subr_definition_stack[subr_definition_stack_len - 1];
  } else {
    current_subr = 0;
  }
}

void call_subr(label_t label) {
  label_type_t label_type = label->label_type;
  if (label_type == INITIAL_LABEL) {
    fprintf(stderr, "sboot_encoding:  May not call a subr before it is begun.\n");
    exit(EXIT_FAILURE);
  } else if (label_type == INITIAL_SUBR_LABEL) {
    fprintf(stderr, "sboot_encoding:  May not call a subr before it has ended.\n");
    exit(EXIT_FAILURE);
  } else if (label_type != SUBR_LABEL) {
    fprintf(stderr, "sboot_encoding:  May not call a non-subr label.\n");
    exit(EXIT_FAILURE);
  }

  sboot_size_t command_offset = get_sboot_buffer_len();
  store(COMMAND_CALL_UINT32); // Store a dummy command.
  uint32 offset = get_sboot_buffer_len() - label->jump_target;
#ifdef SBOOT_OPTIMIZE_CALLS
  if (offset <= UINT8_MAX) {
    store_at(COMMAND_CALL_UINT8, command_offset);
    store((uint8)offset);
  } else if (offset <= UINT16_MAX) {
    store_at(COMMAND_CALL_UINT16, command_offset);
    store_uint16((uint16)offset);
  } else if (offset <= UINT24_MAX) {
    store_at(COMMAND_CALL_UINT24, command_offset);
    store_uint24(offset);
  } else {
    // No need to actually overwrite the command
    // store_at(COMMAND_CALL_UINT32, command_offset);
    store_word(offset);
  }
#else // SBOOT_OPTIMIZE_CALLS
  store_word(offset);
#endif // SBOOT_OPTIMIZE_CALLS
}

void get_current_seconds(wreg_t wreg) {
  store(COMMAND_GET_CURRENT_SECONDS);
  store(wreg);
}

void assemble_string_msg(wreg_t four_bytes, wreg_t sender_x, wreg_t sender_y) {
  store(COMMAND_ASSEMBLE_STRING_MSG);
  store(four_bytes);
  store(sender_x);
  store(sender_y);
}

void all_done() {
  store(COMMAND_DONE);
}

void note_boot_pass_succeeded() {
  store(COMMAND_NOTE_BOOT_PASS_SUCCEEDED);
}

void note_boot_pass_failed(wreg_t result_vector0_wreg, wreg_t result_vector1_wreg) {
  store(COMMAND_NOTE_BOOT_PASS_FAILED);
  store(result_vector0_wreg);
  store(result_vector1_wreg);
}

void note_boot_done() {
  store(COMMAND_NOTE_BOOT_DONE);
}

// If the test fails, jump to the returned label.  This allows the
// use of convenient structures such as:
//
// label_t after_boot_pass_msg_check =
//   if_ri(WREG_MSG_TYPE, OP_EQ, BOOT_PASS_MSG); {
//   debug("DBG: watchfor message: target_msg == self.BOOT_PASS_MSG");
// }
// label(after_boot_pass_msg_check);
//
static label_t begin_if(char *id) {
  // Check if we are about to exceed the "if" nesting depth:
  if (if_stack_len >= SBOOT_IF_STACK_MAX_DEPTH) {
    fprintf(stderr, "sboot_encoding: too many nested 'if' statements at %s.\n", id);
    exit(EXIT_FAILURE);
  }

  label_t label = future_label(id);
  if_stack[if_stack_len++] = label;
  return label;
}

label_t if_ri(wreg_t wreg, sboot_op_t op, uint32 value, char *id) {
  label_t label = begin_if(id);
  op_ri(WREG_TEST, wreg, op, value);
  go_to_if_false(label, WREG_TEST);
  return label;
}

label_t if_rr(wreg_t wreg1, sboot_op_t op, wreg_t wreg2, char *id) {
  label_t label = begin_if(id);
  op_rr(WREG_TEST, wreg1, op, wreg2);
  go_to_if_false(label, WREG_TEST);
  return label;
}

label_t if_rl(wreg_t wreg, sboot_op_t op,
	      uint32 *list_data, list_len_t list_len,
	      char *id) {
  label_t label = begin_if(id);
  op_rl(WREG_TEST, wreg, op, list_data, list_len);
  go_to_if_false(label, WREG_TEST);
  return label;
}

label_t if_true(wreg_t wreg, char *id) {
  label_t label = begin_if(id);
  go_to_if_false(label, wreg);
  return label;
}

label_t if_false(wreg_t wreg, char *id) {
  label_t label = begin_if(id);
  go_to_if_true(label, wreg);
  return label;
}

label_t if_zero(wreg_t wreg, char *id) {
  label_t label = begin_if(id);
  go_to_if_nonzero(label, wreg);
  return label;
}

label_t if_nonzero(wreg_t wreg, char *id) {
  label_t label = begin_if(id);
  go_to_if_zero(label, wreg);
  return label;
}

void end_if(char *id) {
  if (if_stack_len <= 0) {
    fprintf(stderr, "sboot_encoding: attempt to end an 'if' when not in one (%s).\n",
	    id);
    exit(EXIT_FAILURE);
  }

  label_t if_label = if_stack[--if_stack_len];
  if (strcmp(id, if_label->name) != 0) {
    fprintf(stderr, "sboot_encoding: attempt to end if(\"%s\") with end_if(\"%s\").\n",
	    if_label->name, id);
    exit(EXIT_FAILURE);
  }

  label(if_label);
}

// Generate program info.  This could be split into a separate file.
//
// Usage:
// begin_program_info();
// store_program_info_xxx(...);
// ...
// end_program_info();
//
// ... generate the program
//
// finish_program_info();
//
// TODO: I have second thoughts about carrying program length and CRC in
// INFO commands.  Maybe it would be smarter to make them be a fixed-length
// header before the start of the program.

static void store_info_header(sboot_program_info_t info_type, uint8 info_length) {
  store(COMMAND_PROGRAM_INFO);
  store(info_type);
  store(info_length);
}

// Reserve four bytes (zeroed) for the length of the final program,
// including all INFO headers.  Later, but before computing the CRC,
// we'll fill in the actual length.
//
// TODO: Makes the assumption that sboot_size_t is uint32.
void reserve_program_info_length() {
  store_info_header(SBOOT_INFO_LENGTH, sizeof(sboot_size_t));
  program_info_length_target = sboot_buffer_len;
  store_word(0); // Dummy value, will be updated later.
}

// TODO: Makes the assumption that sboot_size_t is uint32.
static void update_program_info_length() {
  // fprintf(stderr, "update_program_info_length program_info_length_target=%d sboot_buffer_len=%d\n",
  //	  program_info_length_target, sboot_buffer_len);
  if (program_info_length_target == 0) {
    fprintf(stderr, "sboot_encoding: attempt to update the program info length without a reservation\n");
    exit(EXIT_FAILURE);
  }
  store_word_at(sboot_buffer_len, program_info_length_target);
}

// Reserve 2 bytes (zeroed) for a CRC.  Later, after everything else is generated,
// including setting the length in its INFO header, we'll calculate a CRC and store
// it.
void reserve_program_info_crc() {
  store_info_header(SBOOT_INFO_CRC, sizeof(uint16));
  program_info_crc_target = sboot_buffer_len;
  store_uint16(0); // Initial value, will be updated later.
}

// Calculate a CRC from the end of this INFO header to the end of
// the program.  It would be a good idea to put this header first
// in the program.
static void update_program_info_crc() {
  if (program_info_crc_target == 0) {
    fprintf(stderr, "sboot_encoding: attempt to update the program info CRC without a reservation\n");
    exit(EXIT_FAILURE);
  }
  
  // The following code was vilely stolen from "crc.c", then
  // modified to use local types:
  uint16 crcval;
  crcval = 0xffff; // CCITT CRC                                                                                                                                                 
  sboot_size_t i;
  for (i = (program_info_crc_target + sizeof(uint16)); i < sboot_buffer_len; i++) {
    uint16 t;
    t = crcval ^ (uint16)sboot_buffer[i];
    t = (t ^ (t << 4)) & 0xff;
    crcval = (crcval >> 8) ^ (t << 8) ^ (t << 3) ^ (t >> 4);
  }
  crcval = ~crcval;

  store_uint16_at(crcval, program_info_crc_target);
}

void begin_program_info() {
  if (sboot_buffer_len != 0) {
    fprintf(stderr, "sboot_encoding: begin_program_info called when sboot_buffer_len=%d\n",
	    sboot_buffer_len);
    exit(EXIT_FAILURE);
  }
  reserve_program_info_crc();
  reserve_program_info_length();
  //  fprintf(stderr, "program_info_length_target=%d, program_info_crc_target=%d\n",
  //	  program_info_length_target, program_info_crc_target);
}

static void store_program_info_string(sboot_program_info_t info_type, char *str) {
  int len = strlen(str);
  if (len > 255) {
    fprintf(stderr, "sboot_encoding: info %d string length is %d, must not exceed 255.\n",
	    info_type, len);
    exit(EXIT_FAILURE);
  }
  store_info_header(info_type, len);
  for (int i = 0; i < len; i++) {
    store(str[i]);
  }
}

void store_program_info_test_name(char *test_name) {
  store_program_info_string(SBOOT_INFO_TEST_NAME, test_name);
}

void store_program_info_test_type(char *test_type) {
  store_program_info_string(SBOOT_INFO_TEST_TYPE, test_type);
}

void store_program_info_l1_file_name(char *filename) {
  store_program_info_string(SBOOT_INFO_L1_FILE_NAME, filename);
}

void store_program_info_l2_file_name(char *filename) {
  store_program_info_string(SBOOT_INFO_L2_FILE_NAME, filename);
}

void store_program_info_sboot_creation_by(char *username) {
  store_program_info_string(SBOOT_INFO_SBOOT_CREATION_BY, username);
}

void store_program_info_sboot_creation_host(char *username) {
  store_program_info_string(SBOOT_INFO_SBOOT_CREATION_HOST, username);
}

static void store_program_info_time(sboot_program_info_t info_type, time_t sometime) {
  char mytime[16]; // YYYYMMDD.HHMMSS and trailing NULL

  struct tm sometm;
  gmtime_r(&sometime, &sometm);
  sprintf(mytime, "%04d%02d%02d.%02d%02d%02d",
	  sometm.tm_year + 1900,
	  sometm.tm_mon + 1,
	  sometm.tm_mday,
	  sometm.tm_hour,
	  sometm.tm_min,
	  sometm.tm_sec);
  store_program_info_string(info_type, mytime);
}

void store_program_info_sboot_creation_time(time_t sometime) {
  store_program_info_time(SBOOT_INFO_SBOOT_CREATION_TIME, sometime);
}

void store_program_info_l1_file_time(time_t sometime) {
  store_program_info_time(SBOOT_INFO_L1_FILE_TIME, sometime);
}

void store_program_info_l2_file_time(time_t sometime) {
  store_program_info_time(SBOOT_INFO_L2_FILE_TIME, sometime);
}

void end_program_info() {
  store_info_header(SBOOT_INFO_END, 0);
}

void finish_program_info() {
  update_program_info_length();
  update_program_info_crc();
}

void read_program_byte(wreg_t dest_wreg,
		       wreg_t src_program_number_wreg,
		       wreg_t src_offset_wreg) {
  store(COMMAND_READ_PROGRAM_BYTE);
  store(src_program_number_wreg);
  store(src_offset_wreg);
  store(dest_wreg);
}

void get_number_of_programs(wreg_t dest_wreg) {
  store(COMMAND_GET_NUMBER_OF_PROGRAMS);
  store(dest_wreg);
}

void get_first_program_number(wreg_t dest_wreg) {
  store(COMMAND_GET_FIRST_PROGRAM_NUMBER);
  store(dest_wreg);
}

void validate_program_number(wreg_t dest_wreg, wreg_t src_program_number_wreg) {
  store(COMMAND_VALIDATE_PROGRAM_NUMBER);
  store(src_program_number_wreg);
  store(dest_wreg);
}

// Update the CRC WREG by applying the CRC kernel calculation
// to the byte in the byte WREG.
void crc_kernel(wreg_t crc_wreg, wreg_t byte_wreg) {
  store(COMMAND_CRC_KERNEL);
  store(crc_wreg);
  store(byte_wreg);
}

// Reconfigure the UART to the MAESTRO.
void change_uart_speed(bool high_speed) {
  store(COMMAND_CHANGE_UART_SPEED);
  store(high_speed);
}
