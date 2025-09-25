/***************************************************************************

									tx_manager.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To handle all tx and rx between stacks, flash, and the radio.
See the .h file for more details.

Changelog:

Date 	 |  Au.  |	Notes
07-27-10	mra		Added title block.		

****************************************************************************
*/

#include "tx_manager.h"

#module
#define TX_DEBUG_LEVEL 2   // The default level for debugging messages from this module


void TX_nonBlockProcess(nonBlock_t * NB)
{
	// Run the correct function passing it the data.
	
	switch(NB->Func_To_Run)
	{
		case FuncA: NB->Func_To_Run = TX_Waiting(NB->d_ptr);break;	
		case FuncB: NB->Func_To_Run = TX_ReadFlash(NB->d_ptr);	break;		
		case FuncC: NB->Func_To_Run = TX_ReadTable(NB->d_ptr);break;		
		case FuncD: NB->Func_To_Run = TX_WriteFlash(NB->d_ptr);break;		
		case FuncE: NB->Func_To_Run = TX_SendMHX(NB->d_ptr);break;			
		case FuncF: NB->Func_To_Run = TX_Cleanup(NB->d_ptr);break;
		case FuncG: NB->Func_To_Run = TX_CleanupFlash(NB->d_ptr);break;	
		default: sendDBGALL(USER_PORT,"TX NB ERROR!",TX_DEBUG_LEVEL); break;
	}

	return;
}

void TX_initStruct(TX_t * T,
					unsigned int table_ptr,
					unsigned int read_ptr,
					unsigned long * flash_ptr,
					unsigned int wait,
					unsigned char stackmode=1,
					unsigned long alloc_start=0x000000,
					unsigned long alloc_size=0x001000)
{

	if(stackmode==1)
	{
	// Data Table Settings (deprecated.  TODO: Remove)
		T->table_ptr=NULL;							// Location of the RAM Table
		T->read_index_ptr=NULL;						// RAM Packet Index
		T->typeinfo=8;								// The location of the type of packet. Header(6) + Length(2)

	// Flash Settings
		T->RW_ptr=flash_ptr;						// Location of the flash R/W pointer in RAM
		T->allocation_start_addr=alloc_start;		// Starting address of the allocation in flash
		T->allocation_size=alloc_size;				// Size of the allocation
		T->NewData=1;								// New Data Assumption. 1 for TRUE, 0 for FALSE.

	// MHX Settings
		T->WaitTime=wait;							// Time to wait before assuming connection established
		T->_time = SEC_TIMER;						// Initialize the comm timer	
		T->d_ptr=T->read_index_ptr;					// Deprecated.
		T->d_size=0;								// 
		T->CleanupFunction=FuncA;					

	// Stack Settings
		T->stack_ptr = (stack_t*)table_ptr;			// Sets a pointer to the stack for stackmode	
		T->stackmode = stackmode;
	}

	else{
		sendDBGALL(USER_PORT,"ERROR! NO LONGER SUPPORTS LIFO RAM TABLE! FREEZING");
		for(;;);
	}


	//unsigned char dbg[80]; sprintf(dbg,"\r\nNewData=%i",T->NewData); sendDBGALL(USER_PORT,dbg);

	return;
}


// FuncA
// Precondition: 
//			None
// Postconditions: 
//			T->WritingFunc is set according to whether or not we have a connection   AND
//			The proper function is returned so as to start the transfer process
FuncNum TX_Waiting(unsigned int d)
{
	TX_t * T;
	T = (TX_t*)d;
	unsigned char carrier=0;
	unsigned char table=0;
//	unsigned char * tbl;
//	int index;
	
	if(T->stackmode)
	{
		stack_t * S;
		unsigned char len;
		
		//sendMSGALL(USER_PORT,"\r\nWaiting...");

		// Check the stack

		S = T->stack_ptr;
		stack_toplength(S,&len);
		if(len==0) table = 0; 
		else table = 1;


		// Check the connection
		carrier=0;			// Assume no connection
		T->WritingFunc=FuncD;	// Assume we are writing to flash
		#ifdef __MHX__
		if(MHXConnected()) 
		{
			if(T->_time<=SEC_TIMER)	
			{
				carrier=1;
				T->WritingFunc=FuncE;	// MHX as Writing Function
			}
		}
		else
		{
			T->_time = SEC_TIMER + T->WaitTime;		// Bump up the start time.
		}
		#else
		T->_time = SEC_TIMER + T->WaitTime;		// Bump up the start time.
		#endif


		// Go to the correct place
		if(!table && !carrier) return FuncA;	// loop
		if(table && !carrier) return FuncC;		// Read Stack --> WriteFlash --> Cleanup Stack
		if(!table && carrier) return FuncB;		// Read Flash --> MHX --> CleanupFlash
		if(table && carrier) return FuncC;		// Read Stack --> MHX --> Cleanup Stack 

	}// if stackmode
	else
	{
		sendDBGALL(USER_PORT,"ERROR! NO LONGER SUPPORTS LIFO RAM TABLE! FREEZING");
		for(;;);
	}//else stackmode

	return FuncA;							// default loop
}


