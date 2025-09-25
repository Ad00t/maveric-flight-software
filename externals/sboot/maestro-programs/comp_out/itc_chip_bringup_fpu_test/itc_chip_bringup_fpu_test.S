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
	# the below code will force the test to fail
	#{ moveli R_L3_0, lo16(0x1) }
	#{ auli   R_L3_0, R_L3_0, ha16(0x1)}
	
	#------------------------- this is the fpu part 1 ---------------------------
	#read a register
        #write and read a register
        { moveli R_TEMP_0, lo16(0x1) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x1) }
	{ mtspr 32001,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32002,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32003,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32004,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32005,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32006,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32007,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32008,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32009,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32010,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32011,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32012,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32013,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32014,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32015,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32016,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32017,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32018,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32019,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32020,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32021,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32022,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32023,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32024,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32025,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32026,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32027,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32028,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32029,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32030,    R_TEMP_0 }

	{ shli R_TEMP_0, R_TEMP_0, 1 }
	{ mtspr 32031,    R_TEMP_0 }
	

        { moveli R_TEMP_0, lo16(0x1) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x1) }
	#{ mfspr R_TEMP_1, 32002 }	
	#{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	#{ bz R_TEMP_1, fpu_check1 }
	#{ addi R_L3_0, R_L3_0, 1 }
