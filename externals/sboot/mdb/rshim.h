// rshim.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

/*
 * This file contains constants related to the Tilera "rshim" interface used
 * in the Maestro (Tilera49) chip.  The constant names and values that appear
 * here were extracted from the file "rshim.py", which contained the following
 * copyright notice:
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

#ifndef __MAESTRO_RSHIM_H
#define __MAESTRO_RSHIM_H

#define RSHIM_BASELINE_STAT 0x000
#define RSHIM_BASELINE_CTL  0x004
#define RSHIM_SCRATCHPAD    0x00c

#define RSHIM_STN_DATA      0x600
#define RSHIM_STN_CTL       0x604

#define RSHIM_IDN_DATA_0    0x700
#define RSHIM_IDN_DATA_1    0x704
#define RSHIM_IDN_DATA_2    0x708
#define RSHIM_IDN_DATA_3    0x70c
#define RSHIM_IDN_DATA_4    0x710
#define RSHIM_IDN_DATA_5    0x714
#define RSHIM_IDN_DATA_6    0x718
#define RSHIM_IDN_DATA_7    0x71c
#define RSHIM_IDN_DATA_8    0x720
#define RSHIM_IDN_DATA_9    0x724
#define RSHIM_IDN_DATA_10   0x728
#define RSHIM_IDN_DATA_11   0x72c
#define RSHIM_IDN_DATA_12   0x730
#define RSHIM_IDN_DATA_13   0x734
#define RSHIM_IDN_DATA_14   0x738
#define RSHIM_IDN_DATA_15   0x73c
#define RSHIM_IDN_CTL       0x740

#define RSHIM_CHANNEL_RSHIM 0
#define RSHIM_CHANNEL_SPI   1
#define RSHIM_CHANNEL_I2CM  2
#define RSHIM_CHANNEL_I2CS  3
#define RSHIM_CHANNEL_UART  4
#define RSHIM_CHANNEL_HPI   5

#define RSHIM_LAST_CHANNEL RSHIM_CHANNEL_HPI

// Constants for the location of RShim
#define RSHIM_XLOC  1
#define RSHIM_YLOC  4

// UART registers:
#define RSHIM_UART_DIVISOR 0x064

// UART divisor register contents;
#define RSHIM_UART_DIVISOR_9600_BPS   0x28B
#define RSHIM_UART_DIVISOR_115200_BPS 0x036

#endif // __MAESTRO_RSHIM_H
