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
// R_L3_2 = returned data
// R_L3_3 = original addr to invert (in inverted tag loop)
// R_L3_4 = xor mask to invert data
// R_L3_5 = index loop counter
// R_L3_6 = base address (different for way 0/1 to use different tags)
// R_L3_7 = index loop counter start value different for way 0/1 to avoid istream
// R_L3_8 = # of indexes to test for this way (index loop counter starting value)
// R_L3_9 = saved link register
// R_L3_11 = saved boot test control word (passed in R_L3_0, which is the return reg) for tester functional patterns
//           bit 0 indicates we should not output error messages and end test on first detected error
                
// set this to 0 to not stop on the first detected error.
#define stop_on_err_imm 1

boot_test_3:
        move	R_L3_11, R_L3_0
        add     R_L3_0, zero, zero		// setup return value (count of failing addrs)
        add     R_L3_9, zero, lr		// save Link Register

        //------------------- Version Block 1
#if __tile_chip__ == 0
        // This block only in Tile64 
        // assume we are in 64K mode, change max indexes if we are in 32K mode
        addli   R_L3_8, zero, 512
        mfspr   R_TEMP_0, SPR_CBOX_CACHE_CONFIG
        shri    R_TEMP_0, R_TEMP_0, 1
        bbns    R_TEMP_0, test_way_0
#else                
        // On W, we can only test 32K due to CRAM only having 2 ways, regardless of cbox_cache_config
        // Actually, we can test entire 64K (all 4 ways) with JZ's diag trick, so set that up
        // code generously adapted from from dv/tests/memory/cram_with_diag.S
        // Enable diag tracing, but never take samples (sample_sel field is zero)
        mtspr SPR_DIAG_TRACE_WAY, zero
        movei R_TEMP_0, 4
        mtspr SPR_DIAG_TRACE_CTL, R_TEMP_0
        drain
#endif                   
        //-------------------

        // init index loop counter        
        addli   R_L3_8, zero, 256

test_way_0:
        // for way 0, skip low 4K due to istream (start with index = 64, tag = 0)
        moveli  R_L3_6, lo16(0x50002000)
        auli    R_L3_6, R_L3_6, ha16(0x50002000)
        addi    R_L3_7, R_L3_8, -128
        jf      l2_tag_test

test_way_1:
        // Switch to way 1, use different tags, check all indexes
        moveli  R_L3_6, lo16(0x51000000)
        auli    R_L3_6, R_L3_6, ha16(0x51000000)
        add     R_L3_7, R_L3_8, zero
        addi    R_TEMP_0, zero, 0x3
        mf
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain
        jf      l2_tag_test

        //------------------- Version Block 2
#if __tile_chip__ == 1
        // On W, test the other two ways by playing with diag
test_way_2:
        // Leave CAR pointing at "second available way", but disable way 1 with diag
        moveli  R_L3_6, lo16(0x52000000)
        auli    R_L3_6, R_L3_6, ha16(0x52000000)
        add     R_L3_7, R_L3_8, zero		// 32K (1 way only) / 2B halfword = 16K
        addi    R_TEMP_0, zero, 0x2
        mf
        mtspr   SPR_DIAG_TRACE_WAY, R_TEMP_0
        drain
        jf      l2_tag_test

test_way_3:
        // Leave CAR pointing at "second available way", but disable ways 1 & 2 with diag
        moveli  R_L3_6, lo16(0x53000000)
        auli    R_L3_6, R_L3_6, ha16(0x53000000)
        add     R_L3_7, R_L3_8, zero		// 32K (1 way only) / 2B halfword = 16K
        addi    R_TEMP_0, zero, 0x6
        mf
        mtspr   SPR_DIAG_TRACE_WAY, R_TEMP_0
        drain
        jf      l2_tag_test
#endif        
        //-------------------
        
l2_tag_test:
// L2 tag RAM contents test
// set the xor bit mask for when we need to invert
        
        //------------------- Version block 3
#if __tile_chip__ == 1
         // Wachusett Version              
        moveli  R_L3_4, lo16(0xffffc000)
        auli    R_L3_4, R_L3_4, ha16(0xffffc000)