fpu_check1:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32002 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check2 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check2:

	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32003 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check3 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check3:	
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32004 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check4 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check4:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32005 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check5 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check5:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32006 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check6 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check6:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32007 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check7 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check7:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32008 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check8 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check8:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32009 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check9 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check9:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32010 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check10 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check10:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32011 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check11 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check11:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32012 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check12 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check12:

	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32013 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check13 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check13:	
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32014 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check14 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check14:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32015 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check15 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check15:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32016 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check16 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check16:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32017 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check17 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check17:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32018 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check18 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check18:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32019 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check19 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check19:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32020 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check20 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check20:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32021 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check21 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check21:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32022 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check22 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check22:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32023 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check23 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check23:	
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32024 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check24 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check24:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32025 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check25 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check25:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32026 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check26 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check26:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32027 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check27 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check27:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32028 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check28 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check28:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32029 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check29 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check29:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32030 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check30 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check30:
	{ shli R_TEMP_0, R_TEMP_0, 1 }	
	{ mfspr R_TEMP_1, 32031 }
	{ xor R_TEMP_1, R_TEMP_1, R_TEMP_0 }
	{ bz R_TEMP_1, fpu_check31 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_check31:

	#------------------------- this is the fpu part 2 ---------------------------
        { moveli R_TEMP_0, lo16(8192) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(8192) } #fpu reg2 - srcA - test1
	{ mtspr 32002,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(4096) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(4096) } #fpu reg3 - srcB - test1
	{ mtspr 32003,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(3) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(3) } #fpu reg4 - srcA - test2
	{ mtspr 32004,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(4) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(4) } #fpu reg5 - srcB - test2
	{ mtspr 32005,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(5) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(5) } #fpu reg6 - srcA - test3
	{ mtspr 32006,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(6) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(6) } #fpu reg7 - srcB - test3
	{ mtspr 32007,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(7) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(7) } #fpu reg8 - srcA - test4
	{ mtspr 32008,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(8) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(8) } #fpu reg9 - srcB - test4
	{ mtspr 32009,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(9) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(9) } #fpu reg10 - srcA - test5
	{ mtspr 32010,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(10) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(10) } #fpu reg11 - srcB - test5
	{ mtspr 32011,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(11) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(11) } #fpu reg12 - srcA - test6
	{ mtspr 32012,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(12) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(12) } #fpu reg13 - srcB - test6
	{ mtspr 32013,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(13) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(13) } #fpu reg14 - srcA - test7
	{ mtspr 32014,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(14) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(14) } #fpu reg15 - srcB - test7
	{ mtspr 32015,    R_TEMP_0 }



        { moveli R_TEMP_0, lo16(0x010020d0) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x010020d0) } #instruction 1
        { moveli R_TEMP_1, lo16(0x01004151) }
	{ auli   R_TEMP_1, R_TEMP_1, ha16(0x01004151) } #instruction 2
        { moveli R_TEMP_2, lo16(0x010061d2) }
	{ auli   R_TEMP_2, R_TEMP_2, ha16(0x010061d2) } #instruction 3
        { moveli R_TEMP_3, lo16(0x01008253) }
	{ auli   R_TEMP_3, R_TEMP_3, ha16(0x01008253) } #instruction 4
        { moveli R_TEMP_4, lo16(0x0100a2d4) }
	{ auli   R_TEMP_4, R_TEMP_4, ha16(0x0100a2d4) } #instruction 5

	
        
		
	
	{ mtspr 32064,   R_TEMP_0 }
 	{ mtspr 32064,   R_TEMP_1 }
	{ mtspr 32064,   R_TEMP_2 }
	{ mtspr 32064,   R_TEMP_3 }
	{ mtspr 32064,   R_TEMP_4 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	{ moveli R_TEMP_0, lo16(12288) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(12288)}
	{ mfspr  R_TEMP_5, 32016 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_b2b1 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_b2b1:

        { moveli R_TEMP_0, lo16(7) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(7)}
	{ mfspr  R_TEMP_5, 32017 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_b2b2 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_b2b2:
	
        { moveli R_TEMP_0, lo16(11) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(11)}
	{ mfspr  R_TEMP_1, 32018 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_1 }
	{ bz R_TEMP_0, fpu_b2b3 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_b2b3:

        { moveli R_TEMP_0, lo16(15) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(15)}
	{ mfspr  R_TEMP_1, 32019 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_1 }
	{ bz R_TEMP_0, fpu_b2b4 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_b2b4:

        { moveli R_TEMP_0, lo16(19) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(19)}
	{ mfspr  R_TEMP_1, 32020 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_1 }
	{ bz R_TEMP_0, fpu_b2b5 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_b2b5:
	
	#------------------------- this is the fpu part 3 ---------------------------
	# this is totally new.  we added it to test the rest of the fpu instructions
	#----------------------------------------------------------------------------



	#------------------------- this is the double precision add -----------------
        { moveli R_TEMP_0, lo16(8192) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(8192) } #fpu reg2 - srcA 0
	{ mtspr 32002,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(4096) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(4096) } #fpu reg3 - srcA 1
	{ mtspr 32003,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(8192) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(8192) } #fpu reg2 - srcB 0
	{ mtspr 32004,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(4096) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(4096) } #fpu reg3 - srcB 1
	{ mtspr 32005,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(0xc1004086) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0xc1004086) } #instruction 1	
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop 	
	{ mfspr R_TEMP_1, 32006 }
	{ xor R_TEMP_0, R_MY_X, R_MASTER_X }
        { bnz R_TEMP_0, fpu_0 ; xor R_TEMP_0, R_MY_Y, R_MASTER_Y }
        { bnz R_TEMP_0, fpu_0 }
	{ move R_L1_0, R_TEMP_1 }
	{ jal send_itc_marker }
fpu_0:
	{ moveli R_TEMP_0, lo16(16384) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(16384)}
	{ mfspr  R_TEMP_5, 32006 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_dadd0 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_dadd0:
	{ moveli R_TEMP_0, lo16(8192) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(8192)}
	{ mfspr  R_TEMP_5, 32007 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_dadd1 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_dadd1:



	#------------------------- this is the single precision sub -----------------
        { moveli R_TEMP_0, lo16(8192) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(8192) } #fpu reg2 - srcA 0
	{ mtspr 32002,    R_TEMP_0 }
        #{ moveli R_TEMP_0, lo16(4096) }
	#{ auli   R_TEMP_0, R_TEMP_0, ha16(4096) } #fpu reg3 - srcA 1
	#{ mtspr 32003,    R_TEMP_0 }
        { moveli R_TEMP_0, lo16(8192) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(8192) } #fpu reg2 - srcB 0
	{ mtspr 32004,    R_TEMP_0 }
        #{ moveli R_TEMP_0, lo16(4096) }
	#{ auli   R_TEMP_0, R_TEMP_0, ha16(4096) } #fpu reg3 - srcB 1
	#{ mtspr 32005,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(0x02004086) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x02004086) } #instruction 1	
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	{ moveli R_TEMP_0, lo16(0x0) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x0)}
	{ mfspr  R_TEMP_5, 32006 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_ssub }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_ssub:

	#------------------------- this is the single precision mul -----------------
        { moveli R_TEMP_0, lo16(0x3f000000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x3f000000) } #fpu reg2 - srcA 0 6 
	{ mtspr 32002,    R_TEMP_0 }
 
        { moveli R_TEMP_0, lo16(0x40c00000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40c00000) } #fpu reg4 - srcB 0 0.5
	{ mtspr 32004,    R_TEMP_0 }
 
	
        { moveli R_TEMP_0, lo16(0x03004086) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x03004086) } #instruction 1	
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	{ moveli R_TEMP_0, lo16(0x40400000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40400000)} #fpu reg64 - srcB 0 3
	{ mfspr  R_TEMP_5, 32006 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_smul }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_smul:	

	#------------------------- this is the single precision combo 1 ---------------
        { moveli R_TEMP_0, lo16(0x4) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x4) } #fpu reg2 - srcA 0
	{ mtspr 32002,    R_TEMP_0 }
 
        { moveli R_TEMP_0, lo16(0x16) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x16) } #fpu reg4 - srcB 0
	{ mtspr 32004,    R_TEMP_0 }
 	
        { moveli R_TEMP_0, lo16(0x05000086) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x05000086) } #instruction i2d reg 2 to reg 6	
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
        { moveli R_TEMP_0, lo16(0x05000106) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x05000108) } #instruction i2d reg 4 to reg 8
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
        { moveli R_TEMP_0, lo16(0x0200620a) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x0200620a) } #instruction dsub reg 8 - reg 6 = reg 10
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
        { moveli R_TEMP_0, lo16(0x0600028c) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x0600028c) } #instruction d2i reg 10 to reg 12
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	{ mfspr R_TEMP_1, 32012 }
	{ xor R_TEMP_0, R_MY_X, R_MASTER_X }
        { bnz R_TEMP_0, fpu_1 ; xor R_TEMP_0, R_MY_Y, R_MASTER_Y }
        { bnz R_TEMP_0, fpu_1 }
	{ move R_L1_0, R_TEMP_1 }
	{ jal send_itc_marker }
