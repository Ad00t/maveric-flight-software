/***************************************************************************

									ports.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide i/o functionality for 4 ports

Changelog:

Date 	 |  Au.  |	Notes
03-14-10	mra		Created.
04-07-10 	mra		Upgraded to include handshaking
07-28-10	mra		Now includes the pin_selects.  Note there is a level of indirection
					to the actual pin values.  See pins.h for more.

****************************************************************************
*/

#include <ports.h>

// Check #defines and error out if necessary
#ifndef U1TX_PIN
#error Undefined in ports.
#endif
#ifndef U1RX_PIN
#error Undefined in ports.
#endif
#ifndef U2TX_PIN
#error Undefined in ports.
#endif
#ifndef U2RX_PIN
#error Undefined in ports.
#endif
#ifndef U3TX_PIN
#error Undefined in ports.
#endif
#ifndef U3RX_PIN
#error Undefined in ports.
#endif
#ifndef U4TX_PIN
#error Undefined in ports.
#endif
#ifndef U4RX_PIN
#error Undefined in ports.
#endif

//========================================
//  		   PIN SELECTS
//========================================

// Port 0
#pin_select U1TX = U1TX_PIN
#pin_select U1RX = U1RX_PIN

// Port 1
#pin_select U2TX = U2TX_PIN
#pin_select U2RX = U2RX_PIN

// Port 2
#pin_select U3TX = U3TX_PIN
#pin_select U3RX = U3RX_PIN

// Port 3
#pin_select U4TX = U4TX_PIN
#pin_select U4RX = U4RX_PIN

//==================================================================
//  		Serial Port Initialization
//===================================================================
#use rs232(baud = COM_A_BAUD, UART1, bits = 8, STREAM = COM_A, ERRORS)
#use rs232(baud = COM_B_BAUD, UART2, bits = 8, STREAM = COM_B, ERRORS)
#use rs232(baud = COM_C_BAUD, UART3, bits = 8, STREAM = COM_C, ERRORS) // To/From other PIC
#use rs232(baud = COM_D_BAUD, UART4, bits = 8, STREAM = COM_D, ERRORS)

#MODULE // Limit the scope of globals declared here

//========================================
//  		 Private Globals
//========================================
static unsigned char buffer0[BUFFER_SIZE_A]; // COM A Buffer
static unsigned char buffer1[BUFFER_SIZE_B]; // COM B Buffer
static unsigned char buffer2[BUFFER_SIZE_C]; // COM C Buffer
static unsigned char buffer3[BUFFER_SIZE_D]; // COM D Buffer

volatile int next_in0 = 0; // Index of the next character to be written to the COM A buffer
volatile int next_out0 = 0; // Index of the next character to be read from the COM A buffer

volatile int next_in1 = 0; // Index of the next character to be written to the COM B buffer
volatile int next_out1 = 0; // Index of the next character to be read from the COM B buffer

volatile int next_in2 = 0; // Index of the next character to be written to the COM C buffer
volatile int next_out2 = 0; // Index of the next character to be read from the COM C buffer

volatile int next_in3 = 0; // Index of the next character to be written to the COM D buffer
volatile int next_out3 = 0; // Index of the next character to be read from the COM D buffe

// Port Configuration Control
unsigned char PORT_HANDSHAKING[NUM_PORTS] = {0, 0, 0, 0}; // Flags for handshaking per port

// Indexable pins -- CTS's that are checked to control outflow
const unsigned int PORT_CTS[NUM_PORTS] = {COM_A_CTS, COM_B_CTS, COM_C_CTS, COM_D_CTS};
// Indexable pins -- RTS's that are set to control inflow
const unsigned int PORT_RTS[NUM_PORTS] = {COM_A_RTS, COM_B_RTS, COM_C_RTS, COM_D_RTS};

// Overflow flag
// unsigned char g_overflow_detect[NUM_PORTS];		// Port overflow detection flag.  MRA: was
// removed due to weird compiler bug...

