// boot.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>
// boot_messages.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

/*
 * This file contains constants related to the Tilera bootstrap message
 * interface used in the Maestro (Tilera49) chip.  The constant names and
 * values that appear here were extracted from the file "boot.py", which
 * contained the following copyright notice:
 *
 * Copyright 2009 Tilera Corporation. All Rights Reserved.
 *
 *   The source code contained or described herein and all documents
 *   related to the source code ("Material") are owned by Tilera
 *   Corporation or its suppliers or licensors.  Title to the Material
 *   remains with Tilera Corporation or its suppliers and licensors. The
 *   software is licensed under the Tilera MDE License.
 *
 *   Unless otherwise agreed by Tilera in writing, you may not remove or
 *   alter this notice or any other notice embedded in Materials by Tilera
 *   or Tilera's suppliers or licensors in any way.
 */

#ifndef __MAESTRO_BOOT_H
#define __MAESTRO_BOOT_H

#define BOOT_MSG_NO_MSG        0
#define BOOT_MSG_PASS_MSG      1
#define BOOT_MSG_FAIL_MSG      2
#define BOOT_MSG_DONE_MSG      3
#define BOOT_MSG_RDY_MSG       4
#define BOOT_MSG_BOOT_PASS_MSG 5
#define BOOT_MSG_PROMPT_MSG    6
#define BOOT_MSG_TIMEOUT_MSG   7
#define BOOT_MSG_STRING_MSG    8
#define BOOT_MSG_INFO_MSG      10
#define BOOT_MSG_DEBUG         11

#define BOOT_HIGHEST_MSG_TYPE BOOT_MSG_DEBUG

#define BOOT_NW_CORNER_X 1 // Northwest corner of rectangle to boot
#define BOOT_NW_CORNER_Y 1 // Northwest corner of rectangle to boot
#define BOOT_SE_CORNER_X 7 // Southeast corner of rectangle to boot
#define BOOT_SE_CORNER_Y 7 // Southeast corner of rectangle to boot

#define BOOT_MSHIM_X 7 // Location of Mshim for L2 boot
#define BOOT_MSHIM_Y 0 // Location of Mshim for L2 boot

#define BOOT_START_ADDRESS 0x800 // Default address for starting L2 tests

#define BOOT_IS_DEFACED 0xdefaced // The chip has not been reset recently.

#define BOOT_UART_BLOCK_SIZE 0x2000  // 8K

#define BOOT_CHANGE_PLL_FREQ FALSE
#define BOOT_SYNC_ON_DN_READY TRUE

#define BOOT_TEST_MASK_LOW  0xffffffff
#define BOOT_TEST_MASK_HIGH 0xffffffff

#define RUN_BOOT_TEST_QUIET FALSE
#define USE_TIMEOUT_FOR_BOOT_TEST FALSE

#define BOOT_POST_CODE_LOAD_WAIT 0
#define BOOT_POST_BOOT_ARG_WAIT 0

#endif // __MAESTRO_BOOT_H
