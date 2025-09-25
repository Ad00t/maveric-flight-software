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

//#include "itc_chip_bringup_defs_mike.h"
//#include "itc_chip_bringup_l1boot.S"
//#include "itc_chip_bringup_utils_mike.S"
        
#include "itc_chip_bringup_defs.h"
#include "itc_chip_bringup_l1boot.S"
#include "itc_chip_bringup_utils.S"
        
###################################################
# boot_test
#
# On exit:                      
# R_L3_0 = failure count, 0 = test passed
###################################################
        
# Must be multiple of 4 (bytes)	
//#define LONG_TEST_SIZE (128*1024*1024)
#define LONG_TEST_SIZE (32*1024*1024)
//#define LONG_TEST_SIZE (1024)
//#define LONG_TEST_SIZE (32)

#define SHORT_TEST_SIZE (32)
#define PRINT_MASK (0xffffff)
// #define MAX_ERRORS 128	
#define MAX_ERRORS 128
         
boot_test_3:
        { jrp lr; move R_L3_0, zero }              
        
###################################################
# boot_mem_test
#
# On exit:                      
# R_L3_0 = failure count, 0 = test passed
###################################################
        
boot_mem_test_3:            	

        # save link reg

        { move R_L3_4, lr }


        # Disable cache as RAM mode so we're talking to memory, not the L2 cache
	{ mf }
        { mtspr	SPR_CBOX_CACHEASRAM_CONFIG, zero }
        
        # disable caching so that blocks aren't cached after the store to the first word occurs
        { mtspr	AER_1, zero }

        
 
