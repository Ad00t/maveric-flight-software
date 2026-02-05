/***************************************************************************

									lower_tx.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: A stack monitor that delivers anything placed on the monitored stack
	to a stack on the other processor, with a prepended command.

Changelog:

Date 	 |  Au.  |	Notes
09-02-10	mra		Created.

****************************************************************************
*/

#include <lower_tx.h>

#MODULE  // Limit the scope of globals declared here


void initTX_Properties(	tx_properties_t * T, 
					   	stack_t * S,
						short Packetize,			// Flag on whether to packetize 
						int PktType,				// The type of packet (if packetizing)
						unsigned char reserved1,				// The reserved bytes to add
						unsigned char reserved2)
{
	T->S=S;
	T->Packetize=Packetize;
	T->PktType=PktType;
	T->reserved1=reserved1;
	T->reserved2=reserved2;
}


void CheckAndSend(tx_properties_t * T)				// Call repeatedly to check the stack
{
	int len;
	unsigned char * data_ptr;
	stack_t * S;

	S=T->S;									// Set up a pointer to the stack	

	int offset=7;						// AMount of bytes necessary for appended command and parameters (defualts to max)

	stack_toplength(S,&len);				// Acquire the length
	if(len==0) return;						// Get out quickly if there's nothing on the stack
	if(len>=stack_getsize(S)-offset) 
	{
		sendDBGALL(USER_PORT, "\r\nPossible error.  Max size of stack reached.");
		// Pop the pointer to (hopefully) clear the error. We're dropping this data.
		stack_pop_ptr(S, &data_ptr,&len);		// Pointer to pointer used here
		return;
	}
	if(len>255-offset)
	{
		sendDBGALL(USER_PORT, "\r\nToo big to pass to other PPM!");
		return;
	}
	// Something is on the stack and is the right size...
	
	// Pop the pointer.  We're going to use it immediately.
	stack_pop_ptr(S, &data_ptr,&len);			// Pointer to pointer used here

	if(T->Packetize)
	{
		// FOR PACKETIZING
		offset=7;
	
		// Shift everything to the right by offset to make room for command and parameters
		memmove(data_ptr+offset,data_ptr,len);
	
		// Set the command and parameters
		// Eg: (total length) 133 (stack number) (pkt type) (reserved space byte 1) (reserved space byte 2) (len) (data)
		// Bytes:   1          2       3          4                 5                          6              7     8
		memset(&data_ptr[0],(unsigned char)(len+offset),1);	// Total Length
		memset(&data_ptr[1],PacketizeAndPushCommand,1);		// Command
		memset(&data_ptr[2],1,1);							// Stack Number
		memset(&data_ptr[3],MISC_PACKET,1);					// Packet Type
		memset(&data_ptr[4],0x00,1);						// Reserved 1
		memset(&data_ptr[5],PPM_NUM,1);						// Reserved 2
		memset(&data_ptr[6],(unsigned char)len,1);			// Length
	}
	else
	{
		// FOR JUST PUSHING
		offset=3;
	
		// Shift everything to the right by offset to make room for command and parameters
		memmove(data_ptr+offset,data_ptr,len);
	
		// Set the command and parameters
		// Eg: (cmd) (stack number) (len) (data)
		// Bytes: 1       2           3       4        
		//memset(&data_ptr[0],(unsigned char)(len+offset),1);		// Total Length
		memset(&data_ptr[0],PushCommand,1);					// Command
		memset(&data_ptr[1],1,1);							// Stack Number
		memset(&data_ptr[2],(unsigned char)len,1);			// Length
	}
	// Debug
//	sendDBGALL(USER_PORT,"\r\n\n");
//	HexPrint(USER_PORT,(unsigned char*)data_ptr,len+offset);

	// Send it off
	sendPPM(OTHER_PPM_PORT, (unsigned char*)data_ptr, len+offset);

	return;
}
