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

// R_L3_11 = saved boot test control word (passed in R_L3_0, which is the return reg) for tester functional patterns
//           bit 0 indicates we should not output error messages and end test on first detected error

// set this to 0 to not stop on the first detected error.
#define stop_on_err_imm 1

boot_test_3:
        move	R_L3_11, R_L3_0
        add     R_L3_0, zero, zero		// setup return value (count of failing addrs)
        add     R_L3_9, zero, lr		// save Link Register
        
// Walking 1's for entire datapath to L2
// Walking 1's will catch any datapath sa1, sa0, short errors.  It does this by writing a 1 to each bit
// in the dpath individually, and then immediately reading it back.
// sa1 -- caught because instead of reading back just the 1 we set, the sa1 bit will be set
// sa0 -- caught because when we try to write that bit to 1, we will read back 0
// short -- two bits will be set to 1 instead of just the one bit we wanted to set
// open -- will read back 0 instead of 1
// The l2 is physically arranged as 4 banks, each bank is 16B wide * 1024 lines deep = 16K * 4 banks = 64K L2.
// Only one bank is written at a time, so the L2 datapath is 16B wide.
// We set each bit of the dpath to 1 individually - first by setting each bit of a sw to 1,
// then by doing 4 sw to consecutive word addresses.  We then do repeat this for an address on each bank.
//
// addr = 0;
// data = 1;
// for (word_in_dpath = 4; word_in_dpath > 0; word_in_dpath--) {   
//   for (bit_in_word = 32; bit_in_word > 0; bit_in_word--) {
//     *addr = data;
//     temp_1 = *addr;
//     if (temp1 != data) {
//       fail();
//     }
//     data = data << 1; // actually a rotate so last iteration sets up next
//   }
//   addr = addr + 4;
// }
        // setup way 0 first for dpath testing
        addi    R_TEMP_0, zero, 0x1
        mf
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain
           
        
        // I just picked an index that is not otherwise used.  This assumes 64K mode.
        // Index should work in 32K mode too, only difference is that we can't test the 32K
        // block that is not being used.  The SPR_CBOX_CACHE_CONFIG is read only, so we can't
        // switch which 32K block to use.
        moveli  R_L3_1, lo16(0x00003fc0)        // R_L3_1 = addr
        auli    R_L3_1, R_L3_1, ha16(0x00003fc0)
        addi    R_L3_2, zero, 1                 // R_L3_2 = data
        addi    R_L3_5, zero, 4                 // loop counter for bank in L2, must set index/way accordingly

walk_l2_bank:           
        addli   R_L3_3, zero, 4                 // loop counter for word in L2 dpath, 4 words * 4B = 16B L2 dpath width
walk_l2_block:
        addli   R_L3_4, zero, 32                // loop counter for bit in word, 32 bits = 4B in a word        
