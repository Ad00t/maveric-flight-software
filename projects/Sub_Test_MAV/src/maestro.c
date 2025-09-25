// maestro.c  05-Dec-2011  Craig Milo Rogers <rogers@isi.edu>

#include "maestro.h"

#ifdef __MAESTRO__

#include "flash.h"

#include "sboot_types.h"
#include "sboot_execution.h"
#include "sboot_execution_info.h"
#include "sboot_execution_state.h"
#include "sboot_features.h"
#include "sboot_callbacks.h"
#include "sboot_enable_maestro_phase.h"

#ifdef MAESTRO_PROGRAMS_IN_ROM
#include "sboot/includes/sboot_rom_programs.h"
#endif // MAESTRO_PROGRAMS_IN_ROM

#ifdef MAESTRO_PROGRAMS_IN_FLASH
#include "maestro_flash.h"
#endif // MAESTRO_PROGRAMS_IN_FLASH


// Which MAESTRO test program should we execute?
//
// 0 ==> Execute a program from raw flash memory.  If there are multiple
//       programs in flash memory, the initial program offset can be
//       used to select a specific program.
// else  The program number identifies a specific program in ROM.
// 
static uint8 maestro_program_number;

// MAESTRO UART parity and stop bit settings.
static uint8 maestro_uart_mode;

// Do we want to see characters received on the UART?
//
// This flag is reset to its default value when the MAESTRO code is
// initialized.
static bool maestro_show_received_chars;

// Do we want to see program memory bytes as they
// are accessed?
//
// This flag is reset to its default value when the MAESTRO code is
// initialized.
static bool maestro_show_program_fetches;

#ifdef MAESTRO_NONBLOCKING_UART_OUTPUT
// Do we want to use nonblocking UART output?
//
// This flag is reset to its default value when the MAESTRO code is
// initialized.
static bool maestro_nonblocking_uart_output_mode;
#endif // MAESTRO_NONBLOCKING_UART_OUTPUT

// These are the MAESTRO beacon values.
//
// TODO: Should these be defined in a header somewhere?
unsigned int maestro_beacon1; // Enabled bit, state-before-stop, Next state and current state.
unsigned int maestro_beacon2; // Low 16 bits of program offset.
unsigned int maestro_beacon3; // Program number, upper 8 bits of program offset.
unsigned int maestro_beacon4; // Identifies the last non-debug shout.
unsigned long maestro_beacon5; // MAESTRO BOOT_PASS vector0;
unsigned long maestro_beacon6; // MAESTRO BOOT_PASS vector1;

#ifdef MAESTRO_QUELL_ITAG
#ifdef __ITAG_H__
static bool quellItag;
#endif // __ITAH_H__
#endif // MAESTRO_QUELL_ITAG

#ifdef MAESTRO_UART_IO_COUNTERS
// Counters for the number of characters sent to or received
// from the MAESTRO by UART.
//
// Note: These counters will be reset whenever the
// MAESTRO is reset.  Thus, if you run multiple MAESTRO
// test programs, only the values for the last test program
// will be reported.
static unsigned long maestro_uart_chars_sent;
static unsigned long maestro_uart_chars_received;

#ifdef MAESTRO_NONBLOCKING_UART_OUTPUT
// Count the number of times that UART output was defered
// to avoid blocking.
static unsigned long maestro_uart_nonblocking_path_taken;
#endif // MAESTRO_NONBLOCKING_UART_OUTPUT
#endif // MAESTRO_UART_IO_COUNTERS

// Filter shout messages based on severity level.  Messages below the
// filter level are ignored.  There is one filter setting for console-style
// output (sendDBGALL), and a separate filter setting for packetize-and-push
// output.
static uint8 maestro_shout_severity_senddbgall_filter;
static uint8 maestro_shout_severity_packetize_filter;

#ifdef MAESTRO_PACKETIZATION_IMPLIES_YIELD
// Due to a buffering limitation in packetization path, we're
// better off doing a yield after each packetized message.
static bool maestro_packetization_yield;
#endif // MAESTRO_PACKETIZATION_IMPLIES_YIELD

// Should MAESTRO operation be inhibited due to power problems?
// The power level will be a small integer, probably 0==>low, 1==>medium,
// and 2==>high.  There will be two thresholds:  one for starting
// or resuming a MAESTRO program, and one for continuing to execute it.
// The power level must be greater than or equal to the applicable
// threshold.
static uint8 maestro_system_power_threshold_to_start_running;
static uint8 maestro_system_power_threshold_to_continue_running;

// Make the controls over program timeout be easier to use by
// saving them across soft inits.
static uint16 maestro_program_timeout;
static bool maestro_enable_program_timeout;
static bool maestro_override_program_timeout;

// Forward definition for code in "main.c":
//
// TODO: This really ought to be in a header file.
#ifdef __MYSTACK__
void MaestroPacketizeAndPush(unsigned char * msg, int len, int max_len);
#endif // __MYSTACK__

unsigned char getBatteryState(void);

// MAESTRO bootstrap ("sboot") glue:

// The message is in the dbgbuf.  It must be null-terminated.
// "len" must exclude the NUL termination character.
//
// TODO: len is not needed when not packetizing.  Should we optimize?
void sboot_send_msg(sboot_shout_severity_t severity, uint16 len) {
  if (severity >= maestro_shout_severity_senddbgall_filter) {
    sendDBGALL(USER_PORT, "\r\n");
    sendDBGALL(USER_PORT, dbgbuf);
    if (strlen(EOL)) {
      sendDBGALL(USER_PORT, EOL);
    }
  }

#ifdef __MYSTACK__
  if (severity >= maestro_shout_severity_packetize_filter) {
    MaestroPacketizeAndPush(dbgbuf, (int)len, DBG_BUF_SIZE);

#ifdef MAESTRO_PACKETIZATION_IMPLIES_YIELD
    // We should set maestro_packetization_yield when we packetize
    // a shout.
    maestro_packetization_yield = TRUE;
#endif // MAESTRO_PACKETIZATION_IMPLIES_YIELD
  }
#endif // __MYSTACK__
}

// Send a message from the dbgbuf.  It must be null-terminated.
// This routine applies a DEBUG severity level to the message.
//
// TODO: I probably want another routine like this that applies
// a higher severity level to the message.
void maestro_msg_dbgbuf() {
  int len;
  len = strlen(dbgbuf);
  sboot_send_msg(SBOOT_SHOUT_DEBUG, len);
}

// Send a string literal message.  It must be null-terminated.  It is copied
// to the dbgbuf.  This routine applies a DEBUG severity level to the message.
//
// TODO: I probably want another routine like this that applies
// a higher severity level to the message.
void maestro_msg(unsigned char *msg) {
  int len;
  len = strlen(msg);
  memcpy(dbgbuf, msg, len);
  dbgbuf[len] = '\0';
  sboot_send_msg(SBOOT_SHOUT_DEBUG, len);
}

// Is the system power level sufficiently high?
// Returns TRUE when power is OK, else FALSE.
int maestro_check_system_power(unsigned int threshold) {
  return getBatteryState() >= threshold;
}

