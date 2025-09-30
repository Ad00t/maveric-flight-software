// maestro_flash.c  30-Dec-2011  Craig Milo Rogers <rogers@isi.edu>

// This code deals with the PAYLOAD flash partition, without a
// directory.
//
// The PAYLOAD flash partition is defined by PAYLOAD_ADDR and
// PAYLOAD_ALLOC_SIZE.  Programs may be stored in this area, and
// are identified by the offset at which they begin.  They are
// executed as program "0".

// This code is intended to be #included by "maestro.c", per
// AENEAS coding conventions.

#include "flash.h"

#include "address.h"
#include "maestro.h"
#include "maestro_flash.h"
#include "sboot_types.h"

// The base offset for program 0, raw flash.
static unsigned long maestro_flash_execution_base;

#ifdef MAESTRO_SFFS
static bool maestro_sffs_cache1_next;

static bool maestro_sffs_cache1_valid;
static maestro_sffs_slotnum_t maestro_sffs_cache1_slotnum;
static maestro_sffs_slot_t maestro_sffs_cache1_entry;

static bool maestro_sffs_cache2_valid;
static maestro_sffs_slotnum_t maestro_sffs_cache2_slotnum;
static maestro_sffs_slot_t maestro_sffs_cache2_entry;

static bool maestro_sffs_cached_freespace_offset_valid;
static uint32 maestro_sffs_cached_freespace_offset;
#endif // MAESTRO_SFFS

// In spite of the name, this doesn't initialize the flash per-se.
// It initializes the flash-handling software.
void initialize_maestro_flash() {
	maestro_flash_execution_base = MAESTRO_FLASH_EXECUTION_BASE_DEFAULT;

#ifdef MAESTRO_SFFS
	maestro_sffs_cache1_next = TRUE;
	maestro_sffs_cache1_valid = FALSE;
	maestro_sffs_cache2_valid = FALSE;
	maestro_sffs_cached_freespace_offset_valid = FALSE;
#endif // MAESTRO_SFFS
}

// Set the flash execution base offset.
// This is used when executing program 0.
void set_maestro_flash_execution_base(unsigned long value) {
	sprintf(dbgbuf, "Setting the MAESTRO raw flash execution base to 0x%Lx", value);
	maestro_msg_dbgbuf();
	maestro_flash_execution_base = value;
}

bool is_maestro_flash_program_valid() {
	int result;
	result = maestro_flash_execution_base != MAESTRO_FLASH_INVALID_EXECUTION_BASE;
	return result;
}

void clear_maestro_flash(unsigned long offset, unsigned long erase_len) {
	sprintf(dbgbuf, "Clearing MAESTRO flash: offset=0x%Lx, len=0x%Lx.", offset, erase_len);
	maestro_msg_dbgbuf();

	unsigned long start_addr;
	start_addr = PAYLOAD_PROGRAM_ADDR + offset;
	unsigned int start_block;
	start_block = FlashAddrToBlock(start_addr);
	unsigned long start_block_begin_addr, start_block_end_addr;
	BlockToFlashAddr(start_block, &start_block_begin_addr, &start_block_end_addr);
	if (start_block_begin_addr != start_addr) {
		maestro_msg("PAYLOAD clear start is not block-aligned.");
		return;
	}

	if (offset + erase_len - 1 > PAYLOAD_PROGRAM_ALLOC_SIZE) {
		maestro_msg("Attempt to clear beyond the PAYLOAD end.");
		return;
	}

	unsigned long end_addr;
	end_addr = start_addr + erase_len - 1;
	unsigned int end_block;
	end_block = FlashAddrToBlock(end_addr);
	unsigned long end_block_begin_addr, end_block_end_addr;
	BlockToFlashAddr(end_block, &end_block_begin_addr, &end_block_end_addr);
	if (end_block_end_addr != end_addr) {
		maestro_msg("PAYLOAD clear end is not block-aligned.");
		return;
	}

	flashEraseBetweenBlocks(start_block, end_block);
	maestro_msg("MAESTRO flash erase complete.");
}