//==================================================================
//  		Interrupt Service Routines for Serial Ports
//===================================================================
#int_rda // Port 0
void serial_isr() {
	static int a;
	const unsigned char SPACE = 30;
	buffer0[next_in0] = fgetc(COM_A);
	next_in0++;
	if (next_in0 > BUFFER_SIZE_A - 1)
		next_in0 = 0; // rollover the circular buffer

	// Check for overflow
	/*	if(next_in0==next_out0)
		{
			g_overflow_detect[0]=1;	// Overflow detected
			return;
		}
	*/
	// Check if you have to send a STOP signal along the RTS line
	// This math looks really crazy, but it's hard to check a circular
	// buffer for the write pointer being "within" a range of the read
	// pointer and also "behind" it.  Regular absolute value incorrectly
	// activates when the write pointer is in "front" of the read ptr,
	// and not using aboslute value breaks on the wraparound case, where
	// wr ptr is near the top of the buffer and read is near zero.
	// So, convuluted math instead.

	if (PORT_HANDSHAKING[0]) {
		a = next_out0 - next_in0;
		if ((a <= SPACE && a > 0) || (BUFFER_SIZE_A - abs(a) <= SPACE && next_out0 < SPACE)) {
			output_high(PORT_RTS[0]); // Raise RTS to STOP
		}
	} // Handshaking
}

#int_rda2 // Port 1
void serial_isr2() {
	static int a;
	const unsigned char SPACE = 30;
	buffer1[next_in1] = fgetc(COM_B);
	next_in1++;
	if (next_in1 > BUFFER_SIZE_B - 1)
		next_in1 = 0; // rollover the circular buffer
	//   if(next_in1==next_out1) {g_overflow_detect[1]=1; return;}	// Check for overflow
	if (PORT_HANDSHAKING[1]) {
		a = next_out1 - next_in1;
		if ((a <= SPACE && a > 0) || (BUFFER_SIZE_B - abs(a) <= SPACE && next_out1 < SPACE)) {
			output_high(PORT_RTS[1]); // Raise RTS to STOP
		}
	} // Handshaking
}

#int_rda3 // Port 2
void serial_isr3() {
	static int a;
	const unsigned char SPACE = 30;
	buffer2[next_in2] = fgetc(COM_C);
	//	HexPrint(USER_PORT,&buffer2[next_in2],1);
	//	fputc('-',COM_B);
	next_in2++;
	if (next_in2 > BUFFER_SIZE_C - 1)
		next_in2 = 0; // rollover the circular buffer
	//	if(next_in2==next_out2) {g_overflow_detect[2]=1; return;}	// Check for overflow
	if (PORT_HANDSHAKING[2]) {
		a = next_out2 - next_in2;
		if ((a <= SPACE && a > 0) || (BUFFER_SIZE_C - abs(a) <= SPACE && next_out2 < SPACE)) {
			output_high(PORT_RTS[2]); // Raise RTS to STOP
		}
	} // Handshaking
}

#int_rda4 // Port 3
void serial_isr4() {
	static int a;
	const unsigned char SPACE = 30;
	buffer3[next_in3] = fgetc(COM_D);
	next_in3++;
	if (next_in3 > BUFFER_SIZE_D - 1)
		next_in3 = 0; // rollover the circular buffer
	//	if(next_in3==next_out3) g_overflow_detect[3]=1;	// Check for overflow
	if (PORT_HANDSHAKING[3]) {
		a = next_out3 - next_in3;
		if ((a <= SPACE && a > 0) || (BUFFER_SIZE_D - abs(a) <= SPACE && next_out3 < SPACE)) {
			output_high(PORT_RTS[3]); // Raise RTS to STOP
		}
	} // Handshaking
}

//---------- resetPort ----------------
// PURPOSE: Clears out the specified buffer and resets both the read and write indices to 0.
// UNMODIFIED VARIABLES: port
// MODIFIED VARIABLES: next_in#, next_out#
// GLOBALS: next_in#, next_out#, and buffer# -- where # is the port.
// RETURN VALUE: none                                                        //?? Should this have a
// return of success or failure? NOTES:
// -------------------------------------
void resetPort(int port) {
	if (port >= NUM_PORTS || port < 0)
		return; // Validate we are within range

	switch (port) {
	case 0:
		memset(buffer0, 0, sizeof(buffer0));
		next_in0 = 0;
		next_out0 = 0;
		//		g_overflow_detect[0]=0;
		break;
	case 1:
		memset(buffer1, 0, sizeof(buffer1));
		next_in1 = 0;
		next_out1 = 0;
		//		g_overflow_detect[1]=0;
		break;
	case 2:
		memset(buffer2, 0, sizeof(buffer2));
		next_in2 = 0;
		next_out2 = 0;
		//		g_overflow_detect[2]=0;
		break;
	case 3:
		memset(buffer3, 0, sizeof(buffer3));
		next_in3 = 0;
		next_out3 = 0;
		//		g_overflow_detect[3]=0;
		break;

	default:
		break;
	} // switch
} // resetPort()

