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

        // Use way 1 in L2 to avoid istream conflicts
        addi    R_TEMP_0, zero, 0x3
        mf
        mtspr   SPR_CBOX_CACHEASRAM_CONFIG, R_TEMP_0
        drain
        
// Walking 1's for entire datapath to L1
// Walking 1's will catch any datapath sa1, sa0, short errors.  It does this by writing a 1 to each bit
// in the dpath individually, and then immediately reading it back.
// sa1 -- caught because instead of reading back just the 1 we set, the sa1 bit will be set
// sa0 -- caught because when we try to write that bit to 1, we will read back 0
// short -- two bits will be set to 1 instead of just the one bit we wanted to set
// open -- will read back 0 instead of 1
// The l1 is physically arranged as 2 banks, each bank is 8B wide * 512 lines deep = 4K * 2 banks = 8K L2.
// Only one bank is written at a time, so the L2 datapath is 8B wide.
// We set each bit of the dpath to 1 individually - first by setting each bit of a sw to 1,
// then by doing 2 sw to consecutive word addresses.  We then do repeat this for an address on each bank.
//
// addr = 0;
// data = 1;
// for (word_in_dpath = 2; word_in_dpath > 0; word_in_dpath--) {   
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

        // First, bring a block into the L1, both ways to test path to both banks
        moveli  R_L3_1, lo16(0x00002000)               // R_L3_1 = addr
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        sw      R_L3_1, zero
        lw      R_TEMP_0, R_L3_1
        addli   R_L3_1, R_L3_1, 0x1000
        sw      R_L3_1, zero
        lw      R_TEMP_0, R_L3_1
        nop						// give lw time to fill L1
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        
        
        moveli  R_L3_1, lo16(0x00002000)        // R_L3_1 = addr
        auli    R_L3_1, R_L3_1, ha16(0x00002000)
        addi    R_L3_2, zero, 1                 // R_L3_2 = data
        addi    R_L3_6, zero, 2			// loop counter for bank in L1

walk_l1_bank:
        addli   R_L3_3, zero, 4                 // R_L3_3 = loop counter for word in L1 dpath, 2 words * 4B = 8B L2 dpath width
walk_l1_block:
        addli   R_L3_4, zero, 32                // R_L3_4 = loop counter for bit in word, 32 bits = 4B in a word
walk_word:         
        sw      R_L3_1, R_L3_2
        nop						// give sw time to update L1
        nop
        nop
        nop
        lw      R_L3_5, R_L3_1
        sne     R_TEMP_0, R_L3_2, R_L3_5
        bbnst   R_TEMP_0, continue_data_walk_1s
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x00da1bad)        // DAta path l1 BAD
        auli    R_L1_1, R_L1_1, ha16(0x00da1bad)
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
continue_data_walk_1s:
        rli     R_L3_2, R_L3_2, 1               // rotate register to set next bit
        addi    R_L3_4, R_L3_4, -1
        bgzt    R_L3_4, walk_word

        addi    R_L3_1, R_L3_1, 4
        addi    R_L3_3, R_L3_3, -1
        bgzt    R_L3_3, walk_l1_block
        
        moveli  R_L3_1, lo16(0x00003000)        // R_L3_1 = addr
        auli    R_L3_1, R_L3_1, ha16(0x00003000)
        addi    R_L3_6, R_L3_6, -1
        bgzt    R_L3_6, walk_l1_bank



// Walking 1's for address bus to L2
// First, we take a base address, and one by one write a pattern (0xaa) to every
// power of two offset from the base.  eg, 0x80000000, 0x80000001, 0x80000002,
// 0x80000004, 0x80000008, 0x80000010, etc.  Then we write the inverse of the pattern (0x55)
// to the base addr.  Next, check that every other addr still contains the original
// pattern -- if any addr bit is stuck at 1, it will contain the inverse as well, and
// we should detect this error.
// Next, we want to detect stuck at 0's.  To do this, we write the original pattern (0xaa)
// into the base addr, so all power of two addrs contain the pattern.  Then, for each
// addr we write the inverse, and do the following:
// 1) check the base addr -- if it is the inverse, we know the addr bit that is stuck at 0
//    when checking the tag bits, we have to set another index bit (so we don't evict istream code at index 0)
//    so checking that particular power of two addr is also required for sa0 faults
//    between two of the addr lines.

// R_L3_1 = addr we are testing
// R_L3_2 = base addr (I now think this breaks if base is non-zero...  review code)
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
        addi    R_L3_5, zero, 13

// So we don't mess with the boot code, just set the highest tag bit in the AER register.
// Since this is used for all addrs, should be ok until we get to testing the AER bits.
// At that point, we change the index anyway, so we'll be ok.
// However we will be checking against addr 0x0, so we still need to write the base pattern
// there with the AER register set to 0.                                
        sb      R_L3_1, R_L3_4
        nop
        nop
        nop
        lb      R_TEMP_0, R_L3_1                // ld to fill the L1
        mf
        addi    R_TEMP_0, zero, 8
        mtspr   SPR_AER_0, R_TEMP_0
        drain
                        
