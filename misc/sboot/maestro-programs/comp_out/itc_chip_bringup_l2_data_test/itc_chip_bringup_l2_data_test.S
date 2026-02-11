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
// R_L3_5 = base address (different for way 0/1 to use different tags)
// R_L3_6 = actual returned data
// R_L3_7 = halfword loop counter start value, different for way 0/1 to avoid istream
// R_L3_8 = halfword loop counter start value depending on 64/32K mode
// R_L3_9 = saved link register
// R_L3_11 = saved boot test control word (passed in R_L3_0, which is the return reg) for tester functional patterns
//           bit 0 indicates we should not output error messages and end test on first detected error
                
// set this to 0 to not stop on the first detected error.
#define stop_on_err_imm 1

boot_test_3:
        move	R_L3_11, R_L3_0        
        add     R_L3_0, zero, zero		// setup return value (count of failing addrs)
        add     R_L3_9, zero, lr		// save Link Register        

        //------------------- Version block 1
#if __tile_chip__ == 1
         // Wachusett Version              
        // On W, we can only test 32K due to CRAM only having 2 ways, regardless of cbox_cache_config
        // Actually, we can test entire 64K (all 4 ways) with JZ's diag trick, so set that up
        // code generously adapted from from dv/tests/memory/cram_with_diag.S
        // Enable diag tracing, but never take samples (sample_sel field is zero)
        mtspr SPR_DIAG_TRACE_WAY, zero
        movei R_TEMP_0, 4
        mtspr SPR_DIAG_TRACE_CTL, R_TEMP_0
        drain
        
        addli   R_L3_8, zero, 8192
        mfspr   R_TEMP_0, SPR_AER_1
        moveli  R_TEMP_1, lo16(0x08000000)  // Set bit 27 to not alloc in L1
        auli    R_TEMP_1, R_TEMP_1, ha16(0x08000000)
        or      R_TEMP_2, R_TEMP_1, R_TEMP_0
        mtspr   SPR_AER_1, R_TEMP_2
        drain                        
#else
         // Tile64 Version
        // assume we are in 64K mode, change # of halfword writes if we are in 32K mode
        addli   R_L3_8, zero, 16384
        mfspr   R_TEMP_0, SPR_CBOX_CACHE_CONFIG
        shri    R_TEMP_0, R_TEMP_0, 1
        bbns    R_TEMP_0, test_way_0
        addli   R_L3_8, zero, 8192
#endif
        //-------------------
        
test_way_0:
        // for way 0, skip low 8K due to istream
        moveli  R_L3_5, lo16(0x00002000)
        auli    R_L3_5, R_L3_5, ha16(0x00002000)
        addli   R_L3_7, R_L3_8, -4096		// 32K (1 way only) - 8K istream = 24K / 2B halfword = 12K
        jf      l2_ram_contents_test

test_way_1:
        // Switch to way 1, use a different tag, and check all indexes
        moveli  R_L3_5, lo16(0x00010000)
        auli    R_L3_5, R_L3_5, ha16(0x00010000)
        add     R_L3_7, R_L3_8, zero		// 32K (1 way only) / 2B halfword = 16K 
        addi    R_TEMP_0, zero, 0x3
        mf
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain
        jf      l2_ram_contents_test
        
        //------------------- Version Block 2
#if __tile_chip__ == 1
        // On W, test the other two ways by playing with diag
test_way_2:
        // Leave CAR pointing at "second available way", but disable way 1 with diag
        moveli  R_L3_5, lo16(0x00020000)
        auli    R_L3_5, R_L3_5, ha16(0x00020000)
        add     R_L3_7, R_L3_8, zero		// 32K (1 way only) / 2B halfword = 16K
        addi    R_TEMP_0, zero, 0x2
        mf
        mtspr   SPR_DIAG_TRACE_WAY, R_TEMP_0
        drain
        jf      l2_ram_contents_test

test_way_3:
        // Leave CAR pointing at "second available way", but disable ways 1 & 2 with diag
        moveli  R_L3_5, lo16(0x00030000)
        auli    R_L3_5, R_L3_5, ha16(0x00030000)
        add     R_L3_7, R_L3_8, zero		// 32K (1 way only) / 2B halfword = 16K
        addi    R_TEMP_0, zero, 0x6
        mf
        mtspr   SPR_DIAG_TRACE_WAY, R_TEMP_0
        drain
        jf      l2_ram_contents_test
#endif
        //-------------------
        
// L2 RAM contents test
// first, write into entire L2 array
l2_ram_contents_test:
        //------------------- Version Block 3
#if __tile_chip__ == 0        
        // This block only in Tile64 
        // turn cacheable bit back on
        mf
        mfspr   R_TEMP_0, SPR_AER_1
        moveli  R_TEMP_1, lo16(0x10000000)
        auli    R_TEMP_1, R_TEMP_1, ha16(0x10000000)
        or      R_TEMP_2, R_TEMP_1, R_TEMP_0
        mtspr   SPR_AER_1, R_TEMP_2
        drain 
#endif        
        //-------------------
       
        add     R_L3_1, zero, R_L3_5
        add     R_L3_2, zero, zero
        add     R_L3_3, zero, R_L3_7

write_l2:
        sh      R_L3_1, R_L3_2
        addi    R_L3_1, R_L3_1, 2
        addi    R_L3_2, R_L3_2, 1
        addli   R_L3_3, R_L3_3, -1
        bgzt    R_L3_3, write_l2