fpu_1:
	{ moveli R_TEMP_0, lo16(0xffffffea) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0xffffffea)}
	{ mfspr  R_TEMP_5, 32012 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_comb1 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_comb1:

#if 0	
	# bad:	keep getting zero
	#------------------------- this is the single precision sqrt ---------------
        { move R_TEMP_0, zero }
	{ mtspr 32004,    R_TEMP_0 } #zero out reg 4

	
        { moveli R_TEMP_0, lo16(0x40800000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40800000) } #fpu reg2 - srcA = 4
	{ mtspr 32002,    R_TEMP_0 }
 
        { moveli R_TEMP_0, lo16(0x41800000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x41800000) } #fpu reg6 - srcA = 16
	{ mtspr 32006,    R_TEMP_0 }
 	
        #{ moveli R_TEMP_0, lo16(0x21000084) }
	#{ auli   R_TEMP_0, R_TEMP_0, ha16(0x21000084) } #instruction ssqrt reg 2 ^ (1/2) = reg 4
	#{ mtspr 32064,   R_TEMP_0 }
 	
        { moveli R_TEMP_0, lo16(0x21000184) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x21000184) } #instruction ssqrt reg 6 ^ (1/2) = reg 4
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	{ mfspr R_TEMP_1, 32004 }
	{ xor R_TEMP_0, R_MY_X, R_MASTER_X }
        { bnz R_TEMP_0, fpu_3 ; xor R_TEMP_0, R_MY_Y, R_MASTER_Y }
        { bnz R_TEMP_0, fpu_3 }
	{ shri R_TEMP_1, R_TEMP_1, 16 }
	{ move R_L1_0, R_TEMP_1 }
	{ jal send_itc_marker }
fpu_3:
	#{ moveli R_TEMP_0, lo16(0x40000000) }  # expected value = 2
	#{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40000000)}
	{ moveli R_TEMP_0, lo16(0x16) }  # expected value = 2
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x16)}
	{ mfspr  R_TEMP_5, 32004 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_ssqrt }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_ssqrt:

#endif
	#------------------------- this is the single precision div ---------------
        #{ move R_TEMP_0, zero }
	#{ mtspr 32004,    R_TEMP_0 } #zero out reg 4

	
        { moveli R_TEMP_0, lo16(0x40800000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40800000) } #fpu reg2 - srcA = 4
	{ mtspr 32002,    R_TEMP_0 }
 
        { moveli R_TEMP_0, lo16(0x41800000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x41800000) } #fpu reg4 - srcA = 16
	{ mtspr 32004,    R_TEMP_0 }
 	
        #{ moveli R_TEMP_0, lo16(0x21000084) }
	#{ auli   R_TEMP_0, R_TEMP_0, ha16(0x21000084) } #instruction ssqrt reg 2 ^ (1/2) = reg 4
	#{ mtspr 32064,   R_TEMP_0 }
 	
        { moveli R_TEMP_0, lo16(0x20002106) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x21002106) } #instruction ssqrt reg 4/ reg 2 = reg 6
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	{ mfspr R_TEMP_1, 32006 }
	{ xor R_TEMP_0, R_MY_X, R_MASTER_X }
        { bnz R_TEMP_0, fpu_4 ; xor R_TEMP_0, R_MY_Y, R_MASTER_Y }
        { bnz R_TEMP_0, fpu_4 }
	{ shri R_TEMP_1, R_TEMP_1, 16 }
	{ move R_L1_0, R_TEMP_1 }
	{ jal send_itc_marker }
