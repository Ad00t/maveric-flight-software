/***************************************************************************

									mystack.h

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: A generic implementation of a circular lifo stack.

Usage: 
		1. Create a stack using the macro MAKE_STACK.
			- Ex:  MAKE_STACK(stack,stackhandle,20,2);   // Creates a stack called stack, with a handler called stackhandle, to hold 20 items of size 2.
		2. Use the other functions to push, peek, pop, etc the stack, by passing 
		   the stack handle to each of them.
			- Ex: stack_push(stackhandle,data,length);

TO DO:
	Upgrade to use an integer-based length.

Changelog:

Date 	 |  Au.  |	Notes
07-27-10	mra		Created title block.

****************************************************************************
*/

#ifndef __MYSTACK__
#define __MYSTACK__


//======================
//  	Dependencies
//======================
#include "errors.h"

//======================
//  	Macros
//======================
// USE THIS TO CREATE A STACK of X items each with Y size
// Example:  MAKE_STACK(stack,stackhandle,20,2);   // Creates a stack called stack, with a handler called stackhandle, to hold 20 items of size 2.
//           In this example, you pass around the address of stackhandle to all stack functions.
//           You must ensure that the lifetime of the stack itself is not destroyed.  
//			 Stacks cannot be global, because the macro uses statements to set the stack properties
#define MAKE_STACK(stackname,handlename,x,y) unsigned char stackname[x][y+1];stack_t handlename;handlename.data = stackname;handlename.itemsize = y;handlename.num_items = x;


//======================
//  	Structures
//======================
typedef struct {
	unsigned char * data;
	int top;
	int itemsize;
	int num_items;
} stack_t;


//======================
//  	Functions
//======================
void stack_init(stack_t * S);
ReturnErr_t stack_push(stack_t * S, unsigned char * data, int len);
void stack_pop(stack_t * S, unsigned char * data, int * len);
void stack_top(stack_t * S, unsigned char * data, int * len);
ReturnErr_t stack_peek(stack_t * S, unsigned char item, unsigned char * data, int * len);
void stack_toplength(stack_t * S, int * len);			// Returns the length of the top item
void stack_pop_ptr(stack_t * S, unsigned char ** data_ptr,int * len);		// Returns a TEMPORARY pointer to the top of the stack.  Must be used immediately or it will be overwritten on next push

// Query functions
int stack_getsize(stack_t * S);			// Returns the max size
#endif
