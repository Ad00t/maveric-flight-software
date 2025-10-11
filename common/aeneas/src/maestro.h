// maestro.h  05-Dec-2011  Craig Milo Rogers <rogers@isi.edu>

#ifndef __MAESTRO_H__
#define __MAESTRO_H__

#ifdef UPPER_PPM
#define __MAESTRO__

// Define MAESTRO_SPLIT_COMPILATION to compile all the code in
// several components.  The flag can be set on the compiler's
// command line, e.g.:
//
// ccsc +FD '#MAESTRO_SPLIT_COMPILATION=true' +EXPORT main.c
// ccsc +FD +EXPORT sboot/execution/sboot_execution.c
// ccsc +FD +EXPORT sboot/execution/sboot_execution_info.c
// ccsc +FD +EXPORT sboot/maestro-programs/rom/maestro_test_programs.c
// ccsc +FD LINK="main.hex=sboot/execution/sboot_execution.o,sboot/execution/sboot_execution_info.o,sboot/maestro-programs/rom/maestro_test_programs.o,main.o"
//
//#define MAESTRO_SPLIT_COMPILATION

// Define MAESTRO_QUELL_ITAG in order to inhibit iTAG response
// processing when the MAESTRO controls the UART.
#define MAESTRO_QUELL_ITAG

// Define MAESTRO_SHARE_UART4 to include the code needed to share
// UART4 with the iTAG device.
#define MAESTRO_SHARE_UART4

#ifdef CUSTOM_PINS
#undef MAESTRO_SHARE_UART4
#endif // CUSTOM_PINS

// Define MAESTRO_UART4_PULLUP_ITAG to pull up PIC24F's UART output line
// to the iTag when the UART chip is switched to the MAESTRO's use.  This
// option is effective only when MAESTRO_SHARE_UART4 is defined.
#define MAESTRO_UART4_PULLUP_ITAG

// Define MAESTRO_UART4_MANAGE_SEND_DATA_PIN to manage the state of
// the MAESTRO_SEND_DATA pin as follows:
//
// 1) Float the pin during initialization.
// 2) Pull the pin up before powering on the MAESTRO chip.
// 3) Float the pin after powering down the MAESTRO chip.
#define MAESTRO_UART4_MANAGE_SEND_DATA_PIN

// Define MAESTRO_PROGRAMS_IN_ROM to include test programs in ROM.
#define MAESTRO_PROGRAMS_IN_ROM

// Define MAESTRO_PROGRAMS_IN_FLASH to include test programs in FLASH.
#define MAESTRO_PROGRAMS_IN_FLASH

// Define MAESTRO_SFFS to include a simple flash filesystem.
#define MAESTRO_SFFS

// If ROM test programs are supported, make the first rom program be the
// default program.  Otherwise, make raw flash the default program.
#ifndef MAESTRO_PROGRAM_NUMBER_DEFAULT
#ifdef MAESTRO_PROGRAMS_IN_ROM
#define MAESTRO_PROGRAM_NUMBER_DEFAULT 1
#else
#define MAESTRO_PROGRAM_NUMBER_DEFAULT 0
#endif // MAESTRO_PROGRAMS_IN_ROM
#endif // MAESTRO_PROGRAM_NUMBER_DEFAULT

// By default, do not show characters received on the UART.
#define MAESTRO_SHOW_RECEIVED_CHARS_DEFAULT FALSE

// By default, do not show program memory fetches.
#define MAESTRO_SHOW_PROGRAM_FETCHES_DEFAULT FALSE

// Define MAESTRO_NONBLOCKING_UART_OUTPUT to enable the nonblocking
// UART output code.  Enabling this option should decrease the
// possibility that the sboot code will hog the CPU when sending
// large blocks of data to the MAESTRO.
#define MAESTRO_NONBLOCKING_UART_OUTPUT

// By default, use nonblocking UART output mode if
// MAESTRO_NONBLOCKING_UART_OUTPUT is defined.
#define MAESTRO_NONBLOCKING_UART_OUTPUT_MODE_DEFAULT TRUE

