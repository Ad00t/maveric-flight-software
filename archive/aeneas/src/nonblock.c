/*************************************************************************
//
//								nonblock.c
//		
//
****************************************************************************/
#include "nonblock.h"

// Initializes a NB Handler with the given data pointer
void initNBHandler(nonBlock_t * NB, unsigned int D)
{
	NB->Func_To_Run = FuncA;
	NB->d_ptr = D;
}