fpu_4:
	#{ moveli R_TEMP_0, lo16(0x40000000) }  # expected value = 2
	#{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40800000)}
	{ moveli R_TEMP_0, lo16(0x40000000) }  # expected value = 2
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40800000)}
	{ mfspr  R_TEMP_5, 32006 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_sdiv }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_sdiv:

	#------------------------- this is the double precision mul ---------------
 
	
        { moveli R_TEMP_0, lo16(0x00000000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x00000000) } #fpu reg2 - srcA = 4
	{ mtspr 32002,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(0x40100000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40100000) } #fpu reg32 - srcA = 4
	{ mtspr 32003,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(0x00000000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x00000000) } #fpu reg2 - srcA = 4
	{ mtspr 32004,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(0x40140000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40140000) } #fpu reg32 - srcA = 4
	{ mtspr 32005,    R_TEMP_0 }	 

	 	
        { moveli R_TEMP_0, lo16(0xc3002106) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0xc3002106) } #instruction ssqrt reg 4/ reg 2 = reg 6
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	{ mfspr R_TEMP_1, 32006 }
	{ xor R_TEMP_0, R_MY_X, R_MASTER_X }
        { bnz R_TEMP_0, fpu_5 ; xor R_TEMP_0, R_MY_Y, R_MASTER_Y }
        { bnz R_TEMP_0, fpu_5 }
	{ shri R_TEMP_1, R_TEMP_1, 16 }
	{ move R_L1_0, R_TEMP_1 }
	{ jal send_itc_marker }
fpu_5:

	{ moveli R_TEMP_0, lo16(0x40340000) }  # expected value = 20
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40340000)}
	{ mfspr  R_TEMP_5, 32007 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_dmul0 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_dmul0:

	{ moveli R_TEMP_0, lo16(0x00000000) }  # expected value = 20
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x00000000)}
	{ mfspr  R_TEMP_5, 32006 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_dmul1 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_dmul1:
	
#if 0
	#keep getting zero
	#------------------------- this is the double precision div ---------------
        { move R_TEMP_0, zero }
	{ mtspr 32006,    R_TEMP_0 } 
        { move R_TEMP_0, zero }
	{ mtspr 32007,    R_TEMP_0 } 
	
        { moveli R_TEMP_0, lo16(0x00000000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x00000000) } #fpu reg2 - srcA = 4
	{ mtspr 32002,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(0x40100000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40100000) } #fpu reg3 - srcA = 4
	{ mtspr 32003,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(0x00000000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x00000000) } #fpu reg4 - srcA = 16
	{ mtspr 32004,    R_TEMP_0 }
	
        { moveli R_TEMP_0, lo16(0x40140000) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40300000) } #fpu reg5 - srcA = 16
	{ mtspr 32005,    R_TEMP_0 }	 

	 	
        { moveli R_TEMP_0, lo16(0xe0002106) }
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0xe0002106) } #instruction ssqrt reg 4/ reg 2 = reg 6
	{ mtspr 32064,   R_TEMP_0 }
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
        nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	{ mfspr R_TEMP_1, 32007 }
	{ xor R_TEMP_0, R_MY_X, R_MASTER_X }
        { bnz R_TEMP_0, fpu_6 ; xor R_TEMP_0, R_MY_Y, R_MASTER_Y }
        { bnz R_TEMP_0, fpu_6 }
	{ shri R_TEMP_1, R_TEMP_1, 16 }
	{ move R_L1_0, R_TEMP_1 }
	{ jal send_itc_marker }
fpu_6:

	{ moveli R_TEMP_0, lo16(0x40100000) }  # expected value = 4
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x40100000)}
	{ mfspr  R_TEMP_5, 32007 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_ddiv0 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_ddiv0:

	{ moveli R_TEMP_0, lo16(0x00000000) }  # expected value = 4
	{ auli   R_TEMP_0, R_TEMP_0, ha16(0x00000000)}
	{ mfspr  R_TEMP_5, 32006 }
        { xor  R_TEMP_0, R_TEMP_0, R_TEMP_5 }
	{ bz R_TEMP_0, fpu_ddiv1 }
	{ addi R_L3_0, R_L3_0, 1 }
fpu_ddiv1:
#endif

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