// Power on the MAESTRO board(s) in response to a program command or a user
// command.  Ensure that other lines, suce as MAESTRO_SEND_DATA and
// MAESTRO_RESET, are in appropriate initial states.
void power_on_maestro() {
  maestro_msg("Powering on the MAESTRO.");
#ifdef MAESTRO_UART4_MANAGE_SEND_DATA_PIN
  output_high(MAESTRO_SEND_DATA);
#endif // MAESTRO_UART4_MANAGE_SEND_DATA_PIN
  output_low(MAESTRO_RESET);
  output_high(MAESTRO_PWR);
}

// Power off the MAESTRO board(s) in reponse to a program command or a
// user command.  Ensure that other lines, such as MAESTRO_SEND_DATA and
// MAESTRO_RESET, are in appropriate states.
static void power_off_maestro_actions() {
  output_low(MAESTRO_RESET);
  output_low(MAESTRO_PWR);
#ifdef MAESTRO_UART4_MANAGE_SEND_DATA_PIN
  output_float(MAESTRO_SEND_DATA);
#endif // MAESTRO_UART4_MANAGE_SEND_DATA_PIN
}

// Power off the MAESTRO in response to a user command.  Issue a feedback
// message telling the user that we're executing the command.
void power_off_maestro() {
  maestro_msg("Powering off the MAESTRO.");
  power_off_maestro_actions();
}

// Manipulate the MAESTRO reboot line.
//
// This routine exists solely to convert an int into the known
// TRUE and FALSE values, acting as a buffer between "main.c"
// and "sboot_execute.c".
void reset_maestro(int value) {
  if (value) {
    sboot_reset_maestro(TRUE);
  } else {
    sboot_reset_maestro(FALSE);
  }
}

// This special sequence unlocks the PIC's pin routing
// control registers.
//
// Note: The code generated by the CCS PCD compile is not the
// same as that shown in the assembly language example in
// PIC24FJ256GA110_Family_Data_Sheet_revB.pdf.  The difference
// is timing:  the CCS compiler's generate code will perform
// a data memory read between each write to the OSCCON register,
// while the example code performs back-to-back writes.  It
// may be necessary to replace the C code, below, with assembly
// language code.
//
// TODO: Test if this code works, and replace it if necessary.
//
// TODO: Move this code somewhere more appropriate, and add a
// header file for it.
void pic_iolock_unlock() {
  // IOLOCK unlock
  // PIC_OSCCON = PIC_OSCCON_COMMAND1;
  // PIC_OSCCON = PIC_OSCCON_COMMAND2;
  // PIC_OSCCON_IOLOCK = PIC_OSCCON_IOLOCK_UNLOCK;
  //
  // Note: In the following code, CCS PCD changes the
  // "BCLR" to "BCLR.B".  Is this important?
  //
  // TODO: Test to see if this works.
#ASM ASIS
  MOV #0x0742,W1
  MOV #0x46,W2
  MOV #0x57,W3
  MOV.B W2,[W1]
  MOV.B W3,[W1]
  BCLR 0x0742,#6
#ENDASM
}

// This special sequence locks the PIC's pin routing
// control registers.
//
// Note: The code generated by the CCS PCD compile is not the
// same as that shown in the assembly language example in
// PIC24FJ256GA110_Family_Data_Sheet_revB.pdf.  The difference
// is timing:  the CCS compiler's generate code will perform
// a data memory read between each write to the OSCCON register,
// while the example code performs back-to-back writes.  It
// may be necessary to replace the C code, below, with assembly
// language code.
//
// TODO: Test if this code works, and replace it if necessary.
//
// TODO: Move this code somewhere more appropriate, and add a
// header file for it.
void pic_iolock_lock() {
  // IOLOCK lock
  // PIC_OSCCON = PIC_OSCCON_COMMAND1;
  // PIC_OSCCON = PIC_OSCCON_COMMAND2;
  // PIC_OSCCON_IOLOCK = PIC_OSCCON_IOLOCK_LOCK;
  //
  // Note: In the following code, CCS PCD changes the
  // "BSET" to "BSET.B".  Is this important?
  //
  // TODO: Test to see if this works.
#ASM ASIS
  MOV #0x0742,W1
  MOV #0x46,W2
  MOV #0x57,W3
  MOV.B W2,[W1]
  MOV.B W3,[W1]
  BSET 0x0742,#6
#ENDASM
}

// The MAESTRO shares a UART, UART4, with the iTAG on the upper PIC24
// processor.  The serial data lines from the MAESTRO and the iTAG are
// connected to different pins on the PIC24.  PIC24 Peripheral
// Pin Selection is used to dynamically connect UART4 to the proper
// pins.
//
// One concern with this arrangement is:  what is the state
// of the serial lines when the UART is not connected to the pins?  Are
// they floating, or are there pullup/pulldown resistors?  Some
// potential problems are:
//
// 1) floating serial lines could result in noise characters being
//    received by the PIC24, the MAESTRO or the iTAG.
//
// 2) floating lines might accumulate unwanted charge.
//
// Noise coming going into the MAESTRO probably doesn't matter,
// because we will reset the MAESTRO after connecting it.
//
// Noise coming into the PIC24 after connection to the MAESTRO
// might matter to the current MAESTRO bootstrap code, although
// it may be possible to handle it by adding an (additional?)
// delay and input buffer flush to the initialization sequence.
//
// Noise coming into the PIC24 after connection to the iTAG
// might be a problem;  I do not know.
//
// Noise coming into the iTAG could be a problem.
//
// One possible fix to this problem is to program the serial
// output pins as parallel output portbits using the TRIS register,
// driving a suitable bit value (high or low) when the UART is
// disconnected from the pin.
//
// TODO: Evaluate the state of the disconnected serial lines,
// and ameliorate problems if possible.
//
// STATUS: There are now compile-time options that will enable
// the following behavor:
//
// 1) The pin carrying serial data to the iTAG will be pulled up
//    when the UART is being used for the MAESTRO.
//
// 2) The pin carrying serial data to the MAESTRO will be left
//    floating when the MAESTRO is not in use.

// This sets the MAESTRO UART mode, but it does not take effect
// until configure_maestro_uart() is called.
void set_maestro_uart_mode(unsigned int value) {
  if (value < 6) {
    sprintf(dbgbuf, "Setting the MAESTRO UART mode to 0x%x.", value);
    maestro_msg_dbgbuf();
    maestro_uart_mode = value;
  } else {
    sprintf(dbgbuf, "Invalid MAESTRO UART mode %u.", value);
    maestro_msg_dbgbuf();
  }
}

