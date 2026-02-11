// sboot_shout.h  21-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

// This file defines severity codes for messages issued by the sboot
// execution code.

#ifndef __SBOOT_SHOUT_H
#define __SBOOT_SHOUT_H

enum sboot_shout_severity_enum {
  SBOOT_SHOUT_DEBUG,   // Debugging information.
  SBOOT_SHOUT_MESSAGE, // A normal message we'd like someone to see.
  SBOOT_SHOUT_FAILURE  // Something very bad happened.
};

typedef enum sboot_shout_severity_enum sboot_shout_severity_t;

// Replace sboot_shout_reason_enum with uint16 and #define's
// to circumvent a CCS compiiler bug.
typedef uint16 sboot_shout_reason_t;
#define SBOOT_SHOUT_NONE                           0
#define SBOOT_SHOUT_MAESTRO_STRING_MESSSAGE        1
#define SBOOT_SHOUT_FETCH_INPUT_BYTE_ERROR         2
#define SBOOT_SHOUT_INVALID_BREG                   3
#define SBOOT_SHOUT_INVALID_WREG                   4
#define SBOOT_SHOUT_FETCH_DATA_ERROR               5
#define SBOOT_SHOUT_TIMEOUT_READING_UART           6
#define SBOOT_SHOUT_UNKNOWN_OPCODE                 7
#define SBOOT_SHOUT_UNKNOWN_LIST_OPCODE            8
#define SBOOT_SHOUT_DATA_OP_UNKNOWN_COMMAND        9
#define SBOOT_SHOUT_BAD_JUMP_OFFSET_WIDTH         10
#define SBOOT_SHOUT_BAD_JUMP_OFFSET_0             11
#define SBOOT_SHOUT_BAD_CALL_OFFSET_WIDTH         12
#define SBOOT_SHOUT_CALL_STACK_OVERFLOW           13
#define SBOOT_SHOUT_CALL_STACK_UNDERFLOW          14
#define SBOOT_SHOUT_BAD_COMMAND                   15
#define SBOOT_SHOUT_UNINITIALIZED_EXECUTION_STATE 16
#define SBOOT_SHOUT_UNKNOWN_EXECUTION_STATE       17
#define SBOOT_SHOUT_FAIL_MSG_TILE_X               18
#define SBOOT_SHOUT_FAIL_MSG_TILE_Y               19
#define SBOOT_SHOUT_EXITING_DUE_TO_FAIL_MSG       20
#define SBOOT_SHOUT_BOOT_PASS_EXPECTED_TILE_X     21
#define SBOOT_SHOUT_BOOT_PASS_EXPECTED_TILE_Y     22
#define SBOOT_SHOUT_BOOT_PASS_GOT_TILE_X          23
#define SBOOT_SHOUT_BOOT_PASS_GOT_TILE_Y          24
#define SBOOT_SHOUT_BOOT_PASS_MSG_FROM_WRONG_TILE 25
#define SBOOT_SHOUT_LEVEL_1_BOOT_TEST_FAILED_RSHIM 26
#define SBOOT_SHOUT_LEVEL_1_BOOT_TEST_FAILED_SPI  27
#define SBOOT_SHOUT_LEVEL_1_BOOT_TEST_FAILED      28
#define SBOOT_SHOUT_UNABLE_TO_BOOT_THROUGH_RSHIM  29
#define SBOOT_SHOUT_ERROR_GENERATING_SBOOT        30
#define SBOOT_SHOUT_WRONG_CHIP_REV                31
#define SBOOT_SHOUT_WRONG_SHIM_DISABLE            32
#define SBOOT_SHOUT_GET_MESSAGE_TIMED_OUT         33
#define SBOOT_SHOUT_UNEXPECTED_MESSAGE            34
#define SBOOT_SHOUT_UNEXPECTED_MESSAGE_EXITING    35
#define SBOOT_SHOUT_NONZERO_DATA_TILE_X           36
#define SBOOT_SHOUT_NONZERO_DATA_TILE_Y           37
#define SBOOT_SHOUT_NONZERO_DATA                  38
#define SBOOT_SHOUT_NONZERO_DATA_IN_MESSAGE       39
#define SBOOT_SHOUT_RSHIM_ACCESS_FAILED_LOW_SPEED 40
#define SBOOT_SHOUT_RSHIM_ACCESS_FAILED_HIGH_SPEED 41
#define SBOOT_SHOUT_CONVERTING_MESSAGE_TO_FAIL    42
#define SBOOT_SHOUT_LEVEL_2_TEST_PASSED           43
#define SBOOT_SHOUT_LEVEL_1_BOOT_TEST_PASSSED     44
#define SBOOT_SHOUT_UNRECOGNIZED_BOOT_MESSAGE     45
#define SBOOT_SHOUT_TILE_X                        46
#define SBOOT_SHOUT_TILE_Y                        47
#define SBOOT_SHOUT_PROGRAM_INFO                  48
#define SBOOT_SHOUT_SHUTDOWN_REQUESTED            49
#define SBOOT_SHOUT_INVALID_BYTE_INDEX            50
#define SBOOT_SHOUT_CRC_TEST_PASSED               51
#define SBOOT_SHOUT_CRC_TEST_FAILED               52
#define SBOOT_SHOUT_AT_LEAST_ONE_CRC_TEST_FAILED  53
#define SBOOT_SHOUT_FAILED_TO_CHANGE_UART_TO_HIGH_SPEED 54
#define SBOOT_SHOUT_FAILED_TO_CHANGE_UART_TO_LOW_SPEED  55
#define SBOOT_SHOUT_CRC_FAILED_TO_GET_PROGRAM_LENGTH    56
#define SBOOT_SHOUT_CRC_PROGRAM_BYTE_FETCH_FAILED       57
#define SBOOT_SHOUT_PROGRAM_TIMEOUT                     58
#define SBOOT_SHOUT_SHUTDOWN_REQUESTED_WHEN_ENABLING_MAESTRO 59
#define SBOOT_SHOUT_CRC_TEST_SUCCEEDED                       60
#define SBOOT_SHOUT_TEST_NAME                                61

#endif // __SBOOT_SHOUT_H