boot_mem_test_run_it:
	
        # Disable cache as RAM mode so we're talking to memory, not the L2 cache
	{ mf }
        { mtspr	SPR_CBOX_CACHEASRAM_CONFIG, zero }
        
        # disable caching so that blocks aren't cached after the store to the first word occurs
        { mtspr	AER_1, zero }
        
        { mf } # copied from hypervisor L1 boot code - what's the point of this?
        { drain } # copied from hypervisor L1 boot code - what's the point of this?

	# Run extensive test if this is boot master
        { xor R_TEMP_0, R_MY_X, R_MASTER_X }
        { bnz R_TEMP_0, boot_mem_test_not_master; xor R_TEMP_0, R_MY_Y, R_MASTER_Y }
        { bnz R_TEMP_0, boot_mem_test_not_master }

        # Set up start address
        { move R_L2_0, zero }
        
        #-------------------------------mem test 1 -----------------------------------
        #data = address
        #-------------------------------mem test 1 -----------------------------------
        { movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xafe } # address
	{ jal output_msg_1 }
        { movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xad} # address
	{ jal output_msg_1 }
        # Set up size
        { moveli R_L2_1, lo16(LONG_TEST_SIZE) }
        { auli R_L2_1, R_L2_1, ha16(LONG_TEST_SIZE) }
        
        # Run data equals address test

        { move R_L2_2, zero }
        { jal dae_test_write_2 }
        { jal dae_test_read_check_2 }

        { move R_L3_0, R_L2_3 }


        #-------------------------------mem test 2 -----------------------------------
        #data = !address
        #-------------------------------mem test 2 -----------------------------------
        { movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xafe } # address
	{ jal output_msg_1 }
        { movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xbad } # address
	{ jal output_msg_1 }

        
        # Reload size parameter because it gets blown away by the routine
        { moveli R_L2_1, lo16(LONG_TEST_SIZE) }
        { auli R_L2_1, R_L2_1, ha16(LONG_TEST_SIZE) }

        { moveli R_L2_2, 0xffffffff }
        { jal dae_test_write_2 }
        { jal dae_test_read_check_2 }
        { add R_L3_0, R_L3_0, R_L2_3 }


        #-------------------------------mem test 3 -----------------------------------
        #data = 0x55555555
        #-------------------------------mem test 3 -----------------------------------
        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xafe } # address
	#{ jal output_msg_1 }
        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0x5 } # address
	#{ jal output_msg_1 }


        
        # Reload size parameter because it gets blown away by the routine
        #{ moveli R_L2_1, lo16(LONG_TEST_SIZE) }
        #{ auli R_L2_1, R_L2_1, ha16(LONG_TEST_SIZE) }

        #{ moveli R_L2_2, 0x00000005 }
        #{ jal duncan_mem_test_2 }
        #{ jal duncan_read_mem_test_2 }
        #{ add R_L3_0, R_L3_0, R_L2_3 }

        #-------------------------------mem test 4 -----------------------------------
        #data = 0xaaaaaaaa
        #-------------------------------mem test 4 -----------------------------------
        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xafe } # address
	#{ jal output_msg_1 }
        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xa } # address
	#{ jal output_msg_1 }


        
        # Reload size parameter because it gets blown away by the routine
        #{ moveli R_L2_1, lo16(LONG_TEST_SIZE) }
        #{ auli R_L2_1, R_L2_1, ha16(LONG_TEST_SIZE) }

        #{ moveli R_L2_2, 0x0000000a }
        #{ jal duncan_mem_test_2 }
        #{ jal duncan_read_mem_test_2 }
        #{ add R_L3_0, R_L3_0, R_L2_3 }

        #-------------------------------mem test 5 -----------------------------------
        #data = 0x00000000
        #-------------------------------mem test 5 -----------------------------------
        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xafe } # address
	#{ jal output_msg_1 }
        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0x0 } # address
	#{ jal output_msg_1 }


        
                
        # Reload size parameter because it gets blown away by the routine
        #{ moveli R_L2_1, lo16(LONG_TEST_SIZE) }
        #{ auli R_L2_1, R_L2_1, ha16(LONG_TEST_SIZE) }

        #{ moveli R_L2_2, 0x00000000 }
        #{ jal duncan_mem_test_2 }
        #{ jal duncan_read_mem_test_2 }
        #{ add R_L3_0, R_L3_0, R_L2_3 }

        #-------------------------------mem test 6 -----------------------------------
        #data = 0x0000000f
        #-------------------------------mem test 6 -----------------------------------
        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xafe } # address
	#{ jal output_msg_1 }
        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xf } # address
	#{ jal output_msg_1 }

        
        # Reload size parameter because it gets blown away by the routine
        #{ moveli R_L2_1, lo16(LONG_TEST_SIZE) }
        #{ auli R_L2_1, R_L2_1, ha16(LONG_TEST_SIZE) }

        #{ moveli R_L2_2, 0x0000000f }
        #{ jal duncan_mem_test_2 }
        #{ jal duncan_read_mem_test_2 }
        #{ add R_L3_0, R_L3_0, R_L2_3 }

        #{ jal itc_mem_end_m }


        #
	# Now, write data to memory that the other tiles will read and check
        # This makes sure that we're really writing to external memory (and not just our own cache)
        #
        
        # Set up start address
	{ move R_L0_0, R_MASTER_X; move R_L0_1, R_MASTER_Y }
        { jal calc_src_id_0 }
        { shli R_L2_0, R_L0_2, 16 }
        
        # Set up sizeGot Boot Message: Pass .  Sender X Y = 1 4 , Data = 0x24 ( 36 )

        { moveli R_L2_1, lo16(SHORT_TEST_SIZE) }
        { auli R_L2_1, R_L2_1, ha16(SHORT_TEST_SIZE) }

        # Run data equals address test
        { move R_L2_2, zero }
        { jal dae_test_write_2 }
        
	# make sure those stores are in memory
        { mf }
	
itc_mem_end_m :
        # return
        { jrp R_L3_4 }
        
        #################################################R##

