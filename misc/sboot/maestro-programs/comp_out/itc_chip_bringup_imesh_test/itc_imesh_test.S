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

	#test 1, udn west 2 east
	#check to see if it is a west edge tile
	{ xori R_TEMP_0, R_MY_X, 0x1 }
        { bz R_TEMP_0, west_edge }
	
	#check to see if it is a east edge tile
	{ xori R_TEMP_0, R_MY_X, 0x7 }
        { bz R_TEMP_0, east_edge }

	#rest of tiles just return
	{ jal test2 }
	
west_edge:
	{ moveli R_L0_0, 0x7 }
	{ mfspr  R_TEMP_0, SPR_IDN_TILE_COORD }
	{ shri   R_TEMP_0, R_TEMP_0, 7 }
	{ andi   R_TEMP_0, R_TEMP_0, 0xf }
	{ move   R_L0_1, R_TEMP_0 }
	{ move   R_L0_2, zero }
	{ move   R_L0_3, R_MY_X }
	{ move   R_L0_4, R_MY_Y }
	{ move   R_L0_5, zero }
	{ jal send_msg_0 }
	{ jal test2 }

east_edge:
	{ jal get_msg_0 }

	#----------- debug --------
        { bnz R_TEMP_0, test2 ; xori R_TEMP_0, R_MY_Y, 0x2 }
        { bnz R_TEMP_0, test2 }
	{ move  R_L1_0, R_MY_X }
	{ jal send_itc_marker }
	{ move  R_L1_0, R_MY_Y }
	{ jal send_itc_marker_m }
	#----------- debug --------
	
	{ xor R_TEMP_0, R_L0_2, R_MY_Y }
	{ bnz R_TEMP_0, fail_test }	
	{ jal test2 }

fail_test:
        # make it fail if it is not zero
        { ori R_L3_0, R_L3_0, 0x1}
	{ jal test3 }
test2:


	#test 1, udn east 2 west
	#check to see if it is a west edge tile
	{ xori R_TEMP_0, R_MY_X, 0x7 }
        { bz R_TEMP_0, west_edge2 }
	
	#check to see if it is a east edge tile
	{ xori R_TEMP_0, R_MY_X, 0x1 }
        { bz R_TEMP_0, east_edge2 }

	#rest of tiles just return
	{ jal test3 }
	
west_edge2:
	#{ jal get_msg_0 }
	{ jal test3 }
	
east_edge2:
	
	{ moveli R_L0_0, 0x7 }
	{ move   R_L0_1, R_MY_Y }
	{ move   R_L0_2, zero }
	{ move   R_L0_3, R_MY_X }
	{ move   R_L0_4, R_MY_Y }
	{ move   R_L0_5, zero }
	#{ jal send_msg_0 }
	{ jal test3 }
	
fail_test2:
        # make it fail if it is not zero
        { ori R_L3_0, R_L3_0, 0x2}
	{ jal test3 }

test3:
	#test 3, udn south 2 north
	#check to see if it is a north edge tile
	{ xori R_TEMP_0, R_MY_Y, 0x1 }
        { bz R_TEMP_0, north_edge3 }
	
	#check to see if it is a south edge tile
	{ xori R_TEMP_0, R_MY_Y, 0x7 }
        { bz R_TEMP_0, south_edge3 }

	#rest of tiles just return
	{ jal test4 }
	
north_edge3:
	{ jal get_msg_0 }
	{ xor R_TEMP_0, R_L0_1, R_MY_X }
	{ bnz R_TEMP_0, fail_test3 }
	{ jal test4 }
	
south_edge3:
	
	{ move   R_L0_0, R_MY_X }
	{ moveli R_L0_1, 0x1 }
	{ move   R_L0_2, zero }
	{ move   R_L0_3, R_MY_X }
	{ move   R_L0_4, R_MY_Y }
	{ move   R_L0_5, zero }
	{ jal send_msg_0 }
	{ jal test4 }
	
fail_test3:
        # make it fail if it is not zero
        { ori R_L3_0, R_L3_0, 0x4}
	{ jal test4 }
	
test4:
	#test 3, udn north 2 south
	#check to see if it is a north edge tile
	{ xori R_TEMP_0, R_MY_Y, 0x1 }
        { bz R_TEMP_0, north_edge4 }
	
	#check to see if it is a south edge tile
	{ xori R_TEMP_0, R_MY_Y, 0x7 }
        { bz R_TEMP_0, south_edge4 }

	#rest of tiles just return
	{ jal test5 }
	