//FuncB
// Precondition: 
//			Nothing in the table but a connection is live
// Postconditions: 
//			T->d_ptr points to data read from flash
//			T->d_size contains the correct size of data
//			T->CleanupFunction is set to erase the flash that has been read
FuncNum TX_ReadFlash(unsigned int d)
{
	TX_t * T;
	T = (TX_t*)d;
	//dataPacket_t dp;

	#define MAX_READ_SIZE 		FLASH_BLOCK_SIZE

	unsigned long s_addr,e_addr;
	unsigned int blocknum;
	unsigned int readSize;
//	const unsigned int alloc_size = FLASH_BLOCK_SIZE;
	unsigned long rwaddr;
	FuncNum nextFunc;
	unsigned char allocation[MAX_READ_SIZE+1];

	rwaddr = *T->RW_ptr;

	// 1. Check if there's any data in flash to read (?).
	// 2. Read the block of memory containing the Read/Write Pointer up to the pointer
	// 3. Record relevant data in the T structure

	// 4. (Next Function) Transmit the block of memory up to the pointer.  (Do not packetize anything.  Read directly from flash)

	//sendDBGALL(USER_PORT,"\r\n M",TX_DEBUG_LEVEL);

	// 1. Check if there's any data in flash to read (?).
	if(!T->NewData)
	{
		//sendDBGALL(USER_PORT,"\r\nNo New Data",TX_DEBUG_LEVEL); 
		//delay_ms(1000);
		return FuncA;
	}
	
	//2. Read the block of memory containing the Read/Write Pointer up to the pointer	
	
	// This section of code reads memory from the beginning of the block address to the current
	// write pointer 

	// Compute current block and block addresses
	blocknum = FlashAddrToBlock(rwaddr);
	BlockToFlashAddr(blocknum,&s_addr,&e_addr);
	
	//sprintf(dbg,"\r\nB:%u,S:%u,E:%u",blocknum,s_addr,e_addr); sendDBGALL(USER_PORT,dbg); delay_ms(500);
	//sprintf(dbg,"\r\nRSz:%u,RW:%u,S:%u",readSize,rwaddr,s_addr); sendDBGALL(USER_PORT,dbg); delay_ms(500);

	// Compute size to read
//	sendDBGALL(USER_PORT,"B");
	if(rwaddr<s_addr) return FuncA;  // Bail if the addresses are wrong
	if(rwaddr==s_addr && rwaddr!=T->allocation_start_addr)   // If we're in the same spot as the beginning of the block, and the block is not the first one in the allocation...
	{
		// If the addresses are the same, move 1 byte lower and try again.
		DecrementAddrCircular(T->RW_ptr,T->allocation_start_addr,T->allocation_start_addr+T->allocation_size-1,1);
		return FuncA;					// bail and try again
	}
	readSize = rwaddr-s_addr;
	
	// Read the flash.  
	//sendDBGALL(USER_PORT,"C");
	if(readSize>MAX_READ_SIZE) 
	{
		sendDBGALL(USER_PORT,"\r\nFATAL ERROR! Not enough ram to read the block from flash!");
		sendDBGALL(USER_PORT,"\r\n\n\t\tFREEZING!");
		for(;;){};
	}
	flashRead(s_addr,readSize,allocation);

	// 4. Record relevant data in the T structure
	//sendDBGALL(USER_PORT,"D");
	T->d_ptr=allocation;
	T->d_size=readSize;
	T->CleanupFunction=FuncG;

	if(readSize>0)
	{
		sendDBGALL(USER_PORT,"F",TX_DEBUG_LEVEL);
		//sprintf(dbgbuf," %01u bytes: block %01u, addr %Lx",readSize,blocknum,s_addr); 
		//sendDBGALL(USER_PORT,dbgbuf,TX_DEBUG_LEVEL);
	}

	// 5. Continue to MHX Immediately
	nextFunc = TX_SendMHX((unsigned int)T);
	return nextFunc;
	//return FuncE;
}