boot_mem_test_not_master:  
	#<DL-hack debug stuff>             	
	{ j itc_mem_end_s }
	#<DL-hack debug stuff>
        #
	# Read and check the data set up by the boot master
        #
        
        # Start address is the master's base address
	{ move R_L0_0, R_MASTER_X; move R_L0_1, R_MASTER_Y }
        { jal calc_src_id_0 }
        { shli R_L2_0, R_L0_2, 16 }
        
        # Set up size
        { moveli R_L2_1, lo16(SHORT_TEST_SIZE) }
        { auli R_L2_1, R_L2_1, ha16(SHORT_TEST_SIZE) }

        # Run data equals address test
        { move R_L2_2, zero }
        { jal dae_test_read_check_2 }
        { move R_L3_0, R_L2_3 }

        #
	# Now write and read my own space
        #
        
        # Set up start address - every tile uses a different space based on its source ID
	{ move R_L0_0, R_MY_X; move R_L0_1, R_MY_Y }
        { jal calc_src_id_0 }
        { shli R_L2_0, R_L0_2, 16 }
        
        # Set up size
        { moveli R_L2_1, lo16(SHORT_TEST_SIZE) }
        { auli R_L2_1, R_L2_1, ha16(SHORT_TEST_SIZE) }

        # Run data equals address test
        { move R_L2_2, zero }
        { jal dae_test_write_2 }
        { jal dae_test_read_check_2 }
        { add R_L3_0, R_L3_0, R_L2_3 }

itc_mem_end_s :
        # return
        { jrp R_L3_4 }

        
                
###################################################
# dae_test_write - write data for data equals address test
#
# On entry:     
# R_L2_0 = start address
# R_L2_1 = size in bytes <=== NOTE - this value is destroyed
# R_L2_2 = xor value
###################################################
        
dae_test_write_2:       	
        # save link reg
        { move R_L2_5, lr }
        
	#
        # Store loop
        #

        { mtspr SPR_PASS, R_L2_0 }
        { mtspr SPR_PASS, R_L2_1 }
        { mtspr SPR_PASS, R_L2_2 }
        
	#
        # Store loop
        #

        # init current address register        	
        { move R_TEMP_1, R_L2_0 }

	# init size register
        { move R_TEMP_2, R_L2_1 }

        # Store data = address
        #{ jal stupid_hard_code_write }
        { move R_TEMP_3, zero }
store_loop:     	
        { mtspr SPR_PASS, R_TEMP_1 }

        # Calculate the write data
        { xor R_TEMP_0, R_TEMP_1, R_L2_2 }

        # Store the data and increment the address


       
        #dl-hack, write to every addresses with the same value
        #pattern 1
	#{ moveli R_TEMP_3,        lo16(0x00000000) }
	#{ auli R_TEMP_3,R_TEMP_3, ha16(0x00000000) }
        #pattern 2
	#{ moveli R_TEMP_3,        lo16(0xabcdabcd) }
	#{ auli R_TEMP_3,R_TEMP_3, ha16(0xabcdabcd) }
        #pattern 3
	#{ moveli R_TEMP_3,        lo16(0xbeefcafe) }
	#{ auli R_TEMP_3,R_TEMP_3, ha16(0xbeefcafe) }
        #pattern 4
	#{ moveli R_TEMP_3,        lo16(0x51515151) }
	#{ auli R_TEMP_3,R_TEMP_3, ha16(0x51515151) }
        #pattern 5
	#{ moveli R_TEMP_3,        lo16(0xc0c0c0c0) }
	#{ auli R_TEMP_3,R_TEMP_3, ha16(0xc0c0c0c0) }
        #pattern 6
	#{ moveli R_TEMP_3,        lo16(0x5a5a5a5a) }
	#{ auli R_TEMP_3,R_TEMP_3, ha16(0x5a5a5a5a) }
        #pattern 7, it's beef and addr
	#{ moveli R_TEMP_3,        lo16(0xbeef0000) }
	#{ auli R_TEMP_3,R_TEMP_3, ha16(0xbeef0000) }
        #{ or R_TEMP_3, R_TEMP_3, R_TEMP_1 }
        
        #pattern 7, it's beef and addr
	#{ moveli R_TEMP_3,        lo16(0x00000000) }
	#{ auli R_TEMP_3,R_TEMP_3, ha16(0x00000000) }
        #{ or R_TEMP_3, R_TEMP_3, R_TEMP_1 }
        
	
        #{ sw R_TEMP_1, R_TEMP_3; addi R_TEMP_1, R_TEMP_1, 4} 
        { sw R_TEMP_1, R_TEMP_0; addi R_TEMP_1, R_TEMP_1, 4} 
        # check to see if we've reached the end value for the loop  
	{ addi R_TEMP_2, R_TEMP_2, -4 }    

        #dl-hack, write to every addresses with the same value
                  
        { bgzt R_TEMP_2, store_loop }

        # Return	
        { jrp R_L2_5 }



