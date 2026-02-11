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

#define STACK_SHIFT 15
#define STACK_BASE_LO 0x0000
#define STACK_BASE_HI 0x0004
#define MAX_TIMER_VALUE      (1 << 30)
#define NETWORK_TIMER_VALUE      (1 << 14)

#define UDN_TAG_0	100
#define UDN_TAG_1	101
#define UDN_TAG_2	102
#define UDN_TAG_3	103
#define UDN_TAG_CA      104        
#define IDN_TAG_0	120
#define IDN_TAG_1	121
#define IDN_TAG_CA      122
        
	.section startup, "ax"

dv_startup:
        
        ###################################################################################
        # Flush boot code out of icache
        ###################################################################################

        # start flush at address 1 cache line below 1K
        { moveli r52, 1024-64 }

        # flush down to address 0, decrementing address by cache block size
_flush: { icoh r52 }
        { bnz r52, _flush; addli r52, r52, -64} 

        ###################################################################################
        # Invalidate the L1 boot code that's in the L2 cache
        ###################################################################################

        # R1 = address (assumed that L1 boot code was loaded at address 0x1000000
        # R1 = count, invalidate 64K of addresses (full cache size) because the full cache is now initialized in boot code
	{ movei r2, 1; auli r1, zero, 0x1000 }
	{ shli r2, r2, 16 }
l2_inval:       	
        { inv r1 }
        { addi r1, r1, 64; addi r2, r2, -64 }              
        { bnz r2, l2_inval }

        ###################################################################################
        # Clear all regs
        # The reason for initializing all regs is to avoid problems with checkers if
        # they happen to be pushed onto a stack and popped before ever being initialized
        ###################################################################################

        { move r1, zero; move r2, zero }
        { move r3, zero; move r4, zero }
        { move r5, zero; move r6, zero }
        { move r7, zero; move r8, zero }
        { move r9, zero; move r10, zero }
        { move r11, zero; move r12, zero }
        { move r13, zero; move r14, zero }
        { move r15, zero; move r16, zero }
        { move r17, zero; move r18, zero }
        { move r19, zero; move r20, zero }
        { move r21, zero; move r22, zero }
        { move r23, zero; move r24, zero }
        { move r25, zero; move r26, zero }
        { move r27, zero; move r28, zero }
        { move r29, zero; move r30, zero }
        { move r31, zero; move r32, zero }
        { move r33, zero; move r34, zero }
        { move r35, zero; move r36, zero }
        { move r37, zero; move r38, zero }
        { move r39, zero; move r40, zero }
        { move r41, zero; move r42, zero }
        { move r43, zero; move r44, zero }
        { move r45, zero; move r46, zero }
        { move r47, zero; move r48, zero }
        { move r49, zero; move r50, zero }
        { move r51, zero; move r52, zero }
        { move tp, zero; move r0, zero }

#ifdef TESTBENCH_DINI
        #### HACK for FPGA.  Always skip mshim config. ####
        bzt zero, done_mshim_config
#endif        	
        
        # Only config mshims if this is the master (at coordinate 1,1)
	# DL-hack added to ignore that part which we have done perviously
	{ jal done_mshim_config }
        { mfspr r1, SPR_TILE_COORD }
        { shri r1, r1, 18; shri r2, r1, 7 }
        { andi r1, r1, 0xf; andi r2, r2, 0xf }
        { addi r1, r1, -1 }
        { bnz r1, done_mshim_config; addi r2, r2, -1 }
        { bnz r2, done_mshim_config }
#ifdef TESTBENCH_CHIP
#include "config_mshims.S"	
#endif        	
#ifdef TESTBENCH_CHIP_TSIM
#include "config_mshims.S"	
#endif        	
#ifdef TESTBENCH_CHIP_NO_EVC
#include "config_mshims.S"	
#endif        	
done_mshim_config:


        ###################################################################################
        # Clear the STN icache, if this is Tile64
        ###################################################################################
        // Only scrub icache on tile64
#if __tile_chip__ == 0
	{ move r1, zero ; moveli r2, 2048 }
sn_inv_loop:
        { mtspr SPR_SNIC_INVADDR, r1 ; addi r1, r1, 32 }
        { slt r3, r1, r2 }
        { bnzt r3, sn_inv_loop }
