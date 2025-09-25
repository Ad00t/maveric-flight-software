// sboot_types.h  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

// This file contains some type definitions to facilitate porting the
// Maestro serial bootstrap code to the CCS PCD C compiler for the
// PIC24 processor family.

/*
 * This file contains names and concepts related to the Tilera bootstrap
 * message interface used in the Maestro (Tilera49) chip.  Some of these names
 * and concepts were extracted from the files containing the following
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

#ifndef __SBOOT_TYPES_H
#define __SBOOT_TYPES_H

#if defined(__PCD__)
typedef unsigned char uchar; // Use for printable text.
typedef unsigned short uint8; // Use for binary bytes.
typedef unsigned int uint16; // Use for counters that exceed uint8.
typedef unsigned long uint32; // Use only when required, such as when
                             // processing Maestro 32-bit data.
typedef unsigned char bool; // Boolean type.

#else
typedef unsigned char uchar; // Use for printable text.
typedef unsigned char uint8; // Use for binary bytes.
typedef unsigned short uint16; // Use for counters that exceed uint8.
typedef unsigned int uint32; // Use only when required, such as when
                             // processing Maestro 32-bit data.
typedef unsigned char bool; // Boolean type.
#endif // defined(__PCD__)

#if defined(__PCD__)
// The PIC24 has a modified Harvard architecture, with a strong distinction
// between pointers to RAM and pointers to ROM.  The CCS PCD compiler needs to
// be told when pointers are to ROM.  String literal are stored in ROM by
// default.
//
// If we accept that default, then define string_literal as:
// typedef char ROM *string_literal;

// AENEAS has chosen to pass strings in RAM:
#ifndef __COMMON_H__
#device PASS_STRINGS=IN_RAM
#endif
typedef char *string_literal;
#else
// Otherwise, assume a unified address space.
typedef char const *string_literal;
#endif // !defined(__PCD__)

typedef uint16 address_t; // Tilera address -- 10 bits wide.
typedef uint8 channel_t; // Tilera channel -- 3 bits wide.
typedef uint8 boot_msg_t; // A message code from the Tilera chip.

typedef uint32 sboot_timer_t; // Holds seconds for timers.

typedef uint8 list_len_t; // Max length of a list_op list.

// sboot_program_number_t defines a program number.
typedef uint8 sboot_program_number_t;

// sboot_size_t defines the size of an sboot program input file.  Can it
// be uint16, instead?  We'll have to change some code, if so.
typedef uint32 sboot_size_t;

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // FALSE

#define UINT8_MAX 255
#define UINT16_MAX 65535
#define UINT24_MAX 16777215

#define BYTES_PER_UINT8  1
#define BYTES_PER_UINT16 2
#define BYTES_PER_UINT24 3
#define BYTES_PER_UINT32 4

// BYTES_PER_WORD could be derived from sizeof.
//
// TODO: Does it make sense to maintain a distinction between "word" and
// "uint32"?
#define BYTES_PER_WORD 4

// SBOOT_FETCH_ERROR is a special value used by
// sboot_fetch_program_byte(offset) and other routines.
// It is a uint16 value that is "out of band" to any uint8
// data value.  sboot_fetch_program_byte(offset) returns this value
// when the offset is out of bounds.
#define SBOOT_FETCH_ERROR 0xffff

#endif // __SBOOT_TYPES_H