bool copy_maestro_program_to_flash(uint8 program_number, unsigned long flash_offset) {
	sprintf(dbgbuf, "Copying MAESTRO program %u to flash at offset 0x%Lx", program_number,
			flash_offset);
	maestro_msg_dbgbuf();

	uint32 program_length;
	if (!sboot_get_program_length(program_number, &program_length)) {
		sprintf(dbgbuf, "ERROR: Program copy failed to get the length of program %u.",
				program_number);
		maestro_msg_dbgbuf();
		return FALSE;
	}

	// Copy from ROM to flash in chunks.
	unsigned char buffer[FLASH_BLOCK_SIZE];
	unsigned int bufptr;
	bufptr = 0;
	sboot_size_t program_offset;
	program_offset = 0;

	while (program_length > 0) {
		uint16 program_byte;
		program_byte = sboot_fetch_program_byte(program_number, program_offset);
		if (program_byte == SBOOT_FETCH_ERROR) {
			sprintf(dbgbuf, "ERROR: Program copy failed to get the byte at 0x%Lx.", program_offset);
			maestro_msg_dbgbuf();
			return FALSE;
		}
		program_offset = program_offset + 1; // Avoid possible compiler bug.
		program_length = program_length - 1;

		if (bufptr >= sizeof(buffer)) {
			// The buffer is full, write it to flash.
			ReturnErr_t result;
			result = flashWriteSafe(PAYLOAD_PROGRAM_ADDR + flash_offset, bufptr, buffer,
									PAYLOAD_PROGRAM_ADDR,
									PAYLOAD_PROGRAM_ADDR + PAYLOAD_PROGRAM_ALLOC_SIZE - 1);
			if (result != SUCCESS) {
				sprintf(dbgbuf,
						"ERROR: Program copy to flash failed with error %d at offset 0x%Lx.",
						result, flash_offset);
				maestro_msg_dbgbuf();
				return FALSE;
			}
			flash_offset = flash_offset + sizeof(buffer);
			bufptr = 0;
		}

		buffer[bufptr] = (unsigned char)program_byte;
		bufptr = bufptr + 1; // Avoid possible compiler bug.
	}

	if (bufptr > 0) {
		// Copy the final program bytes from the buffer to flash:
		ReturnErr_t result;
		result = flashWriteSafe(PAYLOAD_PROGRAM_ADDR + flash_offset, bufptr, buffer,
								PAYLOAD_PROGRAM_ADDR,
								PAYLOAD_PROGRAM_ADDR + PAYLOAD_PROGRAM_ALLOC_SIZE - 1);
		if (result != SUCCESS) {
			sprintf(dbgbuf,
					"ERROR: Program copy to flash failed with error %d at offset 0x%Lx at end.",
					result, flash_offset);
			maestro_msg_dbgbuf();
			return FALSE;
		}
	}

	maestro_msg("MAESTRO program copied to flash.");
	return TRUE;
}

bool copy_current_maestro_program_to_flash(unsigned long flash_offset) {
	return copy_maestro_program_to_flash(maestro_program_number, flash_offset);
}

uint16 sboot_fetch_flash_program_byte(uint32 program_offset) {
	// This code should use is_maestro_flash_program_valid(), but I
	// open-coded it for putative efficiency.
	if (maestro_flash_execution_base == MAESTRO_FLASH_INVALID_EXECUTION_BASE) {
		return SBOOT_FETCH_ERROR;
	}

	uint32 payload_offset;
	payload_offset = program_offset + maestro_flash_execution_base;
	if (payload_offset > PAYLOAD_PROGRAM_ALLOC_SIZE || payload_offset < program_offset) {
		return SBOOT_FETCH_ERROR;
	}

	unsigned char char_buf;
	flashRead(PAYLOAD_PROGRAM_ADDR + payload_offset, (unsigned int)1, &char_buf);
	return (uint16)char_buf;
}

#ifdef MAESTRO_SFFS

int maestro_sffs_get_number_of_slots() {
	return (int)MAESTRO_SFFS_SLOTS;
}

uint32 maestro_sffs_get_slot_offset(maestro_sffs_slotnum_t slotnum) {
	// Note: the compiler miscomplied:
	// return slotnum * MAESTRO_SFFS_SLOT_SIZE_IN_BYTES;
	uint32 offset;
	offset = ((int)slotnum) * MAESTRO_SFFS_SLOT_SIZE_IN_BYTES;
	return offset;
}

// If successful, stores the slot's entry in a structure supplied
// by the caller.
bool maestro_sffs_read_slot(maestro_sffs_slotnum_t slotnum, maestro_sffs_slot_t* entryp) {
	if (slotnum > MAESTRO_SFFS_SLOTS) {
		sprintf(dbgbuf, "ERROR: maestro_sffs_read_slot: slotnum %u > max-slots %u", slotnum,
				MAESTRO_SFFS_SLOTS);
		maestro_msg_dbgbuf();
		return FALSE;
	}

	uint32 slot_offset;
	slot_offset = maestro_sffs_get_slot_offset(slotnum);

	if (MAESTRO_SFFS_SLOT_SIZE_IN_BYTES != sizeof(maestro_sffs_slot_t)) {
		return FALSE;
	}

	flashRead(PAYLOAD_PROGRAM_ADDR + slot_offset, sizeof(maestro_sffs_slot_t), entryp);
	return TRUE;
}

