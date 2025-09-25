// create_sboot.c  21-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

/*
 * This is a test harness for a serial bootstrapper for the Maestro chip.
 * This program takes one or two Maestro (Tilera49) bootstrap files (level 1
 * boot and the optional level 2 boot) and packages them for execution by
 * the simplified serial bootstrapper.
 */

/*
 * This file contains constants relaled to the Tilera interfaces used
 * in the Maestro (Tilera49) chip.  Some constant names and values that appear
 * here were extracted from the file "boot.py" and "config.py", which contained
 * the following copyright notice:
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

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "../includes/sboot_enable_maestro_phase.h"
#include "../includes/sboot_encoding.h"
#include "../includes/sboot_program.h"
#include "../includes/sboot_program_info.h"
#include "../includes/sboot_types.h"
#include "../includes/sboot_op.h"

#include "create_sboot.h"

#include "../mdb/boot.h"
#include "../mdb/config.h"
#include "../mdb/msh.h"
#include "../mdb/rshim.h"

static uint8 get_message_timeout;
static uint16 program_timeout;
static uint8 read_timeout;
static uint8 skip_read_timeout;
static uint8 skip_program_timeout;
static char *test_name;
static char *l1_filename;
static FILE *l1_file;
static char *l2_filename;
static FILE *l2_file;
static char *sboot_filename;
static FILE *sboot_file;
static char *test_type;
static bool print_pass;
static bool print_results;
static bool verbose;
static bool high_speed_uart;
static bool do_check_uart_divisor_low_speed;
static bool do_check_uart_divisor_high_speed;
static bool reset_before_divisor_check;
static uint8 reset_seconds;
static bool do_enable_maestro;
static uint8 maestro_post_reset_delay;
static uint8 maestro_power_on_delay;
static uint8 maestro_uart_configuration_delay;

// The following values were taken from "boot.py"
// Northwest corner of rectangle to boot
static uint8 nw_corner_x = BOOT_NW_CORNER_X;
static uint8 nw_corner_y = BOOT_NW_CORNER_Y;

// Southeast corner of rectangle to boot
static uint8 se_corner_x = BOOT_SE_CORNER_X;
static uint8 se_corner_y = BOOT_SE_CORNER_Y;

// Location of Mshim for L2 boot
static uint8 mshim_x = BOOT_MSHIM_X;
static uint8 mshim_y = BOOT_MSHIM_Y;

// Default address for starting L2 tests
static uint32 boot_start_address = BOOT_START_ADDRESS;

// The following values were taken from "config.py".
static uint8 boot_master_x = BOOT_MASTER_X;
static uint8 boot_master_y = BOOT_MASTER_Y;

// The following values were taken from "boot.py".
static bool change_pll_freq = BOOT_CHANGE_PLL_FREQ;
static bool sync_on_dn_ready = BOOT_SYNC_ON_DN_READY;
                                                                                                                                                 
// The following values were taken from "boot.py".
static uint32 boot_test_mask_low = BOOT_TEST_MASK_LOW;
static uint32 boot_test_mask_high = BOOT_TEST_MASK_HIGH;
static bool run_boot_test_quiet = RUN_BOOT_TEST_QUIET;
static bool use_timeout_for_boot_test = USE_TIMEOUT_FOR_BOOT_TEST;

// The following values were taken from "boot.py".
static uint8 post_code_load_wait = BOOT_POST_CODE_LOAD_WAIT; // seconds
static uint8 post_boot_arg_wait = BOOT_POST_BOOT_ARG_WAIT; // seconds

// Ths following value was taken from "boot.py":
static bool stripe = FALSE; // TODO: define a default value.

// The following values were taken from "boot.py".
static uint32 diag_mux_control;
static uint32 diag_trace_control;
static uint32 diag_trace_way;
static uint32 diag_watch_control;
static uint32 diag_watch_val;
static uint32 diag_watch_mask;
static uint32 diag_bcast_control;
static uint32 diag_bcast_mask;

// A config object, mirroring the code in "config.py" abbd "config_mdg.py":
static config_t config;

// Turn off diagnostic tracing (boot args will leave SPRs at their reset value)
// The following code was extracted from "boot.py":
void diag_trace_off() {
  // Diag Boot Args = just default to their reset value here                                                                                                                 
  diag_mux_control   = 0x415;
  diag_trace_control = 0; // Not actually reset value - turn off all diag enables and sampling                                                                          
  diag_trace_way     = 8; // Not actually reset value - but my value is better (use way 3, reset value is way 0)                                                        
  diag_watch_control = 0;
  diag_watch_val     = 0;
  diag_watch_mask    = 0;
  diag_bcast_control = 0;
  diag_bcast_mask    = 0;
}

// Just display all the vars that control boot stuff
// The following code was extracted from "boot.py":
void display_config() {
  fprintf(stderr, "Boot Master      X Y = %d %d\n", boot_master_x, boot_master_y);
  fprintf(stderr, "NorthWest Corner X Y = %d %d\n", nw_corner_x, nw_corner_y);
  fprintf(stderr, "SouthEast Corner X Y = %d %d\n", se_corner_x, se_corner_y);
  fprintf(stderr, "Mshim Location   X Y = %d %d\n", mshim_x, mshim_y);
  fprintf(stderr, "Level 2 code Start Address 0x%X\n", boot_start_address);
}

// The following code was extracted from "boot.py":
void check_rectangle() {
  // "Verify that the specified boot master and rectangle to boot are sane"
  if (nw_corner_x > se_corner_x) {
    fprintf(stderr, "ERROR: Bad Boot rectangle specified - Bad X value for corners\n");
    display_config();
    exit(EXIT_FAILURE);
  }

  if (nw_corner_y > se_corner_y) {
    fprintf(stderr, "ERROR: Bad Boot rectangle specified - Bad Y value for corners\n");
    display_config();
    exit(EXIT_FAILURE);
  }

  // On TLR chip, boot master must be on west edge                                                                                                                           
  // ***CMR* In "boot.py", this constraint was applied to all
  // chip types.  Is this correct?
  // TODO: Research if this constraint applies to MAESTRO.
  if (boot_master_x != nw_corner_x) {
    fprintf(stderr, "ERROR: Boot Master must be on West edge\n");
    display_config();
    exit(EXIT_FAILURE);
  }

  // Check that boot master is within rectangle                                                                                                                              
  if (boot_master_x < nw_corner_x) {
    fprintf(stderr, "ERROR: Boot Master X dimension is outside NW corner\n");
    display_config();
    exit(EXIT_FAILURE);
  }
  if (boot_master_y < nw_corner_y) {
    fprintf(stderr, "ERROR: Boot Master Y dimension is outside NW corner\n");
    display_config();
    exit(EXIT_FAILURE);
  }
  if (boot_master_x > se_corner_x) {
    fprintf(stderr, "ERROR: Boot Master X dimension is outside SE corner\n");
    display_config();
    exit(EXIT_FAILURE);
  }

  if (boot_master_y > se_corner_y) {
    fprintf(stderr, "ERROR: Boot Master Y dimension is outside SE corner\n");
    display_config();
    exit(EXIT_FAILURE);
  }
}

// This code was taken from gen_file_words(...) in "boot.py".  The Python
// version returned a list of words, but this version deals directly with
// a buffer of bytes in little endian order.
//
// This routine malloc's a data buffer, the caller should free it when done.
uint32 gen_file_words(char *filename, FILE *binfile,
		      bool is_l2_file,  bool is_dv_l1_file,
		      uint8 **bufptr) {
  struct stat statbuf;
  if (fstat(fileno(binfile), &statbuf)) {
    perror(filename);
    exit(EXIT_FAILURE);
  }
  off_t filesize = statbuf.st_size;

  int alignment_in_words = 16;
  uint32 alignment_mask = 0xf;
  int extra_bytes_for_padding = (alignment_in_words * BYTES_PER_WORD);

  // TODO: Don't need the extra padding when is_l2_file is true or is_dv_l1_file is false.
  uint8 *bufp = malloc(filesize + extra_bytes_for_padding);
  if (bufp == 0) {
    perror("Error in malloc");
    exit(EXIT_FAILURE);
  }
  *bufptr = bufp;

  for (off_t i = 0; i < filesize; i++) {
    int c = fgetc(binfile);// TODO: check for error.
    if (c == EOF) {
      fprintf(stderr, "Premature EOF reading %s\n", filename);
      exit(EXIT_FAILURE);
    }
    bufp[i] = c;
  }
  return filesize;

  if(is_l2_file) {
    // is_l2_file param indicates if this binary is a level 2 boot/test file.                                                                                              
    // If it is, we need to delete the final 2 words since tile-mkrom appends boot args we don't want here.                                                              
    filesize -= 8;

  } else if (is_dv_l1_file) {
    // This constant is a guess, based on observation.  There is
    // no guarantee that it is, in fact, constant.  The python
    // code did not perform a check of actual file length against
    // expected file length check.  It calculated  the data
    // alignment using the expected size, but then used the actual
    // size to perform the adjustment.
    //
    // TODO: Verify that this header length is constant.  If the
    // header length is not constant, or cannot be calculated
    // (perhaps there may be multiple segments in the file, or
    // something like that), switch to the python code's method
    // of adding padding.
    uint32 l1boot_header_len_in_bytes = 12;

    // The first word of the file contains its expected size in words.
    uint32 expected_size_in_words =
      bufp[0] |
      bufp[1] << 8 |
      bufp[2] << 16 |
      bufp[3] << 24;

    uint32 expected_size_in_bytes = expected_size_in_words * BYTES_PER_WORD;

    // The expected size should match the size from the operating system,
    // adjusted for the header length.
    if (expected_size_in_bytes != (filesize - l1boot_header_len_in_bytes)) {
      fprintf(stderr, "Error in expected size in %s: stat reports %d bytes, file header reports %d bytes\n",
	      filename, filesize, expected_size_in_bytes);
      exit(EXIT_FAILURE);
    }

    // Size of data section must be 16 word aligned.  Pad the end with zeros if it is not.
    uint32 misalignment_in_words = expected_size_in_words & alignment_mask;
    if (misalignment_in_words != 0) {
      uint32 extra_words_needed = alignment_in_words - misalignment_in_words;
      uint32 extra_bytes_needed = extra_words_needed * BYTES_PER_WORD;
      uint32 aligned_size_in_words = expected_size_in_words + extra_words_needed;
      uint32 aligned_size_in_bytes = aligned_size_in_words * BYTES_PER_WORD;

      // Update the internal size word:
      bufp[0] = aligned_size_in_words & 0xff;
      bufp[1] = (aligned_size_in_words >> 8) & 0xff;
      bufp[2] = (aligned_size_in_words >> 16) & 0xff;
      bufp[3] = (aligned_size_in_words >> 24) & 0xff;

      // Save the final word, the jump address:
      uint8 jump0 = bufp[filesize - 4];
      uint8 jump1 = bufp[filesize - 3];
      uint8 jump2 = bufp[filesize - 2];
      uint8 jump3 = bufp[filesize - 1];

      // TODO: verify that the padding is correct!
      // pad with 0's:
      for (uint32 idx = filesize - BYTES_PER_WORD;
	   idx < filesize + extra_bytes_needed - BYTES_PER_WORD; idx++) {
	bufp[idx] = 0;
      }

      // Rewrite the jump address at the end:
      bufp[filesize + extra_bytes_needed - 4] = jump0;
      bufp[filesize + extra_bytes_needed - 3] = jump1;
      bufp[filesize + extra_bytes_needed - 2] = jump2;
      bufp[filesize + extra_bytes_needed - 1] = jump3;
    }
  }
  return filesize;
}

#define BOOT_ARGS_BUFFER_LEN 100
static uint32 bootargs[BOOT_ARGS_BUFFER_LEN];
static int bootargs_len;

static void init_bootargs() {
  bootargs_len = 0;
}

static void append_bootarg(uint32 value) {
  if (bootargs_len >= BOOT_ARGS_BUFFER_LEN) {
    fprintf(stderr, "Boot args buffer overflowed.\n");
    exit(EXIT_FAILURE);
  }
  bootargs[bootargs_len++] = value;
}

// These are the first boot args passed to the boot code.
// They are consumed by the boot master and then forwarded to all other tiles.
// Code taken from gen_boot_args(...) in "boot.py":
static int gen_boot_args0() {
  init_bootargs();

  if (verbose) {
    debug_value("rshim X = ", RSHIM_XLOC);
    debug_value("rshim Y = ", RSHIM_YLOC);
    debug_value("se corner X = ", se_corner_x);
    debug_value("se corner Y = ", se_corner_y);
    debug_value("nw corner X = ", nw_corner_x);
    debug_value("nw corner Y = ", nw_corner_y);
    debug_value("boot master X = ", boot_master_x);
    debug_value("boot master Y = ", boot_master_y);
  }
  append_bootarg(0xffffffff); // First boot arg is always "-1" so boot master can identify himself                                                                  
  append_bootarg((RSHIM_XLOC << 28) |
		 (RSHIM_YLOC << 24) |
		 (se_corner_x << 20) |
		 (se_corner_y << 16) |
		 (nw_corner_x << 12) |
		 (nw_corner_y << 8) |
		 (boot_master_x << 4) |
		 (boot_master_y << 0));
  
  // Push the boot arg that is the value for the big endian SPR.
  // Note that we push this value for both Tile64 and Wachusett
  // even though Tile64 does not have this SPR.  The value is simply
  // not used by boot code assembled for Tile64.
  append_bootarg(0); // Not big-endian.

  // Diag Tracing boot args                                                                                                                                         
  append_bootarg(diag_mux_control);
  append_bootarg(diag_trace_control);
  append_bootarg(diag_watch_control);
  append_bootarg(diag_watch_val);
  append_bootarg(diag_watch_mask);
  append_bootarg(diag_bcast_control);
  append_bootarg(diag_bcast_mask);

  return bootargs_len;
}

// These boot args are consumed by the boot master and NOT forwarded to the other tiles.
// The last argument in this phase indicates whether or not a sync with the
// external host will be performed after the dynamic network is known to be up
// Code taken from gen_boot_args(...) in "boot.py":
static int gen_boot_args1() {
  init_bootargs();

  // UART divisor boot arg
  // If the system allows UART access after reset, leave the UART divisor alone (pass boot arg of zero).
  // However, it if does not, need the tile code to fix the uart divisor and/or protocol mode so we have rshim access.                                               
  // ...
  // <BOOTARG 11>                                                                                                                                                     
  // <JSB 20100611>
  //
  // Since we are booting over the UART for ITC, prevent the boot code from
  // changing UART settings.
  append_bootarg(0);
  //</JSB>

  // Let the chip know if we want the dynamic network ready sync message.
  // <BOOTARG 12>
  if (sync_on_dn_ready) {
    append_bootarg(1);
  } else {
    append_bootarg(0);
  }

  return bootargs_len;
}

// This is the last set of boot args.
// They are consumed by the boot master and NOT forwarded to the other tiles.
// Code taken from gen_boot_args(...) in "boot.py":
static int gen_boot_args2() {
  init_bootargs();


  // cclk PLL programming value
  uint32 pll_value = 0;
  if (change_pll_freq) {
    // [pll_r, pll_q, pll_n, pll_m] = self.tlr.compute_cclk_pll_parameters(self.config.cclk_freq, .005, self.config.rclk_rate)
    // pll_value = 1 | (pll_r << 1) | (pll_q << 4) | ( pll_n << 7) | (pll_m << 13)
    // print "Setting cclk to %dMHz based on ref clock of %dMHz; m = %d, n = %d, q = %d, r = %d" % (self.config.cclk_freq, self.config.rclk_rate, pll_m, pll_n, pll_q, pll_r)
    fprintf(stderr, "Not prepared to change the PLL value.\n");
    exit(EXIT_FAILURE);
  }

  // <BOOTARG 13>                                                                                                                                                     
  append_bootarg(pll_value);

  // PCIe 0 config write boot arg
  // <BOOTARG 14>
  append_bootarg(0); // No PCI on MDB

  // PCIe 1 config write boot arg
  // <BOOTARG 15>
  append_bootarg(0); // No PCI on MDB

  // Pass masks that indicate which tiles should run the boot test
  // <BOOTARGS 16-17>
  append_bootarg(boot_test_mask_low);
  append_bootarg(boot_test_mask_high);

  // Pass boot test control word
  uint32 boot_test_control = 0;
  if (run_boot_test_quiet) {
    boot_test_control |= 1;
  }
  if (use_timeout_for_boot_test) {
    boot_test_control |= 2;
  }

  // <BOOTARGS 18>
  append_bootarg(boot_test_control);

  return bootargs_len;
}


// "Push a boot argument to the level 1 boot code"
void boot_arg_push(uint32 value) {
  stn_push(value);
  if (post_boot_arg_wait != 0) {
    wait(post_boot_arg_wait);
  }
}

// "Read rshim baseline status and check the chip_rev field against the user specified chip rev."
void check_chip_rev() {

  if (verbose) {
    debug("DBG boot.l1_load: checking chip rev");
  }

  // Read the rshim register containing the chip rev
  read_word(WREG0, /* = */ RSHIM_BASELINE_STAT, RSHIM_CHANNEL_RSHIM);

  // chip_rev = (status_value >> 16) & 0xff
  op_ri(WREG0, /* = */ WREG0, OP_SHIFT_RIGHT, 16);
  op_ri(WREG0, /* = */ WREG0, OP_AND_BITS, 0xff);
  if (verbose) {
    debug_wreg("DBG chip_rev = ", WREG0);
  }

  // if chip_rev not in revs_list:
  static uint32 revs_list[] = { 0x00, 0x01, 0x81 };
  list_len_t revs_list_len = sizeof(revs_list) / sizeof(uint32);
  if_rl(WREG0, OP_NOT_IN, revs_list, revs_list_len, "chip_rev"); {
    fail_wreg(SBOOT_SHOUT_WRONG_CHIP_REV,
	      "ERROR: Chip rev in rshim register does not match chip specified",
	      WREG0);
  } end_if("chip_rev");

  // Check that shim disable register is correct
  // TODO: Parameterize this address.
  read_word(WREG0, /* = */ 0x880, RSHIM_CHANNEL_RSHIM);
  fail_if_nonzero(SBOOT_SHOUT_WRONG_SHIM_DISABLE,
		  "ERROR: Shim disable register not correct for this chip type; got ", WREG0, WREG0);
}

