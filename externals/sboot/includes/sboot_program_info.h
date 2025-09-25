// sboot_program_info.h  19-Dec-2011 Craig Milo Rogers <rogers@isi.edu>

// This file contains definitions used for encoding metadata about
// test programs for execution on the Maestro chip in the Aenas satellite.

#ifndef __SBOOT_PROGRAM_INFO__H
#define __SBOOT_PROGRAM_INFO__H

// The COMMAND_PROGRAM INFO header has a 1-byte info-type field
// followed by a 1-byte length field.  The contents following the
// header depends upon the info type.

enum sboot_program_info_enum {
  SBOOT_INFO_END,                 // Marks the end of the INFO entries for a program.
  SBOOT_INFO_LENGTH,              // 4 byte length field -- includes entire program.
  SBOOT_INFO_CRC,                 // 2 bytes -- CRC calculated with this field 0.
  SBOOT_INFO_TEST_NAME,           // Test name.
  SBOOT_INFO_TEST_TYPE,           // Test harness type.
  SBOOT_INFO_L1_FILE_NAME,        // L1 boot file name, might be truncated
  SBOOT_INFO_L1_FILE_TIME,        // YYYYMMDD.HHMMSS
  SBOOT_INFO_L2_FILE_NAME,        // L2 boot file name, might be truncated
  SBOOT_INFO_L2_FILE_TIME,        // YYYYMMDD.HHMMSS
  SBOOT_INFO_SBOOT_CREATION_TIME, // YYYYMMDD.HHMMSS
  SBOOT_INFO_SBOOT_CREATION_BY,   // user name
  SBOOT_INFO_SBOOT_CREATION_HOST  // system name
};

typedef enum sboot_program_info_enum sboot_program_info_t;

#endif // __SBOOT_PROGRAM_INFO_H
