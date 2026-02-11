// convert_sboot_to_code.c  06-Dec-2011  Craig Milo Rogers <rogers@isi.edu>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convert_sboot_to_code.h"
#include "read_sboot_program.h"

#include "../includes/sboot_types.h"

#define MAX_BYTES_PER_LINE 10

#define MAX_TEST_PROGRAMS 1000

// ***************************************************************
// Here's the main entry point.
//
// TODO: add verbose control.
int main(argc, argv)
int argc;                               /* Number of arguments. */
char **argv;                            /* Argument array. */
{
  bool verbose = VERBOSE_DEFAULT;
  uint16 segment_limit = SEGMENT_LIMIT_DEFAULT;
  char * outfilename = 0;

  if (argc) {
    // Skip the first argument, the command name.
    argc--;
    argv++;
  }

  while (argc > 0) {
    // Terrminate option processing if the next argument does not start
    // with "--":
    char *arg = *argv;
    if (arg[0] != '-' || arg[1] != '-') {
      break;
    }
    argc--; argv++;

    if (strcmp(arg, OUTPUT_OPTION) == 0) {
      if (argc--) {
        outfilename = *argv++;
      } else {
        fprintf(stderr, "Missing output file argument for %s\n",
		OUTPUT_OPTION);
        exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, SEGMENT_LIMIT_OPTION) == 0) {
      if (argc--) {
	char *segment_limit_string = *argv++;
	segment_limit = atoi(segment_limit_string);// TODO: do error checks
      } else {
	fprintf(stderr, "Missing boundary argument for %s\n",
		SEGMENT_LIMIT_OPTION);
	exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, VERBOSE_OPTION) == 0) {
      verbose = TRUE;

    } else {
      fprintf(stderr, "Unknown option \"%s\"\n", arg);
      exit(EXIT_FAILURE);
    }
  }

  if (argc == 0) {
    fprintf(stderr, "Must supply at least one sboot file.\n");
    exit(EXIT_FAILURE);
  }

  if (outfilename == 0) {
    fprintf(stderr, "Must supply an output file.\n");
    exit(EXIT_FAILURE);
  }

  FILE * outfile = fopen(outfilename, "w");
  if (!outfile) {
    perror(outfilename);
    exit(EXIT_FAILURE);
  }

  // This header will facilitate separate compilation with the CCS PCD
  // C compiler.
  //
  // TODO: Find a cleaner way to do this.  For one thing, the
  // relative path references are way uncool.
  fprintf(outfile, "#if defined(__PCD__) && !defined(__24FJ256GA110_H__)\n");
  fprintf(outfile, "#include \"../../../common.h\"\n");
  fprintf(outfile, "#define __24FJ256GA110_H__\n");
  fprintf(outfile, "#endif // defined(__PCD__)\n\n");

  fprintf(outfile, "#include \"../../includes/sboot_types.h\"\n");

  int test_number = 0;
  char *test_names[MAX_TEST_PROGRAMS];
  bool first = TRUE;
  unsigned long total_length = 0L;
  while (argc > 0) {
    const char * const filename = *argv++; argc--;
    test_number++;  // The first test will be number 1.

    // The sboot virtual machine program is read into in this buffer:
    uint8 *input_buf;
    const sboot_size_t input_len =
      read_sboot_program(filename, &input_buf, verbose);
    total_length += input_len;

    // The name of the test is the name of the file, excluding the
    // filename extension and any directory nanes.
    char * const testname = (char *)malloc(strlen(filename) + 1);
    if (!testname) {
      perror("Error allocating test name buffer: ");
      exit(EXIT_FAILURE);
    }
    char *lastname = rindex(filename, '/');
    if (lastname) {
      strcpy(testname, lastname + 1);
    } else {
      strcpy(testname, filename);
    }
    char * const dotp = rindex(testname, '.');
    if (!dotp) {
      fprintf(stderr, "Need a '.' in \"%s\"\n", filename);
      exit(EXIT_FAILURE);
    }
    *dotp = '\0';

    test_names[test_number - 1] = testname;

    if (!first) {
      fprintf(outfile, "\n\n");
    }
    fprintf(outfile, "static uint16 sboot_fetch_%s(uint32 offset) {\n", testname);

    int segment_number = 0;
    uint16 segment_length = 0;
    bool need_segment = TRUE;

    int col = 0;
    for (sboot_size_t i = 0; i < input_len; i++) {
      if (segment_length >= segment_limit) {
	fprintf(outfile, "\n  }; // %s_data%d\n", testname, segment_number);
	segment_length = 0;
	segment_number++;
	need_segment = TRUE;
	col = 0;
      }
      if (need_segment) {
	fprintf(outfile, "  static rom unsigned char %s_data%d[] = {\n", testname, segment_number);
	need_segment = FALSE;
	col = 0;
      }
      if (col >= MAX_BYTES_PER_LINE) {
	fprintf(outfile, "\n");
	col = 0;
      }
      fprintf(outfile, " %d,", input_buf[i]);
      col++;
      segment_length++;
    }

    // Finish off the last segment:
    fprintf(outfile, "\n  }; // %s_data%d\n", testname, segment_number);
    fprintf(outfile, "\n");

    // Check overall length first.
    fprintf(outfile, "  if (offset >= %Lu) {\n", input_len);
    fprintf(outfile, "    return SBOOT_FETCH_ERROR;\n");
    fprintf(outfile, "  }\n");

    // Check each segment for the one we want:
    for (int i = 0; i <= segment_number; i++) {
      if (i > 0) {
	fprintf(outfile, "  offset -= %d;\n", segment_limit);
      }
      fprintf(outfile, "  if (offset < %u) {\n", segment_limit);
      fprintf(outfile, "    uint8 value;\n");
      fprintf(outfile, "    value = %s_data%d[offset];\n", testname, i);
      fprintf(outfile, "    return (uint16)value;\n");
      fprintf(outfile, "  }\n");
    }

    // Fall-through error.
    fprintf(outfile, "  return SBOOT_FETCH_ERROR;\n");
    fprintf(outfile, "} // sboot_fetch_%s(...)\n", testname);

    free(input_buf);
    first = FALSE;
  }

  int number_of_tests = test_number;

  fprintf(outfile, "\n");
  fprintf(outfile, "uint16 sboot_fetch_rom_program_byte(uint8 program_number, uint32 offset) {\n");
  fprintf(outfile, "  switch(program_number) {\n");
  for (int i = 0; i < number_of_tests; i++) {
    char *testname = test_names[i];
    fprintf(outfile, "  case %d: return sboot_fetch_%s(offset);\n", i + 1, testname);

    // Free memory for neatness:
    test_names[i] = 0;
    free(testname);
  }
  fprintf(outfile, "  default: return SBOOT_FETCH_ERROR;\n");
  fprintf(outfile, "  }\n");
  fprintf(outfile, "}\n");

  fprintf(outfile, "\n");
  fprintf(outfile, "uint8 sboot_number_of_rom_programs() {\n");
  fprintf(outfile, "  return %d;\n", number_of_tests);
  fprintf(outfile, "}\n");

  fclose(outfile);

  printf("Number of tests: %d\n", number_of_tests);
  printf("Total length: %Lu\n", total_length);
}