label_t generate_check_for_message() {
  static bool generated = FALSE;
  static label_t subr_label;

  if (generated) {
    return subr_label;
  }
  generated = TRUE;

  subr_label = begin_subr("check_for_message");
  
  set_word(WREG_MSG_TYPE, /* = */ BOOT_MSG_NO_MSG);
  set_word(WREG_SENDER_X, /* = */ 0); // set to -1 in the python code, is this important?
  set_word(WREG_SENDER_Y, /* = */ 0); // set to -1 in the python code, is this important?
  set_word(WREG_RSHIM_SCRATCHPAD, /* = */ 0);
  set_word(WREG_SPI_SCRATCHPAD, /* = */ 0);

  if (verbose) {
    debug("DBG-boot. check_for_message from UART interface.");
    debug_value("DBG-boot. check_for_message read from location: ", RSHIM_SCRATCHPAD);
    debug_value("DBG-boot. check_for_message on channel : ", RSHIM_CHANNEL_HPI);
  }
  read_byte(WREG0, /* = */ RSHIM_SCRATCHPAD, RSHIM_CHANNEL_HPI);

  if (verbose) {
    debug_wreg("DBG-boot. check_for_message read_byte hpi_scratchpad = ", WREG0);
  }

  // If that is still 0, we're done
  if_zero(WREG0, "RSHIM_SCRATCHPAD first byte is zero"); {
    subr_return();
  } end_if("RSHIM_SCRATCHPAD first byte is zero");

  // Otherwise, go get the whole register
  if (verbose) {
    debug("DBG-boot. check_for_message hpi_scratchpad != 0, will get whole register");
  }

  read_word(WREG0, /* = */ RSHIM_SCRATCHPAD, RSHIM_CHANNEL_HPI);

  if (verbose) {
    debug_value("DBG-boot. check_for_message CHANNEL_HPI = ", RSHIM_CHANNEL_HPI);
    debug_value("DBG-boot. check_for_message Address = ", RSHIM_SCRATCHPAD);
    debug_wreg("DBG-boot. check_for_message hpi_scratchpad value = ", WREG0);
  }

  // If that is now 0, we're done
  // TODO: flag this as an error?
  if_zero(WREG0, "RSHIM_SCRATCHPAD is zero"); {
    subr_return(WREG0);
  } end_if("RSHIM_SCRATCHPAD is zero");

  if (verbose) {
    debug_wreg("DBG: boot.check_for_message hpi_scratchpad !=0; ", WREG0);
  }

  // Get the components of the message
  op_ri(WREG_MSG_TYPE, /* = */ WREG0, OP_AND_BITS, 0x3f);
  op_ri(WREG_SENDER_X, /* = */ WREG0, OP_SHIFT_RIGHT, 18);
  op_ri(WREG_SENDER_X, /* = */ WREG_SENDER_X, OP_AND_BITS, 0x3ff);
  op_ri(WREG_SENDER_Y, /* = */ WREG0, OP_SHIFT_RIGHT, 7);
  op_ri(WREG_SENDER_Y, /* = */ WREG_SENDER_Y, OP_AND_BITS, 0x3ff);

  if (verbose) {
    debug_wreg("DBG: boot.check_for_message msg_type = ", WREG_MSG_TYPE);
    debug_wreg_wreg("DBG: boot.check_for_message sender X,Y = ", WREG_SENDER_X, WREG_SENDER_Y);

    debug_value("DBG: boot.check_for_message self.rshim.CHANNEL_RSHIM ",
		RSHIM_CHANNEL_RSHIM);
    debug_value("DBG: boot.check_for_message self.rshim.SCRATCHPAD ",
		RSHIM_SCRATCHPAD);
  }

  // Get the second word from the RSHIM scratchpad:
  read_word(WREG_RSHIM_SCRATCHPAD, /* = */ RSHIM_SCRATCHPAD,
	    RSHIM_CHANNEL_RSHIM);

  // If the message is a boot pass message, get the another word from the
  // SPI scratchpad reg:
  if_ri(WREG_MSG_TYPE, OP_EQ, BOOT_MSG_BOOT_PASS_MSG, "boot_msg_pass_msg"); {
    read_word(WREG_SPI_SCRATCHPAD, /* = */ RSHIM_SCRATCHPAD,
	      RSHIM_CHANNEL_SPI);
    if (verbose) {
      debug_wreg("DBG: boot.check_for_message spi_scratchpad ",
		 WREG_SPI_SCRATCHPAD);
    }
  } end_if("boot_msg_pass_msg");

  if_true(WREG_CFM_DEQUEUE, "dequeue"); {
    if (verbose) {
      debug("DBG: boot.check_for_message clear hpi scratch pad to let TILE know we got message!");
    }
    send_word(RSHIM_SCRATCHPAD, RSHIM_CHANNEL_HPI, 0);
  } end_if("dequeue");

  // Interpret the message a bit
  label_t after_message_interpretation = future_label("after_message_interpretation");
  {
    op_ri(WREG0, /* = */ WREG_MSG_TYPE, OP_EQ, 0);
    op_ri(WREG1, /* = */ WREG_MSG_TYPE, OP_GT, BOOT_HIGHEST_MSG_TYPE);
    op_rr(WREG0, /* = */ WREG0, OP_OR, WREG1);
    if_true(WREG0, "unrecognized message type"); {
      shout_wreg(SBOOT_SHOUT_UNRECOGNIZED_BOOT_MESSAGE,
		 "ERROR: got unrecognized Boot message type code: ",
		 WREG_MSG_TYPE);
      shout(SBOOT_SHOUT_CONVERTING_MESSAGE_TO_FAIL,
	    "Converting message to Fail type");
      set_word(WREG_MSG_TYPE, /* = */ BOOT_MSG_FAIL_MSG);
      go_to(after_message_interpretation);
    } end_if("unrecognized message type");

    if (print_results) {
      if_ri(WREG_MSG_TYPE, OP_EQ, BOOT_MSG_STRING_MSG, "assemble string message"); {
	assemble_string_msg(WREG_RSHIM_SCRATCHPAD,
			    WREG_SENDER_X, WREG_SENDER_Y);
	go_to(after_message_interpretation);
      } end_if("assemble string message");

      if (!print_pass) {
	// NOTE: after_message_interpretation is used for the second time here.
	if_ri(WREG_MSG_TYPE, OP_EQ, BOOT_MSG_PASS_MSG, "not print_pass"); {
	  go_to(after_message_interpretation);
	} end_if("not print_pass");
      }

      if (verbose) {
	debug_wreg("Got Boot Message: ", WREG_MSG_TYPE);
	debug_wreg_wreg("Sender X,Y:       ", WREG_SENDER_X, WREG_SENDER_Y);
	debug_wreg_wreg("Data:             ", WREG_RSHIM_SCRATCHPAD, WREG_SPI_SCRATCHPAD);
      }
    }
  }
  label(after_message_interpretation);

  end_subr(subr_label);

  return subr_label;
}