###################################################
# dae_test_read_check - read and check for data equals address test
#
# On entry:     
# R_L2_0 = start address
# R_L2_1 = size in bytes <=== NOTE - this value is destroyed
# R_L2_2 = xor value
#
# On exit:      
# R_L2_3 = fail count                        
###################################################
        
dae_test_read_check_2:     
        # save link reg
        { move R_L2_5, lr }

        { mtspr SPR_PASS, R_L2_0 }
        { mtspr SPR_PASS, R_L2_1 }
        { mtspr SPR_PASS, R_L2_2 }

	#
        # Load loop
        #

        # Start return value at 0
        { move R_L2_3, zero }

        # init current address register        	
        { move R_L2_4, R_L2_0 }

        # Read/check data = address
load_loop:     	
	# Check the failure count
        { addi R_TEMP_0, R_L2_3, -(MAX_ERRORS) }
        { bz R_TEMP_0, load_return }
                
        # Calculate the expected data value
        { xor R_L2_7, R_L2_4, R_L2_2 }
	
        # Load the data
        { lw R_L2_6, R_L2_4 }
        { mtspr SPR_PASS, R_L2_6 }

        # check the load data
        { xor R_TEMP_0, R_L2_6, R_L2_7 }
	#DL-hack remove the below line, will print out every single read
        { bz R_TEMP_0, load_check_addr } 
        
        # Increment failure count
        { addi R_L2_3, R_L2_3, 1 }

        #{ jal load_check_addr }
        
        { movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xbad }
	{ jal output_msg_1 }
        { movei R_L1_0, PASS_MSG; move R_L1_1, R_L2_4 } # address
	{ jal output_msg_1 }
        { movei R_L1_0, PASS_MSG; move R_L1_1, R_L2_7 } # expected data
	{ jal output_msg_1 }
        { movei R_L1_0, PASS_MSG; move R_L1_1, R_L2_6 } # actual data
	{ jal output_msg_1 }
        
load_check_addr:
        { moveli R_TEMP_0, lo16(PRINT_MASK) }
        { auli R_TEMP_0, R_TEMP_0, ha16(PRINT_MASK) }
        { and R_TEMP_0, R_TEMP_0, R_L2_4 }
        { bnz R_TEMP_0, load_inc_addr }
        { movei R_L1_0, PASS_MSG; move R_L1_1, R_L2_6 } # actual data
	#Dl-hack the below line print out the data again, and it's confusing me!
	#{ jal output_msg_1 }
        
load_inc_addr:  	
        # Increment the address        
        # check to see if we've reached the end value for the loop
        #dl-hack
        #{ addi R_L2_4, R_L2_4, 64; addi R_L2_1, R_L2_1, -64 } 
        { addi R_L2_4, R_L2_4, 4; addi R_L2_1, R_L2_1, -4 } 

        #{ movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xaaa }
	#{ jal output_msg_1 }
        #{ movei R_L1_0, PASS_MSG; move R_L1_1, R_L2_1 } # address
	#{ jal output_msg_1 }
	
        { bgzt R_L2_1, load_loop }

