//
// Property of Tilera Corporation.  Proprietary and Confidential.
// Copyright 2006
//
//  tb1_rsh_jtag.c
//
//  Carl Ramey
//  11.21.06
//
//  routines to provide tile SW access to the chip's jtag controller
//

#include "testbench_defs.h"
#include <sys/archlib.h>
#include "dvlib.h"
#include "tb1_rsh_jtag.h"

// generic JTAG-rcv function
void rcv_jtag_serial_data(uint32_t *data, uint32_t jtag_inst, int start_bit, int num_bits)
{
  int curr_wd_idx = 0;
  int curr_wr_size;
  int shift_type = 1;
  int capture_data;

  // setup jtag controller
  send_config_write(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,
                    RSHIM_CFG_JTAG_SETUP,
                    (5 | // 25MHz, jtag enabled
                     (jtag_inst << 16))); // jtag instruction being accessed
 
  // send data words
  while(num_bits) {    
    
    capture_data = 0; // overridden below if we want this data

    // get to start bit
    if (start_bit) {
      curr_wr_size = (start_bit > 32) ? 32 : start_bit;
      start_bit -= curr_wr_size;      
    }
    else { // real bits to rcv
      curr_wr_size = (num_bits > 32) ? 32 : num_bits;
      num_bits -= curr_wr_size;
      capture_data = 1;
    }

    // trigger the shift
    send_config_write(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,
                      RSHIM_CFG_JTAG_CONTROL,
                      (curr_wr_size |         // shift count
                       (shift_type << 7)));   // shift type - 1st or continuation
    
    shift_type = 2; // subsequent shifts

    // wait for shift to complete before sending the next one
    while(send_config_read(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,RSHIM_CFG_JTAG_CONTROL));

    if (capture_data)
      data[curr_wd_idx++] = send_config_read(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,RSHIM_CFG_JTAG_DATA);

  }
}


// generic JTAG-send function - give it bits and a jtag_instruction and it uses the rshim jtag controller to send data into the jtag isntruction register(s)
void send_jtag_serial_data(uint32_t *data, int chain_size, uint32_t jtag_inst, int start_bit, int num_bits)
{
  int bits_rem = chain_size;
  int curr_wd_idx = 0;
  int curr_wr_size;
  int shift_type = 1;

  // setup jtag controller
  send_config_write(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,
                    RSHIM_CFG_JTAG_SETUP,
                    (5 | // 25MHz, jtag enabled
                     (jtag_inst << 16))); // jtag instruction being accessed

  
  // send data words
  while(bits_rem) {    
    curr_wr_size = (bits_rem > 32) ? 32 : bits_rem;

    // zero pad until start bit
    if (start_bit) {
      if (curr_wr_size > start_bit) 
        curr_wr_size = start_bit;
      send_config_write(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,RSHIM_CFG_JTAG_DATA,0);
      start_bit -= curr_wr_size;      
    }
    else if (num_bits) { // real bits to send
      if (curr_wr_size > num_bits)
        curr_wr_size = num_bits;
      // setup the data
      send_config_write(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,
                        RSHIM_CFG_JTAG_DATA,
                        data[curr_wd_idx++]);
      num_bits -= curr_wr_size;
    }
    else { // padding remainder with zeros
      send_config_write(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,RSHIM_CFG_JTAG_DATA,0);
    }

    // trigger the write
    send_config_write(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,
                      RSHIM_CFG_JTAG_CONTROL,
                      (curr_wr_size |         // shift count
                       (shift_type << 7)));   // shift type - 1st or continuation
    
    bits_rem -= curr_wr_size;

    shift_type = 2; // subsequent shifts

    // wait for shift to complete before sending the next one
    while(send_config_read(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,RSHIM_CFG_JTAG_CONTROL));

  }


  // do the update
  send_config_write(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,
                    RSHIM_CFG_JTAG_CONTROL,
                    (3 << 7));   // shift type - update

}