// Get message from boot program by polling scratchpad.  Return array of
// 4 words: [msg_type, sender_x, sender_y, rshim_scratchpad]
// ***CMR* Actually, 5 words, the spi_scratchpad appears at the end.
//
// "boot.py" computes the minimum timeout, but doesn't do anything with it.
label_t generate_get_message() {
  static bool generated = FALSE;
  static label_t get_message_subr;

  if (generated) {
    return get_message_subr;
  }

  label_t check_for_message_subr = generate_check_for_message();

  get_message_subr = begin_subr("get_message");

  get_current_seconds(WREG_GM_START_TIMEOUT);

  {
    label_t get_message_loop = current_label("generate_get_message:get_message_loop");
    
    set_word(WREG_CFM_DEQUEUE, /* = */ TRUE);
    call_subr(check_for_message_subr);

    // Return if we got a message.
    if_ri(WREG_MSG_TYPE, OP_NE, BOOT_MSG_NO_MSG, "no message"); {
      subr_return();
    } end_if("no message");

    // Check for timeout.
    // TODO: It would be nice to calculate the actual elapsed time,
    // so we could display it.
    op_ri(WREG0, /* = */ WREG_GM_START_TIMEOUT, OP_TIMEOUT, get_message_timeout);
    fail_if_true(SBOOT_SHOUT_GET_MESSAGE_TIMED_OUT,
		 "ERROR: Get message timed out ", WREG0, WREG0);

    go_to(get_message_loop);
  }
  
  end_subr(get_message_subr);
  generated = TRUE;

  return get_message_subr;
}