// Configure UART4 for MAESTRO use.
//
// TODO: Wait for queued output to finish.
void configure_maestro_uart(unsigned int high_speed) {
  unsigned int mode;
  mode = 0x8000 | maestro_uart_mode;
  // Note:  %x prints only the lowest byte -- CCS PCD bug.
  sprintf(dbgbuf, "Configuring the MAESTRO UART to mode 0x%Lx on port %u.",
	  (unsigned long)mode, MAESTRO_UART_PORT);
  maestro_msg_dbgbuf();

  // Default: need to ensure 8 data bits, even parity, 2 stop bits.
  // #use rs232(baud=MAESTRO_COM_BAUD, MAESTRO_UART, bits=8, parity=E, stop=2, stream=MAESTRO_COM, ERRORS)
  // setup_uart(MAESTRO_COM_BAUD, MAESTRO_COM);
  PIC_U4MODE = mode;
  PIC_U4STA = 0x400;

  if (high_speed) {
    PIC_U4MODE_BRGH = 1;
    PIC_U4BRG = 0x22;
  } else {
    PIC_U4MODE_BRGH = 1;
    PIC_U4BRG = 0x1A0;
  }

#ifdef MAESTRO_QUELL_ITAG
#ifdef __ITAG_H__
  quellItag = TRUE;
#endif // __ITAH_H__
#endif // MAESTRO_QUELL_ITAG

#ifdef MAESTRO_SHARE_UART4

#ifdef MAESTRO_UART4_PULLUP_ITAG
  // Ensure that the UART send line to the iTAG is not left floating.
  output_high(U4TX_PIN);
#endif // MAESTRO_UART4_PULLUP_ITAG

  // Need to route the UART to the MAESTRO output pins.
  // I tried using XMIT=PIN_F13, RCV=PIN_D15 in the #use rs232(...)
  // call, above, but the code it generated looked wrong.
  // UART4 Receive: PIN_F13 = PIC pin 39 (RP31)
  // UART4 RX: RPINR27 U4RXR<5:0>
  // U4TX: PIN_D15 = PIC pin 48 (RP5)
  pic_iolock_unlock();
  PIC_U4RXR = PIC_PIN_RP31;  
  PIC_RP5R = PIC_U4TX_OUTPUT_FUNCTION;
  PIC_RP25 = PIC_NULL_OUTPUT_FUNCTION;
  pic_iolock_lock();
#endif // MAESTRO_SHARE_UART4

  // Clear any buffered input and start fresh.
  // TODO: Perhaps there should ba a delay before this reset?
  resetPort(MAESTRO_UART_PORT);
  setPortHandshaking(MAESTRO_UART_PORT, 0);// ASSUME no hardware handshaking.
}

// Restore the UART settings to thw way they were before they
// were configured for MAESTRO use.  Instead of saving the settings during
// MAESTRO setup and restoring the settings here, this code just sets
// the settings to what it thinks are the appropriate values for the iTAG.
//
// TODO: Wait for queued output to finish.
void restore_maestro_uart() {
  maestro_msg("Restoring the MAESTRO UART to iTAG mode.");

#ifdef MAESTRO_SHARE_UART4
  // Need to ensure 8 data bits, no parity, 1 stop bits.
  // #use rs232(baud=MAESTRO_COM_BAUD_RESTORE, MAESTRO_UART, bits=8, parity=N, stop=1, stream=MAESTRO_COM, ERRORS)
  // setup_uart(MAESTRO_COM_BAUD_RESTORE, MAESTRO_COM);
  PIC_U4MODE = 0x8000;
  PIC_U4STA = 0x400;
  PIC_U4MODE_BRGH = 1;
  PIC_U4BRG = 0x22;

  // UART4 Receive: PIN_D12 = PIC pin 79 (RPI42)
  // UART4 RX: RPINR27 U4RXR<5:0>
  // UART4 Transmit:  PIN_D4 = PIC pin 81 (RP25)
  pic_iolock_unlock();
  PIC_U4RXR = PIC_PIN_RPI42;  
  PIC_RP25 = PIC_U4TX_OUTPUT_FUNCTION;
  PIC_RP5R = PIC_NULL_OUTPUT_FUNCTION;
  pic_iolock_lock();
#endif // MAESTRO_SHARE_UART4

  // Clear any buffered input and start fresh.
  // TODO: Perhaps there should ba a delay before this reset?
  resetPort(MAESTRO_UART_PORT);
  setPortHandshaking(MAESTRO_UART_PORT, 0);// ASSUME no hardware handshaking.

#ifdef MAESTRO_QUELL_ITAG
#ifdef __ITAG_H__
  quellItag = FALSE;
#endif // __ITAH_H__
#endif // MAESTRO_QUELL_ITAG
}

#ifdef MAESTRO_QUELL_ITAG
#ifdef __ITAG_H__
int maestro_quell_itag() {
  return quellItag;
}
#endif // __ITAH_H__
#endif // MAESTRO_QUELL_ITAG

// Update the beacon variables.
//
// TODO: Use named constants for these field shifts.
// TODO: This code may be called fairly often.  Is it worth trying to optimize
// it more?
void update_maestro_beacons() {
  if (SBOOT_NUMBER_OF_EXECUTION_STATES < 16) {
    unsigned int boot_pass_succeeded;
    boot_pass_succeeded = 0;
    if (sboot_get_boot_pass_succeeded()) {
      boot_pass_succeeded= 1 << 15;
    }
    unsigned int boot_pass_failed;
    boot_pass_failed = 0;
    if (sboot_get_boot_pass_failed()) {
      boot_pass_failed = 1 << 14;
    }
    unsigned int boot_done;
    boot_done = 0;
    if (sboot_get_boot_done()) {
      boot_done = 1 << 13;
    }
    unsigned int maestro_is_enabled;
    maestro_is_enabled = 0;
    if (sboot_is_maestro_enabled()) {
      maestro_is_enabled = 1 << 12;
    }
    maestro_beacon1 =
      boot_pass_succeeded | boot_pass_failed | boot_done | maestro_is_enabled |
      (((unsigned int)sboot_get_execution_state_before_stop_byte()) << 8) |
      (((unsigned int)sboot_get_execution_next_state_byte()) << 4) |
      (unsigned int)sboot_get_execution_state_byte();
  } else {
    maestro_beacon1 =
      make16(sboot_get_execution_next_state_byte(),
	     sboot_get_execution_state_byte());
  }

// TODO: Check the assumption that the program offset does
// not exceed 24 bits.
  uint32 program_offset;
  program_offset = sboot_get_program_offset();
  maestro_beacon2 = (unsigned int)(program_offset & 0xffff);
  maestro_beacon3 =
    (((unsigned int)maestro_program_number) << 8) |
    (unsigned int)(program_offset >> 16);

  // maestro_beacon4 holds the most recent shout severity and reason codes,
  // excluding shouts with reason code SBOOT_SHOUT_NONE.  See
  // sboot_send_shout(...) for more details.

  // TODO: Perhaps I should set these throuh a callback
  // interface, rather than poll for them.
  maestro_beacon5 = sboot_get_boot_pass_vector0();
  maestro_beacon6 = sboot_get_boot_pass_vector1();
}

static void initialize_maestro_common() {
  output_low(MAESTRO_RESET); // Ensure that the MAESTRO is in a reset state.
  sboot_set_trace(FALSE); // TODO: Take this out of common.
  sboot_initialize(maestro_program_number);

#if defined(MAESTRO_QUELL_ITAG) && defined(__ITAG_H__)
  quellItag = FALSE;
#endif // MAESTRO_QUELL_ITAG && __ITAG_H__

#ifdef MAESTRO_PACKETIZATION_IMPLIES_YIELD
  maestro_packetization_yield = FALSE;
#endif // MAESTRO_PACKETIZATION_IMPLIES_YIELD

  maestro_beacon4 = SBOOT_SHOUT_NONE;
  update_maestro_beacons();
}