walk_word:         
        sw      R_L3_1, R_L3_2
        lw      R_L3_8, R_L3_1
        sne     R_TEMP_0, R_L3_2, R_L3_8	// if what we read doesn't match what we wrote, branch to fail_dpath
        bbnst   R_TEMP_0, continue_data_walk_1s
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x00da2bad)        // DAta path L2 BAD
        auli    R_L1_1, R_L1_1, ha16(0x00da2bad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_2                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_8                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_data_walk_1s:
        rli     R_L3_2, R_L3_2, 1               // rotate register to set next bit
        addi    R_L3_4, R_L3_4, -1
        bgzt    R_L3_4, walk_word

        addi    R_L3_1, R_L3_1, 4
        addi    R_L3_3, R_L3_3, -1
        bgzt    R_L3_3, walk_l2_block

        addi    R_L3_5, R_L3_5, -1
        // set index, bit to use the correct L2 bank
        addi    R_TEMP_0, R_L3_5, -2
        bgzt    R_TEMP_0, use_first_index
        // If we are in 32K mode, we cannot do the second "index", the index MSB does not select
        // the bank, it is read from SPR_CBOX_CACHE_CONFIG which is read only.  so no need to do
        // the rest of the dpath test (those two banks aren't used).
        // unfortunately on W we can only test 32K (so first two ways of all indexes) in CRAM mode
        
        //------------------- Version block 1
#if __tile_chip__ == 1
         // Wachusett Version              
        jf        addr_path_test
#else        
         // Tile64 Version
        mf
        mfspr   R_TEMP_1, SPR_CBOX_CACHE_CONFIG
        drain
        // bit 1 indicates if we are in 32K mode or not
        shri    R_TEMP_1, R_TEMP_1, 1
        bbs     R_TEMP_1, addr_path_test
        // otherwise we are in 64K mode, set the index msb to test other two banks
        moveli  R_L3_1, lo16(0x00007fc0)        // R_L3_1 = addr
        auli    R_L3_1, R_L3_1, ha16(0x00007fc0)        
        bbs     R_L3_5, use_way_1
        addi    R_TEMP_0, zero, 0x1
        jf      set_cram_way
#endif        
        //-------------------        
        
use_first_index:
        moveli  R_L3_1, lo16(0x00003fc0)        // R_L3_1 = addr
        auli    R_L3_1, R_L3_1, ha16(0x00003fc0)        
        bbs     R_L3_5, use_way_1
        addi    R_TEMP_0, zero, 0x1
        jf      set_cram_way
use_way_1:
        auli    R_L3_1, R_L3_1, ha16(0x00010000)        
        addi    R_TEMP_0, zero, 0x3
set_cram_way:
        mf
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain        
        bgzt    R_L3_5, walk_l2_bank

        // Use way 1 to avoid istream conflicts
        addi    R_TEMP_0, zero, 0x3
        mf
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain

addr_path_test:
// Walking 1's for address bus to L2
// First, we take a base address, and one by one write a pattern (0xaa) to every
// power of two offset from the base.  eg, 0x80000000, 0x80000001, 0x80000002,
// 0x80000004, 0x80000008, 0x80000010, etc.  Then we write the inverse of the pattern (0x55)
// to the base addr.  Next, check that every other addr still contains the original
// pattern -- if any addr bit is stuck at 1, it will contain the inverse as well, and
// we should detect this error.
// Next, we want to detect stuck at 0's.  To do this, we write the original pattern (0xaa)
// into the base addr, so all power of two addrs contain the pattern.  Then, for each
// addr we write the inverse, and check the base addr.  If it is the inverse, 
// we know the addr bit that is stuck at 0.
//    when checking the tag bits, we have to set another index bit (so we don't evict istream code at index 0)
//    so checking that particular power of two addr is also required for sa0 faults
//    between two of the addr lines.

// R_L3_1 = addr we are testing
// R_L3_2 = base addr
// R_L3_3 = addr bit offset from base        
// R_L3_4 = test pattern
// R_L3_5 = loop counter for addr bit we are testing
// R_L3_6 = loop counter to check other addrs vs. addr bit we are testing
// R_L3_7 = AND/XOR mask to look at byte at addr; generate inverted pattern
// R_L3_8 = loaded data
                
        add     R_L3_1, zero, zero
        add     R_L3_2, zero, zero
        addi    R_L3_3, zero, 1
        addli   R_L3_4, zero, 0xaa
        //------------------- Version Block 2
#if __tile_chip__ == 0
        // This block only in Tile64 
        addi    R_L3_5, zero, 16
        mf
        mfspr   R_TEMP_1, SPR_CBOX_CACHE_CONFIG
        drain
        // bit 1 indicates if we are in 32K mode or not
        shri    R_TEMP_1, R_TEMP_1, 1
        bbns    R_TEMP_1, write_addr_default
#endif        
        //-------------------        
        // 32K mode, one less index, one more tag bit        
        addi    R_L3_5, zero, 15

// First, write a predefined pattern (0xaa) to base and every power of two offset
// When we hit the bit 15 (first tag bit)(14 for 32K mode), must use a different index for each following tag bit, 
// and an index above istream and that is not a power of two (we already using those).
// this assumes the index bits have no problems, but we are testing those individually anyway.
// The reason we have to use a different index (and hence different base) for each tag bit is because otherwise
// we keep overwriting index 0 when individually setting each tag bit.  So for each tag bit, we use a different
// index as the base addr.
// This loop writes the base addr, and with bits 14-0 set
write_addr_default:
        sb      R_L3_1, R_L3_4
        add     R_L3_1, R_L3_2, R_L3_3
        rli     R_L3_3, R_L3_3, 1
        // decrement loop counter
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_addr_default

        addli   R_L3_6, zero, 0x3a00		// R_L3_6 = index to use
        //------------------- Version block 3
#if __tile_chip__ == 0
        // This block only in Tile64 
        addi    R_L3_5, zero, 17
        mf
        mfspr   R_TEMP_1, SPR_CBOX_CACHE_CONFIG
        drain
        // bit 1 indicates if we are in 32K mode or not
        shri    R_TEMP_1, R_TEMP_1, 1
        bbns    R_TEMP_1, write_tag_bits
#endif
        //-------------------        
        // 32K mode, one less index, one more tag bit
        addi    R_L3_5, zero, 18
write_tag_bits:
        // write base addr for this tag bit to way 0, write addr with tag set to way 1
        mf
        addi    R_TEMP_0, zero, 0x1
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain
        add     R_TEMP_0, R_L3_2, R_L3_6
        sb      R_TEMP_0, R_L3_4
        mf
        addi    R_TEMP_0, zero, 0x3
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain        
        add     R_L3_1, R_L3_1, R_L3_6
        sb      R_L3_1, R_L3_4
        add     R_L3_1, R_L3_2, R_L3_3
        addi    R_L3_6, R_L3_6, 0x40		// go to next index for next tag
        rli     R_L3_3, R_L3_3, 1
        // decrement loop counter
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_tag_bits

        // must now write tags with bits [35:32] set from SPR_AER_0
        addi    R_L3_5, zero, 1
write_aer_bits:
        add     R_L3_1, zero, R_L3_6
        // write base addr to way 0 with all tag bits zeroed out first
        mf
        mtspr   SPR_AER_0, zero
        addi    R_TEMP_0, zero, 0x1
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain
        sb      R_L3_1, R_L3_4
        // and now write to the addr with the tag bit set in AER_0 to way 1
        mf
        mtspr   SPR_AER_0, R_L3_5
        addi    R_TEMP_0, zero, 0x3
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain
        sb      R_L3_1, R_L3_4
        addi    R_L3_6, R_L3_6, 0x40		// go to next index for next tag
        rli     R_L3_5, R_L3_5, 1
        shri    R_TEMP_0, R_L3_5, 4
        bbnst   R_TEMP_0, write_aer_bits
        
        

        // reset top tag bits in AER_0        
        mf
        mtspr   SPR_AER_0, zero
        drain

        // Everything from now on should hit in the L2, so we shouldn't have to mess
        // with the way specified in SPR_CBOX_CACHEASRAM_CONFIG
// Write the inverse pattern into all base addrs
        moveli  R_L3_7, lo16(0x000000ff)
        auli    R_L3_7, R_L3_7, ha16(0x000000ff)
        xor     R_TEMP_0, R_L3_4, R_L3_7
        sb      R_L3_2, R_TEMP_0

        addli   R_L3_6, zero, 0x3a00		// R_L3_6 = index to use
        //------------------- Version block 4
#if __tile_chip__ == 0
        // This block only in Tile64 
        addi    R_L3_5, zero, 17
        mf
        mfspr   R_TEMP_1, SPR_CBOX_CACHE_CONFIG
        drain
        // bit 1 indicates if we are in 32K mode or not
        shri    R_TEMP_1, R_TEMP_1, 1
        bbns    R_TEMP_1, write_base_addrs_inverse
#endif
        //-------------------        
        // 32K mode, one less index, one more tag bit        
        addi    R_L3_5, zero, 18
write_base_addrs_inverse:
        xor     R_TEMP_0, R_L3_4, R_L3_7
        sb      R_L3_6, R_TEMP_0
        addi    R_L3_6, R_L3_6, 0x40		// go to next index for next tag
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_base_addrs_inverse
        
        
// turn off the cacheable bit so we don't fill into the L1
// and all of the loads come directly from the L2
// TSIM doesn't properly model this, so if running on tsim just let the loads hit the L1
#ifndef STUB_FABRIC
        mfspr   R_TEMP_0, SPR_AER_1
        //------------------- Version block 5
#if __tile_chip__ == 1
         // Wachusett Version        
// On W, set no alloc L1 bit instead of turning off cacheable bit
        moveli  R_TEMP_1, lo16(0x08000000)
        auli    R_TEMP_1, R_TEMP_1, ha16(0x08000000)
        or      R_TEMP_2, R_TEMP_1, R_TEMP_0
#else
         // Tile64 Version
        moveli  R_TEMP_1, lo16(0xefffffff)
        auli    R_TEMP_1, R_TEMP_1, ha16(0xefffffff)
        and     R_TEMP_2, R_TEMP_1, R_TEMP_0
#endif
        //-------------------                
        mf
        mtspr   SPR_AER_1, R_TEMP_2
        drain  
#endif
        
// Check all other power of two addrs, should have original addr
// if one has an inverse, there is a sa1 fault on that addr bit line
        addi    R_L3_1, zero, 1
        addi    R_L3_3, zero, 2
        //------------------- Version Block 6
#if __tile_chip__ == 0
        // This block only in Tile64 
        addi    R_L3_5, zero, 15
        mf
        mfspr   R_TEMP_1, SPR_CBOX_CACHE_CONFIG
        drain
        // bit 1 indicates if we are in 32K mode or not
        shri    R_TEMP_1, R_TEMP_1, 1
        bbns    R_TEMP_1, check_addr_stuck_1        
#endif
        //-------------------        
        // 32K mode, one less index, one more tag bit        
        addi    R_L3_5, zero, 14
check_addr_stuck_1:
        lb      R_L3_8, R_L3_1        
        and     R_L3_8, R_L3_8, R_L3_7
        sne     R_TEMP_0, R_L3_8, R_L3_4
        bbnst   R_TEMP_0, continue_sa1
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x00ad2bad)        // ADdress path L2 BAD
        auli    R_L1_1, R_L1_1, ha16(0x00ad2bad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_4                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_8                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_sa1:
        add     R_L3_1, R_L3_2, R_L3_3
        rli     R_L3_3, R_L3_3, 1
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, check_addr_stuck_1


        addli   R_L3_6, zero, 0x3a00		// R_L3_6 = index to use
        //------------------- Version Block 7
#if __tile_chip__ == 0
        // This block only in Tile64 
        addi    R_L3_5, zero, 17
        mf
        mfspr   R_TEMP_1, SPR_CBOX_CACHE_CONFIG
        drain
        // bit 1 indicates if we are in 32K mode or not
        shri    R_TEMP_1, R_TEMP_1, 1
        bbns    R_TEMP_1, check_tag_sa1
#endif
        //-------------------        
        // 32K mode, one less index, one more tag bit
        addi    R_L3_5, zero, 18
check_tag_sa1:
        add     R_L3_1, R_L3_1, R_L3_6        
        lb      R_L3_8, R_L3_1        
        and     R_L3_8, R_L3_8, R_L3_7
        sne     R_TEMP_0, R_L3_8, R_L3_4
        bbnst   R_TEMP_0, continue_tag_sa1
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x00ad2bad)        // ADdress path L2 BAD
        auli    R_L1_1, R_L1_1, ha16(0x00ad2bad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_4                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_8                  // returned data
	jal     output_msg_1
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_tag_sa1:
        add     R_L3_1, R_L3_2, R_L3_3
        addi    R_L3_6, R_L3_6, 0x40		// go to next index for next tag
        rli     R_L3_3, R_L3_3, 1
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, check_tag_sa1

        // must now check tags with bits [35:32] set from SPR_AER_0
        addi    R_L3_5, zero, 1
check_aer_sa1:
        add     R_L3_1, zero, R_L3_6
        mf
        mtspr   SPR_AER_0, R_L3_5
        drain
        lb      R_L3_8, R_L3_1
        and     R_L3_8, R_L3_8, R_L3_7
        sne     R_TEMP_0, R_L3_8, R_L3_4
        bbnst   R_TEMP_0, continue_aer_sa1
        // if we fail, make sure top tag bits are zeroed out
        // need to reuse a register for this
        mfspr   R_L3_7, SPR_AER_0
        mf
        mtspr   SPR_AER_0, zero
        drain
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x00ad2bad)        // ADdress path L2 BAD
        auli    R_L1_1, R_L1_1, ha16(0x00ad2bad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_4                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_8                  // returned data
	jal     output_msg_1
        mf
        mtspr   SPR_AER_0, R_L3_7
        drain
        moveli  R_L3_7, lo16(0x000000ff)
        auli    R_L3_7, R_L3_7, ha16(0x000000ff)
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_aer_sa1:
        addi    R_L3_6, R_L3_6, 0x40		// go to next index for next tag
        rli     R_L3_5, R_L3_5, 1
        shri    R_TEMP_0, R_L3_5, 4
        bbnst   R_TEMP_0, check_aer_sa1



        // reset top tag bits in AER_0        
        mf
        mtspr   SPR_AER_0, zero
        drain
        mfspr   R_TEMP_0, SPR_AER_1

        //------------------- Version block 8
#if __tile_chip__ == 1
         // Wachusett Version              
        // Don't allocate into L1
        moveli  R_TEMP_1, lo16(0xf7ffffff)
        auli    R_TEMP_1, R_TEMP_1, ha16(0xf7ffffff)
        and     R_TEMP_2, R_TEMP_1, R_TEMP_0
#else
         // Tile64 Version
        // turn cacheable bit back on
        moveli  R_TEMP_1, lo16(0x10000000)
        auli    R_TEMP_1, R_TEMP_1, ha16(0x10000000)
        or      R_TEMP_2, R_TEMP_1, R_TEMP_0
#endif
        //-------------------        
        mf
        mtspr   SPR_AER_1, R_TEMP_2
        drain
        
// Write original pattern into base addrs
        sb      R_L3_2, R_L3_4
        addli   R_L3_6, zero, 0x3a00		// R_L3_6 = index to use
        //------------------- Version Block 9
#if __tile_chip__ == 0
        // This block only in Tile64 
        addi    R_L3_5, zero, 17
        mf
        mfspr   R_TEMP_1, SPR_CBOX_CACHE_CONFIG
        drain
        // bit 1 indicates if we are in 32K mode or not
        shri    R_TEMP_1, R_TEMP_1, 1
        bbns    R_TEMP_1, write_base_addrs_original        
#endif
        //-------------------        
        // 32K mode, one less index, one more tag bit        
        addi    R_L3_5, zero, 18
write_base_addrs_original:
        sb      R_L3_6, R_L3_4
        addi    R_L3_6, R_L3_6, 0x40		// go to next index for next tag
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_base_addrs_original

// Write inverse pattern into each power of two addr individually
// if base addr ends up with inverse pattern, there is a sa0 fault on that addr bit line
// if any other power of two addr has inverse pattern, we have some overlap fault
        addi    R_L3_3, zero, 1
        addi    R_L3_5, zero, 36
        addli   R_L3_6, zero, 0x3a00		// R_L3_6 = index to use
        add     R_L3_1, R_L3_2, R_L3_3
        // this is the base addr to check.  when we get into the tag bits, the base
        // addr is the index we are using for this tag bit.
        add     R_L3_10, R_L3_2, zero
check_addr_stuck_0:
        // turn cacheable bit back on
        mfspr   R_TEMP_0, SPR_AER_1
        
        //------------------- Version block 10
#if __tile_chip__ == 1
          // Wachusett Version              
        moveli  R_TEMP_1, lo16(0xf7ffffff)
        auli    R_TEMP_1, R_TEMP_1, ha16(0xf7ffffff)
        and     R_TEMP_2, R_TEMP_1, R_TEMP_0
#else
          // Tile64 Version
        moveli  R_TEMP_1, lo16(0x10000000)
        auli    R_TEMP_1, R_TEMP_1, ha16(0x10000000)
        or      R_TEMP_2, R_TEMP_1, R_TEMP_0
#endif
        //-------------------                
        mf
        mtspr   SPR_AER_1, R_TEMP_2
        drain
        
        xor     R_TEMP_0, R_L3_4, R_L3_7
        sb      R_L3_1, R_TEMP_0
// turn off the cacheable bit so we don't fill into the L1
// and all of the loads come directly from the L2
// TSIM doesn't properly model this, so if running on tsim just let the loads hit the L1
#ifndef STUB_FABRIC
        mfspr   R_TEMP_0, SPR_AER_1
        //------------------- Version block 11
#if __tile_chip__ == 1
          // Wachusett Version              
        moveli  R_TEMP_1, lo16(0x08000000)
        auli    R_TEMP_1, R_TEMP_1, ha16(0x08000000)
        or      R_TEMP_2, R_TEMP_1, R_TEMP_0
#else
          // Tile64 Version
        moveli  R_TEMP_1, lo16(0xefffffff)
        auli    R_TEMP_1, R_TEMP_1, ha16(0xefffffff)
        and     R_TEMP_2, R_TEMP_1, R_TEMP_0
#endif
        //-------------------        
        mf
        mtspr   SPR_AER_1, R_TEMP_2
        drain  
#endif        

        // reset top tag bits in AER_0 so we can check base addr
        // must save bits so we can restore after checking base
        mfspr   R_TEMP_0, SPR_AER_0
        mf
        mtspr   SPR_AER_0, zero
        drain
        
        // check base addr for sa0.  When checking tag bits, base addr is the index.
        lb      R_L3_8, R_L3_10
        // restore top bits of tag
        mf
        mtspr   SPR_AER_0, R_TEMP_0
        drain
        and     R_L3_8, R_L3_8, R_L3_7
        sne     R_TEMP_0, R_L3_8, R_L3_4
        bbnst   R_TEMP_0, continue_sa0
        // if we fail, make sure top tag bits are zeroed out
        // need to reuse a register for this
        mfspr   R_L3_7, SPR_AER_0
        mf
        mtspr   SPR_AER_0, zero
        drain
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x00ad2bad)        // ADdress path l2 BAD
        auli    R_L1_1, R_L1_1, ha16(0x00ad2bad)
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_1                  // bad addr
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_4                  // expected data
	jal     output_msg_1
        movei   R_L1_0, PASS_MSG
        move    R_L1_1, R_L3_8                  // returned data
	jal     output_msg_1
        mf
        mtspr   SPR_AER_0, R_L3_7
        drain
        moveli  R_L3_7, lo16(0x000000ff)
        auli    R_L3_7, R_L3_7, ha16(0x000000ff)
        movei   R_TEMP_0, stop_on_err_imm
        bnz     R_TEMP_0, done