label_t generate_watch_for_message() {
  static bool generated = FALSE;
  static label_t watch_for_message_subr;

  if (generated) {
    return watch_for_message_subr;
  }

  label_t get_message_subr = generate_get_message();

  watch_for_message_subr = begin_subr("watch_for_message");

  if (verbose) {
    debug_wreg("Expecting message ", WREG_WFM_TARGET_MESSAGE);
    debug_wreg("Allow target message only ",
	       WREG_WFM_ALLOW_TARGET_MESSAGE_ONLY);
  }

  label_t after_get_message_loop = future_label("after_get_message_loop");
  {
    label_t get_message_loop = current_label("generate_watch_for_message:get_message_loop");
    call_subr(get_message_subr);

    // Did we receive the message we wanted?
    if_rr(WREG_WFM_TARGET_MESSAGE, OP_EQ, WREG_MSG_TYPE, "is message the right type"); {
      go_to(after_get_message_loop);
    } end_if("is message the right type");
  
    // Fail if we receive a BOOT_MSG_FAIL_MSG.  Print some useful info first.
    if_ri(WREG_MSG_TYPE, OP_EQ, BOOT_MSG_FAIL_MSG, "boot_msg_fail_msg"); {
      fail_wreg_wreg(SBOOT_SHOUT_EXITING_DUE_TO_FAIL_MSG,
		     "ERROR: Exiting due to FAIL_MSG from tile X,Y ",
		     WREG_SENDER_X, WREG_SENDER_Y);
    } end_if("boot_msg_fail_msg");
    
    // The python code checked for a timeout message here.
    // This implementation will handle timeouts at a lower level.
    // TODO: Consider passing timeouts up a few levels to
    // get error messages that trace the call path.

    // Also fail if in mode where only the target message
    // is allowed.
    if_true(WREG_WFM_ALLOW_TARGET_MESSAGE_ONLY, "allow target message only"); {
      shout_wreg(SBOOT_SHOUT_UNEXPECTED_MESSAGE,
		 "ERROR: Expected message ", WREG_WFM_TARGET_MESSAGE);
      fail_wreg(SBOOT_SHOUT_UNEXPECTED_MESSAGE_EXITING,
		"ERROR: Exiting due to unexpected message ", WREG_MSG_TYPE);
    } end_if("allow target message only");

    go_to(get_message_loop);
  }
  label(after_get_message_loop);

  // Got the desired message.
  if (verbose) {
    debug_wreg("DBG: watch_for_message: msg_type=", WREG_MSG_TYPE);
    debug_wreg_wreg("DBG: watch_for_message: sender X,Y =", WREG_SENDER_X, WREG_SENDER_Y);
    debug_wreg("DBG: watch_for_message: rshim_scratchpad=",
	       WREG_RSHIM_SCRATCHPAD);
    debug_wreg("DBG: watch_for_message: spi_scratchpad=",
	       WREG_SPI_SCRATCHPAD);
  }

  if (verbose) {
    debug("zero data test.");
  }
  // Do some message specific checks
  // Many types should only have message data of zero
  if_nonzero(WREG_RSHIM_SCRATCHPAD, "nonzero WREG_RSHIM_SCRATCHPAD"); {
    static uint32 done_or_prompt_msg[] = { BOOT_MSG_DONE_MSG,
					   BOOT_MSG_PROMPT_MSG };
    list_len_t done_or_prompt_msg_len = sizeof(done_or_prompt_msg) / sizeof(uint32);
    if_rl(WREG_MSG_TYPE, OP_IN, done_or_prompt_msg, done_or_prompt_msg_len, "done_or_prompt_msg"); {
      shout_wreg_wreg(SBOOT_SHOUT_NONZERO_DATA_TILE_X,
		 "ERROR: Tile X,Y = ", WREG_SENDER_X, WREG_SENDER_Y);
      shout_wreg(SBOOT_SHOUT_NONZERO_DATA,
		 "ERROR: Data = ", WREG_RSHIM_SCRATCHPAD);
      fail_wreg(SBOOT_SHOUT_NONZERO_DATA_IN_MESSAGE,
		"ERROR: Got non-zero data in message ", WREG_MSG_TYPE);
    } end_if("done_or_prompt_msg");
  } end_if("nonzero WREG_RSHIM_SCRATCHPAD");

  if (verbose) {
    debug("boot pass test.");
  }
  // Do checks related to boot pass messages.
  if_ri(WREG_MSG_TYPE, OP_EQ, BOOT_MSG_BOOT_PASS_MSG, "BOOT_MSG_BOOT_PASS_MSG"); {
    if (verbose) {
      debug("DBG: watchfor message: target_msg == self.BOOT_PASS_MSG");
    }

    if (verbose) {
      debug("boot master test.");
    }
    // Boot pass messages can only come from the boot master
    op_ri(WREG0, /* = */ WREG_SENDER_X, OP_EQ, boot_master_x);
    op_ri(WREG1, /* = */ WREG_SENDER_Y, OP_EQ, boot_master_y);
    op_rr(WREG0, /* = */ WREG0, OP_AND, WREG1);
    if_false(WREG0, "Boot Pass Msg from wrong tile"); {
      shout_value(SBOOT_SHOUT_BOOT_PASS_EXPECTED_TILE_X,
		  "ERROR: Expected Tile X = ", boot_master_x);
      shout_value(SBOOT_SHOUT_BOOT_PASS_EXPECTED_TILE_Y,
		  "ERROR: Expected Tile Y = ", boot_master_y);
      fail_wreg_wreg(SBOOT_SHOUT_BOOT_PASS_MSG_FROM_WRONG_TILE,
		     "ERROR: Got Boot Pass Msg from wrong tile X,Y=",
		     WREG_SENDER_X, WREG_SENDER_Y);
    } end_if("Boot Pass Msg from wrong tile");

    if (verbose) {
      debug("Failure bits test.");
    }
    // If there are any bits set in either of the two 32-bit result vectors,
    // that's a failure
    op_ri(WREG0, /* = */ WREG_RSHIM_SCRATCHPAD, OP_EQ, 0);
    op_ri(WREG1, /* = */ WREG_SPI_SCRATCHPAD, OP_EQ, 0);
    op_rr(WREG0, /* = */ WREG0, OP_AND, WREG1);
    if_false(WREG0, "Level 1 Boot test failed"); {
      note_boot_pass_failed(WREG_RSHIM_SCRATCHPAD, WREG_SPI_SCRATCHPAD);
      fail_wreg_wreg(SBOOT_SHOUT_LEVEL_1_BOOT_TEST_FAILED,
		     "ERROR: Level 1 Boot test failed: vector is non-zero ",
		     WREG_RSHIM_SCRATCHPAD, WREG_SPI_SCRATCHPAD);
    } end_if("Level 1 Boot test failed");
  } end_if("BOOT_MSG_BOOT_PASS_MSG");

  // If here, we passed all checks.  If it is a kind of message that ends a test, print a passing message
  if (print_results) {
    static uint32 done_or_pass_msg[] = { BOOT_MSG_DONE_MSG,
					 BOOT_MSG_BOOT_PASS_MSG };
    list_len_t done_or_pass_msg_len = sizeof(done_or_pass_msg) / sizeof(uint32);
    if_rl(WREG_MSG_TYPE, OP_IN, done_or_pass_msg, done_or_pass_msg_len, "done_or_pass_msg"); {
      if (verbose) {
	debug("DBG: watchfor message: print_results and target_message DONE_MSG and BOOT_PASS_MSG");
      }

      if_ri(WREG_MSG_TYPE, OP_EQ, BOOT_MSG_DONE_MSG, "Level 2 Test Passed"); {
	note_boot_done();
	shout_wreg_wreg(SBOOT_SHOUT_LEVEL_2_TEST_PASSED,
			"Level 2 Test Passed on tile X,Y = ",
			WREG_SENDER_X, WREG_SENDER_Y);
      } end_if("Level 2 Test Passed");

      if_ri(WREG_MSG_TYPE, OP_EQ, BOOT_MSG_BOOT_PASS_MSG, "Level 1 Boot Test Passed"); {
	note_boot_pass_succeeded();
	shout_wreg_wreg(SBOOT_SHOUT_LEVEL_1_BOOT_TEST_PASSSED,
			"Level 1 Boot Test Passed on tile X,Y = ",
			WREG_SENDER_X, WREG_SENDER_Y);
      } end_if("Level 1 Boot Test Passed");
    } end_if("done_or_pass_msg");
  }
  end_subr(watch_for_message_subr);
  generated = TRUE;

  return watch_for_message_subr;
}

// print_pass is a global parameter in this implementation
void watch_for_message(uint32 target, bool allow_target_only) {
  label_t watch_for_message_subr = generate_watch_for_message();

  set_word(WREG_WFM_TARGET_MESSAGE, /* = */ target);
  set_word(WREG_WFM_ALLOW_TARGET_MESSAGE_ONLY, /* = */ allow_target_only);
  call_subr(watch_for_message_subr);
}

// "Given a binary file, read it in and write contents to the chip over the STN (using rshim)"
void load_file(char *filename, FILE *binfile, bool is_l2_file,  bool is_dv_l1_file) {

  uint8 *bufptr;
  uint32 filelen = gen_file_words(filename, binfile, is_l2_file, is_dv_l1_file, &bufptr);

  // Max list over uart is 8K, so break it up if needed                                                                                                                      
  uint32 start_index = 0;
  uint32 end_index = BOOT_UART_BLOCK_SIZE;

  while (end_index < filelen) {
    uint32 segment_len = end_index - start_index;
    send_bytes(RSHIM_STN_DATA, RSHIM_CHANNEL_RSHIM, (uint16)segment_len, bufptr + start_index);
    start_index += BOOT_UART_BLOCK_SIZE;
    end_index   += BOOT_UART_BLOCK_SIZE;
  }

  if (start_index < filelen) {
    uint32 segment_len = filelen - start_index;
    send_bytes(RSHIM_STN_DATA, RSHIM_CHANNEL_RSHIM, (uint16)segment_len, bufptr + start_index);
  }

  free(bufptr);

  // Send boot args
  if (verbose) {
    debug("MLS: boot.l1_load;Generating boot args, post l1_load_test_only?");
  }

  uint32 boot_args_len = gen_boot_args0();
  uint32 *bootargs_ptr = bootargs;
  while (boot_args_len--) {
    boot_arg_push(*bootargs_ptr++);
  }

  if (verbose) {
    debug("DBG: boot.l1_load;Generation boot args completed");
  }

  // Push arguments that are only going to the boot master (not being sent along to all tiles) last   
  boot_args_len = gen_boot_args1(&bootargs);
  bootargs_ptr = bootargs;
  while (boot_args_len--) {
    boot_arg_push(*bootargs_ptr++);
  }

  if (verbose) {
    debug("DBG: boot.l1_load; Generation boot args push completed");
  }

  // Delay so we don't check for messages before uart gets set up
  wait(1);
  if (verbose) {
    debug("DBG: boot.l1_load sleep done");
  }

  // wait for ready message
  if (sync_on_dn_ready) {
    if (verbose) {
      debug_value("DBG: l1_load watch_for_message = ", BOOT_MSG_RDY_MSG);
    }
    watch_for_message(BOOT_MSG_RDY_MSG, FALSE);
    /*
    if self.watch_for_message(self.RDY_MSG) != 0:
                print "ERROR: Test failed during L1 boot.  Exiting"
                return 2
     */
    if (verbose) {
      debug("DBG: l1_load-sync_on_dn_ready");
    }
  }

  if (verbose) {
    debug("DBG: l1_load MADE IT HERE 1 ");
  }

  // drive GPIO pins: not done on MDB.
  //  Send the rest of the boot args in  
  if (verbose) {
    debug("DBG: l1_load getting ready for gen_boot_args(2)");
  }
  boot_args_len = gen_boot_args2(&bootargs);
  if (verbose) {
    debug("DBG l1_load: Sending rest of boot args");
  }
  bootargs_ptr = bootargs;
  while (boot_args_len--) {
    boot_arg_push(*bootargs_ptr++);
  }

  check_chip_rev();
}

void reset_maestro_and_delay() {
  reset_maestro(reset_seconds);
  if (maestro_post_reset_delay > 0) {
    wait(maestro_post_reset_delay);
  }
}

void check_uart_divisor_low_speed() {
  if (verbose) {
    printf("Checking the UART divisor for low speed operation.\n");
    debug("Checking the UART divisor register for low speed operation");
  }

  read_word(WREG0, RSHIM_UART_DIVISOR, RSHIM_CHANNEL_UART);
  if (verbose) {
    debug_wreg("DBG: UART divisor: ", WREG0);
  }

  if_ri(WREG0, OP_NE, RSHIM_UART_DIVISOR_9600_BPS, "RShim access failed"); {
    fail_wreg(SBOOT_SHOUT_RSHIM_ACCESS_FAILED_LOW_SPEED,
	      "ERROR: RShim access failed at low speed.", WREG0);
  } end_if("RShim access failed");
}

