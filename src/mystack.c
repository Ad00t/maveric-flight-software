/*  MyStack.c 

A generic implementation of a circular lifo stack.

*/

#include "mystack.h"

#module

void stack_init(stack_t * S)
{
	int i;
	
	for(i=0;i<S->num_items;i++)
	{
		*(S->data+(i*S->itemsize))=0;
	}

	S->top=0;
}

ReturnErr_t stack_push(stack_t * S, unsigned char * data, int len)
{
	int i;
	ReturnErr_t retval=SUCCESS;

	// Validate length
	if(len>=S->itemsize) {return STACK_TOO_BIG_TO_PUSH;}

	// Check if the spot is already empty
	if(*(S->data+(S->top*S->itemsize)+0)!=0)
		S->top = S->top+1;	// If not, move forward 1 spot

	// Rollover check
	if(S->top >= S->num_items) S->top = 0;

	// Check for overwrite
	if(*(S->data+(S->top*S->itemsize)+0)!=0) {retval = STACK_WARN_OVERWRITE;}

	// Set length
	*(S->data+(S->top*S->itemsize)+0)=len;

	// Copy data
	for(i=0;i<len;i++)
	{
		*(S->data+(S->top*S->itemsize)+i+1)=data[i];
	}

	return retval;
}

// TODO: What happens if you pop an empty stack?
void stack_pop(stack_t * S, unsigned char * data, int * len)
{
//	unsigned char dbgbuf[30];
	
	*len = *(S->data+(S->top*S->itemsize)+0);

	memcpy(data,(S->data+(S->top*S->itemsize)+1),*len);

	*(S->data+(S->top*S->itemsize)+0)=0;
	S->top = S->top-1;

	// Rollover check
	if(S->top<0) S->top = S->num_items-1;
}

// Peeks at the top of the stack
void stack_top(stack_t * S, unsigned char * data, int * len)
{
	*len = *(S->data+(S->top*S->itemsize)+0);
	memcpy(data,(S->data+(S->top*S->itemsize)+1),*len);
	//data = (S->data+(S->top*S->itemsize)+1);
}


void stack_toplength(stack_t * S, int * len)
{
	*len = *(S->data+(S->top*S->itemsize)+0);
}


ReturnErr_t stack_peek(stack_t * S, unsigned char item, unsigned char * data, int * len)
{
	// Validate
	if(item>=S->num_items) {*len=0; return STACK_INVALID_ITEM;}

	*len = *(S->data+(item*S->itemsize)+0);
	memcpy(data,(S->data+(item*S->itemsize)+1),*len);

	return SUCCESS;
}

// Memory-free version of popping, but must be used carefully
void stack_pop_ptr(stack_t * S, unsigned char ** data_ptr,int * len)		// Returns a TEMPORARY pointer to the top of the stack.  Must be used immediately or it will be overwritten on next push
{
	// Sets the length
	*len = *(S->data+(S->top*S->itemsize)+0);

	// Sets the pointer
	*data_ptr = (S->data+(S->top*S->itemsize)+1);

	// Marks the data as cleared
	*(S->data+(S->top*S->itemsize)+0)=0;
	S->top = S->top-1;

	// Rollover check
	if(S->top<0) S->top = S->num_items-1;

}

int stack_getsize(stack_t * S)
{
	return S->itemsize;
}