// This is called whenever the PIC24 code restarts, such as
// on powerup, or when a hard reset is requested through the
// command interface.
void initialize_maestro() {
  power_off_maestro_actions();
  maestro_program_number = MAESTRO_PROGRAM_NUMBER_DEFAULT;

  maestro_uart_mode = MAESTRO_UART_MODE_DEFAULT;

  maestro_show_received_chars = MAESTRO_SHOW_RECEIVED_CHARS_DEFAULT;
  maestro_show_program_fetches = MAESTRO_SHOW_PROGRAM_FETCHES_DEFAULT;

#ifdef MAESTRO_NONBLOCKING_UART_OUTPUT
  maestro_nonblocking_uart_output_mode = MAESTRO_NONBLOCKING_UART_OUTPUT_MODE_DEFAULT;
#endif // MAESTRO_NONBLOCKING_UART_OUTPUT

#ifdef MAESTRO_UART_IO_COUNTERS
  maestro_uart_chars_sent = 0;
  maestro_uart_chars_received = 0;
#ifdef MAESTRO_NONBLOCKING_UART_OUTPUT
  maestro_uart_nonblocking_path_taken = 0;
#endif // MAESTRO_NONBLOCKING_UART_OUTPUT
#endif// MAESTRO_UART_IO_COUNTERS

  maestro_shout_severity_senddbgall_filter = MAESTRO_SHOUT_SEVERITY_SENDDBGALL_FILTER_DEFAULT;
  maestro_shout_severity_packetize_filter = MAESTRO_SHOUT_SEVERITY_PACKETIZE_FILTER_DEFAULT;

#ifdef MAESTRO_PROGRAMS_IN_FLASH
  initialize_maestro_flash();
#endif // MAESTRO_PROGRAMS_IN_FLASH

  maestro_system_power_threshold_to_start_running =
    MAESTRO_SYSTEM_POWER_THRESHOLD_TO_START_RUNNING_DEFAULT;
  maestro_system_power_threshold_to_continue_running =
    MAESTRO_SYSTEM_POWER_THRESHOLD_CONTINUE_TO_RUNNING_DEFAULT;

  maestro_program_timeout = SBOOT_PROGRAM_TIMEOUT_DEFAULT;
  maestro_enable_program_timeout = SBOOT_ENABLE_PROGRAM_TIMEOUT_DEFAULT;
  maestro_override_program_timeout = SBOOT_OVERRIDE_PROGRAM_TIMEOUT_DEFAULT;

  initialize_maestro_common();
}

// This is called to perform a user-commanded MAESTRO initialization.
void initialize_maestro_command(unsigned int value) {
  if (value) {
    maestro_msg("Hard MAESTRO initialization.");
    initialize_maestro();
  } else {
    maestro_msg("Soft MAESTRO initialization.");
    initialize_maestro_common();
  }
}

// Start a MAESTRO test program running, after checking the current system
// power level.  Provide feedback to the user, and force in a few values
// with settings we want to remember across soft inits.
void start_maestro_test() {
  if (!maestro_check_system_power((unsigned int)maestro_system_power_threshold_to_start_running)) {
    sprintf(dbgbuf, "Cannot start the MAESTRO test program %u due to insufficient system power.", maestro_program_number);
    maestro_msg_dbgbuf();
    return;
  }

  sprintf(dbgbuf, "Starting MAESTRO test program %u.", maestro_program_number);
  maestro_msg_dbgbuf();
  sboot_set_program_timeout(maestro_program_timeout);
  sboot_set_enable_program_timeout(maestro_enable_program_timeout);
  sboot_set_override_program_timeout(maestro_override_program_timeout);
  sboot_start();
  update_maestro_beacons();
}

void stop_maestro_test() {
  maestro_msg("Stopping the MAESTRO test.");
  sboot_stop();
  update_maestro_beacons();
}

void resume_maestro_test() {
  if (!maestro_check_system_power((unsigned int)maestro_system_power_threshold_to_start_running)) {
    sprintf(dbgbuf, "Cannot resume the MAESTRO test program %u due to insufficient system power.", maestro_program_number);
    maestro_msg_dbgbuf();
    return;
  }

  maestro_msg("Resuming the MAESTRO test.");
  sboot_resume();
  update_maestro_beacons();
}

// This is called by the background task dispatcher in main().
void run_maestro_test() {
  sboot_execute();
  update_maestro_beacons();
}

void set_maestro_program_offset(unsigned long value) {
  sprintf(dbgbuf, "Setting the MAESTRO program offset to 0x%Lx (%Lu).",
	  value, value);
  maestro_msg_dbgbuf();
  sboot_set_program_offset((sboot_size_t)value);

  // Set the current command offset, too, so the output of
  // report_maestro_status() is a little more sane.
  sboot_set_current_command_offset((sboot_size_t)value);

  update_maestro_beacons();
}
void show_maestro_program_offset() {
  unsigned long value;
  value = sboot_get_program_offset();
  sprintf(dbgbuf, "The MAESTRO program offset is 0x%Lx (%Lu).",
	  value, value);
  maestro_msg_dbgbuf();
}

// Get/set the max number of commands to execute before automatically yielding
// control.
void set_maestro_yield_limit(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO yield limit to %u.", value);
  maestro_msg_dbgbuf();
  sboot_set_max_commands_before_yield((uint16)value);
  update_maestro_beacons();
}
void show_maestro_yield_limit() {
  unsigned int value;
  value = sboot_get_max_commands_before_yield();
  sprintf(dbgbuf, "The MAESTRO yield limit is %u.", value);
  maestro_msg_dbgbuf();
}

// Get/set the trace flag, which controls certain low-level debugging output.
// The trace flag is supported by sboot only when the SBOOT_TRACE flag is
// defined during compilation.
void set_maestro_trace(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO trace flag to %d.",
	  value);
  maestro_msg_dbgbuf();
  if (value) {
    sboot_set_trace(TRUE);
  } else {
    sboot_set_trace(FALSE);
  }
  update_maestro_beacons();
}
void show_maestro_trace() {
  bool value;
  value = sboot_get_trace();
  if (value) {
    sprintf(dbgbuf, "The MAESTRO trace flag is TRUE.");
  } else {
    sprintf(dbgbuf, "The MAESTRO trace flag is FALSE.");
  }
  maestro_msg_dbgbuf();
}

// Get/set a flag that controls low-level debugging feedback for characters
// that are received from the UART.
void set_maestro_show_received_chars(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO show-received-chars flag to %d.",
	  value);
  maestro_msg_dbgbuf();
  if (value) {
    maestro_show_received_chars = TRUE;
  } else {
    maestro_show_received_chars = FALSE;
  }
  update_maestro_beacons();
}
void show_maestro_show_received_chars() {
  if (maestro_show_received_chars) {
    maestro_msg("The MAESTRO show-received-chars flag is TRUE");
  } else {
    maestro_msg("The MAESTRO show-received-chars flag is FALSE");
  }
}

// Get/set a flag that controls low-level debugging feedback for 
// program fetches.
void set_maestro_show_program_fetches(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO show-program-fetches flag to %d.",
	  value);
  maestro_msg_dbgbuf();
  if (value) {
    maestro_show_program_fetches = TRUE;
  } else {
    maestro_show_program_fetches = FALSE;
  }
  update_maestro_beacons();
}
void show_maestro_show_program_fetches() {
  if (maestro_show_program_fetches) {
   maestro_msg("The MAESTRO show-program-fetches flag is TRUE.");
  } else {
   maestro_msg("The MAESTRO show-program-fetches flag is FALSE.");
  }
}