continue_sa0:        
        // turn cacheable bit back on
        mfspr   R_TEMP_0, SPR_AER_1
        //------------------- Version block 12
#if __tile_chip__ == 1
          // Wachusett Version                      
        moveli  R_TEMP_1, lo16(0xf7ffffff)
        auli    R_TEMP_1, R_TEMP_1, ha16(0xf7ffffff)
        and     R_TEMP_2, R_TEMP_1, R_TEMP_0
#else
          // Tile64 Version
        moveli  R_TEMP_1, lo16(0x10000000)
        auli    R_TEMP_1, R_TEMP_1, ha16(0x10000000)
        or      R_TEMP_2, R_TEMP_1, R_TEMP_0        
#endif
        //-------------------        
        mf
        mtspr   SPR_AER_1, R_TEMP_2
        drain
        sb      R_L3_1, R_L3_4                  // store original pattern back into the current addr
        shli    R_L3_3, R_L3_3, 1
        add     R_L3_1, R_L3_2, R_L3_3
decrement_loop_sa0:
        addi    R_L3_5, R_L3_5, -1
        //------------------- Version Block 13
#if __tile_chip__ == 0
        // This block only in Tile64 
        addi    R_TEMP_0, R_L3_5, -21
        mf
        mfspr   R_TEMP_1, SPR_CBOX_CACHE_CONFIG
        drain
        // bit 1 indicates if we are in 32K mode or not
        shri    R_TEMP_1, R_TEMP_1, 1
        bbns    R_TEMP_1, continue_dec_loop_sa0