// Define MAESTRO_UART_IO_COUNTERS to include code to count
// and report the number of characters sent and received
// by UART to/from the MAESTRO.  If MAESTRO_NONBLOCKING_UART_OUTPUT
// is also defined, there will be a counter for the number of
// times that UART output was deferred because the UART transmit
// buffer was full.  Note that this last counter may be incremented
// multiple times per character sent.
#define MAESTRO_UART_IO_COUNTERS

// The default minimal severity level for shout messages to be printed
// with sendDBGALL(...): print non-debug messages.
#define MAESTRO_SHOUT_SEVERITY_SENDDBGALL_FILTER_DEFAULT 1

// The default minimal severity level for shout messages to be queued
// for packetization: packetize non-debug messages.
#define MAESTRO_SHOUT_SEVERITY_PACKETIZE_FILTER_DEFAULT 1

// Due to a buffering limitation in packetization path, we're
// better off doing a yield after each packetized message.
#define MAESTRO_PACKETIZATION_IMPLIES_YIELD

// Should MAESTRO operation be inhibited due to power problems?
// Power level will be a small integer, probably 0==>low, 1==>medium,
// and 2==>high.  There will be two thresholds:  one for starting
// or resuming a MAESTRO program, and one for executing it.
// The power level must be greater than or equal to the
// applicable threshold.
#define MAESTRO_SYSTEM_POWER_THRESHOLD_TO_START_RUNNING_DEFAULT 2
#define MAESTRO_SYSTEM_POWER_THRESHOLD_CONTINUE_TO_RUNNING_DEFAULT 1

// There's a problem with overwritng on the hyperterminal.
#define EOL ""
//#define EOL "\r\n"

// Is the system power level sufficiently high?
// Returns TRUE when power is OK, else FALSE.
int maestro_check_system_power(unsigned int threshold);

// MAESTRO bootstrap ("sboot") glue:

void power_on_maestro();

void power_off_maestro();

void reset_maestro(int value);

void set_maestro_uart_mode(unsigned int value);

void configure_maestro_uart(unsigned int high_speed);

void restore_maestro_uart();

void initialize_maestro();

// When value == 0, do a partial initialization.
// Otherwise, initialize more completely.
void initialize_maestro_command(unsigned int value);

void start_maestro_test();

void stop_maestro_test();

void resume_maestro_test();

void run_maestro_test();

void set_maestro_program_offset(unsigned long value);
void show_maestro_program_offset();

void set_maestro_yield_limit(unsigned int value);
void show_maestro_yield_limit();

void set_maestro_trace(unsigned int value);
void show_maestro_trace();

void set_maestro_show_received_chars(unsigned int value);
void show_maestro_show_received_chars();

void set_maestro_show_program_fetches(unsigned int value);
void show_maestro_show_program_fetches();

void set_maestro_wreg(unsigned int wreg_index, unsigned long value);
void show_maestro_wreg(unsigned int wreg_index);

void set_maestro_nonblocking_uart_output_mode(unsigned int value);

void send_to_maestro_uart(unsigned int value);

#if defined(MAESTRO_QUELL_ITAG) && defined(__ITAG_H__)
int maestro_quell_itag();
#endif // MAESTRO_QUELL_ITAG && __ITAH_H__

void report_maestro_status();

void view_maestro_beacons();

void view_uart_io_counters();

void set_maestro_system_power_threshold_to_start_running(unsigned int value);
void set_maestro_system_power_threshold_to_continue_running(unsigned int value);
void show_maestro_system_power();

void set_maestro_program_timeout(unsigned int value);
void show_maestro_program_timeout();

void set_maestro_enable_program_timeout(unsigned int value);
void show_maestro_enable_program_timeout();

void set_maestro_program_start_time(unsigned long start_time);
void show_maestro_program_start_time();

void examine_maestro_program(unsigned int value);

void set_maestro_flash_execution_base(unsigned long value);

void clear_maestro_flash(unsigned long offset, unsigned long erase_len);

void copy_maestro_program_to_flash(unsigned long offset);

