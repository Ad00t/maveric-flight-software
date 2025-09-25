//
// Copyright 2009 Tilera Corporation. All Rights Reserved.
// 
//   The source code contained or described herein and all documents
//   related to the source code ("Material") are owned by Tilera
//   Corporation or its suppliers or licensors.  Title to the Material
//   remains with Tilera Corporation or its suppliers and licensors. The
//   software is licensed under the Tilera MDE License.
// 
//   Unless otherwise agreed by Tilera in writing, you may not remove or
//   alter this notice or any other notice embedded in Materials by Tilera
//   or Tilera's suppliers or licensors in any way.
//
//

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

// R_L3_1 = addr under test
// R_L3_2 = data we are storing, expect to load
// R_L3_3 = halfword loop counter
// R_L3_4 = xor mask to invert data
// R_L3_5 = actual returned data
// R_L3_9 = saved link register
// R_L3_11 = saved boot test control word (passed in R_L3_0, which is the return reg) for tester functional patterns
//           bit 0 indicates we should not output error messages and end test on first detected error

// set this to 0 to not stop on the first detected error.
#define stop_on_err_imm 1

boot_test_3:
        move	R_L3_11, R_L3_0
        add     R_L3_0, zero, zero		// setup return value (count of failing addrs)
        add     R_L3_9, zero, lr		// save Link Register        
        
// L1 data RAM contents test
// first, write into entire L1 array
// do this by storing to 8K region in L2, and then loading back (which returns data from L2, but brings block into L1)
        moveli  R_L3_1, lo16(0x00002000)
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        add     R_L3_2, zero, zero
        addli   R_L3_3, zero, 4096		// 8K / 2 = 4K halfwords

write_l2:
        sh      R_L3_1, R_L3_2
        addi    R_L3_1, R_L3_1, 2
        addi    R_L3_2, R_L3_2, 1
        addli   R_L3_3, R_L3_3, -1
        bgzt    R_L3_3, write_l2

#        movei	R_L1_0, PASS_MSG
#        moveli  R_L1_1, 0xbcd
#	jal     output_msg_1
        
        moveli  R_L3_1, lo16(0x00002000)
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        add     R_L3_2, zero, zero
        addli   R_L3_3, zero, 512		// 8K / 4B per word = 2048 words / 4 words per L1 block = 512
fill_into_l1:
        lw      R_L3_2, R_L3_1
        addi    R_L3_1, R_L3_1, 16		// L1 block is 16B, so move ld addr to next block
        addli   R_L3_3, R_L3_3, -1
        bgzt    R_L3_3, fill_into_l1

// second, load again to check data from L1, invert data, and write again (store fifo updates L1)
        moveli  R_L3_1, lo16(0x00002000)
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        add     R_L3_2, zero, zero
        addli   R_L3_3, zero, 4096
        moveli  R_L3_4, lo16(0xffffffff)
        auli    R_L3_4, R_L3_4, ha16(0xffffffff)

check_l1_part_1:
        lh      R_L3_5, R_L3_1
        sne     R_TEMP_0, R_L3_2, R_L3_5
        bbns    R_TEMP_0, continue_part_1
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0001dbad)        // indicate failure in L1D
        auli    R_L1_1, R_L1_1, ha16(0x0001dbad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_2                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_5                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_part_1:
        xor     R_TEMP_1, R_L3_2, R_L3_4
        sh      R_L3_1, R_TEMP_1
        nop					// nops to be sure that store updates L1 before next load (ld to same index will inval store fifo)
        nop
        nop
        nop
        addi    R_L3_1, R_L3_1, 2
        addi    R_L3_2, R_L3_2, 1
        addli   R_L3_3, R_L3_3, -1
        bgzt    R_L3_3, check_l1_part_1

// third, check that the inverted data reads back ok
        moveli  R_L3_1, lo16(0x00002000)
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        moveli  R_L3_2, lo16(0xffffffff)
        auli    R_L3_2, R_L3_2, ha16(0xffffffff)
        addli   R_L3_3, zero, 4096

check_l1_part_2:
        lh      R_L3_5, R_L3_1
        sne     R_TEMP_0, R_L3_2, R_L3_5
        bbns    R_TEMP_0, continue_part_2
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0001dbad)        // indicate failure in L1D
        auli    R_L1_1, R_L1_1, ha16(0x0001dbad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_2                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_5                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_part_2:
        addi    R_L3_1, R_L3_1, 2
        addi    R_L3_2, R_L3_2, -1
        addi    R_L3_3, R_L3_3, -1
        bgzt    R_L3_3, check_l1_part_2

// R_L3_0 return value should have been set to 0 at start, and incremented if any failure occured
done:
        mf
        add     lr, zero, R_L3_9                // restore link register
        jrp     lr
        
boot_mem_test_3:
        { jrp lr; move R_L3_0, zero }
        
# Make sure this last address is 64-byte aligned so that the code above is padded to a full cache line
# so that when it is loaded into the L2 cache in cache-as-RAM mode, there are no parity errors                
.align 64
_last_addr:             