// If successful, returns a pointer to a cached copy of the entry.
// The pointer should be considered invalid when the next
// maestro_sffs_read_slot_cached(...) or maestro_sffs_uncache(...)
// call is made.
bool maestro_sffs_read_slot_cached(maestro_sffs_slotnum_t slotnum, maestro_sffs_slot_t** entrypp) {
	if (maestro_sffs_cache1_valid && slotnum == maestro_sffs_cache1_slotnum) {
		maestro_sffs_cache1_next = FALSE;
		*entrypp = &maestro_sffs_cache1_entry;
		return TRUE;
	} else if (maestro_sffs_cache2_valid && slotnum == maestro_sffs_cache2_slotnum) {
		maestro_sffs_cache1_next = TRUE;
		*entrypp = &maestro_sffs_cache2_entry;
		return TRUE;
	}

	maestro_sffs_slot_t* entryp;
	if (maestro_sffs_cache1_next) {
		entryp = &maestro_sffs_cache1_entry;
		maestro_sffs_cache1_next = FALSE;
	} else {
		entryp = &maestro_sffs_cache2_entry;
		maestro_sffs_cache1_next = TRUE;
	}

	if (!maestro_sffs_read_slot(slotnum, entryp)) {
		return FALSE;
	}
	*entrypp = entryp;
	return TRUE;
}

void maestro_sffs_uncache(maestro_sffs_slotnum_t slotnum) {
	if (maestro_sffs_cache1_valid && slotnum == maestro_sffs_cache1_slotnum) {
		maestro_sffs_cache1_valid = FALSE;
	} else if (maestro_sffs_cache2_valid && slotnum == maestro_sffs_cache2_slotnum) {
		maestro_sffs_cache2_valid = FALSE;
	}
}

// Return the offset of the start of free space.
bool maestro_sffs_get_free_space_offset(uint32* freespace_offsetp) {
	if (maestro_sffs_cached_freespace_offset_valid) {
		*freespace_offsetp = maestro_sffs_cached_freespace_offset;
		return TRUE;
	}

	uint32 freespace_offset;
	freespace_offset = MAESTRO_SFFS_RESERVED_BYTES;

	maestro_sffs_slotnum_t slotnum;
	for (slotnum = 0; slotnum < MAESTRO_SFFS_SLOTS; slotnum++) {
		maestro_sffs_slot_t entry;
		if (!maestro_sffs_read_slot(slotnum, &entry)) {
			sprintf(dbgbuf,
					"ERROR: maestro_sffs_get_free_space_offset(...) failed to read slot %u.",
					slotnum);
			maestro_msg_dbgbuf();
			return FALSE;
		}
		if (entry.program_allocated == MAESTRO_SFFS_ASSERTED) {
			uint32 next_offset;
			next_offset = entry.program_offset + entry.program_length;
			if (next_offset > freespace_offset) {
				freespace_offset = next_offset;

				// sprintf(dbgbuf, "Slot %u offset=%Lu length=%Lu freespace=%Lu",
				// 	slotnum, entry.program_offset, entry.program_length, freespace_offset);
				// maestro_msg_dbgbuf();
			}
		}
	}
	*freespace_offsetp = freespace_offset;

	maestro_sffs_cached_freespace_offset = freespace_offset;
	maestro_sffs_cached_freespace_offset_valid = TRUE;

	// sprintf(dbgbuf, "Free space offset %Lu", freespace_offset);
	// maestro_msg_dbgbuf();

	return TRUE;
}

// Return the number of bytes of free space available.
bool maestro_sffs_get_free_space_length(uint32* freespace_bytesp) {
	uint32 freespace_offset;
	if (!maestro_sffs_get_free_space_offset(&freespace_offset)) {
		maestro_msg("ERROR: maestro_sffs_get_free_space_length failed in "
					"maestro_sffs_get_free_space_offset(...).");
		return FALSE;
	}
	*freespace_bytesp = PAYLOAD_PROGRAM_ALLOC_SIZE - freespace_offset;
	return TRUE;
}

