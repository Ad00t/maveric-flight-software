// sboot_state.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

// This file defines the execution state values used by the
// Maestro serial bootstrap code.

#ifndef __SBOOT_EXECUTION_STATE_H
#define __SBOOT_EXECUTION_STATE_H

enum sboot_execution_state_enum {
  SBOOT_STATE_UNINITIALIZED =             0, // Should be 0 to detect failure to init.
  SBOOT_STATE_INITIALIZED =               1,
  SBOOT_STATE_STOPPED =                   2,
  SBOOT_STATE_RUNNING =                   3,
  SBOOT_STATE_WAITING =                   4,
  SBOOT_STATE_RESETTING_MAESTRO =         5,
  SBOOT_STATE_SENDING_DATA_TO_MAESTRO =   6,
  SBOOT_STATE_READING_DATA_FROM_MAESTRO = 7,
  SBOOT_STATE_FAILED =                    8,
  SBOOT_STATE_DONE =                      9,
  SBOOT_NUMBER_OF_EXECUTION_STATES
};

typedef enum sboot_execution_state_enum sboot_execution_state_t;

#endif // __SBOOT_EXECUTION_STATE_H