void check_uart_divisor_high_speed() {
  if (verbose) {
    printf("Checking the UART divisor for high speed operation.\n");
    debug("Checking the UART divisor register for high speed operation");
  }

  read_word(WREG0, RSHIM_UART_DIVISOR, RSHIM_CHANNEL_UART);
  if (verbose) {
    debug_wreg("DBG: UART divisor: ", WREG0);
  }

  if_ri(WREG0, OP_NE, RSHIM_UART_DIVISOR_115200_BPS, "RShim access failed"); {
    fail_wreg(SBOOT_SHOUT_RSHIM_ACCESS_FAILED_HIGH_SPEED,
	      "ERROR: RShim access failed at high speed.", WREG0);
  } end_if("RShim access failed");
}

void change_uart_to_high_speed() {
  if (verbose) {
    printf("Changing the UART to high speed operation.\n");
    debug("Changing the UART to high speed operation");
  }

  if (verbose) {
    debug_value("DBG: Setting UART divisor to ", RSHIM_UART_DIVISOR_115200_BPS);
  }
  send_word(RSHIM_UART_DIVISOR, RSHIM_CHANNEL_UART, RSHIM_UART_DIVISOR_115200_BPS);

  // TODO: Need a custom wait.
  if (maestro_uart_configuration_delay > 0) {
    wait(maestro_uart_configuration_delay);
  }

  // Change the UART speed on the local port.
  change_uart_speed(TRUE);

  // TODO: Should drain the UART.

  // TODO: Need a custom wait.
  if (maestro_uart_configuration_delay > 0) {
    wait(maestro_uart_configuration_delay);
  }
}

void l1_load_test_only() {
  load_file(l1_filename, l1_file, FALSE, TRUE);
}

// "Given a L1 boot binary, load it (plus the L1 boot args).  Assumes the chip just came out of reset."
void l1_load() {
  if (verbose) {
    debug("DBG: Booting through rshim.UART");
  }
  read_word(WREG0, /* = */ RSHIM_SCRATCHPAD, RSHIM_CHANNEL_RSHIM);
  if (verbose) {
    debug_wreg("DBG: UART-checkuart_scratchpad: ", WREG0);
  }
  if_ri(WREG0, OP_EQ, BOOT_IS_DEFACED, "Unable to boot through RShim"); {
    fail(SBOOT_SHOUT_UNABLE_TO_BOOT_THROUGH_RSHIM,
	 "ERROR: Unable to boot through RShim.  You need to reset the MAESTRO chip first.");
  } end_if("Unable to boot through RShim");

  if (verbose) {
    debug("DBG: boot.l1_load; boot l1_load_test_only Started");
  }
  l1_load_test_only();
  if (verbose) {
    debug("DBG: boot.l1_load; boot l1_load_test_only completed");
  }

  if (post_code_load_wait) {
    if (verbose) {
      debug_value("DBG: boot.l1_load; post_code_load_wait = ", post_code_load_wait);
    }
    wait(post_code_load_wait);
  }
}

bool l1_test(bool speed_regress_mode, bool top_harness) {
  if (top_harness && verbose) {
    debug("Generated using the l1_test harness.");
  }

  check_rectangle();
  l1_load();

  if (verbose) {
    debug("l1_test looking for a BOOT_PASS_MSG.");
  }
  watch_for_message(BOOT_MSG_BOOT_PASS_MSG, speed_regress_mode);
  if (verbose) {
    debug("l1_test passed.");
  }
  return TRUE;
}

