// sboot_features.h  03-Nov-2011 Craig Milo Rogers <rogers@isi.edu>

#ifndef __SBOOT_FEATURES_H
#define __SBOOT_FEATURES_H

// These features control whether or not certain structures in the sboot
// input stream are encoded more compactly when possible.  Disabling
// these features will decrease the size of the runtime code, at the
// expense of increasing the size of the sboot input stream and
// donsequently increasing the time it takes to process the sboot
// input stream.

// Define SBOOT_OPTIMIZE_IMMEDIATES to encode immediate data
// operands more compactly when possible.
#define SBOOT_OPTIMIZE_IMMEDIATES

// Define SBOOT_OPTIMIZE_JUMPS to encode backwards jumps more
// compactly when possible.
#define SBOOT_OPTIMIZE_JUMPS

// Define SBOOT_OPTIMIZE_CALLS to encode subroutine calls more
// compactly when possible.
#define SBOOT_OPTIMIZE_CALLS

// sboot maintains a virtual machine call stack.  How big does it
// need to be?
#define SBOOT_CALL_STACK_SIZE 6

// How big a buffer should we use for building string messages
// from MAESTRO prior to shouting them?  Messages that exceed this
// length will be split into multiple shouts.
#define SBOOT_STRING_MSG_BUF_MAX_LEN 80

// How many commands should we execute before yielding?  This is a
// default value that may be overridden via the sboot_executing API.
// It may *not* be overridden from within a program.
// This value should be limited to a uint16.
#define SBOOT_EXECUTE_MAX_COMMANDS_BEFORE_YIELD_DEFAULT 20

// If SBOOT_TRACE is defined, code will be compiled that prints execution
// traces using printf(...).  A few static variables will be defined to assist
// with debugging.
//
// #define SBOOT_TRACE

// This is the default read timeout.
#define SBOOT_READ_TIMEOUT_DEFAULT 60

// This is the default program timeout.
#define SBOOT_PROGRAM_TIMEOUT_DEFAULT 60

// Is the program timeout enabled by default?
#define SBOOT_ENABLE_PROGRAM_TIMEOUT_DEFAULT TRUE

// Is the program timeout overidden by default?
#define SBOOT_OVERRIDE_PROGRAM_TIMEOUT_DEFAULT FALSE

// Maximum length for a test name.
#define SBOOT_TEST_NAME_MAX_LEN 80

#endif // __SBOOT_FEATURES_H