load_return:    	
        # Return	
        { jrp R_L2_5 }


###################################################
# duncan_mem_test - test memory with different pattern
#
# On entry:     
# R_L2_0 = start address
# R_L2_1 = size in bytes <=== NOTE - this value is destroyed
# R_L2_2 = test pattern
# -     0 = 0x00000000
# -     5 = 0x55555555
# -     a = 0xaaaaaaaa
# -     f = 0xffffffff
###################################################
        
duncan_mem_test_2:       	
        # save link reg
        { move R_L2_5, lr }
        
	#
        # Store loop
        #

        { mtspr SPR_PASS, R_L2_0 }
        { mtspr SPR_PASS, R_L2_1 }
        { mtspr SPR_PASS, R_L2_2 }
        
	#
        # Store loop
        #

        # init current address register        	
        { move R_TEMP_1, R_L2_0 }

	# init size register
        { move R_TEMP_2, R_L2_1 }

        #check to see which pattern we are checking
	{ moveli R_TEMP_3,         lo16(0x0000000f) }
	{ auli R_TEMP_3, R_TEMP_3, ha16(0x0000000f) }
        { xor  R_TEMP_3, R_L2_2, R_TEMP_3 }
        { bz R_TEMP_3, dmt_ff }
	{ moveli R_TEMP_3,         lo16(0x0000000a) }
	{ auli R_TEMP_3, R_TEMP_3, ha16(0x0000000a) }
        { xor  R_TEMP_3, R_L2_2, R_TEMP_3 }
        { bz R_TEMP_3, dmt_aa }
	{ moveli R_TEMP_3,         lo16(0x00000005) }
	{ auli R_TEMP_3, R_TEMP_3, ha16(0x00000005) }
        { xor  R_TEMP_3, R_L2_2, R_TEMP_3 }
        { bz R_TEMP_3, dmt_55 }
        

dmt_00: 
        # Store data = address
        # -0x00000000
        { move R_TEMP_3, zero }
        { jal dmt_store_loop }
dmt_ff: 
        # Store data = address
        # -0xffffffff
	{ moveli R_TEMP_3,        lo16(0xffffffff) }
	{ auli R_TEMP_3,R_TEMP_3, ha16(0xffffffff) }
        { jal dmt_store_loop }
dmt_aa: 
        # Store data = address
        # -0xaaaaaaaa
	{ moveli R_TEMP_3,        lo16(0xaaaaaaaa) }
	{ auli R_TEMP_3,R_TEMP_3, ha16(0xaaaaaaaa) }
        { jal dmt_store_loop }
dmt_55: 
        # Store data = address
        # -0x55555555
	{ moveli R_TEMP_3,        lo16(0x55555555) }
	{ auli R_TEMP_3,R_TEMP_3, ha16(0x55555555) }
        { jal dmt_store_loop }

dmt_store_loop:     	
        { mtspr SPR_PASS, R_TEMP_1 }

        { sw R_TEMP_1, R_TEMP_3; addi R_TEMP_1, R_TEMP_1, 4} 
        # check to see if we've reached the end value for the loop  
	{ addi R_TEMP_2, R_TEMP_2, -4 }    

        #dl-hack, write to every addresses with the same value
                  
        { bgzt R_TEMP_2, dmt_store_loop }

        # Return	
        { jrp R_L2_5 }

###################################################
# duncan_mem_read_test - read and check with different pattern
#
# On entry:     
# R_L2_0 = start address
# R_L2_1 = size in bytes <=== NOTE - this value is destroyed
# R_L2_2 = xor value
#
# On exit:      
# R_L2_3 = fail count                        
###################################################
        
