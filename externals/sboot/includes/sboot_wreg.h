// sboot_wreg.h  28-Nov-2011 Craig Milo Rogers <rogers@isi.edu>

// The wreg (word registers) are 32-bit words stored in the workspace.

#ifndef __SBOOT_WREG_H
#define __SBOOT_WREG_H

// Lots of code assumes that there will be no more than 255 WREG entries.
enum wreg_enum {
  // Scratch registers.  Do not count on these registers
  // being preserved across subroutine calls!
  WREG0,
  WREG1,
  WREG2,
  WREG3,

  // A scratch register used by some higher-level tests:
  // fail_if_not_in_list(...)
  // fail_if_immediate(...)
  // go_to_if_not_in_list(...)
  // go_to_if_immediate(...)
  // go_to_if_wreg(...)
  // if_immediate(...)
  // if_wreg(...)
  // if_list(...)
  WREG_TEST,

  // The message buffer, shared between:
  // watch_for_message(...)
  // get_message(...)
  // check_for_message(...)
  WREG_MSG_TYPE,
  WREG_SENDER_X,
  WREG_SENDER_Y,
  WREG_RSHIM_SCRATCHPAD,
  WREG_SPI_SCRATCHPAD,

  // watch_for_message(...) parameters:
  WREG_WFM_TARGET_MESSAGE, // boot message type
  WREG_WFM_ALLOW_TARGET_MESSAGE_ONLY, // bool

  // get_message(...) variables:
  WREG_GM_START_TIMEOUT, // sboot_timer_t

  // check_for_message(...) parameters:
  WREG_CFM_DEQUEUE, // bool

  // *********************************
  // The following registers are for use by utilities, such
  // as the CRC check.  We might want them to overlay them
  // on top of the proceeding registers.
  // *********************************
  WREG_PROGRAM_NUMBER,
  WREG_PROGRAM_OFFSET,

  // find_program_info(...) parameters and returns:
  WREG_INFO_TYPE,
  WREG_INFO_LENGTH,

  // get_program_length(...)
  WREG_PROGRAM_LENGTH,

  // crc_test()
  WREG_FAILURE_COUNT,
  WREG_SUCCESS_COUNT,
  WREG_PROGRAMS_LEFT,

  // *********************************
  SBOOT_NUMBER_OF_WORD_REGISTERS
};
typedef enum wreg_enum wreg_t;

#endif // __SBOOT_WREG_H