// Takes WREG_PROGRAM_NUMBER and WREG_INFO_TYPE as parameters.
// Initializes and increments WREG_PROGRAM_OFFSET
// Returns WREG_INFO_LENGTH.
// Returns success/failure in WREG0
label_t generate_find_program_info() {
  static bool generated = FALSE;
  static label_t find_program_info_subr;

  if (generated) {
    return find_program_info_subr;
  }

  find_program_info_subr = begin_subr("find_program_info");

  set_word(WREG_PROGRAM_OFFSET, /* = */ 0);

  {
    label_t program_info_loop = current_label("generate_find_program_info:program_info_loop");

    // Read the command byte.  Is it an INFO command?
    read_program_byte(WREG0, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
    if_ri(WREG0, OP_EQ, SBOOT_FETCH_ERROR, "no program byte"); {
      // TODO: Issue failure message.
      debug("find_program_info: no program byte");
      set_word(WREG0, FALSE);
      subr_return();
    } end_if("no program byte");
    if_ri(WREG0, OP_NE, COMMAND_PROGRAM_INFO, "not program info"); {
      // TODO: Issue failure message.
      debug_wreg("find_program_info: not program info: ", WREG0);
      set_word(WREG0, FALSE);
      subr_return();
    } end_if("not program info");
    increment(WREG_PROGRAM_OFFSET);

    // Read the info_type byte into WREG0.
    read_program_byte(WREG0, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
    if_ri(WREG0, OP_EQ, SBOOT_FETCH_ERROR, "no info_type byte"); {
      // TODO: Issue failure message.
      debug("find_program_info: no info_type byte");
      set_word(WREG0, FALSE);
      subr_return();
    } end_if("no info_type byte");
    increment(WREG_PROGRAM_OFFSET);

    if_ri(WREG0, OP_EQ, SBOOT_INFO_END, "info_end byte"); {
      // TODO: Issue failure message.
      debug("find_program_info: info_end byte");
      set_word(WREG0, FALSE);
      subr_return();
    } end_if("info_end byte");

    // Read the info_length byte into WREG_INFO_LENGTH.
    read_program_byte(WREG_INFO_LENGTH, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
    if_ri(WREG_INFO_LENGTH, OP_EQ, SBOOT_FETCH_ERROR, "no info_length byte"); {
      // TODO: Issue failure message.
      debug("find_program_info: no info_length byte");
      set_word(WREG0, FALSE);
      subr_return();
    } end_if("no info_length byte");
    increment(WREG_PROGRAM_OFFSET);

    if_rr(WREG_INFO_TYPE, OP_EQ, WREG0, "found desired info type"); {
      debug_wreg("find_program_info: found info type ", WREG_INFO_TYPE);
      set_word(WREG0, TRUE);
      subr_return();
    } end_if("found desired info type");

    op_rr(WREG_PROGRAM_OFFSET, /* = */ WREG_PROGRAM_OFFSET, OP_ADD, WREG_INFO_LENGTH);
    go_to(program_info_loop);
  }

  end_subr(find_program_info_subr);
  generated = TRUE;

  return find_program_info_subr;
}

// Takes WREG_PROGRAM_NUMBER and WREG_PROGRAM_OFFSET as parameters.
// Increments WREG_PROGRAM_OFFSET
// Returns success/failure in WREG0.
// Returns value on success in WREG1
// Clobbers WREG0, WREG1, WREG2
label_t generate_fetch_info_uint16() {
  static bool generated = FALSE;
  static label_t fetch_info_uint16_subr;

  if (generated) {
    return fetch_info_uint16_subr;
  }

  fetch_info_uint16_subr = begin_subr("fetch_info_uint16");

  read_program_byte(WREG0, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
  if_ri(WREG0, OP_EQ, SBOOT_FETCH_ERROR, "no byte0"); {
    // TODO: Issue failure message.
    debug("fetch_info_uint16: no byte0");
    set_word(WREG0, FALSE);
    subr_return();
  } end_if("no byte0");
  increment(WREG_PROGRAM_OFFSET);

  read_program_byte(WREG1, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
  if_ri(WREG1, OP_EQ, SBOOT_FETCH_ERROR, "no byte1"); {
    // TODO: Issue failure message.
    debug("fetch_info_uint16: no byte1");
    set_word(WREG0, FALSE);
    subr_return();
  } end_if("no byte1");
  increment(WREG_PROGRAM_OFFSET);

  set_word(WREG2, /* = */ 0);
  pack_bytes_into_word(WREG1, WREG2, WREG2, WREG1, WREG0);

  set_word(WREG0, TRUE);
  subr_return();

  end_subr(fetch_info_uint16_subr);
  generated = TRUE;

  return fetch_info_uint16_subr;
}

// Takes WREG_PROGRAM_NUMBER and WREG_PROGRAM_OFFSET as parameters.
// Increments WREG_PROGRAM_OFFSET
// Returns success/failure in WREG0.
// Returns value on success in WREG1
// Clobbers WREG0, WREG1, WREG2, WREG3.
label_t generate_fetch_info_uint32() {
  static bool generated = FALSE;
  static label_t fetch_info_uint32_subr;

  if (generated) {
    return fetch_info_uint32_subr;
  }

  fetch_info_uint32_subr = begin_subr("fetch_info_uint32");

  read_program_byte(WREG0, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
  if_ri(WREG0, OP_EQ, SBOOT_FETCH_ERROR, "no byte0"); {
    // TODO: Issue failure message.
    set_word(WREG0, FALSE);
    subr_return();
  } end_if("no byte0");
  increment(WREG_PROGRAM_OFFSET);

  read_program_byte(WREG1, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
  if_ri(WREG1, OP_EQ, SBOOT_FETCH_ERROR, "no byte1"); {
    // TODO: Issue failure message.
    set_word(WREG0, FALSE);
    subr_return();
  } end_if("no byte1");
  increment(WREG_PROGRAM_OFFSET);

  read_program_byte(WREG2, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
  if_ri(WREG2, OP_EQ, SBOOT_FETCH_ERROR, "no byte2"); {
    // TODO: Issue failure message.
    set_word(WREG0, FALSE);
    subr_return();
  } end_if("no byte2");
  increment(WREG_PROGRAM_OFFSET);

  read_program_byte(WREG3, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
  if_ri(WREG3, OP_EQ, SBOOT_FETCH_ERROR, "no byte3"); {
    // TODO: Issue failure message.
    set_word(WREG0, FALSE);
    subr_return();
  } end_if("no byte3");
  increment(WREG_PROGRAM_OFFSET);

  pack_bytes_into_word(WREG1, WREG3, WREG2, WREG1, WREG0);

  set_word(WREG0, TRUE);
  subr_return();

  end_subr(fetch_info_uint32_subr);
  generated = TRUE;

  return fetch_info_uint32_subr;
}

// Called with the program number in WREG_PROGRAM_NUMBER.
// Sets WREG_INFO_TYPE.
// Sets WREG_PROGRAM_OFFSET.
// Returns with TRUE/FALSE in WREG0.
// On success, returns the program length in WREG_PROGRAM_LENGTH.
// Clobbers WREG0, WREG1, WREG2 and WREG3.
label_t generate_get_program_length() {
  static bool generated = FALSE;
  static label_t get_program_length_subr;

  if (generated) {
    return get_program_length_subr;
  }

  label_t find_program_info_subr = generate_find_program_info();
  label_t fetch_info_uint32_subr = generate_fetch_info_uint32();

  get_program_length_subr = begin_subr("get_program_length");

  set_word(WREG_INFO_TYPE, /* = */ SBOOT_INFO_LENGTH);
  call_subr(find_program_info_subr);
  if_false(WREG0, "failed to find SBOOT_INFO_LENGTH"); {
    // TODO: Issue failure message.
    subr_return();
  } end_if("failed to find SBOOT_INFO_LENGTH");

  call_subr(fetch_info_uint32_subr);
  if_false(WREG0, "failed to fetch length word"); {
    // TODO: Issue failure message.
    subr_return();
  } end_if("failed to fetch length word");

  // WREG0 is already set to TRUE.
  copy_word(WREG_PROGRAM_LENGTH, /* = */ WREG1);
  subr_return();

  end_subr(get_program_length_subr);
  generated = TRUE;

  return get_program_length_subr;
}


// Returns with TRUE/FALSE in WREG0.
label_t generate_check_program_crc() {
  static bool generated = FALSE;
  static label_t check_program_crc_subr;

  if (generated) {
    return check_program_crc_subr;
  }

  label_t get_program_length_subr = generate_get_program_length();
  label_t find_program_info_subr = generate_find_program_info();
  label_t fetch_info_uint16_subr = generate_fetch_info_uint16();

  check_program_crc_subr = begin_subr("check_program_crc");

  call_subr(get_program_length_subr);
  if_false(WREG0, "failed to get program length"); {
    shout(SBOOT_SHOUT_CRC_FAILED_TO_GET_PROGRAM_LENGTH,
	  "check_program_crc: failed to get program length");
    subr_return();
  } end_if("failed to get program length");
  debug_wreg("Program length: ", WREG_PROGRAM_LENGTH);

  set_word(WREG_INFO_TYPE, /* = */ SBOOT_INFO_CRC);
  call_subr(find_program_info_subr);
  if_false(WREG0, "failed to find SBOOT_INFO_CRC"); {
    // TODO: Issue failure message.
    debug("check_program_crc: failed to find SBOOT_INFO_CRC");
    subr_return();
  } end_if("failed to find SBOOT_INFO_CRC");

  call_subr(fetch_info_uint16_subr);
  if_false(WREG0, "failed to fetch stored crc"); {
    // TODO: Issue failure message.
    debug("check_program_crc: failed to fetch stored crc");
    subr_return();
  } end_if("failed to fetch stored crc");
  copy_word(WREG3, /* = */ WREG1); // Save the stored CRC.
  debug_wreg("Stored CRC: ", WREG3);

  op_rr(WREG_PROGRAM_LENGTH, /* = */ WREG_PROGRAM_LENGTH, OP_SUB, WREG_PROGRAM_OFFSET);
  debug_wreg("Calculating CRC on this many bytes: ", WREG_PROGRAM_LENGTH);

  // Initiate the CRC calculation.
  set_word(WREG1, /* = */ 0xffff);

  {
    label_t calc_crc_loop = current_label("check_program_crc:calc_crc_loop");

    if_zero(WREG_PROGRAM_LENGTH, "end of program"); {
      op_ri(WREG1, /* = */ WREG1, OP_INVERT_BITS, 0);
      op_ri(WREG1, /* = */ WREG1, OP_AND_BITS, 0xffff);

      debug_wreg("Calc'd CRC: ", WREG1);
      op_rr(WREG0, /* = */ WREG1, OP_EQ, WREG3);
      subr_return();
    } end_if("end of program");

    read_program_byte(WREG0, /* = */ WREG_PROGRAM_NUMBER, WREG_PROGRAM_OFFSET);
    if_ri(WREG0, OP_EQ, SBOOT_FETCH_ERROR, "program byte fetch failed"); {
      shout(SBOOT_SHOUT_CRC_PROGRAM_BYTE_FETCH_FAILED,
	    "check_program_crc: program byte fetch failed");
      set_word(WREG0, FALSE);
      subr_return();
    } end_if("program byte fetch failed");
    increment(WREG_PROGRAM_OFFSET);
    decrement(WREG_PROGRAM_LENGTH);

    crc_kernel(WREG1, WREG0);
    go_to(calc_crc_loop);
  }

  end_subr(check_program_crc_subr);
  generated = TRUE;

  return check_program_crc_subr;
}

// Generate a program that, when run, will check the integrity of the
// available MAESTRO test programs by calculating a CRC and comparing it
// to a stored CRC.
//
// Clobbers ...
bool crc_test() {
  if (verbose) {
    debug("Generated using the crc_test harness.");
  }

  label_t check_program_crc_subr = generate_check_program_crc();

  get_number_of_programs(WREG_PROGRAMS_LEFT);
  get_first_program_number(WREG_PROGRAM_NUMBER);
  set_word(WREG_FAILURE_COUNT, /* = */ 0);
  set_word(WREG_SUCCESS_COUNT, /* = */ 0);

  label_t after_loop = future_label("crc_test:after_loop");
  {
    label_t program_number_loop = current_label("crc_test:program_number_loop");

    if_zero(WREG_PROGRAMS_LEFT, "no more programs"); {
      go_to(after_loop);
    } end_if("no more programs");

    validate_program_number(WREG0, WREG_PROGRAM_NUMBER);
    if_true(WREG0, "valid program"); {
      call_subr(check_program_crc_subr);
      label_t else_label = future_label("else");
      label_t end_if_label = future_label("end_if");
      go_to_if_false(else_label, WREG0); {
	shout_wreg(SBOOT_SHOUT_CRC_TEST_PASSED, "CRC test passed for program ", WREG_PROGRAM_NUMBER);
	increment(WREG_SUCCESS_COUNT);
	go_to(end_if_label);
      } label(else_label); {
	shout_wreg(SBOOT_SHOUT_CRC_TEST_FAILED, "CRC test FAILED for program ", WREG_PROGRAM_NUMBER);
	increment(WREG_FAILURE_COUNT);
      } label(end_if_label);
    } end_if("valid program");// Silently skip invalid program numbers.
    
    increment(WREG_PROGRAM_NUMBER);
    decrement(WREG_PROGRAMS_LEFT);
    go_to(program_number_loop);
  }
  label(after_loop);

  fail_if_nonzero(SBOOT_SHOUT_AT_LEAST_ONE_CRC_TEST_FAILED,
		  "At least one CRC failure ", WREG_FAILURE_COUNT, WREG_FAILURE_COUNT);

  shout_wreg(SBOOT_SHOUT_CRC_TEST_SUCCEEDED,
	     "CRC check passed all programs", WREG_SUCCESS_COUNT);
  return TRUE;
}

// "Verify that the specified mshim location is sane"
void check_mshim_loc_itc() {
  // Verify that the specified mshim location is on the edge of the specified rectangle
  if (((mshim_x == 0) || (mshim_y == 0)) && mshim_x != 4) {
    return;
  }

  fprintf(stderr, "ERROR: Mshim is not at the correction location");
  display_config();
  exit(EXIT_FAILURE);
}

uint32 mshim_num_itc(uint8 msh_x, uint8 msh_y) {
  uint32 msh_num;
  if (msh_x == 1) {
    if (msh_y == 0) {
      msh_num = 0;
    } else if(msh_y == 8) {
      msh_num = 3;
    } else {
      msh_num = 0xffffffff;
      fprintf(stderr, "ERROR: msh coordinates are not valid: x = %d, y = %d\n", msh_x, msh_y);
    }
  } else if (msh_x == 7) {
    if (msh_y == 0) {
      msh_num = 1;
    } else if (msh_y == 8) {
      msh_num = 2;
    } else {
      msh_num = 0xffffffff;
      fprintf(stderr, "ERROR: msh coordinates are not valid: x = %d, y = %d\n", msh_x, msh_y);
    }
  } else {
    msh_num = 0xffffffff;
    fprintf(stderr, "ERROR: msh coordinates are not valid: x = %d, y = %d\n", msh_x, msh_y);
  }
  return msh_num;
}

static msh_t msh0;
static msh_t msh1;
static msh_t msh2;
static msh_t msh3;

static msh_t *msh_list[] = {
  &msh0, &msh1, &msh2, &msh3
};

bool init_msh_config() {
  printf("Initializing mshim configuration\n");
  bool valid = TRUE;
  for (int shim = 0; shim < 4; shim++) {
    // if not self.config.msh_list[shim].load(self.rshim, self.fpga):
    /* *********
    if (!config_msh_list[shim].load()) {
      valid = false;
    }
    */
  }
  return valid;
}

int gen_mshim_pre_config_boot_args() {
  // Initialize the argument list to be returned
  init_bootargs();

  // Generate the boot mshim and stripe bit argument
  uint32 data = (mshim_x << 18) | (mshim_y << 7);
  if (stripe) {
    data |= 1;
  }
  append_bootarg(data);

  // Generate the boot arg indicating that the boot code should
  // configure the mshims (with values provided as boot args)
  // DL-hack disable mshim_config just to pass the test to test out the flow
  // 1 disable
  // 0 run mshim_config
  append_bootarg(0);
  // arg_list.append(1)
  // DL-hack disable mshim_config just to pass the test to test out the flow

  return bootargs_len;
}

void gen_mshim_config_values(int shim_num) {

  // *****************************
}

int gen_one_mshim_config_boot_args(int shim_num) {
  // Initialize the argument list to be returned
  init_bootargs();

  gen_mshim_config_values(shim_num);

  return bootargs_len;
}

int gen_mshim_post_config_boot_args() {
  // **********
}


// Adapted from get_mshim(...) from "boot.py":
uint8 get_mshim() {
  int None = -1;
  int mshim = None;
  if (mshim_x == 1) {
    if (mshim_y == 0) {
      mshim = 0;
    } else if (mshim_y == 8) {
      mshim = 3;
    } else {
      mshim = None;
    }
  } else if (mshim_x == 7) {
    if (mshim_y == 0) {
      mshim = 1;
    } else if (mshim_y == 8) {
      mshim = 2;
    } else {
      mshim = None;
    }
  }

  if (mshim == None) {
    fprintf(stderr, "ERROR: Current mshim x,y (%d,%d) are illegal\n", mshim_x, mshim_y);
    exit(EXIT_FAILURE);
  }
  return mshim;
}

void push_args(int len) {
  uint32 *bootargs_ptr = bootargs;
  while (len--) {
    stn_push(*bootargs_ptr++);
  }
}

// Excerpted from config_mshim(...) in "boot.py":
void config_mshim() {

  check_mshim_loc_itc();

  uint32 msh_num = mshim_num_itc(mshim_x, mshim_y);

  // DL-hack added to see how the itc is configed on btk side
  fprintf(stderr, "DL-hack print out the config for msh_num 0x%X\n", msh_num);
  //self.display_config()

  // Check that the mshim number for the boot mshim is valid
  if (msh_num == 0xffffffff) {
    fprintf(stderr,  "ERROR: --Mshim coordinates %d,%d are not valid\n", mshim_x, mshim_y);
    exit(EXIT_FAILURE);
  }

  if (!init_msh_config()) {
    fprintf(stderr,  "ERROR: Cannot configure mshims because config info is invalid");
    exit(EXIT_FAILURE);
  }

  if (print_results) {
    printf("Boot memory shim will be shim %d, located at %d,%d\n", msh_num, mshim_x, mshim_y);
  }
  if (verbose) {
    debug_value("Boot memory shim will be shim ", msh_num);
    debug_value("Boot memory shim mshim_x=", mshim_x);
    debug_value("Boot memory shim mshim_y=", mshim_y);
  }

  push_args(gen_mshim_pre_config_boot_args());
  push_args(gen_one_mshim_config_boot_args(get_mshim()));
  push_args(gen_mshim_post_config_boot_args());

  watch_for_message(BOOT_MSG_RDY_MSG, FALSE);

  if (print_results) {
    printf("Mshim Configuration Passed.  Mshim Location X Y: %d %d\n", mshim_x, mshim_y);
    debug_value("Mshim Configuration Passed.  Mshim Location X=", mshim_x);
    debug_value("Mshim Configuration Passed.  Mshim Location Y=", mshim_y);
  }
}

// This handles some parts of "startup.py" that aren't incorporated
// here.
//
// TODO: refactor into code that more closely resembles the Python code.
void startup(){
  config_init(&config);
}

bool l1_and_config_mshim() {
  if (verbose) {
    debug("Generated using the l1_and_config harness.");
  }
  l1_test(FALSE, FALSE);
  config_mshim();
  return FALSE;
}

bool l2_test_one_tile() {
  return FALSE;
}

bool l2_test_all_tiles() {
  return FALSE;
}

bool itc_l2_test_one_tile() {
  return FALSE;
}

bool itc_l2_test_tile11() {
  return FALSE;
}

bool itc_l2_test_all_tile() {
  return FALSE;
}

bool itc_gbe() {
  return FALSE;
}

bool select_test_harness_and_generate() {

  if (strcmp(test_type, TEST_TYPE_CRC) == 0) {
    return crc_test();
  } else if (strcmp(test_type, TEST_TYPE_L1) == 0) {
    return l1_test(FALSE, TRUE);

  } else if (strcmp(test_type, TEST_TYPE_L1_MEM) == 0) {
    return l1_and_config_mshim();

  } else if (strcmp(test_type, TEST_TYPE_L2_1TILE) == 0) {
    return l2_test_one_tile();
  } else if (strcmp(test_type, TEST_TYPE_L2_ALL) == 0) {
    return l2_test_all_tiles();
  } else if (strcmp(test_type, TEST_TYPE_ITC_L2_1TILE) == 0) {
    return itc_l2_test_one_tile();
  } else if (strcmp(test_type, TEST_TYPE_ITC_L2_TILE11) == 0) {
    return itc_l2_test_tile11();
  } else if (strcmp(test_type, TEST_TYPE_ITC_L2_ALL_TILE) == 0) {
    return itc_l2_test_all_tile();
  } else if (strcmp(test_type, TEST_TYPE_ITC_GBE) == 0) {
    return itc_gbe();
  } else {
    fprintf(stderr, "Unknown test type %s\n", test_type);
    exit(EXIT_FAILURE);
  }
}

int main(argc, argv)
int argc;                               /* Number of arguments. */
char **argv;                            /* Argument array. */
{
  printf("Starting create_sboot.\n");

  get_message_timeout = GET_MESSAGE_TIMEOUT_DEFAULT;
  program_timeout = PROGRAM_TIMEOUT_DEFAULT;
  read_timeout = READ_TIMEOUT_DEFAULT;
  skip_read_timeout = SKIP_READ_TIMEOUT_DEFAULT;
  skip_program_timeout = SKIP_PROGRAM_TIMEOUT_DEFAULT;
  test_name = 0;
  l1_filename = 0;
  l2_filename = 0;
  sboot_filename = 0;
  test_type = 0;
  print_pass = PRINT_PASS_DEFAULT;
  print_results = PRINT_RESULTS_DEFAULT;
  verbose = FALSE;
  high_speed_uart = HIGH_SPEED_UART_DEFAULT;
  do_check_uart_divisor_low_speed = CHECK_UART_DIVISOR_LS_DEFAULT;
  do_check_uart_divisor_high_speed = CHECK_UART_DIVISOR_HS_DEFAULT;
  reset_before_divisor_check = RESET_BEFORE_DIVISOR_CHECK_DEFAULT;
  reset_seconds = RESET_SECONDS_DEFAULT;

  do_enable_maestro = ENABLE_MAESTRO_DEFAULT;
  maestro_post_reset_delay = MAESTRO_POST_RESET_DELAY_DEFAULT;
  maestro_power_on_delay = MAESTRO_POWER_ON_DELAY_DEFAULT;
  maestro_uart_configuration_delay = MAESTRO_UART_CONFIGURATION_DELAY_DEFAULT;

  bool l1_file_required = TRUE;

  diag_trace_off();

  if (argc) {
    // Skip the first argument, the command name.                               
    argc--;
    argv++;
  }

  while (argc--) {
    char *arg = *argv++;

    if (strcmp(arg, CHECK_UART_DIVISOR_LS_OPTION) == 0) {
      do_check_uart_divisor_low_speed = TRUE;

    } else if (strcmp(arg, NO_CHECK_UART_DIVISOR_LS_OPTION) == 0) {
      do_check_uart_divisor_low_speed = FALSE;

    } else if (strcmp(arg, CHECK_UART_DIVISOR_HS_OPTION) == 0) {
      do_check_uart_divisor_high_speed = TRUE;

    } else if (strcmp(arg, NO_CHECK_UART_DIVISOR_HS_OPTION) == 0) {
      do_check_uart_divisor_high_speed = FALSE;

    } else if (strcmp(arg, ENABLE_MAESTRO_OPTION) == 0) {
      do_enable_maestro = TRUE;

    } else if (strcmp(arg, NO_ENABLE_MAESTRO_OPTION) == 0) {
      do_enable_maestro = FALSE;

    } else if (strcmp(arg, GET_MESSAGE_TIMEOUT_OPTION) == 0) {
      if (argc--) {
	char *timeout_string = *argv++;
	get_message_timeout = atoi(timeout_string);// TODO: do error checks, overflow check
      } else {
	fprintf(stderr, "Missing timeout argument for %s\n",
		GET_MESSAGE_TIMEOUT_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, HIGH_SPEED_UART_OPTION) == 0) {
      high_speed_uart = TRUE;

    } else if (strcmp(arg, NO_HIGH_SPEED_UART_OPTION) == 0) {
      high_speed_uart = FALSE;

    } else if (strcmp(arg, LEVEL1_FILE_OPTION) == 0) {
      if (argc--) {
	l1_filename = *argv++;
      } else {
	fprintf(stderr, "Missing filename argument for %s\n",
		LEVEL1_FILE_OPTION);
	exit(EXIT_FAILURE);
      }
    } else if (strcmp(arg, NO_LEVEL1_FILE_OPTION) == 0) {
      l1_file_required = FALSE;

    } else if (strcmp(arg, LEVEL2_FILE_OPTION) == 0) {
      if (argc--) {
	l2_filename = *argv++;
      } else {
	fprintf(stderr, "Missing filename argument for %s\n",
		LEVEL2_FILE_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, MAESTRO_POST_RESET_DELAY_OPTION) == 0) {
      if (argc--) {
	char *delay_string = *argv++;
	maestro_post_reset_delay = atoi(delay_string);// TODO: do error checks
      } else {
	fprintf(stderr, "Missing delay argument for %s\n",
		MAESTRO_POST_RESET_DELAY_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, MAESTRO_POWER_ON_DELAY_OPTION) == 0) {
      if (argc--) {
	char *delay_string = *argv++;
	maestro_power_on_delay = atoi(delay_string);// TODO: do error checks
      } else {
	fprintf(stderr, "Missing delay argument for %s\n",
		MAESTRO_POWER_ON_DELAY_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, PROGRAM_TIMEOUT_OPTION) == 0) {
      if (argc--) {
	char *timeout_string = *argv++;
	program_timeout = atoi(timeout_string);// TODO: do error checks, overflow check
      } else {
	fprintf(stderr, "Missing timeout argument for %s\n",
		PROGRAM_TIMEOUT_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, READ_TIMEOUT_OPTION) == 0) {
      if (argc--) {
	char *timeout_string = *argv++;
	read_timeout = atoi(timeout_string);// TODO: do error checks, overflow check
      } else {
	fprintf(stderr, "Missing timeout argument for %s\n",
		READ_TIMEOUT_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, SKIP_READ_TIMEOUT_OPTION) == 0) {
      skip_read_timeout = TRUE;

    } else if (strcmp(arg, NO_SKIP_READ_TIMEOUT_OPTION) == 0) {
      skip_read_timeout = FALSE;

    } else if (strcmp(arg, SKIP_PROGRAM_TIMEOUT_OPTION) == 0) {
      skip_program_timeout = TRUE;

    } else if (strcmp(arg, NO_SKIP_PROGRAM_TIMEOUT_OPTION) == 0) {
      skip_program_timeout = FALSE;

    } else if (strcmp(arg, MAESTRO_UART_CONFIGURATION_DELAY_OPTION) == 0) {
      if (argc--) {
	char *delay_string = *argv++;
	maestro_uart_configuration_delay = atoi(delay_string);// TODO: do error checks
      } else {
	fprintf(stderr, "Missing delay argument for %s\n",
		MAESTRO_UART_CONFIGURATION_DELAY_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, SBOOT_FILE_OPTION) == 0) {
      if (argc--) {
	sboot_filename = *argv++;
      } else {
	fprintf(stderr, "Missing filename argument for %s\n",
		SBOOT_FILE_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, TEST_NAME_OPTION) == 0) {
      if (argc--) {
	test_name = *argv++;
      } else {
	fprintf(stderr, "Missing test name argument for %s\n",
		TEST_NAME_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, TEST_TYPE_OPTION) == 0) {
      if (argc--) {
	test_type = *argv++;
      } else {
	fprintf(stderr, "Missing filename argument for %s\n",
		TEST_TYPE_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, PRINT_PASS_OPTION) == 0) {
      print_pass = TRUE;
    } else if (strcmp(arg, NO_PRINT_PASS_OPTION) == 0) {
      print_pass = FALSE;

    } else if (strcmp(arg, PRINT_RESULTS_OPTION) == 0) {
      print_results = TRUE;
    } else if (strcmp(arg, NO_PRINT_RESULTS_OPTION) == 0) {
      print_results = FALSE;

    } else if (strcmp(arg, RESET_BEFORE_DIVISOR_CHECK_OPTION) == 0) {
      reset_before_divisor_check = TRUE;
    } else if (strcmp(arg, NO_RESET_BEFORE_DIVISOR_CHECK_OPTION) == 0) {
      reset_before_divisor_check = FALSE;

    } else if (strcmp(arg, RESET_SECONDS_OPTION) == 0) {
      if (argc--) {
	char *seconds_string = *argv++;
	reset_seconds = atoi(seconds_string);// TODO: do error checks
      } else {
	fprintf(stderr, "Missing seconds argument for %s\n",
		RESET_SECONDS_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, VERBOSE_OPTION) == 0) {
      verbose = TRUE;
    } else if (strcmp(arg, NO_VERBOSE_OPTION) == 0) {
      verbose = FALSE;

    } else {
      fprintf(stderr, "Unknown option \"%s\"\n", arg);
      exit(EXIT_FAILURE);
    }
  }

  // Check if some required options hae been supplied.
  bool missing_option = FALSE;

  if (!test_name) {
    fprintf(stderr, "Please supply %s and the test name.\n",
	    TEST_NAME_OPTION);
    missing_option = TRUE;
  }
  if (!test_type) {
    fprintf(stderr, "Please supply %s and the test type.\n",
	    TEST_TYPE_OPTION);
    missing_option = TRUE;
  }
  if (l1_file_required && !l1_filename) {
    fprintf(stderr, "Please supply %s and the level 1 boot filename.\n",
	    LEVEL1_FILE_OPTION);
    missing_option = TRUE;
  }
  if (!sboot_filename) {
    fprintf(stderr, "Please supply %s and the sboot output filename.\n",
	    SBOOT_FILE_OPTION);
    missing_option = TRUE;
  }

  // Unless this is TEST_TYPE_L1 or TEST_TYPE_L1_MEM, we'll need to see
  // a level 2 boot file.
  //
  // TODO: This code is fragile.  It would be better to create a test type
  // structure that encodes whether or not a level 2 boot file is needed.
  bool please_open_l2_file = FALSE;
  if (l1_file_required) {
    if (strcmp(test_type, TEST_TYPE_L1) != 0 &&
	strcmp(test_type, TEST_TYPE_L1_MEM) != 0) {
      if (!l2_filename) {
	fprintf(stderr, "Please supply %s and the level 2 boot filename.\n",
		LEVEL2_FILE_OPTION);
	missing_option = TRUE;
      } else {
	please_open_l2_file = TRUE;
      }
    }
  }

  if (missing_option) {
    exit(EXIT_FAILURE);
  }

  if (l1_file_required) {
    // We've done our option checks.  Let's open the input files.
    l1_file = fopen(l1_filename, "rb");
    if (!l1_file) {
      perror(l1_filename);
      exit(EXIT_FAILURE);
    }
    
    if (please_open_l2_file) {
      l2_file = fopen(l2_filename, "rb");
      if (!l2_file) {
	perror(l1_filename);
	exit(EXIT_FAILURE);
      }
    }
  }

  // TODO: There are exit paths that don't close l1_file and
  // l2_file.  We should be a little neater than that.

  // The preliminaries are over.  Prepare to start generating the
  // sboot command stream into memory.
  init_sboot_encoding();
  set_sboot_encoding_verbose(verbose);

  // Generate some info headers:
  begin_program_info();
  store_program_info_test_name(test_name);
  store_program_info_test_type(test_type);
  if (l1_file_required) {
    store_program_info_l1_file_name(l1_filename);
    {
      struct stat statbuf;
      if (fstat(fileno(l1_file), &statbuf)) {
	perror(l1_filename);
	exit(EXIT_FAILURE);
      }
      store_program_info_l1_file_time(statbuf.st_mtime);
    }
    if (please_open_l2_file) {
      store_program_info_l2_file_name(l2_filename);
      
      struct stat statbuf;
      if (fstat(fileno(l2_file), &statbuf)) {
	perror(l2_filename);
	exit(EXIT_FAILURE);
      }
      store_program_info_l2_file_time(statbuf.st_mtime);
    }
  }

  // TODO: Should use getlogin_r(...):
  char *username = getlogin();
  if (username) {
    store_program_info_sboot_creation_by(username);
  }

// TODO: find the proper include file:
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif // HOST_NAME_MAX
  char hostnamebuf[HOST_NAME_MAX + 1];
  if (gethostname(hostnamebuf, sizeof(hostnamebuf)) == 0) {
    store_program_info_sboot_creation_host(hostnamebuf);
  } else {
    perror("error in gethostname");
    exit(EXIT_FAILURE);
  }

  store_program_info_sboot_creation_time(time(0));
  end_program_info();

  if (do_enable_maestro) {
    enable_maestro(SBOOT_POWER_ON_MAESTRO);
    if (maestro_power_on_delay > 0) {
      wait(maestro_power_on_delay);
    }
    enable_maestro(SBOOT_CONFIGURE_MAESTRO_UART);
    if (maestro_uart_configuration_delay > 0) {
      wait(maestro_uart_configuration_delay);
    }
  }

  if (reset_before_divisor_check) {
    reset_maestro_and_delay();
  }

  if (!skip_read_timeout) {
    set_read_timeout(read_timeout);
  }

  if (!skip_program_timeout) {
    set_program_timeout(program_timeout);
  }

  if (do_check_uart_divisor_low_speed) {
    check_uart_divisor_low_speed();
  }

  if (high_speed_uart) {
    change_uart_to_high_speed();
  }

  if (do_check_uart_divisor_high_speed) {
    check_uart_divisor_high_speed();
  }

  // This goes... somewhere.  Before the uart divisor check?  After?
  startup();

  if (select_test_harness_and_generate()) {
    if (verbose) {
      debug("Test passed.");
    }
    all_done();
  } else {
    fail(SBOOT_SHOUT_ERROR_GENERATING_SBOOT,
	 "Error generating the sboot command stream.");
  }

  check_for_unresolved_labels();

  // Update certain program info headers.
  finish_program_info();

  // Neaten up a bit.
  if (l1_file_required) {
    fclose(l1_file); // TODO: check for error.
    if (please_open_l2_file) {
      fclose(l2_file); // TODO: check for error.
    }
  }

  // We've generated the sboot command stream.  Let's open the
  // output file and write the sboot command stream to it:
  sboot_file = fopen(sboot_filename, "wb");
  if (!sboot_file) {
    perror(sboot_filename);
    exit(EXIT_FAILURE);
  }
  sboot_size_t buffer_len = get_sboot_buffer_len();
  sboot_size_t written_len =
    fwrite(get_sboot_buffer(), sizeof(uint8), buffer_len, sboot_file);
  if (written_len != buffer_len) {
    perror(sboot_filename);
    fclose(sboot_file);// This might not be needed.
    exit(EXIT_FAILURE);
  }
  fclose(sboot_file); // TODO: check for error.
  printf("Ending create_sboot.\n");
}
