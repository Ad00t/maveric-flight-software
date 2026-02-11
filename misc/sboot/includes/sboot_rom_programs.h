#ifndef __SBOOT_ROM_PROGRAMS_H__
#define __SBOOT_ROM_PROGRAMS_H__

// How many test programs are available in the PIC24F program ROM?
uint8 sboot_number_of_rom_programs();

// Fetch a byte from a MAESTRO test program in the PIC24F program ROM.
uint16 sboot_fetch_rom_program_byte(uint8 program_number, uint32 offset);

#endif // __SBOOT_ROM_PROGRAMS_H__