//=================== Get messages via serial port (newer) ==========
/*=================================================================
  PURPOSE: To fill a buffer with characters from getByte until one
  of the following ocnditions is met:
	1) A termination character is encountered (success)
	2) A timeout occurs (failure)
	3) The maximum size limit is reached (failure)
  UNMODIFIED VARIABLES: port, term, maxsize
  MODIFIED VARIABLES: contents of ptr
  GLOBALS: SEC_TIMER
  RETURN VALUE: List of ReturnErr_t in error.h
  NOTES:
	The maximum index of the array should be such that array[maxindex] is valid,
	and array[maxindex+1] is invalid.
-----------------------------------------------------------
*/
ReturnErr_t getMSGsafe(int port, unsigned char* ptr, unsigned char term, int maxindex) {
	int n;
	ReturnErr_t retval;
	int valid;
	unsigned long timeout; // moved from globals to here

	n = 0;
	valid = 0;
	timeout = SEC_TIMER + 10; // second limit for message getting

	valid = getByte(port, &ptr[n]); // grabs the first unsigned char (maybe) and places it in ptr

	// Loop while checking time
	while (timeout > SEC_TIMER) {
		restart_wdt(); // kick the dog
		if (valid) // If getByte actually got a byte
		{
			// Check for termination character
			if (ptr[n] == term) {
				ptr[n] = '\0'; // Terminate string
				retval = SUCCESS;
				return retval;
			}

			n++; // move the index forward, effectively saving the byte
			// sprintf(dbgbuf,"n=%i,maxindex=%i",n,maxindex);
			// sendDBGALL(USER_PORT,dbgbuf);
		}

		if (n > maxindex) // Check against the maximum array size.
		// Ensures that ptr[maxsize+1] never happens.
		{
			retval = OUT_OF_BOUNDS;
			return retval;
		}

		valid = getByte(port, &ptr[n]); // get the next byte
	}
	retval = TIMEOUT; // We only get here if the timer overflows
	return retval;
}

void getNumBytesOnPort(int port, int numBytes, unsigned char* buffer) {
	switch (port) {
	case 0: {
		int i;
		for (i = 0; i < numBytes; ++i) {
			if (next_out0 + i < BUFFER_SIZE_A) {
				buffer[i] = buffer0[next_out0 + i];
			} else {
				buffer[i] = buffer0[next_out0 + i - BUFFER_SIZE_A];
			}
		}
		break;
	}
	case 1: {
		int i;
		for (i = 0; i < numBytes; ++i) {
			if (next_out1 + i < BUFFER_SIZE_B) {
				buffer[i] = buffer1[next_out1 + i];
			} else {
				buffer[i] = buffer1[next_out1 + i - BUFFER_SIZE_B];
			}
		}
		break;
	}
	case 2: {
		int i;
		for (i = 0; i < numBytes; ++i) {
			if (next_out2 + i < BUFFER_SIZE_C) {
				buffer[i] = buffer2[next_out2 + i];
			} else {
				buffer[i] = buffer2[next_out2 + i - BUFFER_SIZE_C];
			}
		}
		break;
	}
	case 3: {
		int i;
		for (i = 0; i < numBytes; ++i) {
			if (next_out3 + i < BUFFER_SIZE_D) {
				buffer[i] = buffer3[next_out3 + i];
			} else {
				buffer[i] = buffer3[next_out3 + i - BUFFER_SIZE_D];
			}
		}
		break;
	}
	}
	return;
}

//===================Send a byte =============================
void sendByte(int port, unsigned char b) {
	if (port >= NUM_PORTS || port < 0)
		return; // Validate we are within range

	switch (port) {
	case 0:
		fputc(b, COM_A);
		break;
	case 1:
		fputc(b, COM_B);
		break;
	case 2:
		fputc(b, COM_C);
		break;
	case 3:
		fputc(b, COM_D);
		break;
	}
}