#endif
        //-------------------                
        // 32K mode, one less index, one more tag bit
        addi    R_TEMP_0, R_L3_5, -22
continue_dec_loop_sa0:
        bgz     R_TEMP_0, next_iteration
        addi    R_TEMP_0, R_L3_5, -4
        bgz     R_TEMP_0, next_tag_bit
        // must now use AER to set upper bits
        addi    R_TEMP_0, zero, 0x10
        shr     R_TEMP_1, R_TEMP_0, R_L3_5
        mf
        mtspr   SPR_AER_0, R_TEMP_1
        drain
next_tag_bit:
        move    R_L3_10, R_L3_6
        add     R_L3_1, R_L3_1, R_L3_6        
        addi    R_L3_6, R_L3_6, 0x40		// go to next index for next tag
        
next_iteration:
        bgzt    R_L3_5, check_addr_stuck_0

// R_L3_0 return value should have been set to 0 at start, and incremented if any failure occured
done:
        // reset top tag bits in AER_0        
        mf
        mtspr   SPR_AER_0, zero
        drain
        add     lr, zero, R_L3_9                // restore link register
        jrp     lr

boot_mem_test_3:
        { jrp lr; move R_L3_0, zero }
        
# Make sure this last address is 64-byte aligned so that the code above is padded to a full cache line
# so that when it is loaded into the L2 cache in cache-as-RAM mode, there are no parity errors                
.align 64
_last_addr:             