//FuncC
// Precondition: T->table_ptr contains a pointer to a RAM table  OR
//				 T->stack_ptr contains a pointer to a stack_t	 AND
//				 T->WritingFunc has been set with the correect writing Function
// Postconditions: 
//			T->d_ptr points to the data in the table   AND
//			T->d_size contains the size of the data
//			T->CleanupFunction contains the proper cleanup function call
FuncNum TX_ReadTable(unsigned int d)
{
	TX_t * T;
	T = (TX_t*)d;
//	int index;
//	unsigned char * tbl;
	
	if(T->stackmode)
	{	
		stack_t * S;
		S = T->stack_ptr;
	
		//sendMSGALL(USER_PORT,"\r\nReading Stack...");
		sendDBGALL(USER_PORT,"S",TX_DEBUG_LEVEL);
		//stack_print(S);
		stack_pop(S,T->stackbuf,&T->d_size);
		T->d_ptr = T->stackbuf;
		
		T->CleanupFunction=FuncF;			// FuncF Cleanup for stack
		return T->WritingFunc;				// Go to the proper Writing Function (MHX or Flash)
	}
	else
	{
		sendDBGALL(USER_PORT,"ERROR! NO LONGER SUPPORTS LIFO RAM TABLE! FREEZING");
		for(;;);
	}
}


