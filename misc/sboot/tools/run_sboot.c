// run_sboot.c  20-Oct-2011 Craig Milo Rogers <rogers@isi.edu>

/*
 * This is a test harness for a serial bootstrapper for the Maestro chip.  The
 * serial bootstrapper is intended to be incorporated into the code running on
 * the upper processor of the Aeneas satellite.  This imposes certain
 * stylistic constraints.  For example, the satellite's code imcorporates ".c"
 * files directly, rather than using seperate compilation.
 */

// This file includes code taken from:
// http://www.faqs.org/docs/Linux-HOWTO/Serial-Programming-HOWTO.html
// on 20-Oct-2011.

// TODO: Is this necessary?
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "read_sboot_program.h"

#include "../includes/sboot_types.h"
#include "../includes/sboot_shout.h"
#include "../includes/sboot_workspace.h"
#include "../includes/sboot_callbacks.h"
#include "../includes/sboot_enable_maestro_phase.h"

// Define the command-line options:
#include "run_sboot.h"

// You may ask, why is "sboot_execution.c" included directly?
// This choice was made to simulate the AENEAS software
// development environment, which currently does not link
// separately-compiled modules.
#include "../includes/sboot_execution.h"
#include "../execution/sboot_execution.c"

// Similarly, include the execution info code, even though
// we do not call it yet.
#include "../includes/sboot_execution_info.h"
#include "../execution/sboot_execution_info.c"

static int uart_fd; // the file descriptor for UART I/O.

static bool verbose; // If true, log progress.

static bool show_received_chars; // If true, show what we received from the MAESTRO.

static bool serial_dev_required; // If false, don't try to talk to the MAESTRO.

// The sboot virtual machine program is read into in this buffer:
static uint8 *input_buf;
static sboot_size_t input_len;

// ***************************************************************
// Helper routines for opening the serial device and setting it
// to the proper state for communication with the MAESTRO Development
// Board.
//
static void serial_device_open(const char *serial_device) {
  if (verbose) {
    printf("Opening the serial device \"%s\"", serial_device);
  }

  uart_fd = open(serial_device, O_RDWR | O_NOCTTY ); 
  if (uart_fd <0) {
    perror(serial_device);
    exit(EXIT_FAILURE);
  }

  if (verbose) {
    printf("Opened the serial device.");
  }
}

static void serial_device_config(const int baudrate) {
  if (verbose) {
    printf("Configuring the serial device for baudrate=%d.\n",
	   baudrate);
  }

  struct termios oldtio,newtio;

  // TODO: why bother saving the current settings?  We won't restore them...
  tcgetattr(uart_fd, &oldtio); /* save current port settings */
        
  bzero(&newtio, sizeof(newtio));

  // TODO: This is not the safest way to manipulate these flags.
  // Generally, one should get the current settings and change bits, not
  // whack the flags to 0's.
  //
  // One or both of these two flags are required:
  // CSTOPB = generate two stop bits
  // PARENB = generate a parity bit (default to even parity)
  //
  // CS8 = send 8 data bits per character.
  // CLOCAL = ignore modem control lines (no hardware handshake)
  // CREAD = enable the receiver
  //
  newtio.c_cflag = baudrate | CS8 | CLOCAL | CREAD | CSTOPB | PARENB;

  // IGNPAR = ignore framing errors and parity errors
  // IGNBRK == ignore the break condition.
  //
  newtio.c_iflag = IGNPAR | IGNBRK;
  newtio.c_oflag = 0;
        
  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;
         
  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 0;   /* non-blocking read */
        
  tcflush(uart_fd, TCIFLUSH);
  tcsetattr(uart_fd,TCSANOW, &newtio);

  if (verbose) {
    printf("Configured the serial device.");
  }
}

// **************** SBOOT Callbacks **********************
//
// sboot_execute(...) will call these routines.
//

// Fetch a byte from the virtual machine input buffer.
// Return SBOOT_FETCH_ERROR if the offset is too large.
// Ignore the program number.
uint16 sboot_fetch_program_byte(sboot_program_number_t program_number,
				sboot_size_t offset) {
  /*
  if (verbose) {
    printf("@%d of %d\n", offset, input_len);
  }
  */
  if (offset >= input_len) {
    return SBOOT_FETCH_ERROR;
  }
  return input_buf[offset];
}

void sboot_reset_maestro(bool value) {
  if (verbose) {
    printf(">>>Reset(%d)\n", value);
  }
  if (value) {
    printf("Press the reset button, then hit enter");

    while (TRUE) {
      int c = getchar();
      if (c == '\n' || c == EOF) {
	break;
      }
    }

    // Having reset the MAESTRO, we need to reconfigure the serial port
    // for 9600 bps operation.
    serial_device_config(B9600);
  }
}

void sboot_enable_maestro(sboot_enable_maestro_phase_t phase) {
  if (verbose) {
    printf(">>>Enable(%d)\n", phase);
  }
}

void sboot_disable_maestro() {
  if (verbose) {
    printf(">>>Disable\n");
  }
}

sboot_timer_t sboot_current_seconds() {
  time_t current_seconds = time(0);
  return (sboot_timer_t) current_seconds;
}

// Switch the UART between low speed (9600 bps) and high speed
// (115200 bps) operation.
//
// TODO: Generalize to other speeds.
//
// Returns TRUE if the change appears to have succeeded.
bool sboot_uart_change_speed(bool high_speed) {
  if (high_speed) {
    serial_device_config(B115200);
  } else {
    serial_device_config(B9600);
  }
  return TRUE;
}