//---------- Get Byte ------------------
// PURPOSE: Grabs a byte from a specified buffer
// UNMODIFIED VARIABLES: port
// MODIFIED VARIABLES: data (which is the byte grabbed), and the global next_out# corresponding to
// the port GLOBALS USED: next_in0, next_out0, next_in1, next_out1, next_in2, next_out2, next_in3,
// next_out3 RETURN VALUE: 0 (failure), 1 (success) NOTES:
// -------------------------------------
int getByte(int port, unsigned char* data) {
	int retval;
	retval = 0; // Assume failure

	if (port >= NUM_PORTS || port < 0)
		return retval; // Validate we are within range

	switch (port) {
	case 0:
		if (next_in0 != next_out0) {
			*data = buffer0[next_out0];
			next_out0++;
			if (next_out0 > BUFFER_SIZE_A - 1)
				next_out0 = 0;
			retval = 1;
			if (input_state(PORT_RTS[0]) && PORT_HANDSHAKING[0]) {
				output_low(PORT_RTS[0]); // Clear to send since we've cleared a unsigned char
			}
		}
		break;

	case 1: // UART 1
		if (next_in1 != next_out1) {
			*data = buffer1[next_out1];
			next_out1++;
			if (next_out1 > BUFFER_SIZE_B - 1)
				next_out1 = 0;
			retval = 1;
			if (input_state(PORT_RTS[1]) && PORT_HANDSHAKING[1]) {
				output_low(PORT_RTS[1]); // Clear to send since we've cleared a unsigned char
			}
		}
		break;
	case 2:
		if (next_in2 != next_out2) {
			*data = buffer2[next_out2];
			next_out2++;
			if (next_out2 > BUFFER_SIZE_C - 1)
				next_out2 = 0;
			retval = 1;
			if (input_state(PORT_RTS[2]) && PORT_HANDSHAKING[2]) {
				output_low(PORT_RTS[2]); // Clear to send since we've cleared a unsigned char
			}
		}
		break;
	case 3:
		if (next_in3 != next_out3) {
			*data = buffer3[next_out3];
			next_out3++;
			if (next_out3 > BUFFER_SIZE_D - 1)
				next_out3 = 0;
			retval = 1;
			if (input_state(PORT_RTS[3]) && PORT_HANDSHAKING[3]) {
				output_low(PORT_RTS[3]); // Clear to send since we've cleared a unsigned char
			}
		}
		break;
	default:
		break;
	}

	return retval;
}

//---------- Check Byte ------------------------------------------
// PURPOSE: Checks if there is a byte to grab on a specified port
// UNMODIFIED VARIABLES: port
// MODIFIED VARIABLES: none
// GLOBALS USED: next_in0, next_out0, next_in1, next_out1, next_in2, next_out2, next_in3, next_out3
// RETURN VALUE: 0 (nothing to read), nonzero (number of bytes to read)
// NOTES:
// -------------------------------------------------------------
int CheckByte(int port) {
	int retval;
	retval = 0;
	int a;

	if (port >= NUM_PORTS || port < 0)
		return retval; // Validate we are within range

	switch (port) {
	case 0:
		a = next_in0 - next_out0;
		if (a == 0)
			retval = 0;
		if (a > 0)
			retval = a;
		if (a < 0)
			retval = a + BUFFER_SIZE_A;
		break;
	case 1:
		a = next_in1 - next_out1;
		if (a == 0)
			retval = 0;
		if (a > 0)
			retval = a;
		if (a < 0)
			retval = a + BUFFER_SIZE_B;
		break;
	case 2:
		a = next_in2 - next_out2;
		if (a == 0)
			retval = 0;
		if (a > 0)
			retval = a;
		if (a < 0)
			retval = a + BUFFER_SIZE_C;
		break;
	case 3:
		a = next_in3 - next_out3;
		if (a == 0)
			retval = 0;
		if (a > 0)
			retval = a;
		if (a < 0)
			retval = a + BUFFER_SIZE_D;
		break;
	default:
		break;
	}

	return retval;
}