#else
         // Tile64 Version
        moveli  R_L3_4, lo16(0xffff8000)
        auli    R_L3_4, R_L3_4, ha16(0xffff8000)
#endif        
        //-------------------
        
// set up upper 4 bits of PA to use
        mf
        addi    R_TEMP_0, zero, 0x5
        mtspr   SPR_AER_0, R_TEMP_0
        drain                
// fill tag ram with incrementing pattern.
// use the same pattern in the data, we only need it to identify the tag.
// must increment the tag twice per index, so each way gets a different tag.        
        add     R_L3_1, zero, R_L3_6
        add     R_L3_5, zero, R_L3_7
write_l2_tags:
        sw      R_L3_1, R_L3_1
        auli    R_L3_1, R_L3_1, ha16(0x00010000)
        addli   R_L3_1, R_L3_1, lo16(0x00000040)
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_l2_tags

// next, check that all the tags wrote correctly, invert tag, and write again        
        add     R_L3_1, zero, R_L3_6
        add     R_L3_5, zero, R_L3_7
check_l2_tags_part_1:
        lw      R_L3_2, R_L3_1
        sne     R_TEMP_0, R_L3_1, R_L3_2
        bbnst   R_TEMP_0, continue_part_1
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0002abad)        // indicate failure in L2tAg
        auli    R_L1_1, R_L1_1, ha16(0x0002abad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_6                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_2                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_part_1:
        auli    R_L3_1, R_L3_1, ha16(0x00010000)	// go to the next tag/index
        addli   R_L3_1, R_L3_1, lo16(0x00000040)
        addli   R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, check_l2_tags_part_1

// I have a separate loop to write the inverted tag so we only have to set the upper 4 bits in AER_0 once
        mf
        addi    R_TEMP_0, zero, 0xa                     // invert the upper 4 bits of the tag
        mtspr   SPR_AER_0, R_TEMP_0
        drain                
        add     R_L3_1, zero, R_L3_6
        add     R_L3_5, zero, R_L3_7        
write_inverted_tag:
        xor     R_TEMP_0, R_L3_1, R_L3_4		// invert the tag
        sw      R_TEMP_0, R_TEMP_0			// store the inverted tag
        auli    R_L3_1, R_L3_1, ha16(0x00010000)	// go to the next tag/index
        addli   R_L3_1, R_L3_1, lo16(0x00000040)
        addli   R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_inverted_tag

// finally, check that the inverted value is correct
        add     R_L3_3, zero, R_L3_6
        add     R_L3_5, zero, R_L3_7
check_l2_tags_part_2:
        xor     R_L3_1, R_L3_3, R_L3_4		// get the inverted tag of the original addr
        lw      R_L3_2, R_L3_1
        sne     R_TEMP_0, R_L3_1, R_L3_2	// check that we got the inverted value
        bbnst   R_TEMP_0, continue_part_2
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0002abad)        // indicate failure in L2tAg
        auli    R_L1_1, R_L1_1, ha16(0x0002abad)
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
        auli    R_L3_3, R_L3_3, ha16(0x00010000)        // go to the next original addr
        addli   R_L3_3, R_L3_3, lo16(0x00000040)
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, check_l2_tags_part_2

         // Use base address to determine which way we just tested, and go to next way
        moveli  R_TEMP_0, lo16(0x50002000)
        auli    R_TEMP_0, R_TEMP_0, ha16(0x50002000)
        sub     R_TEMP_1, R_L3_6, R_TEMP_0
        bz      R_TEMP_1, test_way_1

        //------------------- Version Block 4
#if __tile_chip__ == 1
        moveli  R_TEMP_0, lo16(0x51000000)
        auli    R_TEMP_0, R_TEMP_0, ha16(0x51000000)
        sub     R_TEMP_1, R_L3_6, R_TEMP_0
        bz      R_TEMP_1, test_way_2

        moveli  R_TEMP_0, lo16(0x52000000)
        auli    R_TEMP_0, R_TEMP_0, ha16(0x52000000)
        sub     R_TEMP_1, R_L3_6, R_TEMP_0
        bz      R_TEMP_1, test_way_3
#endif
        //-------------------

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