// second, check what we wrote, invert data, and write again
        add     R_L3_1, zero, R_L3_5
        add     R_L3_2, zero, zero
        add     R_L3_3, zero, R_L3_7
        moveli  R_L3_4, lo16(0xffffffff)
        auli    R_L3_4, R_L3_4, ha16(0xffffffff)

check_l2_part_1:
        //------------------- Version block 4
#if __tile_chip__ == 0
        // This block only in Tile64 
        // turn off caching when loading so we don't access the L1
        mf
        mtspr   SPR_AER_1, zero
        drain        
        // turn on caching for next st
        mf
        moveli  R_TEMP_0, lo16(0x10000000)
        auli    R_TEMP_0, R_TEMP_0, ha16(0x10000000)
        mtspr   SPR_AER_1, R_TEMP_0
        drain        
#endif
        //-------------------        
        
        lh      R_L3_6, R_L3_1
        sne     R_TEMP_0, R_L3_2, R_L3_6
        bbnst   R_TEMP_0, continue_part_1
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0002dbad)        // indicate failure in L2D
        auli    R_L1_1, R_L1_1, ha16(0x0002dbad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_2                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_6                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_part_1:
        xor     R_TEMP_2, R_L3_2, R_L3_4
        // must turn on caching so we write into the L2 in CRAM mode (done above after lh)
        // really should already be in L2, but in case we erroneously have a miss
        sh      R_L3_1, R_TEMP_2
        // extra prefetch to blow away the store fifo entry and L1 cache line
        // otherwise, we end up loading from the L1 instead of the L2
        // (and must use prefetch so we don't re-fill L1)
//         lh      zero, R_L3_1
        addi    R_L3_1, R_L3_1, 2
        addi    R_L3_2, R_L3_2, 1
        addli   R_L3_3, R_L3_3, -1
        bgzt   R_L3_3, check_l2_part_1

// third, check that the inverted data reads back ok

        //------------------- Version Block 5
#if __tile_chip__ == 0
        // This block only in Tile64 
// turn off the cacheable bit so we don't fill into the L1
// and all of the loads come directly from the L2
// TSIM doesn't properly model this, so if running on tsim just let the loads hit the L1
#ifndef STUB_FABRIC
        mf
        mfspr   R_TEMP_0, SPR_AER_1
        moveli  R_TEMP_1, lo16(0xefffffff)
        auli    R_TEMP_1, R_TEMP_1, ha16(0xefffffff)
        and     R_TEMP_2, R_TEMP_1, R_TEMP_0
        mtspr   SPR_AER_1, R_TEMP_2
        drain
#endif                
#endif                
        //-------------------
        
        add     R_L3_1, zero, R_L3_5
        moveli  R_L3_2, lo16(0xffffffff)
        auli    R_L3_2, R_L3_2, ha16(0xffffffff)
        add     R_L3_3, zero, R_L3_7

check_l2_part_2:
        lh      R_L3_6, R_L3_1
        sne     R_TEMP_0, R_L3_2, R_L3_6
        bbnst   R_TEMP_0, continue_part_2
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0002dbad)        // indicate failure in L2D
        auli    R_L1_1, R_L1_1, ha16(0x0002dbad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_2                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_6                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_part_2:
        addi    R_L3_1, R_L3_1, 2
        addi    R_L3_2, R_L3_2, -1
        addli   R_L3_3, R_L3_3, -1
        bgzt   R_L3_3, check_l2_part_2                        

         // Use base address to determine which way we just tested, and go to next way
        moveli  R_TEMP_0, lo16(0x00002000)
        auli    R_TEMP_0, R_TEMP_0, ha16(0x00002000)
        sub     R_TEMP_1, R_L3_5, R_TEMP_0
        bz      R_TEMP_1, test_way_1

        //------------------- Version Block 6
#if __tile_chip__ == 1
        moveli  R_TEMP_0, lo16(0x00010000)
        auli    R_TEMP_0, R_TEMP_0, ha16(0x00010000)
        sub     R_TEMP_1, R_L3_5, R_TEMP_0
        bz      R_TEMP_1, test_way_2

        moveli  R_TEMP_0, lo16(0x00020000)
        auli    R_TEMP_0, R_TEMP_0, ha16(0x00020000)
        sub     R_TEMP_1, R_L3_5, R_TEMP_0
        bz      R_TEMP_1, test_way_3
#endif
        //-------------------

// R_L3_0 return value should have been set to 0 at start, and incremented if any failure occured
done:

        //------------------- Version Block 6
#if __tile_chip__ == 0
        // This block only in Tile64 
        // ensure cacheable bit is on
        mf
        moveli  R_TEMP_0, lo16(0x10000000)
        auli    R_TEMP_0, R_TEMP_0, ha16(0x10000000)
        mtspr   SPR_AER_1, R_TEMP_0
        drain                
#endif
        //-------------------
        
        add     lr, zero, R_L3_9                // restore link register
        jrp     lr
        
boot_mem_test_3:
        { jrp lr; move R_L3_0, zero }
        
# Make sure this last address is 64-byte aligned so that the code above is padded to a full cache line
# so that when it is loaded into the L2 cache in cache-as-RAM mode, there are no parity errors        
.align 64
_last_addr:             

