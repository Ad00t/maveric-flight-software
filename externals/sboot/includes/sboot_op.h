// sboot_test_op.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

// This file defines the logic operations used by the the Maestro serial
// bootstrap code.

#ifndef __SBOOT_OP_H
#define __SBOOT_OP_H

// Operation take three arguments.
//
// COMMAND_OP_WREG_UINT32_WREG
// The first source is a word register.
// The second source is a uint32 constant.
// The destination is a word register.
//
// COMMAND_OP_WREG_UINT16_WREG
// The first source is a word register.
// The second source is a uint16 constant.
// The destination is a word register.
//
// COMMAND_OP_WREG_UINT8_WREG
// The first source is a word register.
// The second source is a uint8 constant.
// The destination is a word register.
//
// COMMAND_OP_WREG_ZERO_WREG
// The first source is a word register.
// The second source is constant zero.
// The destination is a word register.
// This is a good encoding for OP_NOT and OP_INVERT_BITS.

enum sboot_op_enum {
  
  // arithmetic operations.  The "reversed" variants are made available
  // is because immediate values are supported only as the second operand
  // at the present time.
  OP_ADD,            // 0
  OP_SUB,            // 1
  OP_SUB_REVERSED,   // 2
  OP_MUL,            // 3
  OP_DIV,            // 4
  OP_DIV_REVERSED,   // 5
  OP_MOD,            // 6
  OP_MOD_REVERSED,   // 7

  // comparison operations:
  OP_EQ,             // 8
  OP_NE,             // 9
  OP_GT,             // 10
  OP_GE,             // 11
  OP_LT,             // 12
  OP_LE,             // 13

  // set membership operations:
  OP_IN,             // 14
  OP_NOT_IN,         // 15

  // logical operations:
  OP_AND,            // 16
  OP_OR,             // 17
  OP_XOR,            // 18

  // logical operation, ignoring second argument:
  OP_NOT,            // 19

  // shift operations:
  OP_SHIFT_RIGHT,    // 20
  OP_SHIFT_LEFT,     // 21

  // bitwise logical operations:
  OP_AND_BITS,       // 22
  OP_OR_BITS,        // 23
  OP_XOR_BITS,       // 24 // bitwise OP_NE
  OP_NAND_BITS,      // 25
  OP_NOR_BITS,       // 26
  OP_XNOR_BITS,      // 27 // bitwise OP_EQ

  // bitwise logical operation, ignoring second argument:
  OP_INVERT_BITS,    // 28

  // bitmask operations, resulting in logical values:
  OP_ALL_ZEROS_UNDER_MASK,     // 29
  OP_NOT_ALL_ZEROS_UNDER_MASK, // 30
  OP_ALL_ONES_UNDER_MASK,      // 31
  OP_NOT_ALL_ONES_UNDER_MASK,  // 32

  // special timeout comparison operator, allowing for rollover.
  // The first operand is the start of the timeout (from current_seconds).
  // The second operand is the duration of the timeout.
  // current_second() is an implicit third operand.
  // The destination gets TRUE if the timeout has passed, else FALSE.
  OP_TIMEOUT                   // 33
};

typedef enum sboot_op_enum sboot_op_t;

#endif // __SBOOT_OP_H