// First, write a predefined pattern (0xaa) to every power of two offset
// When we hit bit 12 (first tag bit), must use a different index for each following tag bit.
// this assumes the index bits have no problems, but we are testing those individually anyway
// must also write the "base" addr with the index bits set for the sa0 checks on tag bits
write_addr_default:
        sb      R_L3_1, R_L3_4
        nop
        nop
        nop
        lb      R_TEMP_0, R_L3_1                // ld to fill the L1
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        add     R_L3_1, R_L3_2, R_L3_3
        shli    R_L3_3, R_L3_3, 1
        // decrement loop counter
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_addr_default

        addi    R_L3_5, zero, 20
        addli   R_L3_6, zero, 0x810		// R_L3_6 = index to use
write_tag_bits:
        add     R_L3_1, R_L3_1, R_L3_6
        // first write the base addr (index bits set) for the tag bit in question
        // since index is incremented sequentially, L2 CRAM st miss bug not an issue
        sb      R_L3_6, R_L3_4
        nop
        nop
        nop
        lb      R_TEMP_0, R_L3_6
        // to avoid CRAM st miss to later 48B bug, must first write to first 16B of L2 cache line
        moveli  R_TEMP_0, lo16(0xffffffc0)
        auli    R_TEMP_0, R_TEMP_0, ha16(0xffffffc0)
        and     R_TEMP_1, R_TEMP_0, R_L3_1
        sw      R_TEMP_1, zero
        sb      R_L3_1, R_L3_4
        nop
        nop
        nop
        lb      R_TEMP_0, R_L3_1                // ld to fill the L1
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        add     R_L3_1, R_L3_2, R_L3_3
        addi    R_L3_6, R_L3_6, 0x10		// go to next index for next tag
        rli     R_L3_3, R_L3_3, 1
        // decrement loop counter
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_tag_bits

        // must now write tags with bits [35:32] set from SPR_AER_0
        addi    R_L3_5, zero, 1
write_aer_bits:
        add     R_L3_1, zero, R_L3_6
        // first write the base addr (index bits set) for the tag bit in question
        mf
        addi    R_TEMP_0, zero, 8
        mtspr   SPR_AER_0, R_TEMP_0
        drain
        // to avoid CRAM st miss to later 48B bug, must first write to first 16B of L2 cache line
        moveli  R_TEMP_0, lo16(0xffffffc0)
        auli    R_TEMP_0, R_TEMP_0, ha16(0xffffffc0)
        and     R_TEMP_1, R_TEMP_0, R_L3_6
        sw      R_TEMP_1, zero
        sb      R_L3_6, R_L3_4
        nop
        nop
        lb      R_TEMP_0, R_L3_6                // ld to fill the L1
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        mf
        mtspr   SPR_AER_0, R_L3_5
        drain
        // to avoid CRAM st miss to later 48B bug, must first write to first 16B of L2 cache line
        moveli  R_TEMP_0, lo16(0xffffffc0)
        auli    R_TEMP_0, R_TEMP_0, ha16(0xffffffc0)
        and     R_TEMP_1, R_TEMP_0, R_L3_1
        sw      R_TEMP_1, zero
        sb      R_L3_1, R_L3_4
        nop
        nop
        nop
        lb      R_TEMP_0, R_L3_1                // ld to fill the L1
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        addi    R_L3_6, R_L3_6, 0x10		// go to next index for next tag
        rli     R_L3_5, R_L3_5, 1
        shri    R_TEMP_0, R_L3_5, 3
        bbnst   R_TEMP_0, write_aer_bits
        
        // reset top tag bits in AER_0        
        mf        
        addi    R_TEMP_0, zero, 8
        mtspr   SPR_AER_0, R_TEMP_0
        drain


// From now on all lds/sts should hit the L1
// Write the inverse pattern into all base addrs
        moveli  R_L3_7, lo16(0x000000ff)
        auli    R_L3_7, R_L3_7, ha16(0x000000ff)
        xor     R_TEMP_0, R_L3_4, R_L3_7
        sb      R_L3_2, R_TEMP_0
        nop
        nop
        
        addi    R_L3_5, zero, 24
        addli   R_L3_6, zero, 0x810		// R_L3_6 = index to use
write_inverse_base_addrs:
        sb      R_L3_6, R_TEMP_0
        nop
        nop
        addi    R_L3_6, R_L3_6, 0x10
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_inverse_base_addrs
        
