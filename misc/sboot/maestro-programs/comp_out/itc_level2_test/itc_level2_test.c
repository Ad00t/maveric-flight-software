/*
 * Copyright 2009 Tilera Corporation. All Rights Reserved.
 * 
 *   The source code contained or described herein and all documents
 *   related to the source code ("Material") are owned by Tilera
 *   Corporation or its suppliers or licensors.  Title to the Material
 *   remains with Tilera Corporation or its suppliers and licensors. The
 *   software is licensed under the Tilera MDE License.
 * 
 *   Unless otherwise agreed by Tilera in writing, you may not remove or
 *   alter this notice or any other notice embedded in Materials by Tilera
 *   or Tilera's suppliers or licensors in any way.
 *
 */


//#include "testbench_defs.h"

//#include <sys/archlib.h>
#include "dvlib.h"

int main()
{

  print("Duncan is running a level 2 c code from ddr2 memory with tile 1,4!");
  //TEST_DONE(0);
  //bringup_done(1, 4, 0);
  bringup_msg(1, 4, 3, 0);

}

