#include <sys/reg.h>

###################################################
# These files must be included in this order        
###################################################
#include "itc_chip_bringup_defs.h"
#include "itc_chip_bringup_l1boot.S"
#include "itc_chip_bringup_utils.S"
        
###################################################
# boot_test
#
# On exit:                      
# R_L3_0 = failure count, 0 = test passed
###################################################
        
boot_test_3:
        # Save the lr because I'm going to call routines
	{ move R_L3_1, lr }

        # Init the fail count return value to indicate no failures
        { move R_L3_0, zero }
	
        #don't run any test, just return 0

boot_test_return:       
        { jrp R_L3_1 }              
        
###################################################
# boot_mem_test
#
# On entry:     
# R_L3_1 = mshim coordinates
#        
# On exit:                      
# R_L3_0 = failure count, 0 = test passed
###################################################
        
boot_mem_test_3:
        # Save the lr because I'm going to call routines
	{ move R_L3_2, lr }

        # Init the fail count return value to indicate no failures
        { move R_L3_0, zero }

        #don't run any test, just return 0
        #the itc_mem_test already test the ddr memories
        #it takes 60 sec to test the 128x1024*1024 ddr memories
        #we are not doing it here to save time

        
boot_mem_test_return:   	
        { jrp R_L3_2 }
        
# Make sure this last address is 64-byte aligned so that the code above is padded to a full cache line
# so that when it is loaded into the L2 cache in cache-as-RAM mode, there are no parity errors                
.align 64
_last_addr:             
