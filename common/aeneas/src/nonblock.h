#ifndef __NONBLOCK__
#define __NONBLOCK__
/*************************************************************************
//
//								nonclock.h
//		
//	A library for using nonblocking processes.  
//
//
****************************************************************************/

/*

Steps for creating a nonblocking process 

  1. Combine all the data you need for the function into a struct. 
  2. Create that struct in an area of code where it's lifetime will not expire across
	 all calls to the nonblocking function.  (This also applies to any contained pointers
	 within the struct.  All items must have a lifetime across all calls to the nonblocking
	 function.)
  3. Split the process up into several sub-processes.  Each of these is called a 
	 "state" of the overall process.  Each state must take as input an
     unsigned int, and return the next state as a FuncNum.
  4. Assign the FuncNum's in a function similar to nonBlockProcess.
  5. Repeated calls to this function will execute in a nonblocking manner

*/

#include "common.h"
#include "pins.h"

enum FuncNum{FuncA,FuncB,FuncC,FuncD,FuncE,FuncF,FuncG,FuncH,FuncI,FuncJ};

//======================
//  	Structs
//======================
typedef struct
{
	FuncNum Func_To_Run;						// Next Function
	unsigned int d_ptr;							// Pointer to necessary data.
} nonBlock_t;

//======================
//  	Functions
//======================
void initNBHandler(nonBlock_t * NB, unsigned int D);  // Initializes a NB Handler with the given data pointer


#endif //__NONBLOCK__