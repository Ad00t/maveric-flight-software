// read_sboot_program.h  06-Dec-2011  Craig Milo Rogers <rogers@isi.edu>

#include "../includes/sboot_types.h"

sboot_size_t read_sboot_program(const char * const sboot_file_name,
				uint8 ** const input_bufpp,
				const bool verbose);