north_edge4:
	
	{ move   R_L0_0, R_MY_X }
	{ moveli R_L0_1, 0x7 }
	{ move   R_L0_2, zero }
	{ move   R_L0_3, R_MY_X }
	{ move   R_L0_4, R_MY_Y }
	{ move   R_L0_5, zero }
	{ jal send_msg_0 }
	{ jal test5 }
	
south_edge4:
	
	{ jal get_msg_0 }
	{ xor R_TEMP_0, R_L0_1, R_MY_X }
	{ bnz R_TEMP_0, fail_test3 }
	{ jal test5 }
	
	
fail_test4:
        # make it fail if it is not zero
        { ori R_L3_0, R_L3_0, 0x4}
	{ jal test5 }

test5:
	#test 5, idn west 2 east
	#check to see if it is a west edge tile
	{ xori R_TEMP_0, R_MY_X, 0x1 }
        { bz R_TEMP_0, west_edge5 }
	
	#check to see if it is a east edge tile
	{ xori R_TEMP_0, R_MY_X, 0x7 }
        { bz R_TEMP_0, east_edge5 }

	#rest of tiles just return
	{ jal test6 }
	
west_edge5:
	{ moveli R_L0_0, 0x7 }
	{ move   R_L0_1, R_MY_Y }
	{ move   R_L0_2, zero }
	{ move   R_L0_3, R_MY_X }
	{ jal idn_send_msg_0 }
	{ jal test6 }

east_edge5:
	{ jal idn_get_msg_0 }
	{ jal test6 }

fail_test5:
        # make it fail if it is not zero
        { ori R_L3_0, R_L3_0, 0x1 }
	{ jal test6 }	

test6:
	#test 5, idn west 2 east
	#check to see if it is a west edge tile
	{ xori R_TEMP_0, R_MY_X, 0x1 }
        { bz R_TEMP_0, west_edge6 }
	
	#check to see if it is a east edge tile
	{ xori R_TEMP_0, R_MY_X, 0x7 }
        { bz R_TEMP_0, east_edge6 }

	#rest of tiles just return
	{ jal test7 }
	
west_edge6:
	{ jal idn_get_msg_0 }
	{ jal test7 }

east_edge6:
	{ moveli R_L0_0, 0x1 }
	{ move   R_L0_1, R_MY_Y }
	{ move   R_L0_2, zero }
	{ move   R_L0_3, R_MY_X }
	{ jal idn_send_msg_0 }
	{ jal test7 }

fail_test6:
        # make it fail if it is not zero
        { ori R_L3_0, R_L3_0, 0x1 }
	{ jal test7 }

test7:	
	#test 5, idn north 2 south
	#check to see if it is a west edge tile
	{ xori R_TEMP_0, R_MY_Y, 0x1 }
        { bz R_TEMP_0, north_edge7 }
	
	#check to see if it is a east edge tile
	{ xori R_TEMP_0, R_MY_Y, 0x7 }
        { bz R_TEMP_0, south_edge7 }

	#rest of tiles just return
	{ jal test8 }
	
south_edge7:
	{ jal idn_get_msg_0 }
	{ jal test8 }

north_edge7:
	{ move R_L0_0, R_MY_X }
	{ moveli   R_L0_1, 0x7 }
	{ move   R_L0_2, zero }
	{ move   R_L0_3, R_MY_X }
	{ jal idn_send_msg_0 }
	{ jal test8 }

fail_test7:
        # make it fail if it is not zero
        { ori R_L3_0, R_L3_0, 0x1 }
	{ jal test8 }

test8:
	#test 5, idn north 2 south
	#check to see if it is a west edge tile
	{ xori R_TEMP_0, R_MY_Y, 0x1 }
        { bz R_TEMP_0, north_edge8 }
	
	#check to see if it is a east edge tile
	{ xori R_TEMP_0, R_MY_Y, 0x7 }
        { bz R_TEMP_0, south_edge8 }

	#rest of tiles just return
	{ jal test9 }
	
south_edge8:
	{ move R_L0_0, R_MY_X }
	{ moveli   R_L0_1, 0x1 }
	{ move   R_L0_2, zero }
	{ move   R_L0_3, R_MY_X }
	{ jal idn_send_msg_0 }
	{ jal test9 }

north_edge8:
	{ jal idn_get_msg_0 }
	{ jal test9 }

fail_test8:
        # make it fail if it is not zero
        { ori R_L3_0, R_L3_0, 0x1 }
	{ jal test9 }

test9:

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
        
boot_mem_test_return:   	
        { jrp R_L3_2 }
        
# Make sure this last address is 64-byte aligned so that the code above is padded to a full cache line
# so that when it is loaded into the L2 cache in cache-as-RAM mode, there are no parity errors                
.align 64
_last_addr:             
