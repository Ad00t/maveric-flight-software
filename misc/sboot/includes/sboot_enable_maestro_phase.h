// sboot_enable_maestro_phase.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

// This file defines the execution state values used by the
// Maestro serial bootstrap code.

#ifndef __SBOOT_ENABLE_MAESTRO_PHASE_H
#define __SBOOT_ENABLE_MAESTRO_PHASE_H

enum sboot_enable_maestro_phase_enum {
  SBOOT_POWER_ON_MAESTRO,
  SBOOT_CONFIGURE_MAESTRO_UART
};

typedef enum sboot_enable_maestro_phase_enum sboot_enable_maestro_phase_t;

#endif // __SBOOT_ENABLE_MAESTRO_PHASE_H