void set_maestro_program_number(unsigned int value);
void show_maestro_program_number();

void set_maestro_shout_severity_packetize_filter(unsigned int value);
void show_maestro_shout_severity_packetize_filter();

void set_maestro_shout_severity_senddbgall_filter(unsigned int value);
void show_maestro_shout_severity_senddbgall_filter();

int is_maestro_program_number_valid(unsigned int program_number);

int get_first_maestro_program_number();
int get_number_of_maestro_programs();

void report_number_of_test_programs();

void list_maestro_programs_brief();

void list_maestro_programs_full();

void list_maestro_program_info(unsigned int value);
void list_current_maestro_program_info();

void check_maestro_program_crc(unsigned int value);
void check_current_maestro_program_crc();

void delete_maestro_program(unsigned int program_number);

// Send a message from the dbgbuf.  It must be null-terminated.
// This routine applies a DEBUG severity level to the message.
void maestro_msg_dbgbuf();

// Send a string literal message.  It must be null-terminated.  It is copied
// to the dbgbuf.  This routine applies a DEBUG severity level to the message.
void maestro_msg(unsigned char *msg);

// Additional glue needed:
// bool find_sboot_test(char *name);

// TODO: provide beacon status.

// =============================================================
// MAESTRO UART manipulation.
//
// TODO: Move these constants to more appropriate header files.

// These are the UART parameters we want for MAESTRO use:
// #use rs232(baud=9600, UART4, bits=8, parity=E, stop=2, STREAM=COM_D, ERRORS, XMIT=PIN_F13, RCV=PIN_D15)

// These are the UART parameters we want for iTAG use:
// #use rs232(baud=COM_D_BAUD, ART4, bits=8, parity=N, stop=1, stream=COM_D, ERRORS, XMIT=PIN_D4, RCV=PIN_D12)

#define MAESTRO_UART_PORT ITAG_PORT // for ports.c routines

#define MAESTRO_UART UART4
#define MAESTRO_COM COM_D
#define MAESTRO_COM_BAUD 9600
#define MAESTRO_COM_BAUD_RESTORE COM_D_BAUD

// Special variables and constants needed for reprogramming UART 4.
#WORD PIC_U4MODE = 0x2B0
#BIT  PIC_U4MODE_BRGH = PIC_U4MODE.3
#WORD PIC_U4STA  = 0x2B2
#BIT  PIC_U4STA_UTXBF = PIC_U4STA.9
#WORD PIC_U4BRG  = 0x2B8

#define MAESTRO_UART_MODE_DEFAULT 3 // even parity, 2 stop bits


// The lower byte in RPINR27, U4RXR, gets the pin number for UART 4's
// receive data input.
#BYTE PIC_U4RXR = 0x06b6

// These two pins are used for UART 4's input.  Pin 97 (RPI42)
// is used for normal (iTag) input, and pin 39 (RP31) is used
// for MAESTRO input.
#define PIC_PIN_RP31  31
#define PIC_PIN_RPI42 42

// UART4's transmit function is U4TX, value 30.
#define PIC_U4TX_OUTPUT_FUNCTION 30

// Map a pin to this value when we don't want to drive it
// from the UART.
#define PIC_NULL_OUTPUT_FUNCTION 0

// Output pin 48 (RP5) is used for UART4 output to MAESTRO.
#BYTE PIC_RP5R = 0x06c5

// Output pin 81 (RP25) is used for UART4 output to iTag.
#BYTE PIC_RP25 = 0x6D9

// IOLOCK definitions -- these definitions might not be
// needed, now that IOLOCK is addressed in assembly code.
//
// TODO: Remove these definitions if indicated.
#BYTE PIC_OSCCON = 0x0742
#BIT PIC_OSCCON_IOLOCK = PIC_OSCCON.6
#define PIC_OSCCON_IOLOCK_UNLOCK 0
#define PIC_OSCCON_IOLOCK_LOCK 1
#define PIC_OSCCON_COMMAND1 0x46
#define PIC_OSCCON_COMMAND2 0x57

#endif // UPPER_PPM

#endif // __MAESTRO_H__
