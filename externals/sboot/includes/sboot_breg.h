// sboot_breg.h  28-Nov-2011 Craig Milo Rogers <rogers@isi.edu>

// The breg (byte registers) are scratchpads used in the workspace
// to assemble data that was received from the MAESTRO.
//
// TODO: The breg workspace can be eliminated by reading into
// wreg registers.

#ifndef __SBOOT_BREG_H
#define __SBOOT_BREG_H

// Code assumes that there will be no more than 255 BREG entries.
enum breg_enum {
  BREG0,
  BREG1,
  BREG2,
  BREG3,

  SBOOT_NUMBER_OF_BYTE_REGISTERS
};
typedef enum breg_enum breg_t;

#endif // __SBOOT_BREG_H
