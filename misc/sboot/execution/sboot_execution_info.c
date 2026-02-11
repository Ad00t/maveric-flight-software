// sboot_execution_info.c  26-Dec-2011 Craig Milo Rogers <rogers@isi.edu>


#if defined(__PCD__) && !defined(__24FJ256GA110_H__)
#include "../../common.h"
#define __24FJ256GA110_H__
#endif // defined(__PCD__)

#include "../includes/sboot_types.h"
#include "../includes/sboot_shout.h"
#include "../includes/sboot_program.h"
#include "../includes/sboot_program_info.h"
#include "../includes/sboot_callbacks.h"
#include "../includes/sboot_execution_shout.h"

// TODO:  This code uses make16(...) and make32(...).
// There should be a header file to define them.

// Returns a uint8 byte from a program, or the special value
// SBOOT_FETCH_ERROR.  Increments the offset on success,
// but not on error.
static uint16 sboot_fetch_info_byte_inc(sboot_program_number_t program_number,
					sboot_size_t *offsetp) {
  sboot_size_t offset = *offsetp;
  uint16 result;
  result = sboot_fetch_program_byte(program_number, offset);
  if (result == SBOOT_FETCH_ERROR) {
    return result;
  }
  offset = offset + 1; // Avoid possible compiler bug
  *offsetp = offset;
  return result;
}

