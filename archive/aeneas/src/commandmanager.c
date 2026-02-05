#include "commandmanager.h"

#include "config.h"
#include "globals.h"

static int typeOfChar(unsigned char c);
static unsigned char CharsToHex(unsigned char first4bits,
								unsigned char second4bits);
static void Alpha2Hex(unsigned char* c);

/*=================== ParseCommandString ===================
  PURPOSE: Take a null-terminated string of numbers and substrings and convert it to
		   a format that can be processed by ProcessCmd().
  UNMODIFIED VARIABLES: original
  MODIFIED VARIABLES: processed
  GLOBALS: none
  RETURN VALUE:  ReturnErr_t type, specifying errors or not.
  NOTES:   The returned string is null-terminated, with the first byte being the length, BUT...

	!!! Do not read the returned string using any built-in string commands.  It may have
	a false null termination if the user entered in a 0 as a command.

	If commandBuffer is not big enough to handle the result, bad juju happens!

	Cannot handle multiple commands currently.

	Output can be reversed with function UnParse().  It may look different, but it will process the
same.

	Examples of parsable strings:
	r 151 // Basic command for manual reboot
	r 158 0 127 "abcde" // More advanced command for forwarding a string to a port.  Uses quotemode.
	r 99 7 13 5 152 6 *000000* 7 158 0 4 "abcd" // Very advanced command. Uses both hexmode and
quotemode.

-----------------------------------------------------------*/
#define MAX_CMD_BUFFER_SIZE 256
ReturnErr_t ParseCommandString(unsigned char* commandString, unsigned char* commandBuffer) {
	int j = 1; // Keeps track of the writing position
	int type = 0; // Holds the type of digit read
	unsigned char c; // A unsigned char for holding the current character read
	unsigned char* ptr; // A ptr for reading
	unsigned char* next; // A temporary ptr used when converting digits
	unsigned char finished = 0; // Flag for finishing
	unsigned char quotemode = 0; // Flag for whether we are inside a pair of quotes or not
	unsigned char hexmode =
		0; // Flag for whether we are inside a pair of asterisks (to interpret hex)
	unsigned char temp[MAX_CMD_BUFFER_SIZE]; // Maximum size of array we can process.

	// Validate input
	if (commandString == NULL)
		return INVALID_ARG; // Null pointer
	if (commandBuffer == NULL)
		return INVALID_ARG; // Null pointer

	ptr = commandString; // Set up the reading pointer

	// Loop across the string, deciding what to do based on the character encountered.
	while (!finished) {
		// Read the character
		c = *ptr;

		// Classify the unsigned char as alpha, numeric, quotes, space, etc...
		type = typeOfChar(c);

		switch (type) {
		case 0: // Anything other than a-z,A-Z,0-9,space,single/double quotes,or NULL or /
			// Action: skip it, unless in quotemode or hexmode
			if (quotemode) {
				temp[j] = c;
				j++;
			} else if (hexmode)
				return INVALID_ARG; // In hexmode, there should only be 0-9,A-F
			ptr++;
			break;
		case 1: // a-z,A-Z
			// Action: copy it directly to the output, unless in hexmode
			// If in hexmode, convert this unsigned char and the next to a single unsigned char
			if (hexmode) {
				if (typeOfChar(*(ptr + 1)) != 1 && typeOfChar(*(ptr + 1)) != 2)
					return FAILURE; // Validate next character is 0-9, a-f, A-F
				temp[j] = CharsToHex(
					*ptr,
					*(ptr +
					  1)); // Converts 2 chars to a single unsigned char, assuming they are hex
				j++;
				ptr = ptr + 2;
			} else {
				temp[j] = c;
				j++; // increment writing counter
				ptr++; // increment reading counter
			}
			break;
		case 2: // digit
			// Action: Convert a series of digits into a single unsigned char, unless in quotemode.
			if (quotemode) {
				temp[j] = c;
				j++;
				ptr++;
			} else if (hexmode) {
				if (typeOfChar(*(ptr + 1)) != 1 && typeOfChar(*(ptr + 1)) != 2)
					return FAILURE; // Validate next character is 0-9, a-f, A-F
				temp[j] = CharsToHex(
					*ptr,
					*(ptr +
					  1)); // Converts 2 chars to a single unsigned char, assuming they are hex
				j++;
				ptr = ptr + 2;
			} else {
				temp[j] = (unsigned char)strtol(
					ptr, &next, 0); // Converts the numbers to a signed long, then to a unsigned
									// char.  Numbers > 256 are wrapped.  Ex: 330-256=74='J'
				if (next - ptr == 0)
					ptr++; // increments the ptr in the case of zero?
				else
					ptr = next; // otherwise jump the ptr ahead
				j++;
			}
			break;
		case 3: // space
			// Action: skip it, unless in quotemode
			// Note this is redundant with case 0, but is reserved in case we want to make this a
			// semicolon deliminator between commands.
			if (quotemode) {
				temp[j] = c;
				j++;
			}
			ptr++;
			break;
		case 4: // quotemode flip
			// Action: turn on/off quotemode.
			// TODO: Needs to be able to handle escaped quotes as \"  [see case 6]
			quotemode = !quotemode;
			ptr++;
			break;
		case 5: // NULL (\0) or comments '/'
			finished = 1;
			break;
		case 6: // Escape character encountered. Special characters, such as \" or \\ or \r or \n
			// Action: convert \" --> unsigned char for quotes, without altering quotemode, so you
			// can have embedded quotes Also: convert \r \n to line feeds and carriage returns.  And
			// \\ to a single slash.
			ptr++;
			switch (*ptr) {
			case '\\': // Escaped \, copy to output and increment ptr.
				temp[j] = '\\';
				ptr++;
				j++;
				break;
			case 'r': // Line return
				temp[j] = '\r';
				ptr++;
				j++;
				break;
			case 'n': // New line
				temp[j] = '\n';
				ptr++;
				j++;
				break;
			case '"': // Escaped double quote
				temp[j] = '"';
				ptr++;
				j++;
				break;
			case '\'': // Escaped single quote
				temp[j] = '\'';
				ptr++;
				j++;
				break;
			case '*': // Escaped star (not entering hexmode)
				temp[j] = '*';
				ptr++;
				j++;
				break;
			default: // Invalid escaped character.  Return failure.
				return FAILURE;
			}
			break; // Not yet implement
		case 7: // Hexmode flip
			hexmode = !hexmode;
			ptr++;
			break;
		default: // Should never be here.
			return UNREACHABLE;
		}

		if (j >= MAX_CMD_BUFFER_SIZE)
			return FAILURE; // check for upper bound
	} // while !finished

	// Validate that we actually wrote a command (that the writer counter has moved off
	// initialization)
	if (j <= 1) {
		return FAILURE;
	}

	// Validate that we did not end in quotemode (indicates mismatched quotes) or hexmode
	if (quotemode) {
		return FAILURE;
	}
	if (hexmode) {
		return FAILURE;
	}

	// Set the length and the null termination
	temp[0] = j - 1;
	temp[j] = '\0';

	memcpy(commandBuffer, temp, j); // Quicker version?

	// sprintf(debug,"\r\nConverted ");
	// sendDBGALL(USER_PORT,debug);
	// sendMSG(USER_PORT,processed,processed[0]+1);

	return SUCCESS;
}
static int typeOfChar(unsigned char c) {
	int type;

	// Classify the unsigned char as alpha, numeric, quotes, space, etc...
	if (isalpha(c))
		type = 1; // a-z,A-Z
	else if (isdigit(c))
		type = 2; // 0-9
	else if (isspace(c))
		type = 3; // (space)
	else if (39 == c || 34 == c)
		type = 4; // 39 and 34 are single and double quotes
	else if ('\0' == c || '/' == c)
		type = 5; // Null termination or a slash signifying comments
	else if ('\\' == c)
		type = 6; // Case 6 is escaped characters, such as \"  and \r.  TBD.
	else if ('*' == c)
		type = 7; // Hexmode
	else
		type = 0; // Anything else

	return type;
}
// Converts 2 chars in Hex to a single unsigned char.
// Eg: 4F --> 79   (4x16 + 4)
// Eg: 11 --> 17   (1x16 + 1)
static unsigned char CharsToHex(unsigned char first4bits, unsigned char second4bits) {
	unsigned char retval;

	// Convert letters to appropriate unsigned char #  (0 (i.e. a unsigned char value of )--> 0.
	// A--> 10.  B-->11.  etc...)
	Alpha2Hex(&first4bits);
	Alpha2Hex(&second4bits);

	// Now combined
	first4bits =
		(unsigned char)(first4bits << 4 & 0xF0); // Shifting by 4 is the same as multiplying by 16.
	second4bits = (unsigned char)(second4bits & 0x0F);
	retval = (first4bits | second4bits); // Add the two together.

	return retval;
}
static void Alpha2Hex(unsigned char* c) {
	switch (*c) {
	case 'A':
	case 'a':
		*c = 0x0A;
		return;

	case 'B':
	case 'b':
		*c = 0x0B;
		return;

	case 'C':
	case 'c':
		*c = 0x0C;
		return;

	case 'D':
	case 'd':
		*c = 0x0D;
		return;

	case 'E':
	case 'e':
		*c = 0x0E;
		return;

	case 'F':
	case 'f':
		*c = 0x0F;
		return;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		*c = *c - 48; // Convert from numeric to actual
		return;

	default:
		return;
	}
}

