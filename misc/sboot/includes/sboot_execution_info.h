#ifndef __SBOOT_EXECUTION_INFO_H
#define __SBOOT_EXECUTION_INFO_H

#include "sboot_types.h"

// Create debugging messages out of the INFO data
// that we expect to find at the start of the program.
//
// WARNING:  This code uses the shout buffer in the execution workspace.//
//
// WARNING: This routine will generate multiple lines of debugging
// output before returning, which might interfere with other operations.
//
// Returns TRUE on normal processing, FALSE if an error occurs.
bool sboot_show_program_info(sboot_program_number_t program_number);

// Get a specific piece of INFO data.
//
// Returns TRUE on normal processing, FALSE if an error occurs.
bool sboot_get_program_info(sboot_program_number_t program_number,
			    sboot_program_info_t desired_info_type,
			    uchar *returned_value_buf,
			    uint16 returned_value_max_len,
			    uint16 *returned_len);

// Look for the program length.
//
// Returns TRUE if it found the program length, which is stored
// in "length".  Otherwise, returns FALSE.
bool sboot_get_program_length(sboot_program_number_t program_number,
			      sboot_size_t *lengthp);

// Check the CRC of the program.
//
// Returns TRUE if the CRC is OK, else FALSE.
bool sboot_check_program_crc(sboot_program_number_t program_number);

#endif // __SBOOT_EXECUTION_INFO_H