// Get/set the WREG registers.
void set_maestro_wreg(unsigned int wreg_index, unsigned long value) {
  sprintf(dbgbuf, "Setting the MAESTRO wreg %u to %Lu (0x%Lx).",
	  wreg_index, value,value);
  maestro_msg_dbgbuf();
  sboot_set_wreg((wreg_t)wreg_index, (uint32)value);
}

void show_maestro_wreg(unsigned int wreg_index) {
  uint32 value;
  bool success;
  success = sboot_get_wreg(wreg_index, &value);
  if (success) {
    sprintf(dbgbuf, "MAESTRO wreg %d = %Lu (0x%Lx).",
	    wreg_index, value, value);
  } else {
    sprintf(dbgbuf, "Error getting MAESTRO wreg %d", wreg_index);
  }
  maestro_msg_dbgbuf();
}

/// Set whether or not to use nonblocking output for the UART.
// There is no corresponding getter, but this flag is included 
// in the report_maestro_status() feedback.
void set_maestro_nonblocking_uart_output_mode(unsigned int value) {
#ifdef MAESTRO_NONBLOCKING_UART_OUTPUT
  sprintf(dbgbuf, "Setting the MAESTRO nonblocking-uart-output-mode flag to %d.",
	  value);
  maestro_msg_dbgbuf();
  if (value) {
    maestro_nonblocking_uart_output_mode = TRUE;
  } else {
    maestro_nonblocking_uart_output_mode = FALSE;
  }
  update_maestro_beacons();
#else // MAESTRO_NONBLOCKING_UART_OUTPUT
  maestro_msg("MAESTRO nonblocking UART mode is not supported.");
#endif // MAESTRO_NONBLOCKING_UART_OUTPUT
}

// Send a character to the UART.
void send_to_maestro_uart(unsigned int value) {
  sprintf(dbgbuf, "Sending %d to the MAESTRO.", value);
  maestro_msg_dbgbuf();
  if (!sboot_uart_send(value)) {
    sprintf(dbgbuf "... sboot_uart_send(%d) failed!", value);
    maestro_msg_dbgbuf();
  }
  update_maestro_beacons();
}

// No beacon update should be necessary for this routine.
// TODO: Should any of these feedback messages go to the
// ground over the radio?
void report_maestro_status() {
  unsigned long command_offset;
  command_offset = (unsigned long)sboot_get_current_command_offset();
  unsigned long additional_offset;
  additional_offset = ((unsigned long)sboot_get_program_offset()) - command_offset;
  sprintf(dbgbuf, "Maestro: %u@%Lu+%Lu [n=%u s=%u e=%u] [f%u m%u p%u q%u r%u t%u y%u]",
	  (unsigned int)sboot_get_execution_state_byte(),
	  command_offset, additional_offset,

	  (unsigned int)sboot_get_execution_next_state_byte(),
	  (unsigned int)sboot_get_execution_state_before_stop_byte(),
	  (unsigned int)sboot_is_maestro_enabled(),

	  (unsigned int)maestro_show_program_fetches,
	  (unsigned int)maestro_shout_severity_senddbgall_filter,
	  (unsigned int)maestro_program_number,
	  (unsigned int)maestro_shout_severity_packetize_filter,
	  (unsigned int)maestro_show_received_chars,
	  (unsigned int)sboot_get_trace(),
	  (unsigned int)sboot_get_max_commands_before_yield()
);
  maestro_msg_dbgbuf();
}

// No beacon update should be necessary for this routine.
//
// TODO: Recode this to use maestro_msg_dbgbuf().
void view_maestro_beacons() {
  sprintf(dbgbuf, "Maestro beacons: %04Lx %04Lx %04Lx %04Lx %08Lx %08Lx",
	  (unsigned long)maestro_beacon1,
	  (unsigned long)maestro_beacon2,
	  (unsigned long)maestro_beacon3,
	  (unsigned long)maestro_beacon4,
	  (unsigned long)maestro_beacon5,
	  (unsigned long)maestro_beacon6);
  maestro_msg_dbgbuf();
}

// When MAESTRO_UART_IO_COUNTERS is defined, report the
// IO counter values.
//
// Note: These counters will be reset whenever the
// MAESTRO is reset.  Thus, if you run multiple MAESTRO
// test programs, only the values for the last test program
// will be reported.
void view_uart_io_counters() {
#ifdef MAESTRO_UART_IO_COUNTERS
  sprintf(dbgbuf, "Maestro UART chars sent=%Lu received=%Lu",
	  maestro_uart_chars_sent, maestro_uart_chars_received);
  maestro_msg_dbgbuf();
#ifdef MAESTRO_NONBLOCKING_UART_OUTPUT
  sprintf(dbgbuf, "Maestro UART nonblockingPathTaken=%Lu",
	  maestro_uart_nonblocking_path_taken);
  maestro_msg_dbgbuf();
#endif // MAESTRO_NONBLOCKING_UART_OUTPUT

#else // MAESTRO_UART_IO_COUNTERS
  maestro_msg("Maestro UART IO counters not supported.");
#endif // MAESTRO_UART_IO_COUNTERS
}

// TODO: check if the value is valid before using it.
void set_maestro_system_power_threshold_to_start_running(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO system power start threshold to %u",
	  maestro_system_power_threshold_to_start_running);
  maestro_msg_dbgbuf();
  maestro_system_power_threshold_to_start_running = value;
}

// TODO: check if the value is valid before using it.
void set_maestro_system_power_threshold_to_continue_running(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO system power continue threshold to %u",
	  maestro_system_power_threshold_to_continue_running);
  maestro_msg_dbgbuf();
  maestro_system_power_threshold_to_continue_running = value;
}

void show_maestro_system_power() {
  unsigned int state = getBatteryState();
  sprintf(dbgbuf, "MAESTRO system power thresholds: run=%u continue=%u battery=%u",
	  maestro_system_power_threshold_to_start_running,
	  maestro_system_power_threshold_to_continue_running,
	  state);
  maestro_msg_dbgbuf();
}

void set_maestro_program_timeout(unsigned int value) {
  // TODO: range check "value"
  unsigned long value32;
  value32 = (unsigned long)value; // Avoid compiler bug in hex format
  sprintf(dbgbuf, "Setting the MAESTRO program timeout to %u (0x%Lx)", value, value32);
  maestro_msg_dbgbuf();
  maestro_program_timeout = value;
  sboot_set_program_timeout(value);
}
void show_maestro_program_timeout() {
  unsigned int value;
  value = sboot_get_program_timeout();
  unsigned long value32;
  value32 = (unsigned long)value; // Avoid compiler bug in hex format.

  // Unless the override flag was set, the program timeout in sboot
  // may have been changed from the value saved in "maestro.c".  If
  // a change has occured, show both values.
  if (value == maestro_program_timeout) {
    sprintf(dbgbuf, "MAESTRO program timeout: sboot=%u (0x%Lx)", value, value32);
  } else {
    unsigned long maestro_program_timeout32;
    maestro_program_timeout32 = (unsigned long)maestro_program_timeout;
    sprintf(dbgbuf, "MAESTRO program timeout: sboot=%u (0x%Lx), maestro=%u (0x%Lx)",
	    value, value32, maestro_program_timeout, maestro_program_timeout32);
  }
  maestro_msg_dbgbuf();
}

