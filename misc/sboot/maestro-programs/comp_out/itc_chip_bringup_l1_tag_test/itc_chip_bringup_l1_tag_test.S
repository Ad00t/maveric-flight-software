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
// R_L3_2 = data loaded
// R_L3_3 = original addr to invert (in inverted tag loop) 
// R_L3_4 = xor mask to invert data
// R_L3_5 = tag loop counter
// R_L3_9 = saved link register
// R_L3_11 = saved boot test control word (passed in R_L3_0, which is the return reg) for tester functional patterns
//           bit 0 indicates we should not output error messages and end test on first detected error
                
// set this to 0 to not stop on the first detected error.
#define stop_on_err_imm 1

boot_test_3:
        move	R_L3_11, R_L3_0
        add     R_L3_0, zero, zero		// setup return value (count of failing addrs)
        add     R_L3_9, zero, lr		// save Link Register        
        
// L1 tag RAM contents test
// fill tag ram with incrementing pattern.
// use the same pattern in the data, we only need it to identify the tag.
// to fill L1, must first store data into L2 and then use loads to bring into L1.
        moveli  R_L3_1, lo16(0x00002000)                // start with index=0, tag=1 (must avoid istream in L2)
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        addli   R_L3_5, zero, 512                       // 512 ways (ie tags) to write

write_l1_tags:
        // to avoid CRAM st miss to later 48B bug, must first write to first 16B of L2 cache line
        moveli  R_TEMP_0, lo16(0xffffffc0)
        auli    R_TEMP_0, R_TEMP_0, ha16(0xffffffc0)
        and     R_TEMP_1, R_TEMP_0, R_L3_1
        sw      R_TEMP_1, zero
        sw      R_L3_1, R_L3_1                          // store writes to L2
        lw      R_L3_2, R_L3_1				// need to fill back into L1
        auli    R_L3_1, R_L3_1, ha16(0x00010000)	// increment tag (use bits that are tag in L2 as well)
        addli   R_L3_1, R_L3_1, lo16(0x00000010)	// increment index
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_l1_tags

// next, check that all the tags wrote correctly, invert tag, and write again        
        moveli  R_L3_1, lo16(0x00002000)
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        addli   R_L3_5, zero, 512

check_l1_tags_part_1:
        lw      R_L3_2, R_L3_1
        sne     R_TEMP_0, R_L3_1, R_L3_2
        bbnst   R_TEMP_0, continue_part_1
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0001abad)        // indicate failure in L1 tag
        auli    R_L1_1, R_L1_1, ha16(0x0001abad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_2                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_part_1:
        auli    R_L3_1, R_L3_1, ha16(0x00010000)	// go to the next tag/index
        addli   R_L3_1, R_L3_1, lo16(0x00000010)
        addli   R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, check_l1_tags_part_1

        // Change CRAM mode to use way 1 so we don't trash our program with these stores
        addi    R_TEMP_0, zero, 0x3
        mf
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain
        
        // Separate loop to write the inverted tag so we overwrite the correct way due to LRU
        moveli  R_L3_1, lo16(0x00002000)
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        addli   R_L3_5, zero, 512
        moveli  R_L3_4, lo16(0xfffff000)
        auli    R_L3_4, R_L3_4, ha16(0xfffff000)

write_inverted_tag:
        xor     R_TEMP_4, R_L3_1, R_L3_4		// invert the tag
        // to avoid CRAM st miss to later 48B bug, must first write to first 16B of L2 cache line
        moveli  R_TEMP_0, lo16(0xffffffc0)
        auli    R_TEMP_0, R_TEMP_0, ha16(0xffffffc0)
        and     R_TEMP_1, R_TEMP_0, R_TEMP_4
        sw      R_TEMP_1, zero
        sw      R_TEMP_4, R_TEMP_4			// store the inverted tag
        lw      R_L3_2, R_TEMP_4			// need to fill back into L1
        auli    R_L3_1, R_L3_1, ha16(0x00010000)	// go to the next tag/index
        addli   R_L3_1, R_L3_1, lo16(0x00000010)
        addli   R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_inverted_tag        

// finally, check that the inverted value is correct
        moveli  R_L3_3, lo16(0x00002000)
        auli    R_L3_3, R_L3_3, ha16(0x00002000)
        addli   R_L3_5, zero, 512
check_l1_tags_part_2:
        xor     R_L3_1, R_L3_3, R_L3_4		// get the inverted tag of the original addr
        lw      R_L3_2, R_L3_1
        sne     R_TEMP_0, R_L3_1, R_L3_2	// check that we got the inverted value
        bbnst   R_TEMP_0, continue_part_2
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0001abad)        // indicate failure in L1D
        auli    R_L1_1, R_L1_1, ha16(0x0001abad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_2                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_part_2:
        auli    R_L3_3, R_L3_3, ha16(0x00010000)	// go to the next tag/index
        addli   R_L3_3, R_L3_3, lo16(0x00000010)
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, check_l1_tags_part_2
        
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