// Extracts a uint16 word from a program, incrementing the offset.
// Returns TRUE if successful, else returns FALSE.
static bool sboot_fetch_info_uint16(sboot_program_number_t program_number,
				    sboot_size_t *offsetp, uint16 *result) {
  uint16 byte0;
  byte0 = sboot_fetch_info_byte_inc(program_number, offsetp);
  if (byte0 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte1;
  byte1 = sboot_fetch_info_byte_inc(program_number, offsetp);
  if (byte1 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  // Do the following in steps to avoid a possible compiler error:
  uint16 value;
  value = make16((uint8)byte1, (uint8)byte0);
  *result = value;
 return TRUE;
}

// Extracts a uint32 word from the command program, incrementing
// the offset.  Returns TRUE if successful, else returns FALSE.
static bool sboot_fetch_info_uint32(sboot_program_number_t program_number,
				    sboot_size_t *offsetp, uint32 *result) {
  uint16 byte0;
  byte0 = sboot_fetch_info_byte_inc(program_number, offsetp);
  if (byte0 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte1;
  byte1 = sboot_fetch_info_byte_inc(program_number, offsetp);
  if (byte1 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte2;
  byte2 = sboot_fetch_info_byte_inc(program_number, offsetp);
  if (byte2 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }
  uint16 byte3;
  byte3 = sboot_fetch_info_byte_inc(program_number, offsetp);
  if (byte3 == SBOOT_FETCH_ERROR) {
    return FALSE;
  }

  *result = make32((uint8)byte3, (uint8)byte2, (uint8)byte1, (uint8)byte0);
  return TRUE;
}

// Look for a specific piece of program info.
//
// Returns TRUE if it found the info, with returned_len set to the
// info length and with the input_offset pointing to the value of
// the info tag.  Otherwise, returns FALSE.
static bool sboot_find_program_info(sboot_program_info_t desired_info_type,
				    sboot_program_number_t program_number,
				    sboot_size_t *offsetp,
				    uint16 *returned_len) {

  // Start processing at the beginning of the specified program:
  sboot_size_t offset;
  offset = 0; // Avoid possible compiler bug.

  for (;;) {
    // Fetch the new command:
    uint16 command;
    command = sboot_fetch_info_byte_inc(program_number, &offset);
    if (command == SBOOT_FETCH_ERROR) {
      // TODO: give a feedback message.
      return FALSE;
    }

    if (command != COMMAND_PROGRAM_INFO) {
      // TODO: give a feedback message.
      return FALSE;
    }

    uint16 info_type;
    info_type = sboot_fetch_info_byte_inc(program_number, &offset);
    if (info_type == SBOOT_FETCH_ERROR) {
      // TODO: give a feedback message.
      return FALSE;
    }

    uint16 info_length;
    info_length = sboot_fetch_info_byte_inc(program_number, &offset);
    if (info_length == SBOOT_FETCH_ERROR) {
      // TODO: give a feedback message.
      return FALSE;
    }

    if (info_type == SBOOT_INFO_END) {
      // We should have seen something by now.
      // TODO: give a feedback message.
      return FALSE;
    }

    if (info_type == desired_info_type) {
      *returned_len = info_length;
      *offsetp = offset;
      return TRUE;
    }

    offset = offset + info_length; // Avoid possible compiler bug.
  }
}


// Transfer a string from the specified program into the shout buffer,
// truncating on overflow.  Returns FALSE on program exhaustion, TRUE
// otherwise (eveon on output truncation).
//
// WARNING:  This code uses the shout buffer in the execution workspace.
static bool sboot_shout_info_stream(sboot_program_number_t program_number,
				    sboot_size_t *offsetp,
				    uint8 len) {
  while (len--) {
    uint16 result = sboot_fetch_info_byte_inc(program_number, offsetp);
    if (result == SBOOT_FETCH_ERROR) {
      return FALSE;
    }

    if (!sboot_shout_uchar((uchar)result)) {
      break;
    }
  }
  return TRUE;
}

// Convenience method to sent a debug message containing the info value 
// that's immediately next in the input stream.
//
// WARNING:  This code uses the shout buffer in the execution workspace.
static void sboot_shout_info_keyval(string_literal msg,
				    sboot_program_number_t program_number,
				    sboot_size_t *offsetp,
				    uint16 len) {
  sboot_start_shouting(SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_PROGRAM_INFO);
  sboot_shout_string(msg);
  sboot_shout_info_stream(program_number, offsetp, (uint8)len);
  sboot_send_shout_done();
}

// Create debugging messages out of the INFO data
// that we expect to find at the start of the program.
//
// WARNING:  This code uses the shout buffer in the execution workspace.
//
// WARNING: This routine will generate multiple lines of debugging
// output before returning, which might interfere with other operations.
//
// Returns TRUE on normal processing, FALSE if an error occurs.
bool sboot_show_program_info(sboot_program_number_t program_number) {
  // Start processing at the beginning of the specified program:
  sboot_size_t offset;
  offset = 0; // Avoid possible compiler bug.

  for (;;) {
    // Fetch the new command:
    uint16 command;
    command = sboot_fetch_info_byte_inc(program_number, &offset);
    if (command == SBOOT_FETCH_ERROR) {
      // TODO: give a feedback message.
      return FALSE;
    }

    if (command != COMMAND_PROGRAM_INFO) {
#if defined(__PCD__) && defined(__MAESTRO__)
      sendDBGALL(userPort, "\n\rsboot_show_program_info: not COMMAND_PROGRAM_INFO\r\n");
#endif
      return FALSE;
    }

    uint16 info_type;
    info_type = sboot_fetch_info_byte_inc(program_number, &offset);
    if (info_type == SBOOT_FETCH_ERROR) {
      // TODO: give a feedback message.
      return FALSE;
    }

    uint16 info_length;
    info_length = sboot_fetch_info_byte_inc(program_number, &offset);
    if (info_length == SBOOT_FETCH_ERROR) {
      // TODO: give a feedback message.
      return FALSE;
    }

    if (info_type == SBOOT_INFO_END) {
      // This is the normal exit, no need for a feedback message.
      return TRUE;
    }

    switch ((sboot_program_info_t)info_type) {
    case SBOOT_INFO_END: {
      // This case should be impossible to reach.
      return FALSE;
    }

    case SBOOT_INFO_LENGTH: {
      uint32 program_length;
      if (!sboot_fetch_info_uint32(program_number, &offset,
				      &program_length)) {
	// TODO: give a feedback message.
	return FALSE;
      }
      sboot_start_shouting(SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_PROGRAM_INFO);
      sboot_shout_string("Length: ");
      sboot_shout_hex_value(program_length);
      sboot_send_shout_done();
      break;
    }

    case SBOOT_INFO_CRC: {
      uint16 crcval;
      if (!sboot_fetch_info_uint16(program_number, &offset, &crcval)) {
	// TODO: give a feedback message.
	return FALSE;
      }
      sboot_start_shouting(SBOOT_SHOUT_DEBUG, SBOOT_SHOUT_PROGRAM_INFO);
      sboot_shout_string("CRC: ");
      sboot_shout_hex_value(crcval);
      sboot_send_shout_done();
      break;
    }

    case SBOOT_INFO_TEST_NAME: {
      sboot_shout_info_keyval("Test name: ", program_number, &offset, info_length);
      break;
    }

    case SBOOT_INFO_TEST_TYPE: {
      sboot_shout_info_keyval("Test harness type: ", program_number, &offset, info_length);
      break;
    }

    case SBOOT_INFO_L1_FILE_NAME: {
      sboot_shout_info_keyval("L1 file: ", program_number, &offset, info_length);
      break;
    }

    case SBOOT_INFO_L1_FILE_TIME: {
      sboot_shout_info_keyval("L1 file time: ", program_number, &offset, info_length);
      break;
    }

    case SBOOT_INFO_L2_FILE_NAME: {
      sboot_shout_info_keyval("L2 file: ", program_number, &offset, info_length);
      break;
    }

    case SBOOT_INFO_L2_FILE_TIME: {
      sboot_shout_info_keyval("L2 file time: ", program_number, &offset, info_length);
      break;
    }

    case SBOOT_INFO_SBOOT_CREATION_TIME: {
      sboot_shout_info_keyval("SBOOT creation time: ", program_number, &offset, info_length);
      break;
    }

    case SBOOT_INFO_SBOOT_CREATION_BY: {
      sboot_shout_info_keyval("SBOOT created by: ", program_number, &offset, info_length);
      break;
    }

    case SBOOT_INFO_SBOOT_CREATION_HOST: {
      sboot_shout_info_keyval("SBOOT created on: ", program_number, &offset, info_length);
      break;
    }

    default:
      offset = offset + info_length; // Avoid possible compile bug.
    }
  }  
}


// Look for a specific piece of program info.
//
// Returns TRUE if it found the info, with returned_len set to the
// full info length and with the returned_value_buffer containing the
// (possibly truncated) info tag.  Otherwise, returns FALSE.
//
// Note: If you want to use the returned value as a null-terminated
// string, it's the caller's job to null-terminate it.
bool sboot_get_program_info(sboot_program_number_t program_number,
			    sboot_program_info_t desired_info_type,
			    uchar *returned_value_buf,
			    uint16 returned_value_max_len,
			    uint16 *returned_len) {

  sboot_size_t offset;
  uint16 len;
  if (!sboot_find_program_info(desired_info_type, program_number, &offset, &len)) {
    return FALSE;
  }
  *returned_len = len;

  uint16 i;
  for (i = 0; i < len; i++) {
    uint16 value_byte;
    value_byte = sboot_fetch_info_byte_inc(program_number, &offset);
    if (value_byte == SBOOT_FETCH_ERROR) {
      // TODO: give a feedback message.
      return FALSE;
    }
    if (i < returned_value_max_len) {
      *returned_value_buf = (uchar)value_byte;
      returned_value_buf = returned_value_buf + 1; // Avoid possible compiler bug. 
    }
  }
  return TRUE;
}

// look for the program length.
//
// Returns TRUE if it found the program length, which is stored
// in "length".  Otherwise, returns FALSE.
bool sboot_get_program_length(sboot_program_number_t program_number,
			      sboot_size_t *lengthp) {

  sboot_size_t offset;
  uint16 info_length_ignored;
  if (!sboot_find_program_info(SBOOT_INFO_LENGTH, program_number, &offset, &info_length_ignored)) {
    // TODO: give a feedback message.
    return FALSE;
  }

  uint32 program_length;
  if (!sboot_fetch_info_uint32(program_number, &offset, &program_length)) {
    // TODO: give a feedback message.
    return FALSE;
  }

  // Return the program length.  Do this operation in small steps to avoid
  // a possible compiler error:
  sboot_size_t length;
  length = (sboot_size_t) program_length;
  *lengthp = length;
  return TRUE;
}


// Check the CRC of the program.
//
// Returns TRUE if the CRC is OK, else FALSE.
bool sboot_check_program_crc(sboot_program_number_t program_number) {
  sboot_size_t program_length;
  if (!sboot_get_program_length(program_number, &program_length)) {
    // TODO: give a feedback message.
    return FALSE;
  }

  sboot_size_t program_offset;
  uint16 info_length_ignored;
  if (!sboot_find_program_info(SBOOT_INFO_CRC, program_number, &program_offset, &info_length_ignored)) {
    // TODO: give a feedback message.
    return FALSE;
  }

  uint16 stored_crcval;
  if (!sboot_fetch_info_uint16(program_number, &program_offset, &stored_crcval)) {
    // TODO: give a feedback message.
    return FALSE;
  }

  // Calculate how many bytes to scan to form the CRC.
  sboot_size_t remaining_length;
  remaining_length = program_length - program_offset;

  // The following code was vilely stolen from "crc.c", then
  // modified to use local types, data fetches, etc.:
  uint16 crcval;
  crcval = 0xffff; // CCITT CRC                                                                                                                                                  

  sboot_size_t i;
  for (i = 0; i < remaining_length; i++) {
    uint16 program_byte;
    program_byte = sboot_fetch_info_byte_inc(program_number, &program_offset);
    if (program_byte == SBOOT_FETCH_ERROR) {
      // TODO: give feedback message;
      return FALSE;
    }
    uint16 t;
    t = crcval ^ program_byte;
    t = (t ^ (t << 4)) & 0xff;
    crcval = (crcval >> 8) ^ (t << 8) ^ (t << 3) ^ (t >> 4);
  }
  crcval = ~crcval;

  if (crcval == stored_crcval) {
    return TRUE;
  } else {
    // TODO: give feedback message;
    return FALSE;
  }
}