void set_maestro_enable_program_timeout(unsigned int value) {
  // TODO: range check "value"
  sprintf(dbgbuf, "Setting the MAESTRO program timeout enable to %u", value);
  maestro_msg_dbgbuf();
  maestro_enable_program_timeout = value;
  sboot_set_enable_program_timeout(value);
}
void show_maestro_enable_program_timeout() {
  unsigned int value;
  value = sboot_get_enable_program_timeout();
  sprintf(dbgbuf, "The MAESTRO program timeout enable is %u", value);
  maestro_msg_dbgbuf();
}

void set_maestro_override_program_timeout(unsigned int value) {
  // TODO: range check "value"
  sprintf(dbgbuf, "Setting the MAESTRO program timeout override to %u", value);
  maestro_msg_dbgbuf();
  maestro_override_program_timeout = value;
  sboot_set_override_program_timeout(value);
}
void show_maestro_override_program_timeout() {
  unsigned int value;
  value = sboot_get_override_program_timeout();
  sprintf(dbgbuf, "The MAESTRO program timeout override is %u", value);
  maestro_msg_dbgbuf();
}

void set_maestro_program_start_time(unsigned long start_time) {
  sprintf(dbgbuf, "Setting the MAESTRO program start time to %Lu", start_time);
  maestro_msg_dbgbuf();
  sboot_set_program_start_time(start_time);
}
void show_maestro_program_start_time() {
  unsigned long value;
  value = sboot_get_program_start_time();
  sprintf(dbgbuf, "The MAESTRO program start time is %Lu", value);
  maestro_msg_dbgbuf();
}

// Print 4 lines of 4 bytes each.  Originally, I printed
// one line with 10 bytes, but sprintf(...) apparently failed.
//
// If the value passed is true, auto-increment the program
// offset.
//
// TODO: Should this code have its own program offset?
//
// No beacon update should be necessary for this routine.
// TODO: Should any of these feedback messages go to the
// ground over the radio?
void examine_maestro_program(unsigned int value) {
  unsigned long program_offset;
  program_offset = sboot_get_program_offset();

  int i;
  for (i = 0; i < 4; i++) {
    unsigned int byte0;
    byte0 = (unsigned int)sboot_fetch_program_byte(maestro_program_number, program_offset + 0);
    unsigned int byte1;
    byte1 = (unsigned int)sboot_fetch_program_byte(maestro_program_number, program_offset + 1);
    unsigned int byte2;
    byte2 = (unsigned int)sboot_fetch_program_byte(maestro_program_number, program_offset + 2);
    unsigned int byte3;
    byte3 = (unsigned int)sboot_fetch_program_byte(maestro_program_number, program_offset + 3);

    sprintf(dbgbuf, "MAESTRO program %u: 0x%Lx (%Lu): %u %u %u %u",
	    maestro_program_number, program_offset, program_offset,
	    byte0, byte1, byte2, byte3);
    maestro_msg_dbgbuf();
    program_offset += 4;
  }
  if (value) {
    sboot_set_program_offset((sboot_size_t)program_offset);
  }
}

int is_maestro_program_number_valid(unsigned int program_number) {
  if (program_number == 0) {
#ifdef MAESTRO_PROGRAMS_IN_FLASH
    return is_maestro_flash_program_valid();
#else
    result = FALSE;
#endif // MAESTRO_PROGRAMS_IN_FLASH
  } else {
#ifdef MAESTRO_PROGRAMS_IN_ROM
    int number_of_rom_programs;
    number_of_rom_programs = sboot_number_of_rom_programs();
    if (program_number <= number_of_rom_programs) {
      return TRUE;
    }
#endif // MAESTRO_PROGRAMS_IN_ROM

#ifdef MAESTRO_SFFS
#ifndef MAESTRO_PROGRAMS_IN_ROM
    int number_of_rom_programs;
    number_of_rom_programs = 0;
#endif // MAESTRO_PROGRAMS_IN_ROM

    return maestro_sffs_is_program_valid(program_number - (number_of_rom_programs + 1));
#endif // MAESTRO_SFFS
  }
  return FALSE;
}

int get_first_maestro_program_number() {
#ifdef MAESTRO_PROGRAMS_IN_FLASH
  return 0;
#else
  return 1;
#endif MAESTRO_PROGRAMS_IN_FLASH
}

int get_number_of_maestro_programs() {
  int number_of_programs;

#ifdef MAESTRO_PROGRAMS_IN_FLASH
  number_of_programs = 1;
#else
  number_of_programs = 0;
#endif MAESTRO_PROGRAMS_IN_FLASH

#ifdef MAESTRO_PROGRAMS_IN_ROM
  // Avoid possible compiler bug.
  number_of_programs = number_of_programs +
    sboot_number_of_rom_programs();
#endif // MAESTRO_PROGRAMS_IN_ROM

#ifdef MAESTRO_SFFS
  number_of_programs = number_of_programs +
    maestro_sffs_get_number_of_slots();
#endif // MAESTRO_SFFS

  if (number_of_programs > 255) {
    number_of_programs = 255; // TODO: Use a named constant.
  }
  return number_of_programs;
}

// Report the first test program number, the number of
// test programs, and the number of valid test programs.
void report_number_of_test_programs() {
  int number_of_valid_programs;
  number_of_valid_programs = 0;

  int number_of_programs;
  number_of_programs = get_number_of_maestro_programs();

  int first_program_number;
  first_program_number = get_first_maestro_program_number();

  sboot_program_number_t program_number;
  for (program_number = first_program_number;
       program_number < number_of_programs;
       program_number++) {

    if (is_maestro_program_number_valid(program_number)) {
      number_of_valid_programs = number_of_valid_programs + 1; // Avoid possible compiler bug.
    }
  }

  uchar programs_in_flash;
#ifdef MAESTRO_PROGRAMS_IN_FLASH
  programs_in_flash = 'Y';
#else
  programs_in_flash = 'N';
#endif // MAESTRO_PROGRAMS_IN_FLASH

  uchar programs_in_rom;
#ifdef MAESTRO_PROGRAMS_IN_ROM
  programs_in_rom = 'Y';
#else
  programs_in_rom = 'N';
#endif // MAESTRO_PROGRAMS_IN_ROM

  uchar programs_in_sffs;
#ifdef MAESTRO_SFFS
  programs_in_sffs = 'Y';
#else
  programs_in_sffs = 'N';
#endif // MAESTRO_SFFS

  sprintf(dbgbuf, "MAESTRO: #programs=%u, #valid=%u, first=%u (FLASH=%c ROM=%c SFFS=%c)",
	  number_of_programs, number_of_valid_programs, first_program_number,
	  programs_in_flash, programs_in_rom, programs_in_sffs);
  maestro_msg_dbgbuf();
}