// Check all other power of two addrs, should have original addr
// if one has an inverse, there is a sa1 fault on that addr bit line
        addi    R_L3_1, zero, 1
        addi    R_L3_3, zero, 2
        addi    R_L3_5, zero, 12
check_addr_stuck_1:
        lb      R_L3_8, R_L3_1        
        and     R_L3_8, R_L3_8, R_L3_7
        sne     R_TEMP_0, R_L3_8, R_L3_4
        bbnst   R_TEMP_0, continue_sa1
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0ad1bad1)        // ADdress path L1 BAD checkpoint 1
        auli    R_L1_1, R_L1_1, ha16(0x0ad1bad1)
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

        addi    R_L3_5, zero, 20
        addli   R_L3_6, zero, 0x810		// R_L3_6 = index to use
check_tag_sa1:
        add     R_L3_1, R_L3_1, R_L3_6        
        lb      R_L3_8, R_L3_1        
        and     R_L3_8, R_L3_8, R_L3_7
        sne     R_TEMP_0, R_L3_8, R_L3_4
        bbnst   R_TEMP_0, continue_tag_sa1
        addi    R_L3_0, R_L3_0, 1		// increment failure count
        bbs	R_L3_11, done
        movei	R_L1_0, PASS_MSG
        moveli  R_L1_1, lo16(0x0ad1bad2)        // ADdress path L1 BAD checkpoint 2
        auli    R_L1_1, R_L1_1, ha16(0x0ad1bad2)
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
        addi    R_L3_6, R_L3_6, 0x10		// go to next index for next tag
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
        moveli  R_L1_1, lo16(0x0ad1bad3)        // ADdress path L1 BAD checkpoint 3
        auli    R_L1_1, R_L1_1, ha16(0x0ad1bad3)
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
        addi    R_L3_6, R_L3_6, 0x10		// go to next index for next tag
        rli     R_L3_5, R_L3_5, 1
        shri    R_TEMP_0, R_L3_5, 3
        bbnst   R_TEMP_0, check_aer_sa1



        // reset top tag bits in AER_0        
        mf
        addi    R_TEMP_0, zero, 8
        mtspr   SPR_AER_0, R_TEMP_0
        drain
                        
        
// Write original pattern into all base addrs
        sb      R_L3_2, R_L3_4
        nop
        nop
        
        addi    R_L3_5, zero, 23
        addli   R_L3_6, zero, 0x810		// R_L3_6 = index to use
write_original_base_addrs:
        sb      R_L3_6, R_L3_4
        nop
        nop
        addi    R_L3_6, R_L3_6, 0x10
        addi    R_L3_5, R_L3_5, -1
        bgzt    R_L3_5, write_original_base_addrs

// Write inverse pattern into each power of two addr individually
// if base addr ends up with inverse pattern, there is a sa0 fault on that addr bit line
// if any other power of two addr has inverse pattern, we have some overlap fault
        addi    R_L3_3, zero, 1
        addi    R_L3_5, zero, 35
        add     R_L3_6, zero, zero              // base address to use (also index counter)
        add     R_L3_1, R_L3_2, R_L3_3
check_addr_stuck_0:
        xor     R_TEMP_0, R_L3_4, R_L3_7
        sb      R_L3_1, R_TEMP_0        
        nop
        nop

        // check base addr for sa0
        mf
        mfspr   R_TEMP_0, SPR_AER_0
        addi    R_TEMP_1, zero, 8
        mtspr   SPR_AER_0, R_TEMP_1
        drain
        lb      R_L3_8, R_L3_6
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
        moveli  R_L1_1, lo16(0x0ad1bad4)        // ADdress path l1 BAD checkpoint 4
        auli    R_L1_1, R_L1_1, ha16(0x0ad1bad4)
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
        sb      R_L3_1, R_L3_4                  // store original pattern back into the current addr
        shli    R_L3_3, R_L3_3, 1
        add     R_L3_1, R_L3_2, R_L3_3
decrement_loop_sa0:
        addi    R_L3_5, R_L3_5, -1
        addi    R_TEMP_0, R_L3_5, -23
        bgz     R_TEMP_0, next_iteration
        addi    R_TEMP_0, R_L3_5, -3
        bgz     R_TEMP_0, next_tag_bit
        // must now use AER to set upper bits
        addi    R_TEMP_0, zero, 0x08
        shr     R_TEMP_1, R_TEMP_0, R_L3_5
        mf
        mtspr   SPR_AER_0, R_TEMP_1
        drain
next_tag_bit:
        // set base addr for this tag bit
        shri    R_TEMP_0, R_L3_1, 12
        bbnst   R_TEMP_0, not_bit_12
        addli   R_L3_6, zero, 0x800		// R_L3_6 = index to use        
not_bit_12:     
        addi    R_L3_6, R_L3_6, 0x10		// go to next index for next tag
        add     R_L3_1, R_L3_1, R_L3_6        
        
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
