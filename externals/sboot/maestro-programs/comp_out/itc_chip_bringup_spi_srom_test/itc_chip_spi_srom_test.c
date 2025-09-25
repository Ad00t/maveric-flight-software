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


#include "testbench_defs.h"

#include <sys/archlib.h>
#include "itc_dvlib.h"
#include "srom_test_funcs.h"

int main()
{

  uint32_t fail_count = 0;

  // Run the destructive write/read test on the SROM
  fail_count += srom_test(RSH0_IDN_X, RSH0_IDN_Y, 20);
            
  TEST_DONE(fail_count);

  idle();
}

