/*********************************************************************

						CAERUS Transfer Manager

Author: Michael Aherne
Date: March, 2010

The purpose of this library is to monitor a stack and deliver any 
content placed on the stack to a connected MHX.  If the MHX is
disconnected, any content placed on the stack will be put into 
flash memory, and will be sent to the ground as soon as a connection
is established.      

To use the library requires 5 lines of code:

CREATE:
	MAKE_STACK(a,stackhandle,length,height);		// Creates a stack.  See mystack.h for information.
	nonblock_t nb_handler;							// Creates a handler for the nonblocking flow control.
	TX_t TX_handler;								// Creates a handler for the data transfer flow control.

INIT: 
	initNBHandler(&nb_handler, &TX_handler);		// Ties the handlers to each other
	TX_initStruct(&TX_handler, &stackhandle, parameters...) // Ties the stack to the transfer handler and initializes according to parameters

RUN:
	TX_nonblockProcess(&nb_handler);				// Call this repeatedly and often.


While you are running, any items pushed onto the stack with stack_push() 
will be immediately transferred to either flash or the MHX, depending
on the connection status.  If in flash, they will be retrieved and sent
to the MHX on the next active connection.  


TODO:
	- Create a flash handler for the flash allocation instead of using a million parameters in initTXHandler.

*********************************************************************
*/

#ifndef __TXMANAGE__
#define __TXMANAGE__

//======================
//  	Dependencies
//======================
#include "common.h"
#include "pins.h"
#include "nonblock.h"			// for the FuncNumbers and nonblock handlers
//#include "ports.h"
#include "debug.h"
#include "flash.h"				// for flash read/write functions
#include "mystack.h"			// for stack functionality

// Settings
#define MAX_STACK_SIZE 		256

// Dependent external variables
//extern unsigned long SEC_TIMER; 

// Dependent external functions
//extern void sendMSGALL(int port, unsigned char * msg);				// A function to send a null-terminated message to a physical port.  Similar to printf.
//extern void sendDBGALL(int port,unsigned char * msg,int lvl=0);
//extern void delay_ms(unsigned long);						
//extern void sendMSG(int port, unsigned char *buf, unsigned char len);
//extern int getByte(int port, unsigned char *data);
//extern int CheckByte(int port);								// Same as getByte, but does not return data.  Just checks the port status.


//========================================================
//  	Super Structure
//     (do not access directly -- use provided functions)
//=========================================================
typedef struct
{
//---- For Process Control
	FuncNum WritingFunc;							// Decides whether to write to flash (FuncD) or MHX (FuncE)

//---- For Data Table
	unsigned char * table_ptr;								// Pointer to RAM Table
	int * read_index_ptr;							// Pointer to RAM Table Index
	int working_ptr;								// A copy of the read index pointer to remain stable throughout the functions
	int typeinfo;									// The location (in terms of characters) of the type specifier in the packet.

//---- For Flash Reading/Writing
	unsigned long * RW_ptr;							// Location of the moving R/W pointer in flash
	unsigned char NewData;									// A flag to indicate new data has been written to flash
	unsigned long allocation_start_addr;			// The static address in flash where the allocation begins
	unsigned long allocation_size;					// The size of the allocation

//----- For MHX
	unsigned char * d_ptr;									// pointer to the data to send out MHX
	unsigned int WaitTime;							// Time to wait before assuming connection established
	unsigned long _time;							// Internal time used in calculating waits and other things
	int d_size;										// the size of the data
	FuncNum CleanupFunction;						// The cleanup function MHX will call

//------ For Stack
	unsigned char * stack_ptr;								// A pointer to the newer implementation with a stack
	unsigned char stackmode;									// A flag for stackmode
	unsigned char stackbuf[MAX_STACK_SIZE];					// Buffer to hold stack data on its way through to flash or MHX
	//unsigned long **db_ptr;						// A pointer to a pointer to data
} TX_t;



//TX_t * Tptr_global;	// Why does this break the 4.109 version compiler!?  Argh!!!


//======================
//  	Functions
//======================
void TX_nonBlockProcess(nonBlock_t * NB);			// The master function.  Use to run everything.

//TBD: TX_masterInit(nonBlock_t * NB,
//					TX_t * T,
//					stack_t * S, ...);				// TBD: Master Initialization Function

void TX_initStruct(TX_t * T,
					unsigned int table_ptr,
					unsigned int read_ptr,
					unsigned long * flash_ptr,
					unsigned int wait,
					unsigned char stackmode=0,
					unsigned long alloc_start=0x000000,
					unsigned long alloc_size=0x001000);			


//======================
//  	States
//======================
FuncNum TX_Waiting(unsigned int d);					// d is a generic pointer.  This can be cast to any useful structure
FuncNum TX_ReadFlash(unsigned int d);
FuncNum TX_ReadTable(unsigned int d);
FuncNum TX_WriteFlash(unsigned int d);
FuncNum TX_SendMHX(unsigned int d);
FuncNum TX_Cleanup(unsigned int d);
FuncNum TX_CleanupFlash(unsigned int d);

#endif //__TXMANAGE__