// List the test names of the programs.
void list_maestro_programs_brief() {
  int number_of_valid_programs;
  number_of_valid_programs = 0;

  int number_of_programs;
  number_of_programs = get_number_of_maestro_programs();

  int first_program_number;
  first_program_number = get_first_maestro_program_number();

  sboot_program_number_t program_number;
  for (program_number = first_program_number;
       program_number < number_of_programs;
       program_number++) {

    if (!is_maestro_program_number_valid(program_number)) {
      // TODO: Control when we present this feedback?
      // sprintf(dbgbuf, "MAESTRO program %u: <invalid>", program_number);
      // maestro_msg_dbgbuf();
      continue;
    }
    number_of_valid_programs = number_of_valid_programs + 1; // Avoid possible compiler bug.

    uchar test_name_buf[SBOOT_TEST_NAME_MAX_LEN + 1]; // extra char for null.
    uint16 value_len;
    if (!sboot_get_program_info(program_number, SBOOT_INFO_TEST_NAME,
				test_name_buf, SBOOT_TEST_NAME_MAX_LEN,
				&value_len)) {
      sprintf(dbgbuf, "MAESTRO program %u: <no test name>", program_number);
      maestro_msg_dbgbuf();
      continue;
    }

    // Null terminate the stored string.
    if (value_len > SBOOT_TEST_NAME_MAX_LEN) {
      value_len = SBOOT_TEST_NAME_MAX_LEN;
    }
    test_name_buf[value_len] = '\0';

    sprintf(dbgbuf, "MAESTRO program %u: %s", program_number, test_name_buf);
    maestro_msg_dbgbuf();
  }

  if (number_of_valid_programs == 0) {
    maestro_msg("No MAESTRO test programs are available.");
  }
}

// List fascinating information about the available MAESTRO test programs.
void list_maestro_programs_full() {
  int number_of_valid_programs;
  number_of_valid_programs = 0;

  int number_of_programs;
  number_of_programs = get_number_of_maestro_programs();

  sboot_program_number_t program_number;
  for (program_number = get_first_maestro_program_number();
       program_number < number_of_programs;
       program_number++) {

    if (!is_maestro_program_number_valid(program_number)) {
      continue;
    }
    number_of_valid_programs = number_of_valid_programs + 1; // Avoid possible compiler bug.

    maestro_msg("=============");
    sprintf(dbgbuf, "MAESTRO program %u:", program_number);
    maestro_msg_dbgbuf();
    sboot_show_program_info(program_number);
  }
  maestro_msg("=============");

  if (number_of_valid_programs == 0) {
    maestro_msg("No MAESTRO test programs are available.");
  }
}

// List fascinating information about the current program, which may be
// in flash or ROM.
void list_maestro_program_info(unsigned int program_number) {
  sprintf(dbgbuf, "MAESTRO program %u:", program_number);
  maestro_msg_dbgbuf();

  sboot_show_program_info(program_number);
}
void list_current_maestro_program_info() {
  list_maestro_program_info(maestro_program_number);
}

// Check the CRC of the current program, which may be in flash or ROM.
void check_maestro_program_crc(unsigned int program_number) {
  if (sboot_check_program_crc(program_number)) {
    sprintf(dbgbuf, "MAESTRO program %u: CRC check passed.", program_number);
  } else {
    sprintf(dbgbuf, "MAESTRO program %u: CRC check failed.", program_number);
  }
  maestro_msg_dbgbuf();
}
void check_current_maestro_program_crc() {
  check_maestro_program_crc(maestro_program_number);
}

// Set the current program number. Program number 0 is "raw" flash memory. ROM
// program numbers follow, then "sffs" flash programs.
void set_maestro_program_number(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO program number to %u.",
	  value);
  maestro_msg_dbgbuf();
  maestro_program_number = (uint8)value;
  sboot_set_program_number(maestro_program_number);
  update_maestro_beacons();
}
void show_maestro_program_number() {
  sprintf(dbgbuf, "The MAESTRO program number is %u.",
	  maestro_program_number);
  maestro_msg_dbgbuf();
}

void set_maestro_shout_severity_senddbgall_filter(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO shout severity sendDBGALL filter to %u.",
	  value);
  maestro_msg_dbgbuf();
  maestro_shout_severity_senddbgall_filter = value;
}
void show_maestro_shout_severity_senddbgall_filter() {
  sprintf(dbgbuf, "The MAESTRO shout severity sendDBGALL filter is set to %u.",
	  maestro_shout_severity_senddbgall_filter);
  maestro_msg_dbgbuf();
}

void set_maestro_shout_severity_packetize_filter(unsigned int value) {
  sprintf(dbgbuf, "Setting the MAESTRO shout severity packetize filter to %u.",
	  value);
  maestro_msg_dbgbuf();
  maestro_shout_severity_packetize_filter = value;
}
void show_maestro_shout_severity_packetize_filter() {
  sprintf(dbgbuf, "The MAESTRO shout severity packetize filter is set to %u.",
	  maestro_shout_severity_packetize_filter);
  maestro_msg_dbgbuf();
}

void delete_maestro_program(unsigned int program_number) {
  if (program_number == 0) {
    maestro_msg("Cannot delete program 0");
    return;

  } else {
#ifdef MAESTRO_PROGRAMS_IN_ROM
    int number_of_rom_programs;
    number_of_rom_programs = sboot_number_of_rom_programs();
    if (program_number <= number_of_rom_programs) {
      sprintf(dbgbuf, "Cannot delete program %u from ROM", program_number);
      maestro_msg_dbgbuf();
      return;
    }
#endif // MAESTRO_PROGRAMS_IN_ROM

#ifdef MAESTRO_SFFS
#ifndef MAESTRO_PROGRAMS_IN_ROM
    int number_of_rom_programs;
    number_of_rom_programs = 0;
#endif // MAESTRO_PROGRAMS_IN_ROM

    unsigned int slotnum;
    slotnum = program_number - (number_of_rom_programs + 1);

    if (maestro_sffs_mark_program_deleted(slotnum)) {
      sprintf(dbgbuf, "Deleted program %u from flash sffs slot %u",
	      program_number, slotnum);
      maestro_msg_dbgbuf();
    } else {
      sprintf(dbgbuf, "Error deleting program %u from flash sffs slot %u",
	      program_number, slotnum);
      maestro_msg_dbgbuf();
    }
#endif // MAESTRO_SFFS
  }
  return;
}

// Additional glue needed:
// bool find_sboot_test(char *name);

// MAESTRO bootstrap ("sboot") callback functions:

uint16 sboot_fetch_program_byte(sboot_program_number_t program_number,
				sboot_size_t offset) {
  uint16 result;
  result = SBOOT_FETCH_ERROR;

  if (program_number == 0) {
#ifdef MAESTRO_PROGRAMS_IN_FLASH
    result =  sboot_fetch_flash_program_byte((uint32)offset);
#endif // MAESTRO_PROGRAMS_IN_FLASH

  } else {
#ifdef MAESTRO_PROGRAMS_IN_ROM
#ifdef MAESTRO_SFFS
    int number_of_rom_programs;
    number_of_rom_programs = sboot_number_of_rom_programs();
    if (program_number <= number_of_rom_programs) {
      result = sboot_fetch_rom_program_byte(program_number, (uint32)offset);
    } else {
      result = maestro_sffs_fetch_program_byte(program_number - number_of_rom_programs - 1, (uint32)offset);
    }
#else // MAESTRO_SFFS
    result = sboot_fetch_rom_program_byte(program_number, (uint32)offset);
#endif // MAESTRO_SFFS

#else // MAESTRO_PROGRAMS_IN_ROM

#ifdef MAESTRO_SFFS
      result = maestro_sffs_fetch_program_byte(program_number - 1, (uint32)offset);
#endif // MAESTRO_SFFS

#endif // MAESTRO_PROGRAMS_IN_ROM
  }

  if (maestro_show_program_fetches) {
    sprintf(dbgbuf, ">>%u: 0x%Lx (%Lu)=%u", maestro_program_number, offset, offset, result);
    maestro_msg_dbgbuf();
  }
  return result;
}