//FuncD
// Preconditions: 
//			T->d_ptr points to the data    AND
//			T->d_size contains the size of the data
//			T->CleanupFunction contains the proper cleanup function call
// Postconditions: 
//			Flash is written with the data.
//			T->RW_ptr is updated correctly
//			T->NewData is set appropriately
FuncNum TX_WriteFlash(unsigned int d)
{
	// We know we got here cause of a table entry in *read_index_ptr-1  or in the stack.

	// 1. The read/write pointer could be anywhere, and could write across page/block boundaries.
	// First we should check if we're on block boundary.  That happens after every cleanup, so we'd
	// want to move forward 1 address unit before writing.

	// 2. We also want to check that we won't write across a block boundary.  Because we send things down as
	// blocks, anything written across a block boundary will be split up.

	// 3. We also need to check if we're reaching the end of the storage space.  If what we're writing needs 
	// to wrap-around, we should do that adjustment to the Write Address BEFORE writing.  Just leave the 
	// upper edge of the allocation blank.

	// 4. We also need to modify the data on the way in to flash to make sure the page number is recorded.(Not Implemented)

	// 5. We also need to check if where we're about to write is empty.  If not, we have to clear it out by
	// erasing the whole block.

	// 6. We'll also use the writeSafe function to ensure we don't exceed our bounds and write correctly across
	// page boundaries.
	

	TX_t * T;
	T = (TX_t*)d;
//	unsigned char * tbl;
//	unsigned char * d_ptr;
	long temp;
//	int index;
//	unsigned char type;
//	unsigned char dbg[60];
	static unsigned char eraseError=0;
	unsigned long start,end;
	
	start = T->allocation_start_addr;			// Starting Address
	end = T->allocation_start_addr+T->allocation_size-1;	// Ending Address

	if(!T->stackmode){
		sendDBGALL(USER_PORT,"ERROR! NO LONGER SUPPORTS LIFO RAM TABLE! FREEZING");
		for(;;);
	}// if !stackmode
	
	//======= STACK MODE ==========
	else{

//	sendMSGALL(USER_PORT,"\r\nWriting Flash...");
	
	// Adjust the flash pointer
	//sprintf(dbg,"\r\n0. Ptr: %Lx",*T->RW_ptr); sendMSGALL(USER_PORT,dbg);

	// 1. Check if we're on a block boundary.  If so, move forward 1.
	if(FlashAddrToBlock(*T->RW_ptr)!=FlashAddrToBlock((*T->RW_ptr)+1))
		 *T->RW_ptr += 1;

	//sprintf(dbg,"\r\n1. Ptr: %Lx",*T->RW_ptr); sendMSGALL(USER_PORT,dbg);

	// 2. Check if we're going to finish in a different block.  If so, move to the beginning of that block
	if(FlashAddrToBlock(*T->RW_ptr)!=FlashAddrToBlock(*T->RW_ptr+T->d_size))
		BlockToFlashAddr((FlashAddrToBlock(*T->RW_ptr+T->d_size)),T->RW_ptr,&temp);

	//sprintf(dbg,"\r\n2. Ptr: %Lx",*T->RW_ptr); sendMSGALL(USER_PORT,dbg);

	// 3. Check if we're reaching the end of the allocation.  If so, wrap around to the beginning.
	if(*T->RW_ptr+T->d_size>end)
		 *T->RW_ptr=start;	

	//sprintf(dbg,"\r\n3. Ptr: %Lx",*T->RW_ptr); sendMSGALL(USER_PORT,dbg);

	// 4. TODO: Modify data to include page number?
	// Probably not going to do this.  Preserve the data in original form so this can be a more generic function

	// Write to flash
	ReturnErr_t errval;
	errval = flashWriteSafe(*T->RW_ptr,T->d_size,T->d_ptr,start,end);
	if(errval!=SUCCESS)
	{
		//sprintf(dbg,"\r\nError %i",errval); sendMSGALL(USER_PORT,dbg);
		switch (errval)
		{
			// 5. Recovering from flash not being empty
			case FLASH_NOT_EMPTY:
				sprintf(dbgbuf,"\r\nTried to write %d bytes to %Lu.  Error:",T->d_size,*T->RW_ptr);
				sendDBGALL(USER_PORT,dbgbuf);
				sendDBGALL(USER_PORT,"\r\nNot Empty.");					
				switch(eraseError)
					{
						case 0:	// First NOT EMPTY error.  Erase the whole block
							flashEraseBlockByAddr(*T->RW_ptr+T->d_size);
							sendDBGALL(USER_PORT,"...Erasing Block.");
							//flashErasePage(FlashAddrToPage(*T->RW_ptr+T->d_size));
							eraseError++;
							return FuncD;
							break;
	
						case 1: // Second NOT EMPTY error.  Try erasing the first page of the write command
							flashErasePage(FlashAddrToPage(*T->RW_ptr));		
							sendDBGALL(USER_PORT,"...Erasing Start Page.");	
							eraseError++;				
							return FuncD;
							break;

						case 2: // Third NOT EMPTY error.  Try erasing both blocks containing the addresses
							flashEraseBlockByAddr(*T->RW_ptr+T->d_size);
							flashEraseBlockByAddr(*T->RW_ptr);
							sendDBGALL(USER_PORT,"...Erasing Start and End Blocks.");	
							eraseError++;
							return FuncD;
							break;
	
						case 3: // Still NOT EMPTY!  Not sure what to do.  Go back to beginning?  Cycle the processor?
							sendDBGALL(USER_PORT,"...Giving Up.");	
							return FuncA;
							break;

						default: break;
					}
					eraseError++;
				break;


			case FLASH_BUSY:
				return FuncD;	// Just try again if busy
				break;

			case OUT_OF_BOUNDS:
				sendDBGALL(USER_PORT,"\r\nOut of Bounds.");	
				break;
			
			case FLASH_PROTECTED:
				// Unprotect the sector and try again on the next pass
				sendDBGALL(USER_PORT,"\r\nProtected.");
				
				break;

			default:
				return FuncA;	// Bail as default
				break;
		}
		return FuncA;			
	}

	//sprintf(dbgbuf," Wrote %i bytes to %Lx ",T->d_size,*T->RW_ptr); 	sendDBGALL(USER_PORT,dbgbuf,TX_DEBUG_LEVEL+1);

	// A successful write.  Update the flash pointer and newdata flag, and reset any errors incurred
	*T->RW_ptr+= T->d_size;
	T->NewData = 1;
	eraseError=0;
	sendDBGALL(USER_PORT,">",TX_DEBUG_LEVEL);

	return T->CleanupFunction;	// Head to the Cleanup Function

	}
}


//FuncE
// Preconditions: 
//			T->d_ptr points to the data    AND
//			T->d_size contains the size of the data
//			T->CleanupFunction contains the proper cleanup function call
// Postconditions: 
//			MHX is written with the data.