bool sboot_uart_send(uint8 byteval) {
  if (verbose) {
    printf(">0x%X\n", byteval);
  }

  if (!serial_dev_required) {
    fprintf(stderr, "ERROR: sboot_uart_send(...) called without a serial device.");
    exit(EXIT_FAILURE);
  }

  ssize_t len = write(uart_fd, &byteval, 1);
  if (len == 1) {
    return TRUE;
  } else {
    // TODO: check for error values.
    return FALSE;
  }
}

uint16 sboot_uart_receive() {
  if (!serial_dev_required) {
    fprintf(stderr, "ERROR: sboot_uart_receive() called without a serial device.");
    exit(EXIT_FAILURE);
  }

  uint8 buf;
  ssize_t len = read(uart_fd, &buf, 1);
  if (len == 1) {
    if (verbose || show_received_chars) {
      printf("<0x%X\n", buf);
    }
    return buf;
  } else {
    // TODO: check for error values.
    return SBOOT_UART_NOT_READY;
  }
}

#define SHOUT_BUF_LEN (120 + 1)
static uchar sboot_shout_buf[SHOUT_BUF_LEN];

// Return a pointer to an output buffer.  Returns TRUE if a buffer
// is available, else FALSE.  The buffer will be returned to
// the callback environment before the next yield.
//
// In exceptional circumstances, sboot_get_shout_buf(...) might
// be called twice in a row without an intervening call to
// sboot_send_shout(...).  It should be safe to return the same
// buffer in those circumstances.
bool sboot_get_shout_buf(uchar **bufp, uint16 *buf_lenp) {
  *bufp = sboot_shout_buf;
  *buf_lenp = SHOUT_BUF_LEN;
  return TRUE;
}



void sboot_send_shout(sboot_shout_severity_t severity,
		      sboot_shout_reason_t reason,
		      uint16 len) {
  uchar *msg = sboot_shout_buf;
  FILE *f;
  switch (severity) {
  case SBOOT_SHOUT_DEBUG:
    f = stdout;
    fprintf(f, "DEBUG: ");
    break;

  case SBOOT_SHOUT_MESSAGE:
    f = stdout;
    fprintf(f, "MESSAGE: ");
    break;

  case SBOOT_SHOUT_FAILURE:
    f = stderr;
    fprintf(f, "FAILURE: ");
    break;

  default:
    fprintf(stderr, "ERROR: Unknown severity %d in sboot_send_shout\n", severity);
    exit(EXIT_FAILURE);
  }

  // Slow, but simple:
  while (len--) {
    fputc((char)(*msg++), f);
  }

  fprintf(f, "\n");
}


// Return TRUE when sboot should yield ASAP.
bool sboot_should_yield_now() {
  return FALSE;
}

// Return TRUE when sboot should shut down ASAP, e.g., due to an anomoly
// in the power system.
bool sboot_should_shut_down() {
  return FALSE;
}

// Get the number of programs.
sboot_program_number_t sboot_get_number_of_programs() {
  return 1;
}

// Get the first program number.
sboot_program_number_t sboot_get_first_program_number() {
  return 1;
}

// Validate the program number.
// Returns TRUE if the program number is valid, else FALSE.
bool sboot_validate_program_number(sboot_program_number_t program_number) {
  return TRUE;
}

// ***************************************************************
static void run_program() {
  if (verbose) {
    printf("Initializing the sboot workspace.\n");
  }
  sboot_initialize((sboot_program_number_t)0); // Dummy program number 0.
  
  if (verbose) {
    printf("Beginning sboot execution.\n");
  }

  sboot_start();
  while (sboot_execute()) {
  }
  
  if (verbose) {
    printf("Ending sboot execution.\n");
  }
}


// ***************************************************************
// Here's the main entry point.
//
// TODO: use command-line parameters to set the baud rate.
//
int main(argc, argv)
int argc;                               /* Number of arguments. */
char **argv;                            /* Argument array. */
{
  // The serial device, which may be set via a command line parameter:
  char *serial_device = SERIAL_DEVICE_DEFAULT;
  verbose = FALSE;
  show_received_chars = FALSE;
  serial_dev_required = TRUE;

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

    if (strcmp(arg, NO_SERIAL_DEVICE_OPTION) == 0) {
      serial_dev_required = FALSE;

    } else if (strcmp(arg, SERIAL_DEVICE_OPTION) == 0) {
      if (argc--) {
        serial_device = *argv++;
      } else {
        fprintf(stderr, "Missing serial device argument for %s\n",
		SERIAL_DEVICE_OPTION);
        exit(EXIT_FAILURE);
      }

    } else if (strcmp(arg, SHOW_RECEIVED_OPTION) == 0) {
      show_received_chars = TRUE;

#ifdef SBOOT_TRACE
    } else if (strcmp(arg, TRACE_OPTION) == 0) {
      sboot_set_trace(TRUE);
#endif

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

  if (serial_dev_required) {
    serial_device_open(serial_device);
    serial_device_config(B9600);
  }

  while (argc--) {
    char *sboot_file_name = *argv++;
    printf("Starting MAESTRO test \"%s\".\n", sboot_file_name);
    
    input_len = read_sboot_program(sboot_file_name, &input_buf, verbose);
    run_program();

    free(input_buf);
    input_len = 0;

    printf("Ending MAESTRO test \"%s\".\n", sboot_file_name);
  }
  printf("Exiting.\n");
}