// When the passed value is true, start the reset.  When the
// value is false, end the reset.  Alternatively, start a reset
// cycle (but don't wait if it takes long!) when passed a true
// value, and ignore calls with a false value.
void sboot_reset_maestro(bool value) {
  if (value) {
    maestro_msg("Starting MAESTRO reset.");
    output_low(MAESTRO_RESET);
  } else {
    maestro_msg("Finishing MAESTRO reset.");
    output_high(MAESTRO_RESET);
  }
}

// Enable the MAESTRO chip.  This may involve turning on power, saving
// the current UART configuration, changing the UART configuration,
// and changing the UART routing.  It may be necessary for this
// activity to take place in several phases, with waits inbetween.
//
// An alternative implementation would be to have seperate callbacks
// for power-on, UART reconfiguration and UART rerouting.  I opted
// for this design in an attempt to reduce the impact to the code base of
// last-minute changes in this area.
//
// Note that resetting the MAESTRO ought to force the UART into low
// speed mode.
//
// TODO: Do we need to flush the UART?
// TODO: Restructure initialization.
void sboot_enable_maestro(sboot_enable_maestro_phase_t phase) {
  switch (phase) {
  case SBOOT_POWER_ON_MAESTRO: {
    power_on_maestro();
    break;
  }

  case SBOOT_CONFIGURE_MAESTRO_UART: {
    configure_maestro_uart(FALSE);
    break;
  }

  default:
    maestro_msg("Invalid phase in sboot_enable_maestro.");
  }
}

// Disable the MAESTRO chip.  This may involve turning off
// power, restoring the UART configuration, and rerouting the UART.
// I chose not to implement phases with possible time delays, because
// it may be necessary to call this routine during an abnormal exit
// from an sboot progam.
//
// TODO: are phases necessary here?
// TODO: Do we need to flush the UART?
void sboot_disable_maestro() {
  power_off_maestro();
  restore_maestro_uart();
}

sboot_timer_t sboot_current_seconds() {
  return (sboot_timer_t) SEC_TIMER;
}

// Switch the UART between low speed (9600 bps) and high speed
// (115200 bps) operation.
//
// TODO: Generalize to other speeds.
//
// Returns TRUE if the change appears to have succeeded.
bool sboot_uart_change_speed(bool high_speed) {
  // This is overkill, in the sense that it may mess with
  // the iTAG more than is necessary.
  //
  // TODO: Track and change settings only when needed?
  configure_maestro_uart(high_speed);
  return TRUE;
}

// Queue a byte in the UART for transmission to the MAESTRO.  Unless the
// MAESTRO_NONBLOCKING_UART_OUTPUT option is selected at compile time, this
// code might block if the UART's transmit buffer is full.
// 
bool sboot_uart_send(uint8 byteval) {

#ifdef MAESTRO_NONBLOCKING_UART_OUTPUT
  if (maestro_nonblocking_uart_output_mode) {
    if (PIC_U4STA_UTXBF) {

#ifdef MAESTRO_UART_IO_COUNTERS
      maestro_uart_nonblocking_path_taken =
	maestro_uart_nonblocking_path_taken + 1; // Avoid possible compiler bug.
#endif // MAESTRO_UART_IO_COUNTERS
      return FALSE;
    }
  }
#endif // MAESTRO_NONBLOCKING_UART_OUTPUT

  sendByte(MAESTRO_UART_PORT, (unsigned char)byteval);

#ifdef MAESTRO_UART_IO_COUNTERS
  maestro_uart_chars_sent =
    maestro_uart_chars_sent + 1; // Avoid possible compiler bug.
#endif // MAESTRO_UART_IO_COUNTERS
  return TRUE;
}

uint16 sboot_uart_receive() {
  unsigned char buf;
  if (getByte(MAESTRO_UART_PORT, &buf)) {
    if (maestro_show_received_chars) {
      sprintf(dbgbuf, "Received 0x%x", buf);
      maestro_msg_dbgbuf();
    }
#ifdef MAESTRO_UART_IO_COUNTERS
    maestro_uart_chars_received =
      maestro_uart_chars_received + 1; // Avoid possible compiler bug.
#endif // MAESTRO_UART_IO_COUNTERS
    return (uint16)buf;
  } else {
    return SBOOT_UART_NOT_READY;
  }
}

// Return a pointer to an output buffer.  Returns TRUE if a buffer
// is available, else FALSE.  The buffer will be returned to
// the callback environment before the next yield.
//
// In exceptional circumstances, sboot_get_shout_buf(...) might
// be called twice in a row without an intervening call to
// sboot_send_shout(...).  It should be safe to return the same
// buffer in those circumstances.
bool sboot_get_shout_buf(uchar **bufp, uint16 *buf_lenp) {
  *bufp = dbgbuf;
  *buf_lenp = DBG_BUF_SIZE;
  return TRUE;
}

void sboot_send_shout(sboot_shout_severity_t severity,
		      sboot_shout_reason_t reason,
		      uint16 len) {
  if (reason != SBOOT_SHOUT_NONE) {
    maestro_beacon4 =
      (((unsigned int)severity) << 14) | ((unsigned int)reason);
  }
  sboot_send_msg(severity, len);
}

// Return TRUE when sboot should yield ASAP.
bool sboot_should_yield_now() {
#ifdef MAESTRO_PACKETIZATION_IMPLIES_YIELD
  if (maestro_packetization_yield) {
    maestro_packetization_yield = FALSE;
    return TRUE;
  }
#endif // MAESTRO_PACKETIZATION_IMPLIES_YIELD
  return RunTasks == SCHEDULED_TASKS;
}

// Return TRUE when sboot should shut down ASAP, e.g., due to an anomoly
// in the power system.
bool sboot_should_shut_down() {
  // TODO: Find appropriate conditions to test, or have an routine, called
  // under a number of conditions, to set a flag that's tested by this
  // routine.
  return !maestro_check_system_power((unsigned int)maestro_system_power_threshold_to_continue_running);
}

// Get the number of MAESTRO programs.
sboot_program_number_t sboot_get_number_of_programs() {
  return get_number_of_maestro_programs();
}

// Get the first MAESTRO program number:
sboot_program_number_t sboot_get_first_program_number() {
  return get_first_maestro_program_number();
}

// Validate the MAESTRO program number.
// Returns TRUE if the program number is valid, else FALSE.
bool sboot_validate_program_number(sboot_program_number_t program_number) {
  return is_maestro_program_number_valid(program_number);
}

// Per AENEAS coding conventions, include code modules directly rather
// than use separate compilation.
//
// TODO: It should be possible to separately compile "maestro_flash.c".
#ifdef MAESTRO_PROGRAMS_IN_FLASH
#include "maestro_flash.c"
#endif // MAESTRO_PROGRAMS_IN_FLASH

#ifndef MAESTRO_SPLIT_COMPILATION
#include "sboot/execution/sboot_execution.c"
#include "sboot/execution/sboot_execution_info.c"

#ifdef MAESTRO_PROGRAMS_IN_ROM
#include "sboot/maestro-programs/rom/maestro_test_programs.c"
#endif // MAESTRO_PROGRAMS_IN_ROM
#endif // MAESTRO_SPLIT_COMPILATION

#endif // __MAESTRO__