ReturnErr_t sendMSG(int port, unsigned char* buf, int len) {
	int n;
	unsigned long timeout;
	const unsigned long DELTA_TIME = 1;
	if (port >= NUM_PORTS || port < 0)
		return FAILURE; // Validate we are within range
	restart_wdt();
	if (PORT_HANDSHAKING[port]) {
		timeout = SEC_TIMER + DELTA_TIME;
		switch (port) {
		case 0:
			for (n = 0; n < len; n++) {
				if (!input_state(PORT_CTS[0])) {
					fputc(buf[n], COM_A);
					timeout = SEC_TIMER + DELTA_TIME;
					// sendByte(port, buf[n]);
				} else {
					n--;
				}
				if (SEC_TIMER > timeout)
					return FAILURE;
			}
			break;
		case 1:
			for (n = 0; n < len; n++) {
				if (!input_state(PORT_CTS[1])) {
					fputc(buf[n], COM_B);
					timeout = SEC_TIMER + DELTA_TIME;
				} else {
					n--;
				}
				if (SEC_TIMER > timeout)
					return FAILURE;
			}
			break;
		case 2:
			for (n = 0; n < len; n++) {
				if (!input_state(PORT_CTS[2])) {
					fputc(buf[n], COM_C);
					timeout = SEC_TIMER + DELTA_TIME;
				} else {
					n--;
				}
				if (SEC_TIMER > timeout)
					return FAILURE;
			}
			break;
		case 3:
			for (n = 0; n < len; n++) {
				if (!input_state(PORT_CTS[3])) {
					fputc(buf[n], COM_D);
					timeout = SEC_TIMER + DELTA_TIME;
				} else {
					n--;
				}
				if (SEC_TIMER > timeout)
					return FAILURE;
			}
			break;
		default:
			break;
		} // switch
	} // if
	else // if handshaking is off
	{
		for (n = 0; n < len; n++) {
			sendByte(port, buf[n]);
		}
	}
	return SUCCESS;
}

// Quick function to send a whole message.
void sendMSGALL(int port, unsigned char* msg) {
	if (port >= NUM_PORTS || port < 0)
		return;
	sendMSG(port, msg, strlen(msg));
}

void setPortHandshaking(int port, unsigned char OnOff) {
	if (port >= NUM_PORTS || port < 0)
		return;
	PORT_HANDSHAKING[port] = OnOff;
}

unsigned char getPortHandshaking(int port) {
	if (port >= NUM_PORTS || port < 0)
		return -1;
	return PORT_HANDSHAKING[port];
}

unsigned char* getPointerToIncomingDataOnPort(int port) {
	switch (port) {
	case 0:
		return &buffer0[next_out0];
	case 1:
		return &buffer1[next_out1];
	case 2:
		return &buffer2[next_out2];
	case 3:
		return &buffer3[next_out3];
	default:
		return NULL;
	}
}

void incrementNumReadBytesBy(int port, int numBytes) {
	switch (port) {
	case 0:
		next_out0 += numBytes;
		if (next_out0 >= BUFFER_SIZE_A) {
			next_out0 -= BUFFER_SIZE_A;
		}
		return;
	case 1:
		next_out1 += numBytes;
		if (next_out1 >= BUFFER_SIZE_B) {
			next_out1 -= BUFFER_SIZE_B;
		}
		return;
	case 2:
		next_out2 += numBytes;
		if (next_out2 >= BUFFER_SIZE_C) {
			next_out2 -= BUFFER_SIZE_C;
		}
		return;
	case 3:
		next_out3 += numBytes;
		if (next_out3 >= BUFFER_SIZE_D) {
			next_out3 -= BUFFER_SIZE_D;
		}
		return;
	default:
		return;
	}
}

void setRTS(int port) // Sets the incoming CTS pin (stops data flow)
{
	if (port >= NUM_PORTS || port < 0)
		return;
	if (PORT_HANDSHAKING[port])
		output_high(PORT_RTS[port]);
}

void clearRTS(int port) // Clears the incoming CTS pin (allows data flow)
{
	if (port >= NUM_PORTS || port < 0)
		return;
	if (PORT_HANDSHAKING[port])
		output_low(PORT_RTS[port]);
}

// 1 for high, 0 for low.  -1 for error
unsigned char checkRTS(int port) // Checks the RTS pin
{
	if (port >= NUM_PORTS || port < 0)
		return -1;

	switch (port) {
	case 0:
		return input_state(PORT_RTS[0]);
	case 1:
		return input_state(PORT_RTS[1]);
	case 2:
		return input_state(PORT_RTS[2]);
	case 3:
		return input_state(PORT_RTS[3]);
	default:
		return -1;
	}
}

unsigned char checkCTS(int port) // Checks the CTS pin (checks if we can send outgoing data)
{
	if (port >= NUM_PORTS || port < 0)
		return -1;

	switch (port) {
	case 0:
		return input_state(PORT_CTS[0]);
	case 1:
		return input_state(PORT_CTS[1]);
	case 2:
		return input_state(PORT_CTS[2]);
	case 3:
		return input_state(PORT_CTS[3]);
	default:
		return -1;
	}
}