uint32_t send_jtag_read(int tile_x, int tile_y, int obj_sel, int offset, int skip_configure) 
{
  uint32_t jtag_send_data[2];
  int shift_bits = INST_SEL_WIDTH * ORIG_DIM_X;
  int start_bit = (ORIG_DIM_X-tile_x-1) * INST_SEL_WIDTH; // first bit of inst_sel register for tile we want to talk to

  // optionally skip configuration phase
  if (!skip_configure) {
    // set the block sel for proper row
    jtag_send_data[0] = (0x1 << tile_y);
    send_jtag_serial_data(jtag_send_data,NUM_BLOCK_SEL_BITS,BLOCKSELBIST,0,NUM_BLOCK_SEL_BITS);
    
    // setup extended mode
    jtag_send_data[0] = 0x1;
    send_jtag_serial_data(jtag_send_data,2,EXTENDED_MODE,0,2);
  }


  // write garbage (but recognizable) data into data field
  jtag_send_data[0] = 0x12345678; // data bits
  // cmd,obj_sel,offset
  jtag_send_data[1] = (0x1 << (DIAG_JT_OFFSET_WIDTH + DIAG_JT_OBJ_SEL_WIDTH)) |  ((obj_sel & 0x1f) << DIAG_JT_OFFSET_WIDTH) | (offset & 0xffff);
          
  send_jtag_serial_data(jtag_send_data,shift_bits,INSTSELBIST,
                        start_bit,INST_SEL_WIDTH-INST_SEL_SHORT_WIDTH); // just write the extended mode data for a single tile
  

  // shift data back to rshim
  rcv_jtag_serial_data(jtag_send_data,INSTSELBIST,
                       start_bit,INST_SEL_WIDTH-INST_SEL_SHORT_WIDTH);

  return jtag_send_data[0];
}

void send_jtag_write(int tile_x, int tile_y, int obj_sel, int offset, uint32_t write_data, int skip_configure) 
{
  uint32_t jtag_send_data[2];
  int shift_bits = INST_SEL_WIDTH * ORIG_DIM_X;
  int start_bit = (ORIG_DIM_X-tile_x-1) * INST_SEL_WIDTH; // first bit of inst_sel register for tile we want to talk to

  // optionally skip configuration phase
  if (!skip_configure) {
    // set the block sel for proper row
    jtag_send_data[0] = (0x1 << tile_y);
    send_jtag_serial_data(jtag_send_data,NUM_BLOCK_SEL_BITS,BLOCKSELBIST,0,0);
    
    // setup extended mode
    jtag_send_data[0] = 0x1;
    send_jtag_serial_data(jtag_send_data,2,EXTENDED_MODE,0,0);
  }

  // data bits
  jtag_send_data[0] = write_data; 
  // cmd,obj_sel,offset
  jtag_send_data[1] = (0x2 << (DIAG_JT_OFFSET_WIDTH + DIAG_JT_OBJ_SEL_WIDTH)) |  ((obj_sel & 0x1f) << DIAG_JT_OFFSET_WIDTH) | (offset & 0xffff);
          
  send_jtag_serial_data(jtag_send_data,shift_bits,INSTSELBIST,
                        start_bit,INST_SEL_WIDTH-INST_SEL_SHORT_WIDTH); // just write the extended mode data for a single tile  
}

uint32_t test_tile_jtag_access() {

  uint32_t fail_count = 0;

  // make sure jtag access enabled before proceeding - this is setup via a strapping pin
  uint32_t jtag_setup = send_config_read(RSH0_IDN_X,RSH0_IDN_Y,CFG_TAG,0,RSHIM_CFG_JTAG_SETUP);
  TEST_PASS(jtag_setup);
  if (!(jtag_setup & 0x1)) {
    TEST_PASS(0xbaddeed8);
    fail_count++;
  }


  // put some data into the L2 - we'll read it back via JTAG
  char data_buf[65536]; // big enough to span both ways of L2
  char *data_buf_aligned = (char *)(((uint32_t)data_buf + 63) & 0xffffffc0);
  *(data_buf_aligned+4) = 0x01;
  *(data_buf_aligned+5) = 0xef;
  *(data_buf_aligned+6) = 0xcd;
  *(data_buf_aligned+7) = 0xab;

  *(data_buf_aligned+32768+4) = 0x89;
  *(data_buf_aligned+32768+5) = 0x67;
  *(data_buf_aligned+32768+6) = 0x45;
  *(data_buf_aligned+32768+7) = 0x23;
  

  uint32_t l2_index = (((uint32_t)data_buf_aligned) >> 4) & 0x7ff; // 11 bits of L2 RAM index (includes 2 bits of slice offset within the 64-byte cacheline)

//<JSB 20081209>
  uint32_t x,y;
  TEST_PASS(5150);
#if defined(MAESTRO_7x7) && defined(TESTBENCH_CORE)
  x = MY_X - 1;
  y = MY_Y - 1;
#else
  x = MY_X;
  y = MY_Y;
#endif

  uint32_t l2_data = send_jtag_read(x,y,
                                    OBJ_SEL_L2_DAT,
                                    ((0<<14) |           // way select - assumes one of the above accesses ended up in way0
                                     (l2_index << 3) |   // l2 index
                                     1),                 // word offset within 144-bit slice
                                    0);                  // don't skip configuration

//</JSB>  

  TEST_PASS(l2_data);
  if ((l2_data != 0xabcdef01) && (l2_data != 0x23456789)) {
    TEST_PASS(0xbaddeed9);
    fail_count++;
  }

  return fail_count;

}

