// maestro_flash.h  30-Dec-2011  Craig Milo Rogers <rogers@isi.edu>

#ifndef __MAESTRO_FLASH_H__
#define __MAESTRO_FLASH_H__

#include "sboot_types.h"

// When the base offset is set to the following value, the
// default flash program (program #0) is considered invalid.
#define MAESTRO_FLASH_INVALID_EXECUTION_BASE 0xffffff

// Set the default base offset for the MAESTRO program 0 in flash.
#define MAESTRO_FLASH_EXECUTION_BASE_DEFAULT MAESTRO_FLASH_INVALID_EXECUTION_BASE

void initialize_maestro_flash();

void set_maestro_flash_execution_base(unsigned long value);

bool is_maestro_flash_program_valid();

void clear_maestro_flash(unsigned long offset, unsigned long erase_len);

bool copy_maestro_program_to_flash(uint8 program_number, unsigned long flash_offset);
bool copy_current_maestro_program_to_flash(unsigned long flash_offset);

uint16 sboot_fetch_flash_program_byte(uint32 offset);

#ifdef MAESTRO_SFFS

typedef uint8 maestro_sffs_slotnum_t;

// The number of directory entries:
#define MAESTRO_SFFS_SLOTS ((maestro_sffs_slotnum_t)200)

// The space reserved for the directory (and possible future backup
// copy).
//
// TODO: We should use constants (or variables!) from the underlying
// flash system to compute this.
#define MAESTRO_SFFS_RESERVED_BYTES (4096 * 2)

struct maestro_sffs_slot_struct {
  uint32 program_offset;
  uint32 program_length;
  bool slot_allocated;
  bool program_allocated;
  bool program_valid;
  bool program_deleted;
  uint32 reserved;
};
typedef struct maestro_sffs_slot_struct maestro_sffs_slot_t;

// The size and internal offsets of a directory entry, in bytes.
//
// TODO: This should be recoded to use a structure for
// the in-flash data structure, or just use the in-memory
// one.
#define MAESTRO_SFFS_SLOT_SIZE_IN_BYTES    16
#define MAESTRO_SFFS_SLOT_PROGRAM_OFFSET    0
#define MAESTRO_SFFS_SLOT_PROGRAM_LENGTH    4
#define MAESTRO_SFFS_SLOT_SLOT_ALLOCATED    8
#define MAESTRO_SFFS_SLOT_PROGRAM_ALLOCATED 9
#define MAESTRO_SFFS_SLOT_PROGRAM_VALID    10
#define MAESTRO_SFFS_SLOT_PROGRAM_DELETED  11

#define MAESTRO_SFFS_ASSERTED 0x00

int maestro_sffs_get_number_of_slots();

uint32 maestro_sffs_get_slot_offset(maestro_sffs_slotnum_t slotnum);

// If successful, stores the slot's entry in a structure supplied
// by the caller.
bool maestro_sffs_read_slot(maestro_sffs_slotnum_t slotnum, maestro_sffs_slot_t *entryp);

// If successful, returns a pointer to a cached copy of the entry.
// The pointer should be considered invalid when the next
// maestro_sffs_read_slot_cached(...) or maestro_sffs_uncache(...)
// call is made.
bool maestro_sffs_read_slot_cached(maestro_sffs_slotnum_t slotnum, maestro_sffs_slot_t **entrypp);
void maestro_sffs_uncache(maestro_sffs_slotnum_t slotnum);

// Return the offset of the start of free space.
bool maestro_sffs_get_free_space_offset(uint32 *freespace_offsetp);

// Return the number of bytes of free space available.
bool maestro_sffs_get_free_space_length(uint32 *freespace_bytesp);

// Return the number of free slots.
bool maestro_sffs_get_free_slots(maestro_sffs_slotnum_t *freeslotsp);

bool maestro_sffs_assert(maestro_sffs_slotnum_t slotnum, int flag_offset);

// Allocate a slot.  Returns TRUE if successful, else FALSE.
bool maestro_sffs_allocate_slot(maestro_sffs_slotnum_t *slotnump);

// Allocate space to a slot, and returns its offset.
// Returns TRUE if successful, else FALSE.
bool maestro_sffs_allocate_space(maestro_sffs_slotnum_t slotnum,
				 uint32 program_length,
				 uint32 *program_offsetp);

// Mark a slot valid after loading a program into it.
// Returns TRUE if successful, else FALSE.
bool maestro_sffs_mark_program_valid(maestro_sffs_slotnum_t slotnum);

// Returns TRUE if the slot contains a valid, undeleted program,
// else FALSE.
bool maestro_sffs_is_program_valid(maestro_sffs_slotnum_t slotnum);

// Get the offset of space allocated to a slot.
// Returns TRUE if successful and the slot has a valid,
// undeleted program, else FALSE.
bool maestro_sffs_get_program_offset(maestro_sffs_slotnum_t slotnum,
				     uint32 *program_offsetp);

// Mark a slot deleted.
// Returns TRUE if successful, else FALSE.
bool maestro_sffs_mark_program_deleted(maestro_sffs_slotnum_t slotnum);

uint16 maestro_sffs_fetch_program_byte(maestro_sffs_slotnum_t slotnum,
				       uint32 program_offset);

bool maestro_sffs_copy_program_to_flash(uint8 program_number);
bool maestro_sffs_copy_current_program_to_flash();

void report_maestro_sffs_free_space();

#endif // MAESTRO_SFFS

#endif // __MAESTRO_FLASH_H__