// Return the number of free slots.
bool maestro_sffs_get_free_slots(maestro_sffs_slotnum_t* freeslotsp) {
	maestro_sffs_slotnum_t free_slots;
	free_slots = 0;
	maestro_sffs_slotnum_t slotnum;
	for (slotnum = 0; slotnum < MAESTRO_SFFS_SLOTS; slotnum++) {
		maestro_sffs_slot_t entry;
		if (!maestro_sffs_read_slot(slotnum, &entry)) {
			return FALSE;
		}
		if (entry.slot_allocated != MAESTRO_SFFS_ASSERTED) {
			free_slots = free_slots + 1;
		}
	}
	*freeslotsp = free_slots;
	return TRUE;
}

bool maestro_sffs_assert(maestro_sffs_slotnum_t slotnum, int flag_offset) {
	maestro_sffs_uncache(slotnum);

	uint32 target_offset;
	target_offset = maestro_sffs_get_slot_offset(slotnum);
	target_offset = target_offset + flag_offset;

	uint8 buffer;
	buffer = MAESTRO_SFFS_ASSERTED;

	ReturnErr_t result;
	result = flashWriteSafe(PAYLOAD_PROGRAM_ADDR + target_offset, 1, &buffer, PAYLOAD_PROGRAM_ADDR,
							PAYLOAD_PROGRAM_ADDR + PAYLOAD_PROGRAM_ALLOC_SIZE - 1);
	if (result != SUCCESS) {
		sprintf(dbgbuf,
				"ERROR: maestro_sffs_assert failed error=%d at offset 0x%Lx (slot=%u fo=%u).",
				result, target_offset, slotnum, flag_offset);
		maestro_msg_dbgbuf();
		return FALSE;
	}
	return TRUE;
}

