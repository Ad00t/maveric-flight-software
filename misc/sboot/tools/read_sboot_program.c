// read_sboot_program.c  06-Dec-2011  Craig Milo Rogers <rogers@isi.edu>

// TODO: Is this necessary?
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "read_sboot_program.h"
#include "../includes/sboot_types.h"

sboot_size_t read_sboot_program(const char * const sboot_file_name,
				uint8 ** const input_bufpp,
				const bool verbose) {
  FILE * const sboot_file = fopen(sboot_file_name, "rb");
  if (!sboot_file) {
    perror(sboot_file_name);
    exit(EXIT_FAILURE);
  }
    
  struct stat statbuf;
  if (fstat(fileno(sboot_file), &statbuf)) {
    perror(sboot_file_name);
    exit(EXIT_FAILURE);
  }
  const off_t filesize = statbuf.st_size;
  const sboot_size_t input_len = filesize;

  if (verbose) {
    printf("%s has length %d.\n", sboot_file_name, filesize);
  }

  uint8 * const input_buf = malloc(filesize);
  *input_bufpp = input_buf;
  if (input_buf == 0) {
    perror("Error in malloc");
    exit(EXIT_FAILURE);
  }

  // The following code is slow, but easily understood.
  int len = 0;
  while (len < filesize) {
    const int c = fgetc(sboot_file);// TODO: check for errors.
    if (c == EOF) {
      fprintf(stderr, "Premature EOF reading %s\n", sboot_file_name);
      exit(EXIT_FAILURE);
    }
    input_buf[len++] = (uint8)c;
  }
  fclose(sboot_file); // TODO: check for errors.

  return input_len;
}
    