// Note that if this function is ever modified to return itself (so that it can be called twice in a 
// row).. the allocation for flash expires after this function exits.  So the data will not be there 
// to send out on the 2nd call.  Bottom line:  Do not return this functions FuncNum!
FuncNum TX_SendMHX(unsigned int d)
{
	TX_t * T;
	T = (TX_t*)d;

	// Declarations (for brevity)
	//int i;
	int tot;

	// If we're in stackmode...
	if(T->stackmode)
	{
		//Nothing special to do...
	}

	tot = T->d_size;

	if(0==tot) 			// Nothing to write
	{	
		T->NewData=0;	// Flip the flag back.  False alarm.
		return FuncA;	// Bail
	}

	// Should be able to send this, with handshaking, out the MHX Port 
	// using sendMSG, because ports.c and ports.h handle handshaking now.

	if(sendMSG(TRANSCEIVER_PORT,T->d_ptr,tot)!=SUCCESS)
	{
		sendDBGALL(USER_PORT," ))X ",TX_DEBUG_LEVEL);
		return FuncA;
	}
	else
	{
		sendDBGALL(USER_PORT," ))) ",TX_DEBUG_LEVEL);
		return T->CleanupFunction;	// Go to the appropriate cleanup function
	}
}


//FuncF
// Preconditions: ...
// Postconditions: RAM Table is cleared of latest entry
FuncNum TX_Cleanup(unsigned int d)
{

	TX_t * T;
	T = (TX_t*)d;
//	unsigned char * tbl;
//	int index;
//	unsigned char * c;

	sendDBGALL(USER_PORT,"\r\n",TX_DEBUG_LEVEL);

	if(T->stackmode)
	{
		T->d_size=0;	// Just clear out the data from the handler.  The stack is already clean.
		return FuncA;
	}

	else
	{
		sendDBGALL(USER_PORT,"ERROR! NO LONGER SUPPORTS LIFO RAM TABLE! FREEZING");
		for(;;);
	}
	
// return FuncA;	// Temporary code to test compiler problems
}


//FuncG
// Preconditions: 
//		T->RW_ptr contains a location in flash to be cleared.
// Postconditions: 
//		The block containing T->RW_ptr is cleared
//		T->RW_ptr is updated to the previous block in memory.
//		T->NeData is set according to whether this block is empty or not.
FuncNum TX_CleanupFlash(unsigned int d)
{

	TX_t * T;
	T = (TX_t*)d;

	unsigned int block;
	unsigned long s_addr,temp;
	unsigned long alloc_start, alloc_end;

	alloc_start = T->allocation_start_addr;			// Starting Address
	alloc_end = T->allocation_start_addr+T->allocation_size-1;	// Ending Address

	sendDBGALL(USER_PORT,"#\r\n",TX_DEBUG_LEVEL);

	// 1. ----
	// 2. Erase the block with the read/write pointer.
	// 3. Move the read/write pointer backwards by a block, looping to the upper end if necessary.
	// 4. Check if that block is empty.  Store the result in the NewData flag.

	//sendMSGALL(USER_PORT,"\r\nCleanup Flash");

	//1. Free the allocated memory at d->d_ptr.
	// NO LONGER APPLICABLE.  NOT USING DYNAMIC MEMORY.

	// 2. Erase the block with the read/write pointer.
	block = FlashAddrToBlock(*T->RW_ptr);
	flashEraseBlockByNumber(block);

	// 3. Move the read/write pointer backwards by the amount of data written, looping to the upper end if necessary.
	//sprintf(dbg,"\r\nRW before %uL",*T->RW_ptr); sendMSGALL(USER_PORT,dbg);
	*T->RW_ptr = *T->RW_ptr - (unsigned long)T->d_size - 1;
	if(*T->RW_ptr<alloc_start)
		*T->RW_ptr = (alloc_end)-(alloc_start-*T->RW_ptr) + 1;
	//sprintf(dbg,"\r\nRW after %uL",*T->RW_ptr); sendMSGALL(USER_PORT,dbg);

	// 4. Check if that block is empty.  Store the result in the NewData flag.
	block = FlashAddrToBlock(*T->RW_ptr);
	BlockToFlashAddr(block,&s_addr,&temp);

	//sprintf(dbg,"\r\nChecking Block %i...",block); sendMSGALL(USER_PORT,dbg);

	if(CheckFlashEmpty(s_addr,FLASH_BLOCK_SIZE))
	{
		//sendMSGALL(USER_PORT,"Empty");		
		T->NewData = 0;
	}
	else
	{
		//sendMSGALL(USER_PORT,"Contains Data");
		T->NewData = 1;
	}

	T->d_size=0;

	return FuncA;
}