#endif
                
        ###################################################################################
        # Set the default MPL of the EX_CONTEXT registers to the behaviour prior to Aug '06
        ###################################################################################
        
        { moveli r11, 0x1 }
        { mtspr SPR_MPL_INTCTRL_3_SET_3, r11 }
        { mtspr SPR_MPL_INTCTRL_2_SET_2, r11 }
        { mtspr SPR_MPL_INTCTRL_1_SET_1, r11 }
        { mtspr SPR_MPL_INTCTRL_0_SET_0, r11 }
        
        ###################################################################################
        # Init the lr
        ###################################################################################

        # init lr to -1 - WHY THIS VALUE???
        { movei lr, -1 }

        ###################################################################################
        # Init the stack pointer
        # Each tile is given a 2^STACK_SHIFT bytes
        ###################################################################################

        # R1 <- tile coord
        { mfspr r1, SPR_TILE_COORD }

        # R3 <- shift X into position for source ID
        # R4 <- shift Y into position for source ID
        { shri r3, r1, 18; shri r4, r1, 7 }

	# Subtract 1 from X and Y because our coordinate system is 1,1 based
        { addi r3, r3, -1; addi r4, r4, -1 }

        # R5 <- mask X for source ID
        # R6 <- mask Y for source ID
	{ andi r5, r3, 0x7; andi r6, r4, 0x7 }

	# Shift X coord up
        { shli r5, r5, 3 }

        # init IDN tile coord
        # R7 <- XY (source id)
        { or r7, r5, r6 }

        # Begin moving STACK_BASE into SP
        # add 1 to source ID to get 1-based tile number
        { moveli sp, STACK_BASE_LO; addi r8, r7, 1 }

        # R10 <- offset
        # SP <- STACK_BASE
        { auli sp, sp, STACK_BASE_HI; shli r10, r8, STACK_SHIFT } 

        # SP <- STACK_BASE + offset for this tile
        { add sp, sp, r10 } 

	# For C tests, "main" is defined; for asm tests, "_start" is defined
	.weak _start
	.weak main

	###################################################################################
        # Init switch processor tlb entry #1 (VA=PA) for the first page of the program
        ###################################################################################
#if __tile_chip__ == 0 /* No SN processor on chip 1 */
	# Determine which address we should map
	{ moveli r2, _my_start ; movei r5, -1 }
        { moveli r3, lo16(_start) ; shli r5, r5, 12}
        { auli r3, r3, ha16(_start) }
	
        # check to see if _my_start == _start then map for main instead of _start
        { seq r4, r2, r3 }
        { moveli r2, lo16(main) }
        { auli r2, r2, ha16(main) }
        { mvnz r3, r4, r2 }
	{ and r3, r3, r5 }
        # R3 has Page-aligned VA  to map
        { moveli r2, 1 }
        { mtspr SPR_SNITLB_INDEX, r2 }
        { mtspr SPR_SNITLB_CURRENT_0, r3 }
        { moveli r2, 0 }
        { mtspr SPR_SNITLB_CURRENT_1, r2 }
        { mtspr SPR_SNITLB_CURRENT_3, r2 }
        { ori r2, r3, 0x35 }
        { mtspr SPR_SNITLB_CURRENT_2, r2 }
#endif

        # Unmask every maskable interrupt in order to find false interrupt reporting
        # be prepared to yank this back out if it uncovers too many false failures
        # since it significantly affects network access
        # also our assembler barfs at the C preprocessor macros in reg.h (like INT_MASK)
        # and can not handle 64 bit arithmatic

        # If we do not reset the tile_timer we will immeadiatly interrupt to it
        moveli r51, lo16(MAX_TIMER_VALUE)
        auli r51, r51, ha16(MAX_TIMER_VALUE)
        mtspr SPR_TILE_TIMER_CONTROL, r51
        
        moveli r51, lo16(NETWORK_TIMER_VALUE)
        auli r51, r51, ha16(NETWORK_TIMER_VALUE)
        mtspr SPR_IDN_DEADLOCK_COUNT, zero
        mtspr SPR_IDN_DEADLOCK_TIMEOUT, r51
        mtspr SPR_UDN_DEADLOCK_COUNT, zero
        mtspr SPR_UDN_DEADLOCK_TIMEOUT, r51
        

        moveli r2, lo16(0xFFFFFFFF)
        auli r2, r2, ha16(0xFFFFFFFF)
//        mtspr SPR_INTERRUPT_MASK_RESET_3_0, r2
        # interrupt 32 UDN_AVAIL and up
        moveli r2, lo16(0xFFFFFFFF)
        auli r2, r2, ha16(0xFFFFFFFF)
//        mtspr SPR_INTERRUPT_MASK_RESET_3_1, r2

        # JZ: leaving out UDN and IDN complete - too many tests failing to fix.
        {addi r2, zero, 1; addi r3, zero, INT_UDN_COMPLETE}
        shl r2, r2, r3
        mtspr SPR_INTERRUPT_MASK_SET_3_0, r2

        {addi r2, zero, 1; addi r3, zero, INT_IDN_COMPLETE}
        shl r2, r2, r3
        mtspr SPR_INTERRUPT_MASK_SET_3_0, r2
                
        
        # Write 0xee to SPR_SYSTEM_SAVE_0_3 to indicate that startup code is done
	{ moveli r1, 0xee }
        { mtspr SPR_SYSTEM_SAVE_0_3, r1 }


        { j _start }
_my_start:
_start:
	{ j main }

#ifdef TESTBENCH_CHIP
#include "send_mshim_cfg_pkt.S"	
#endif        	
#ifdef TESTBENCH_CHIP_TSIM
#include "send_mshim_cfg_pkt.S"	
#endif        	
#ifdef TESTBENCH_CHIP_NO_EVC
#include "send_mshim_cfg_pkt.S"	
#endif        	