// This should be in ports.c
void InitPorts() {
	// Clear out the buffers
	resetPort(0); // Reset port 0
	resetPort(1); // Reset port 1
	resetPort(2); // Reset port 2
	resetPort(3); // Reset port 3

	// Default Handshaking Off
	setPortHandshaking(0, HANDSHAKE_OFF); //
	setPortHandshaking(1, HANDSHAKE_OFF); //
	setPortHandshaking(2, HANDSHAKE_OFF); //
	setPortHandshaking(3, HANDSHAKE_OFF); //

	// Enable the interrupts
	enable_interrupts(
		INT_RDA); // These are the interrupt service routines to grab incoming serial characters
	enable_interrupts(INT_RDA2);
	enable_interrupts(INT_RDA3);
	enable_interrupts(INT_RDA4);

	// Set baud rates and enable
	setup_uart(COM_A_BAUD, COM_A); // Enables the ports
	setup_uart(COM_B_BAUD, COM_B); //
	setup_uart(COM_C_BAUD, COM_C); //
	setup_uart(COM_D_BAUD, COM_D); //
}

//========================================
// 		Development Functions
//========================================

// Note: The below functions are only necessary for testing and development.  They
// can be removed during actual production

#ifdef DEBUG

// Quick function to generate fake data.
unsigned char fakeData() {
	unsigned int num;
	unsigned char retval;

	num = rand(); // From 0 to RAND_MAX
	// num = num * (255/RAND_MAX);	// Scales the input to something between 0 and 255.
	retval = (unsigned char)num;
	// retval = 'b';

	return retval; // returns a random byte
}

void fakeStringOnPort(int port, unsigned char* string, int len) {
	int i;

	for (i = 0; i < len; i++) {
		switch (port) {
		case 0:
			buffer0[next_in0] = string[i];
			next_in0++;
			if (next_in0 > BUFFER_SIZE_A - 1)
				next_in0 = 0;
			break;
		case 1:
			buffer1[next_in1] = string[i];
			next_in1++;
			if (next_in1 > BUFFER_SIZE_B - 1)
				next_in1 = 0;
			break;
		case 2:
			buffer2[next_in2] = string[i];
			next_in2++;
			if (next_in2 > BUFFER_SIZE_C - 1)
				next_in2 = 0;
			break;
		case 3:
			buffer3[next_in3] = string[i];
			next_in3++;
			if (next_in3 > BUFFER_SIZE_D - 1)
				next_in3 = 0;
			break;
		default:
			break;
		} // switch
	} // for
}

// Generate fake data on a port.
// This mimics the action of the Interrupt service routines.
void fakeDataOnPort(int port, int len) {
	int i;

	for (i = 0; i < len; i++) {
		switch (port) {
		case 0:
			buffer0[next_in0] = fakeData();
			next_in0++;
			if (next_in0 > BUFFER_SIZE_A - 1)
				next_in0 = 0;
			break;
		case 1:
			buffer1[next_in1] = fakeData();
			next_in1++;
			if (next_in1 > BUFFER_SIZE_B - 1)
				next_in1 = 0;
			break;
		case 2:
			buffer2[next_in2] = fakeData();
			next_in2++;
			if (next_in2 > BUFFER_SIZE_C - 1)
				next_in2 = 0;
			break;
		case 3:
			buffer3[next_in3] = fakeData();
			next_in3++;
			if (next_in3 > BUFFER_SIZE_D - 1)
				next_in3 = 0;
			break;
		default:
			break;
		} // switch
	} // for
}

void fakeCharOnPort(int port, unsigned char fake) {
	switch (port) {
	case 0:
		buffer0[next_in0] = fake;
		next_in0++;
		if (next_in0 > BUFFER_SIZE_A - 1)
			next_in0 = 0;
		break;
	case 1:
		buffer1[next_in1] = fake;
		next_in1++;
		if (next_in1 > BUFFER_SIZE_B - 1)
			next_in1 = 0;
		break;
	case 2:
		buffer2[next_in2] = fake;
		next_in2++;
		if (next_in2 > BUFFER_SIZE_C - 1)
			next_in2 = 0;
		break;
	case 3:
		buffer3[next_in3] = fake;
		next_in3++;
		if (next_in3 > BUFFER_SIZE_D - 1)
			next_in3 = 0;
		break;
	default:
		break;
	} // switch
}

int CheckOverFlowFlag(int port) {
	//	if(g_overflow_detect[port]!=0) return 1;
	//	else return 0;
	return 0;
}

void ClearOverFlow(int port) {
	//	g_overflow_detect[port]=0;
	return;
}

#endif