// Allocate a slot.  Returns TRUE if successful, else FALSE.
bool maestro_sffs_allocate_slot(maestro_sffs_slotnum_t* slotnump) {
	maestro_sffs_slotnum_t slotnum;
	for (slotnum = 0; slotnum < MAESTRO_SFFS_SLOTS; slotnum++) {
		maestro_sffs_slot_t entry;
		if (!maestro_sffs_read_slot(slotnum, &entry)) {
			return FALSE;
		}
		if (entry.slot_allocated != MAESTRO_SFFS_ASSERTED) {
			*slotnump = slotnum;
			if (maestro_sffs_assert(slotnum, MAESTRO_SFFS_SLOT_SLOT_ALLOCATED)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Allocate space to a slot, and returns its offset.
// Returns TRUE if successful, else FALSE.
bool maestro_sffs_allocate_space(maestro_sffs_slotnum_t slotnum, uint32 program_length,
								 uint32* program_offsetp) {
	uint32 freespace_bytes;
	if (!maestro_sffs_get_free_space_length(&freespace_bytes)) {
		maestro_msg("maestro_sffs_allocate_space: maestro_sffs_get_free_space_length failed");
		return FALSE;
	}
	if (freespace_bytes < program_length) {
		sprintf(dbgbuf, "maestro_sffs_allocate_space: not enough freespace %Lu < %Lu",
				freespace_bytes, program_length);
		maestro_msg_dbgbuf();
		return FALSE;
	}

	uint32 freespace_offset;
	if (!maestro_sffs_get_free_space_offset(&freespace_offset)) {
		maestro_msg("maestro_sffs_allocate_space: maestro_sffs_get_free_space_offset failed");
		return FALSE;
	}

	maestro_sffs_slot_t entry;
	if (!maestro_sffs_read_slot(slotnum, &entry)) {
		sprintf(dbgbuf, "maestro_sffs_allocate_space: read slot %u failed", slotnum);
		maestro_msg_dbgbuf();
		return FALSE;
	}

	if (entry.slot_allocated != MAESTRO_SFFS_ASSERTED) {
		sprintf(dbgbuf, "maestro_sffs_allocate_space: slot %u has not been allocated", slotnum);
		maestro_msg_dbgbuf();
		return FALSE;
	}
	if (entry.program_allocated == MAESTRO_SFFS_ASSERTED) {
		sprintf(dbgbuf, "maestro_sffs_allocate_space: slot %u already has space allocated",
				slotnum);
		maestro_msg_dbgbuf();
		return FALSE; // TODO: Possible idempotency?
	}

	// Write the program_offset and program_length.
	uint32 slot_offset;
	slot_offset = maestro_sffs_get_slot_offset(slotnum);

	uint32 program_offset_offset;
	program_offset_offset = slot_offset + MAESTRO_SFFS_SLOT_PROGRAM_OFFSET;

	{
		ReturnErr_t result;
		result = flashWriteSafe(PAYLOAD_PROGRAM_ADDR + program_offset_offset, 4, &freespace_offset,
								PAYLOAD_PROGRAM_ADDR,
								PAYLOAD_PROGRAM_ADDR + PAYLOAD_PROGRAM_ALLOC_SIZE - 1);
		if (result != SUCCESS) {
			sprintf(dbgbuf,
					"ERROR: maestro_sffs_allocate_space failed writing program_offset error %d at "
					"offset 0x%Lx.",
					result, program_offset_offset);
			maestro_msg_dbgbuf();
			return FALSE;
		}
	}

	uint32 program_length_offset;
	program_length_offset = slot_offset + MAESTRO_SFFS_SLOT_PROGRAM_LENGTH;

	{
		ReturnErr_t result;
		result = flashWriteSafe(PAYLOAD_PROGRAM_ADDR + program_length_offset, 4, &program_length,
								PAYLOAD_PROGRAM_ADDR,
								PAYLOAD_PROGRAM_ADDR + PAYLOAD_PROGRAM_ALLOC_SIZE - 1);
		if (result != SUCCESS) {
			sprintf(dbgbuf,
					"ERROR: maestro_sffs_allocate_space failed writing program_length error %d at "
					"offset 0x%Lx.",
					result, program_length_offset);
			maestro_msg_dbgbuf();
			return FALSE;
		}
	}

	// Clear this flag early just in case maestro_sffs_assert(...)
	// asserts the flag but still reports an error.
	maestro_sffs_cached_freespace_offset_valid = FALSE;

	if (!maestro_sffs_assert(slotnum, MAESTRO_SFFS_SLOT_PROGRAM_ALLOCATED)) {
		sprintf(dbgbuf, "maestro_sffs_allocate_space: error marking space allocated for slot %u",
				slotnum);
		maestro_msg_dbgbuf();
		return FALSE;
	}

	*program_offsetp = freespace_offset;
	return TRUE;
}

// Mark a slot valid after loading a program into it.
// Returns TRUE if successful, else FALSE.
bool maestro_sffs_mark_program_valid(maestro_sffs_slotnum_t slotnum) {
	maestro_sffs_slot_t* entryp;
	if (!maestro_sffs_read_slot_cached(slotnum, &entryp)) {
		return FALSE;
	}
	if (entryp->program_allocated != MAESTRO_SFFS_ASSERTED) {
		// The program space must be allocated first.
		return FALSE;
	}
	if (entryp->program_valid == MAESTRO_SFFS_ASSERTED) {
		// We've already marked this entry valid.  Should we
		// allow this as idempotency?  If so, gott check if
		// the program's been deleted, too.
		return FALSE;
	}
	if (maestro_sffs_assert(slotnum, MAESTRO_SFFS_SLOT_PROGRAM_VALID)) {
		return TRUE;
	}
	return FALSE;
}

// Returns TRUE if the slot contains a valid, undeleted program,
// else FALSE.
bool maestro_sffs_is_program_valid(maestro_sffs_slotnum_t slotnum) {
	// sprintf(dbgbuf, "maestro_sffs_is_program_valid(%u)", slotnum);
	// maestro_msg_dbgbuf();

	maestro_sffs_slot_t* entryp;
	if (!maestro_sffs_read_slot_cached(slotnum, &entryp)) {
		return FALSE;
	}
	if (entryp->program_valid == MAESTRO_SFFS_ASSERTED &&
		entryp->program_deleted != MAESTRO_SFFS_ASSERTED) {
		return TRUE;
	}
	return FALSE;
}

// Get the offset of space allocated to a slot.
// Returns TRUE if successful and the slot has a valid,
// undeleted program, else FALSE.
bool maestro_sffs_get_program_offset(maestro_sffs_slotnum_t slotnum, uint32* program_offsetp) {
	if (maestro_sffs_is_program_valid(slotnum)) {
		maestro_sffs_slot_t* entryp;
		if (!maestro_sffs_read_slot_cached(slotnum, &entryp)) {
			return FALSE;
		}
		*program_offsetp = entryp->program_offset;
		return TRUE;
	}
	return FALSE;
}

// Mark a slot deleted.
// Returns TRUE if successful, else FALSE.
bool maestro_sffs_mark_program_deleted(maestro_sffs_slotnum_t slotnum) {
	if (maestro_sffs_is_program_valid(slotnum)) {
		if (maestro_sffs_assert(slotnum, MAESTRO_SFFS_SLOT_PROGRAM_DELETED)) {
			return TRUE;
		}
	}
	return FALSE;
}

uint16 maestro_sffs_fetch_program_byte(maestro_sffs_slotnum_t slotnum, uint32 program_offset) {
	if (!maestro_sffs_is_program_valid(slotnum)) {
		return SBOOT_FETCH_ERROR;
	}

	uint32 payload_offset;
	if (!maestro_sffs_get_program_offset(slotnum, &payload_offset)) {
		return SBOOT_FETCH_ERROR;
	}

	payload_offset = payload_offset + program_offset;
	// TODO: the overflow check isn't quite right.
	if (payload_offset > PAYLOAD_PROGRAM_ALLOC_SIZE || payload_offset < program_offset) {
		return SBOOT_FETCH_ERROR;
	}

	unsigned char char_buf;
	flashRead(PAYLOAD_PROGRAM_ADDR + payload_offset, (unsigned int)1, &char_buf);
	return (uint16)char_buf;
}

bool maestro_sffs_copy_program_to_flash(uint8 program_number) {
	sprintf(dbgbuf, "Copying MAESTRO program %u to flash.", program_number);
	maestro_msg_dbgbuf();

	uint32 program_length;
	if (!sboot_get_program_length(program_number, &program_length)) {
		sprintf(dbgbuf, "ERROR: Program copy failed to get the length of program %u.",
				program_number);
		maestro_msg_dbgbuf();
		return FALSE;
	}
	sprintf(dbgbuf, "Program length is %Lu", program_length);
	maestro_msg_dbgbuf();

	uint32 freespace_length;
	if (!maestro_sffs_get_free_space_length(&freespace_length)) {
		maestro_msg("ERROR: Program copy failed to get the length of free space.");
		return FALSE;
	}
	sprintf(dbgbuf, "Freespace length is %Lu", freespace_length);
	maestro_msg_dbgbuf();

	if (freespace_length < program_length) {
		sprintf(dbgbuf, "ERROR: Freespace %Lu is less than program length %Lu", freespace_length,
				program_length);
		maestro_msg_dbgbuf();
		return FALSE;
	}

	maestro_sffs_slotnum_t slotnum;
	if (!maestro_sffs_allocate_slot(&slotnum)) {
		maestro_msg("ERROR: no slots left in sffs.");
		return FALSE;
	}
	sprintf(dbgbuf, "Allocated slot %u", slotnum);
	maestro_msg_dbgbuf();

	uint32 program_offset;
	if (!maestro_sffs_allocate_space(slotnum, program_length, &program_offset)) {
		maestro_msg("ERROR: Unable to allocate space for program.");
		return FALSE;
	}
	sprintf(dbgbuf, "Allocated space at offset %Lu", program_offset);
	maestro_msg_dbgbuf();

	if (!copy_maestro_program_to_flash(program_number, program_offset)) {
		return FALSE;
	}

	if (!maestro_sffs_mark_program_valid(slotnum)) {
		maestro_msg("ERROR: Unable to mark program valid.");
		return FALSE;
	}

	maestro_msg("Copy marked valid.");
	return TRUE;
}

bool maestro_sffs_copy_current_program_to_flash() {
	return maestro_sffs_copy_program_to_flash(maestro_program_number);
}

void report_maestro_sffs_free_space() {
	int number_of_slots;
	number_of_slots = maestro_sffs_get_number_of_slots();

	maestro_sffs_slotnum_t number_of_free_slots;
	number_of_free_slots = 0;
	if (!maestro_sffs_get_free_slots(&number_of_free_slots)) {
		maestro_msg("ERROR: unable to get the number of free slots.");
	}

	uint32 space_in_bytes;
	space_in_bytes = PAYLOAD_PROGRAM_ALLOC_SIZE;

	uint32 free_space_in_bytes;
	free_space_in_bytes = 0;
	if (!maestro_sffs_get_free_space_length(&free_space_in_bytes)) {
		maestro_msg("ERROR: unable to get the free space in flash.");
	}

	sprintf(dbgbuf, "Free slots: %u/%u.  Free space: %Lu/%Lu.", (unsigned int)number_of_free_slots,
			number_of_slots, free_space_in_bytes, space_in_bytes);
	maestro_msg_dbgbuf();
}

#endif // MAESTRO_SFFS