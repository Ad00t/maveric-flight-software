/***************************************************************************
		Macro Functionality

Author: Michael Aherne & Fred Tubb
Company: USC ISI / iControl
License: Proprietary

Purpose: To provide macro functionality

TODO: Remove extern dependencies and locally scope any variables we can.

Usage: See the descriptions of each function below.


WHAT DOES A MACRO IN FLASH LOOK LIKE?
The macro's location in flash determines its number.  Each macro takes up a certain size,
so macro number N will be N*MAX_MACRO_SIZE bytes away from the starting address in flash,
denoted in the address.h file.

The macro itself looks like this:

(OVERALL LENGTH) (LENGTH OF CMD1) (CMD1) (CMD1 PARAMETERS) (LENGTH OF CMD 2) (CMD2) (CMD2 PARAMETERS) ...etc

The commands are listed in the function ProcessCmd().  Parameters are optional, and relate to each command.

Macros are controlled by other global arrays, such as the Macro_state[].  These arrays 
control if and how the commands are run.  When ProcessCmd() "runs" a macro, all it
does is change the state in Macro_state[] to MACRO_RUN.  Then, on the next loop in
main, the function checkMacros() will notice the change in state, and call ProcessCmd()
appropriately to run the command.  This is done to avoid recursion.  If ProcessCmd()
could actually run a macro, it would be calling itself, which is not allowed on the PIC.


*******************************************************************************
*/

#ifndef __MACROS__
#define __MACROS__
//========================================
//  			Dependencies
//========================================
#include "common.h"
#include "pins.h"				
#include "address.h"			// Needed for location of macros in flash
#include "flash.h"
#include "crc.h"

extern int processCmd(int port, unsigned char *source, unsigned char *rbcmd, int ncmds, unsigned char ack);	// The main processing function
extern void flash(unsigned char type, unsigned long addr, int len, unsigned char *buf);				// Legacy function for Fred's Code.  Probably messes things up.
extern TFlashProtected globals;							// ***Huge*** custom data structure for saving configuration items to flash
extern void checkSystemEvents();
//extern unsigned long globalEventRegister;

//========================================
//  			Definitions
//========================================
#define NUM_MACROS 			40					// How many macros are there?
#define MAX_MACRO_SIZE		256					// Sets the maximum macro size in chars
#define MACRO_RUN ('R')							// Definition for a type of Macro_state
#define MACRO_BACKGROUND ('B')					// Definition for a type of Macro_state
#define MACRO_STOP ('S')						// Definition for a type of Macro_state
#define BOOT_MACRO			NUM_MACROS-1		// Definition of boot macro #
// Definitions for buffer
//#define BIGGER_BUFFER_SIZE  256				// used in LbufPlus128

//==============================================
//  			Globals (do not directly access!)
//===============================================
unsigned char Macro_state[NUM_MACROS];				// Macro_state  "'R'un" or "'S'top"
unsigned char Macro_loop[NUM_MACROS];   			// Macro Loop counter  
unsigned int delay[NUM_MACROS];						// Delays used between macro commands
unsigned char Macro_ptr[NUM_MACROS];				// Pointer for active macro
unsigned char MacroCflag[NUM_MACROS+3];				// Continuation flag used by MAcro processing (40 used for upload),41 for GPS
//unsigned char LbufPlus128[BIGGER_BUFFER_SIZE];				// general purpose larger buffer


//========================================
//  			Functions
//========================================
void checkMacros();
void MacroCmdRun(int ID, unsigned char mode, unsigned char loop, unsigned char * source);
void MacroStop(int ID);
void Macro(unsigned char state, int ID);
unsigned char readMacro(unsigned char type, unsigned char macroId, unsigned char * macro);
void RunMacro(int ID, unsigned char mode, unsigned char loop);
void clearMacro(unsigned char macroId);
void writeMacro(unsigned char type, unsigned char macroId, unsigned char * macro);
//void appendMacro(unsigned char macroId, unsigned char * buf, unsigned char * cmd, unsigned char cmdLength);		// Looks like this is long gone...
void loadMacro(unsigned char macroId, unsigned char * buf, unsigned char * commands, unsigned char commandsLength);
//void loadMacroEventMask(unsigned char macroId, macroEventInfo_t * eventInfo);



#endif // __MACROS__