#ifdef UPPER_PPM

#include "linkmanager.h"
#include "payloadmanager.h"

void processResetPayloadDataCmd(unsigned char* command) {
	if (*command) {
		eraseAllOfSectionsData(&gFlashManager, PAYLOAD_DATA_SECTION);
		gPayloadManager.numPayloadPacketsStored = 0;
		gFlashManager.numPagesWrittenTo[PAYLOAD_DATA_SECTION] = 0;
	}
	gPayloadManager.numPayloadPacketsSent = 0;
	return;
}

void processPayloadPowerCmd(unsigned char* command) {
	turnPayloadPower(&gPayloadManager, (PayloadPowerState)command[0]);
	return;
}

void processRadioModeCmd(unsigned char* command) {
	switchBeaconTransmitEnabledTo(&gLinkManager, (int8_t)command[0]);
	switchTransceiverModeTo(&gLinkManager, (TransceiverMode)command[1]);
}

void processForwardPayloadCommandCmd(unsigned char* payloadCommand, int payloadCommandLength) {
	sendDataToPayload(&gPayloadManager, (char*)payloadCommand, payloadCommandLength);
	return;
}

void processResendPayloadDataPacketsCmd(unsigned char* command, int commandLength) {
	int numPackets = commandLength / 2;
	int i;
	for (i = 0; i < numPackets; ++i) {
		uint16_t packetNumber = make16(command[0], command[1]);
		sendPayloadPacketNumber(&gPayloadManager, packetNumber);
		command += 2;
	}
}
#endif

#ifdef LOWER_PPM
#endif