duncan_read_mem_test_2:     
        # save link reg
        { move R_L2_5, lr }

        { mtspr SPR_PASS, R_L2_0 }
        { mtspr SPR_PASS, R_L2_1 }
        { mtspr SPR_PASS, R_L2_2 }

	#
        # Load loop
        #

        # Start return value at 0
        { move R_L2_3, zero }

        # init current address register        	
        { move R_L2_4, R_L2_0 }

        #check to see which pattern we are checking
	{ moveli R_TEMP_3,         lo16(0x0000000f) }
	{ auli R_TEMP_3, R_TEMP_3, ha16(0x0000000f) }
        { xor  R_TEMP_3, R_L2_2, R_TEMP_3 }
        { bz R_TEMP_3, drmt_ff }
	{ moveli R_TEMP_3,         lo16(0x0000000a) }
	{ auli R_TEMP_3, R_TEMP_3, ha16(0x0000000a) }
        { xor  R_TEMP_3, R_L2_2, R_TEMP_3 }
        { bz R_TEMP_3, drmt_aa }
	{ moveli R_TEMP_3,         lo16(0x00000005) }
	{ auli R_TEMP_3, R_TEMP_3, ha16(0x00000005) }
        { xor  R_TEMP_3, R_L2_2, R_TEMP_3 }
        { bz R_TEMP_3, drmt_55 }
        

drmt_00: 
        # Store data = address
        # -0x00000000
        { move R_L2_7, zero }
        { jal drmt_load_loop }
drmt_ff: 
        # Store data = address
        # -0xffffffff
	{ moveli R_L2_7,         lo16(0xffffffff) }
	{ auli   R_L2_7, R_L2_7, ha16(0xffffffff) }
        { jal drmt_load_loop }
drmt_aa: 
        # Store data = address
        # -0xaaaaaaaa
	{ moveli R_L2_7,         lo16(0xaaaaaaaa) }
	{ auli   R_L2_7, R_L2_7, ha16(0xaaaaaaaa) }
        { jal drmt_load_loop }
drmt_55: 
        # Store data = address
        # -0x55555555
	{ moveli R_L2_7,         lo16(0x55555555) }
	{ auli   R_L2_7, R_L2_7, ha16(0x55555555) }
        { jal drmt_load_loop }

drmt_load_loop:     	
	# Check the failure count
        { addi R_TEMP_0, R_L2_3, -(MAX_ERRORS) }
        { bz R_TEMP_0, drmt_load_return }
                
        # Load the data
        { lw R_L2_6, R_L2_4 }
        { mtspr SPR_PASS, R_L2_6 }

        # check the load data
        { xor R_TEMP_0, R_L2_6, R_L2_7 }
	#DL-hack remove the below line, will print out every single read
        { bz R_TEMP_0, drmt_load_inc_addr } 
        
        # Increment failure count
        { addi R_L2_3, R_L2_3, 1 }
        
        { movei R_L1_0, PASS_MSG; moveli R_L1_1, 0xbad }
	{ jal output_msg_1 }
        { movei R_L1_0, PASS_MSG; move R_L1_1, R_L2_4 } # address
	{ jal output_msg_1 }
        { movei R_L1_0, PASS_MSG; move R_L1_1, R_L2_7 } # expected data
	{ jal output_msg_1 }
        { movei R_L1_0, PASS_MSG; move R_L1_1, R_L2_6 } # actual data
	{ jal output_msg_1 }
        
drmt_load_inc_addr:  	
        # Increment the address        
        # check to see if we've reached the end value for the loop
        #dl-hack
        { addi R_L2_4, R_L2_4, 4; addi R_L2_1, R_L2_1, -4 } 

        { bgzt R_L2_1, drmt_load_loop }

drmt_load_return:
        # Return	
        { jrp R_L2_5 }


        
# Make sure this last address is 64-byte aligned so that the code above is padded to a full cache line
# so that when it is loaded into the L2 cache in cache-as-RAM mode, there are no parity errors                
.align 64
_last_addr:             
