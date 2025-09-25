/*
 * Copyright 2009 Tilera Corporation. All Rights Reserved.
 * 
 *   The source code contained or described herein and all documents
 *   related to the source code ("Material") are owned by Tilera
 *   Corporation or its suppliers or licensors.  Title to the Material
 *   remains with Tilera Corporation or its suppliers and licensors. The
 *   software is licensed under the Tilera MDE License.
 * 
 *   Unless otherwise agreed by Tilera in writing, you may not remove or
 *   alter this notice or any other notice embedded in Materials by Tilera
 *   or Tilera's suppliers or licensors in any way.
 *
 */

/*
file:    dvlib.c
author:  Jay Stickney
purpose: Common functions for dv applications. 
*/

////////////////////////////////////////////////////////////////////////
//
// This file should NOT include testbench_defs.h.
//
// Any testbench-specific parameters should be passed to functions as
// arguments to functions, not by #defines
//
////////////////////////////////////////////////////////////////////////

#ifndef __DVLIB_C__
#define __DVLIB_C__

#include <sys/archlib.h>
#include "itc_dvlib.h"
#include <sys/reg.h>

//
// Common DV functions.
//

void delay(uint32_t delay) { 
  uint32_t start_time, curr_time; 
  start_time = __insn_mfspr(SPR_CYCLE_LOW);
  curr_time = start_time;
  while  ((curr_time - start_time) < delay) { 
    curr_time = __insn_mfspr(SPR_CYCLE_LOW); 
  }
}

void wait_for_fc(void) { 

  volatile uint32_t data;  
  
  data = __insn_mfspr(SPR_IDN_CA_DATA);  
} 

void wait_for_notification(void) { 

  volatile uint32_t data, tag;
  
  while ((__insn_mfspr(SPR_IDN_DEMUX_CA_COUNT) == 0)) {}  
  tag = __insn_mfspr(SPR_IDN_CA_TAG);  
  data = __insn_mfspr(SPR_IDN_CA_DATA);  
  data = __insn_mfspr(SPR_IDN_CA_DATA);  
} 

uint32_t check_for_interrupt(void) { 

  volatile uint32_t hdr;
  uint32_t type, num;
  
  // By default, will return unassigned
  // value if there is no pending interrupt.
  num = 0xffffffff;

  if((__insn_mfspr(SPR_IDN_DEMUX_CA_COUNT) > 0)) {
    hdr = __insn_mfspr(SPR_IDN_CA_DATA);
    type = (0x00000780 & hdr) >> 7; // capture hdr[10:7], type field
    num = (0xfffff000 & hdr) >> 12; // capture hdr[31:12], protocol specific interrupt number
    
    if(type == 0x7) {
      TEST_PASS(hdr);
      TEST_PASS(type);
      TEST_PASS(num);
    }
    else {
      TEST_FAIL(hdr);
      TEST_FAIL(type);
    }
  }

  return(num);
} 

void idn_send_msg(uint32_t dest_x, uint32_t dest_y, uint32_t tag, uint32_t msg)
{
  RawMsgPkt rawPkt;  

  // Route header.
  rawPkt.bits.routeHdr.length = 3;  
  rawPkt.bits.routeHdr.dest_x = dest_x;
  rawPkt.bits.routeHdr.dest_y = dest_y;
  rawPkt.bits.routeHdr.fbit = 0;  

  // Routing tag
  rawPkt.bits.tag = tag;  

  // Shim header
  rawPkt.bits.ack = 0;  
  rawPkt.bits.iodn_type = IODN_RAW_PKT;  
  rawPkt.bits.bytes = 4;  
  rawPkt.bits.cont = 0;  

  // Payload
  rawPkt.bits.data = msg;

  iodn_send_pkt(rawPkt.word, 3);  
}

uint32_t idn1_get_msg()
{
  // Get raw shim header and throw it away
  iodn1_receive();

  // Get the payload and return it
  return(iodn1_receive());
}

void udn_send_msg(uint32_t dest_x, uint32_t dest_y, uint32_t tag, uint32_t msg)
{
  // Sends fixed size (one word) message to UDN queue specified by tag
  // print("udn_send_msg");
  DynamicHeader route;

  // Send route
  route.bits.dest_x = dest_x;
  route.bits.dest_y = dest_y;
  route.bits.final_route = 0;
  route.bits.length = 2;
  udn_send(route.word);

  // Send tag
  udn_send(tag);

  // Send payload
  udn_send(msg);
  // print("udn_send_msg done");
}

void udn_send_msg_and_id(uint32_t dest_x, uint32_t dest_y, uint32_t tag, uint32_t msg)
{
  // Sends fixed size (one word) message to UDN queue specified by tag

  DynamicHeader route;

  // Send route
  route.bits.dest_x = dest_x;
  route.bits.dest_y = dest_y;
  route.bits.final_route = 0;
  route.bits.length = 3;
  udn_send(route.word);

  // Send tag
  udn_send(tag);

  // Send payload
  udn_send((MY_X << 4) | MY_Y); // x,y in low two nibbles for hex readability
  udn_send(msg);
}

void udn_send_pkt(uint32_t dest_x, uint32_t dest_y, uint32_t *pyld, uint32_t tag) 
{
  // Send route
  DynamicHeader route;
  route.bits.dest_x = dest_x;
  route.bits.dest_y = dest_y;
  route.bits.final_route = 0;
  route.bits.length = 2 + pyld[0];
  udn_send(route.word);

  // Send tag
  udn_send(tag);

  // Send payload
  int i;
  for (i = 0; i <= pyld[0]; i++)
  {
    udn_send(pyld[i]);
  }

}

void udn_get_pyld1(uint32_t *pyld) 
{ 
  // Uses UDN queue 1
  // Assumes first word is number of words following

  int i;

  pyld[0] = udn1_receive();
  for (i = 1; i <= pyld[0]; i++) 
  {
    pyld[i] = udn1_receive();
  }
} 

void udn_get_pyld2(uint32_t *pyld) 
{ 
  // Uses UDN queue 2
  // Assumes first word is number of words following

  int i;

  pyld[0] = udn2_receive();
  for (i = 1; i <= pyld[0]; i++) 
  {
    pyld[i] = udn2_receive();
  }
} 

void broadcast_msg(int corner_x, int corner_y, int dim_x, int dim_y, uint32_t msg, uint32_t tag) 
{
  int x,y;
  int count = 0;
  for (x = (corner_x + dim_x - 1); x >= corner_x; x--)
  {
    for (y = (corner_y + dim_y - 1); y >= corner_y; y--)
    {
      // If not my own coordinates, send a message; can't send a message to myself
      if (TILE_ID(x,y) != MY_ID)
      {
        udn_send_msg(x, y, tag, msg);
        count++;
      }
    }
  }
}

void send_sync(int corner_x, int corner_y, int dim_x, int dim_y)
{
  // Uses UDN tag 0
  broadcast_msg(corner_x, corner_y, dim_x, dim_y, MY_ID, UDN_TAG_0);
}

void get_sync()
{
  // Uses UDN tag 0
  udn0_receive();
}

void send_result(uint32_t dest_x, uint32_t dest_y, uint32_t code)
{
  udn_send_msg(dest_x,  dest_y, UDN_TAG_0, code);
}

uint32_t get_results(int corner_x, int corner_y, int dim_x, int dim_y, uint32_t my_result) 
{
  // Collect completion status from all
  int x, y;
  uint32_t result = my_result;
  uint32_t msg;
  int count = 0;
  for (x = corner_x; x < (corner_x + dim_x); x++)
  {
    for (y = corner_y; y < (corner_y + dim_y); y++)
    {
      // Skip one coordinate because not doing self
      if ((x != MY_X) || (y != MY_Y))
      {
        msg = udn0_receive();
        result = result + msg;
        count++;
      }
    }
  }

  return(result);
}

void check_results(int corner_x, int corner_y, int dim_x, int dim_y, uint32_t my_result) 
{
  uint32_t result = get_results(corner_x, corner_y, dim_x, dim_y, my_result);

  if (result != 0)
  {
    TEST_FAIL(result);
  }
}

void sync_up(uint32_t master_x, uint32_t master_y, int corner_x, int corner_y, int dim_x, int dim_y) 
{
  // Master tile requests completion messages and then collects them
  // All others wait for the request and send completion message
  if (MY_ID == TILE_ID(master_x, master_y))
  {
    send_sync(corner_x, corner_y, dim_x, dim_y);
  }
  else
  {
    // If tile is within rectangle, wait for sync
    if ((MY_X >= corner_x) &&
        (MY_X < (corner_x + dim_x)) &&
        (MY_Y >= corner_y) &&
        (MY_Y < (corner_y + dim_y)))
    {
      get_sync();
    }
  }
}

void do_results(uint32_t master_x, uint32_t master_y, int corner_x, int corner_y, int dim_x, int dim_y, uint32_t code) 
{
  // Master tile requests completion messages and then collects them
  // All others wait for the request and send completion message
  if (MY_ID == TILE_ID(master_x, master_y))
  {
    check_results(corner_x, corner_y, dim_x, dim_y, code);
  }
  else
  {
    // If tile is within rectangle, send result
    if ((MY_X >= corner_x) &&
        (MY_X < (corner_x + dim_x)) &&
        (MY_Y >= corner_y) &&
        (MY_Y < (corner_y + dim_y)))
    {
      send_result(master_x,  master_y, code);
    }
  }
}

void sync_and_check(uint32_t master_x, uint32_t master_y, int corner_x, int corner_y, int dim_x, int dim_y, uint32_t code) 
{
  sync_up(master_x, master_y, corner_x, corner_y, dim_x, dim_y);
  do_results(master_x, master_y, corner_x, corner_y, dim_x, dim_y, code);
}

void setup_static_sync(int corner_x, int corner_y, int dim_x, int dim_y) 
{
  // Create a circular static route through an array of tiles
  // The static route that is set up requires that the array have an even number of rows and at least 2 columns

  if (MY_ID == TILE_ID(corner_x, corner_y)) // Northwest corner
  {
    // south to main, main to east
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,0,SRC_M,0,SRC_S));
  }
  else if (MY_ID == TILE_ID(corner_x, corner_y + dim_y - 1)) // Southwest corner
  {
    // east to main, main to north
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(SRC_M,0,0,0,SRC_E));
  }
  else if (MY_ID == TILE_ID(corner_x + dim_x - 1, corner_y)) // Northeast corner
  {
    // west to main, main to south
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,SRC_M,0,0,SRC_W));
  }
  else if (MY_ID == TILE_ID(corner_x + dim_x - 1, corner_y + dim_y - 1)) // Southeast corner
  {
    // north to main, main to west
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,0,0,SRC_M,SRC_N));
  }
  else if (MY_X == corner_x) // West edge
  {
    // south to main, main to north
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(SRC_M,0,0,0,SRC_S));
  }
  else if (MY_Y == corner_y) // North edge
  {
    // west to main, main to east
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,0,SRC_M,0,SRC_W));
  }
  else if (MY_Y == (corner_y + dim_y - 1)) // South edge
  {
    // east to main, main to west
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,0,0,SRC_M,SRC_E));
  }
  else if (MY_X == (corner_x + dim_x - 1)) // East edge
  {
    if ((MY_Y & 1) == (corner_y & 1)) // Row with same odd/evenness of top row
    {
      // west to main, main to south
      __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,SRC_M,0,0,SRC_W));
    }
    else
    {
      // north to main, main to west
      __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,0,0,SRC_M,SRC_N));
    }
  }
  else if (MY_X == (corner_x + 1)) // Column just inside of west edge
  {
    if ((MY_Y & 1) == (corner_y & 1)) // Row with same odd/evenness of top row
    {
      // north to main, main to east
      __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,0,SRC_M,0,SRC_N));
    }
    else
    {
      // east to main, main to south
      __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,SRC_M,0,0,SRC_E));
    }
  }
  else if ((MY_Y & 1) == (corner_y & 1)) // Row with same odd/evenness of top row
  {
    // west to main, main to east
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,0,SRC_M,0,SRC_W));
  }
  else
  {
    // east to main, main to west
    __insn_mtspr(SPR_SNSTATIC, STATIC_ROUTE(0,0,0,SRC_M,SRC_E));
  }

  // Unfreeze the switch fabric (but not the switch processor)
  __insn_mtspr(SPR_SNCTL, 2);
}

void static_sync(uint32_t master_x, uint32_t master_y, uint32_t value)
{
  if (MY_ID == TILE_ID(master_x, master_y))
  {
    // The master sources a static network word into the chain and then waits for it to come back
    sn_send(value);
    uint32_t msg = sn_receive();
    TEST_PASS(msg);
  }
  else
  {
    // Non-masters wait for a static network word and then pass it along the chain
    sn_send(sn_receive());
  }
}

void start_tile(uint32_t tile_x, uint32_t tile_y, uint32_t mshim_mdn_x, uint32_t mshim_mdn_y) 
{
  // Send the UDN message to tell it what mshim port it should use
  // Send the UDN message to wake the tile up now that the mshim has been configured for it and what mshim port it should use

  LongRawMsgPkt rawPkt;  

  // Start packet is different for bringup tests than others
  // Bringup tests don't send mshim coordinates because this has already been set up in the boot code

  // Route header.
#ifdef BRINGUP
  rawPkt.bits.routeHdr.length = 3;  
#else
  rawPkt.bits.routeHdr.length = 4;  
#endif
  rawPkt.bits.routeHdr.dest_x = tile_x;
  rawPkt.bits.routeHdr.dest_y = tile_y;
  rawPkt.bits.routeHdr.fbit = 0;  

  // Routing tag
  rawPkt.bits.tag = IDN_TAG_0;  

  // Shim header
  rawPkt.bits.ack = 0;  
  rawPkt.bits.iodn_type = IODN_RAW_PKT;  
  rawPkt.bits.bytes = 8;  
  rawPkt.bits.cont = 0;  

  // Payload
#ifdef BRINGUP
  rawPkt.bits.data[0] = 0x800; // start address
  iodn_send_pkt(rawPkt.word, 3);  
#else
  rawPkt.bits.data[0] = TILE_COORD(mshim_mdn_x, mshim_mdn_y); // mshim coordinates to use
  rawPkt.bits.data[1] = 0x800; // start address
  iodn_send_pkt(rawPkt.word, 4);  
#endif
}

uint32_t western_mdn_port_x(uint32_t mshim_x, uint32_t mshim_y)
{
  // Read the mdn config reg
  uint32_t mdn_config;
  mdn_config = send_config_read(mshim_x, mshim_y, 0, 0, 0x14);

  // Check to see if the ports are clockwise or counter-clockwise
  uint32_t clockwise;
  if ((mdn_config & 0x0e000) == 0x0e000)
  {
    clockwise = 1;
  }
  else if ((mdn_config & 0x38000) == 0x38000)
  {
    clockwise = 0;
  }
  else
  {
    TEST_FAIL(mdn_config);
  }

  // Determine the x coordinate of the western-most mshim port
  uint32_t western_mshim_port_x;
  
  if (mshim_y <= CORNER_Y)
  {
    // Mshim on northern edge
    if (clockwise)
    {
      western_mshim_port_x = mshim_x;
    }
    else
    {
      western_mshim_port_x = mshim_x - 2;
    }
  }
  else
  {
    // Mshim on southern edge
    
    if (clockwise)
    {
      western_mshim_port_x = mshim_x - 2;
    }
    else
    {
      western_mshim_port_x = mshim_x;
    }
  }

  return western_mshim_port_x;
}

void start_tiles(uint32_t master_x, uint32_t master_y, int corner_x, int corner_y, int dim_x, int dim_y) 
{
  // This undoes the change that was done in startup.S for the c tests that use this function
  // eventually this should be undone so
  // FIXME BAD ERROR WARNING DANGER WILL ROBINSON!!! (signed off - Nat)
  //     signed off because most .c tests aren't written to be able to handle maskable interrupts at the time
  //     they call start_tiles().  They could be rewritten to do so but our infrastructure has network traffic
  //     going back and forth that c tests may not need to know about so they never set up interrupt handlers
  //     for those maskable interrupts.  However earlier parts of our startup dv infra require those interrupts
  //     to be unmasked AND we made a decision to unmask as many interrupts as possible in order to improve the
  //     the possibility of detecting spurious interrupts during simulation
  // some grep/code review food for next rev 
  __insn_mtspr(SPR_INTERRUPT_MASK_SET_3_0, -1);
  __insn_mtspr(SPR_INTERRUPT_MASK_SET_3_1, -1);

  if (MY_ID == TILE_ID(master_x, master_y))
  {
    // Find out where the mshim's MDN ports are
    // Assumes mshim is on north or south edge of fabric
    // Assumes mshim has MDN connections to three contiguous tiles

    uint32_t my_mshim_x = X_FROM_COORD(__insn_mfspr(SPR_CBOX_MMAP_0));
    uint32_t my_mshim_y = Y_FROM_COORD(__insn_mfspr(SPR_CBOX_MMAP_0));


    uint32_t western_mshim_port_x = western_mdn_port_x(my_mshim_x, my_mshim_y);

    // Start up the tiles
    int x, y;
    int count = 0;
    
    // Config mshim and wake up everyone
    for (x = (corner_x + dim_x-1); x >= corner_x; x--)
    {
      for (y = (corner_y + dim_y-1); y >= corner_y; y--)
      {
        // If not my own coordinates, send a message; can't send a message to myself
        if (TILE_ID(x,y) !=  MY_ID)
        {
          count++;
          
#ifdef BRINGUP_BOARD          
          start_tile(x, y, my_mshim_x, my_mshim_y);
#else
          // Figure out what mshim port this tile should use
          uint32_t mshim_port_x;
          if (x <= western_mshim_port_x)
          {
            mshim_port_x = western_mshim_port_x;
          }
          else if (x >= (western_mshim_port_x+2))
          {
            mshim_port_x = (western_mshim_port_x+2);
          }
          else
          {
            mshim_port_x = (western_mshim_port_x+1);
          }
          start_tile(x, y, mshim_port_x, my_mshim_y);
#endif
        }
      }
    }
  }
  do_results(master_x, master_y, corner_x, corner_y, dim_x, dim_y, 0);
}

void choose_mshim_and_start(uint32_t x, uint32_t y, uint32_t y_split)
{
  // NOTE: This function only works on the full-chip testbench because it assumes the locations of the 4 mshims

  uint32_t mshim_mdn_x, mshim_mdn_y;

  if (bringup_board())
  {
    mshim_mdn_x = CORNER_X + 1;
    mshim_mdn_y = CORNER_Y + 0;
  }
  else 
  {
    if (y < y_split)
    {
      mshim_mdn_y = CORNER_Y + 0;
    }
    else
    {
      mshim_mdn_y = CORNER_Y + 7;
    }
    
    if (x > (CORNER_X + 3))
    {
      if (x > (CORNER_X + 5))
      {
        mshim_mdn_x = CORNER_X + 6;
      }
      else if (x < (CORNER_X + 5))
      {
        mshim_mdn_x = CORNER_X + 4;
      }
      else
      {
        mshim_mdn_x = CORNER_X + 5;
      }
    }
    else
    {
      if (x > (CORNER_X + 2))
      {
        mshim_mdn_x = CORNER_X + 3;
      }
      else if (x < (CORNER_X + 2))
      {
        mshim_mdn_x = CORNER_X + 1;
      }
      else
      {
        mshim_mdn_x = CORNER_X + 2;
      }
    }
  }    

  start_tile(x, y, mshim_mdn_x, mshim_mdn_y);

}


void start_tiles_distributed(int corner_x, int corner_y, int dim_x, int dim_y)
{
  // NOTE: This function only works on the full-chip testbench because it assumes the locations of the 4 mshims

  int x, y;
  for (x = (corner_x + dim_x-1); x >= corner_x; x--)
  {
    for (y = (corner_y + dim_y-1); y >= corner_y; y--)
    {
      // Start up everyone but the master (myself)
      if (TILE_ID(x,y) != MY_ID)
      {
        choose_mshim_and_start(x, y, CORNER_Y+4);
      }
    }
  }
}

void test_end(uint32_t master_x, uint32_t master_y, int corner_x, int corner_y, int dim_x, int dim_y, uint32_t code) 
{
  sync_and_check(master_x, master_y, corner_x, corner_y, dim_x, dim_y, code);

  // Master tile issues done
  if (MY_ID == TILE_ID(master_x, master_y))
  {
    TEST_DONE(0);
  }

  // Everybody idles when done
  idle();
}


void send_config_write(uint32_t io_loc_x, uint32_t io_loc_y, uint32_t ack_tag, uint32_t channel, uint32_t byte_offset, uint32_t data)
{
  issue_config_write(io_loc_x, io_loc_y, ack_tag, channel, byte_offset, data);
  
  wait_for_fc();  
}

void issue_config_write(uint32_t io_loc_x, uint32_t io_loc_y, uint32_t ack_tag, uint32_t channel, uint32_t byte_offset, uint32_t data)
{
  CfgWrPkt cfg_wr_pkt;  

  cfg_wr_pkt.bits.routeHdr.length = 4;
  cfg_wr_pkt.bits.routeHdr.dest_x = io_loc_x;
  cfg_wr_pkt.bits.routeHdr.dest_y = io_loc_y;
  cfg_wr_pkt.bits.routeHdr.fbit = 1;
  cfg_wr_pkt.bits.routeHdr.rsvd = 0;  

  // Shim header
  cfg_wr_pkt.bits.ack = 1;  
  cfg_wr_pkt.bits.iodn_type = IODN_CFG_PKT;  
  cfg_wr_pkt.bits.channel = channel;  
  cfg_wr_pkt.bits.rsvd1 = 0;  
  cfg_wr_pkt.bits.reg_num = (byte_offset >> 2);  

  // From header.
  cfg_wr_pkt.bits.sfb = 0;  
  cfg_wr_pkt.bits.source_x = MY_X;
  cfg_wr_pkt.bits.source_y = MY_Y;
  cfg_wr_pkt.bits.source_channel = channel;
  cfg_wr_pkt.bits.source_tag = ack_tag;

  // data
  cfg_wr_pkt.bits.reg_data = data;

  iodn_send_pkt(cfg_wr_pkt.word, 4);  
}

void issue_config_read(uint32_t io_loc_x, uint32_t io_loc_y, uint32_t ack_tag, uint32_t channel, uint32_t byte_offset)
{
  CfgRdPkt cfg_rd_pkt;  

  cfg_rd_pkt.bits.routeHdr.length = 3;
  cfg_rd_pkt.bits.routeHdr.dest_x = io_loc_x;
  cfg_rd_pkt.bits.routeHdr.dest_y = io_loc_y;
  cfg_rd_pkt.bits.routeHdr.fbit = 1;
  cfg_rd_pkt.bits.routeHdr.rsvd = 0;  

  
  // Shim header
  cfg_rd_pkt.bits.ack = 1;  
  cfg_rd_pkt.bits.iodn_type = IODN_CFG_RD_PKT;
  cfg_rd_pkt.bits.channel = channel;  
  cfg_rd_pkt.bits.rsvd1 = 0;  
  cfg_rd_pkt.bits.reg_num = (byte_offset >> 2);  

  // From header.
  cfg_rd_pkt.bits.sfb = 0;  
  cfg_rd_pkt.bits.source_x = MY_X;
  cfg_rd_pkt.bits.source_y = MY_Y;
  cfg_rd_pkt.bits.source_channel = channel;
  cfg_rd_pkt.bits.source_tag = ack_tag;

  iodn_send_pkt(cfg_rd_pkt.word, 3);  
}

uint32_t send_config_read(uint32_t io_loc_x, uint32_t io_loc_y, uint32_t ack_tag, uint32_t channel, uint32_t byte_offset)
{
  issue_config_read(io_loc_x, io_loc_y, ack_tag, channel, byte_offset);

  return wait_for_rd_comp();
}

void setup_basic_translations()
{
  // Map a 16MB page at zero to zero.
  __insn_mtspr(SPR_ITLB_INDEX, 0);
  __asm__("drain");
  __insn_mtspr(SPR_ITLB_CURRENT_0, 0x0E); // G=1, size=16MB
  __insn_mtspr(SPR_ITLB_CURRENT_1, 0); // PFN_HIGH = 0
  __insn_mtspr(SPR_ITLB_CURRENT_3, 0); // LOTAR = 0
  __insn_mtspr(SPR_ITLB_CURRENT_2, 7); // writable, cacheable, valid
  __asm__("drain");
  __insn_mtspr(SPR_I_PHYSICAL_MEMORY_MODE, 0);
  __asm__("drain");

  // Map a 16MB page at zero to zero 
  __insn_mtspr(SPR_DTLB_INDEX, 0);
  __asm__("drain");
  __insn_mtspr(SPR_DTLB_CURRENT_0, 0x0E); // G=1, size=16MB
  __insn_mtspr(SPR_DTLB_CURRENT_1, 0); // PFN_HIGH = 0
  __insn_mtspr(SPR_DTLB_CURRENT_3, 0); // LOTAR = 0
  __insn_mtspr(SPR_DTLB_CURRENT_2, 7); // writable, cacheable, valid
  __asm__("drain");
  __insn_mtspr(SPR_PHYSICAL_MEMORY_MODE, 0);
  __asm__("drain");

}

// the addr is by default less than 32 bits
// on Wachusett, mimick snc by setting NoAllocL1, NoAllocL2, and non-inclusive bits
// so not cached by requester, everything goes to home, no invals sent.
void setup_snc_page(unsigned int x, unsigned int y, unsigned int addr, unsigned int tb_index)
{
  // Map a 4k page from SNC_VA to SNC_PA
  __insn_mtspr(SPR_DTLB_INDEX, tb_index);
  __asm__("drain");
  __insn_mtspr(SPR_DTLB_CURRENT_0, addr | 0x8); // G=1, size=4K
  __insn_mtspr(SPR_DTLB_CURRENT_1, 0); // PFN_HIGH = 0
  __insn_mtspr(SPR_DTLB_CURRENT_3, (x<<11)|y); // LOTAR = x y
  __insn_mtspr(SPR_DTLB_CURRENT_2, addr | (1<<11) | (1<<10) | (1<<8) | (0x3b)); // no alloc L1, no alloc l2, non-inclusive, non-cacheable, oloc
  __asm__("drain");
}

// the addr is by default less than 32 bits
void setup_proxy_page(unsigned int x, unsigned int y, unsigned int addr, unsigned int tb_index)
{
  // Map a 4k page from SNC_VA to SNC_PA
  __insn_mtspr(SPR_DTLB_INDEX, tb_index);
  __asm__("drain");
  __insn_mtspr(SPR_DTLB_CURRENT_0, addr | 0x8); // G=1, size=4K
  __insn_mtspr(SPR_DTLB_CURRENT_1, 0); // PFN_HIGH = 0
  __insn_mtspr(SPR_DTLB_CURRENT_3, (x<<11)|y); // LOTAR = x y
  __insn_mtspr(SPR_DTLB_CURRENT_2, addr | (0x3b)); // non-cacheable, oloc
  __asm__("drain");
}

// the addr is by default less than 32 bits
void setup_local_page(unsigned int addr, unsigned int tb_index)
{
  // Map a 4k page from SNC_VA to SNC_PA
  __insn_mtspr(SPR_DTLB_INDEX, tb_index);
  __asm__("drain");
  __insn_mtspr(SPR_DTLB_CURRENT_0, addr | 0x8); // G=1, size=4K
  __insn_mtspr(SPR_DTLB_CURRENT_1, 0); // PFN_HIGH = 0
  __insn_mtspr(SPR_DTLB_CURRENT_3, 0); // LOTAR = 0, not used
  __insn_mtspr(SPR_DTLB_CURRENT_2, addr | (0x37)); // cacheable, not-oloc
  __asm__("drain");
}

// the addr is by default less than 32 bits
void setup_sized_local_page(unsigned int addr, unsigned int tb_index, int page_size)
{
  // Map a 4k page from SNC_VA to SNC_PA
  __insn_mtspr(SPR_DTLB_INDEX, tb_index);
  __asm__("drain");
  __insn_mtspr(SPR_DTLB_CURRENT_0, addr | 0x8 | page_size); // G=1
  __insn_mtspr(SPR_DTLB_CURRENT_1, 0); // PFN_HIGH = 0
  __insn_mtspr(SPR_DTLB_CURRENT_3, 0); // LOTAR = 0, not used
  __insn_mtspr(SPR_DTLB_CURRENT_2, addr | (0x37)); // cacheable, not-oloc
  __asm__("drain");
}

// the addr is by default less than 32 bits, allows programmable page size
void setup_snc_page_extended(unsigned int x, unsigned int y, unsigned int addr, unsigned int tb_index, unsigned int page_size)
{
  // Map a 4k page from SNC_VA to SNC_PA
  __insn_mtspr(SPR_DTLB_INDEX, tb_index);
  __asm__("drain");
  __insn_mtspr(SPR_DTLB_CURRENT_0, addr | 0x8 | page_size); // G=1, size=page_size
  __insn_mtspr(SPR_DTLB_CURRENT_1, 0); // PFN_HIGH = 0
  __insn_mtspr(SPR_DTLB_CURRENT_3, (x<<11)|y); // LOTAR = x y
  __insn_mtspr(SPR_DTLB_CURRENT_2, addr | (0x3b)); // non-cacheable, oloc
  __asm__("drain");
}

// the addr is by default less than 32 bits, allows programmable page size
void setup_local_page_extended(unsigned int addr, unsigned int tb_index, unsigned int page_size)
{
  // Map a 4k page from SNC_VA to SNC_PA
  __insn_mtspr(SPR_DTLB_INDEX, tb_index);
  __asm__("drain");
  __insn_mtspr(SPR_DTLB_CURRENT_0, addr | 0x8 | page_size); // G=1, size=page_size
  __insn_mtspr(SPR_DTLB_CURRENT_1, 0); // PFN_HIGH = 0
  __insn_mtspr(SPR_DTLB_CURRENT_3, 0); // LOTAR = 0, not used
  __insn_mtspr(SPR_DTLB_CURRENT_2, addr | (0x37)); // cacheable, not-oloc
  __asm__("drain");
}


uint32_t get_mshim_location(uint32_t upper_pa_bits) {
  switch (upper_pa_bits) {
  case 0:
    return __insn_mfspr(SPR_CBOX_MMAP_0);
    break;
  case 1:
    return __insn_mfspr(SPR_CBOX_MMAP_1);
    break;
  case 2:
    return __insn_mfspr(SPR_CBOX_MMAP_2);
    break;
  case 3:
    return __insn_mfspr(SPR_CBOX_MMAP_0);
    break;
  default:
    TEST_FAIL(0xdead);
    return 0xffffffff;
  }
}

void get_tile_semaphore(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t data = send_config_read(rshim_x, rshim_y, CFG_TAG, RSHIM_CHANNEL, 0x810);
  while (data & 1) 
  {
    data = send_config_read(rshim_x, rshim_y, CFG_TAG, RSHIM_CHANNEL, 0x810);
  }
}

void release_tile_semaphore(uint32_t rshim_x, uint32_t rshim_y)
{
  // Release the semaphore
  send_config_write(rshim_x, rshim_y, CFG_TAG, RSHIM_CHANNEL, 0x810, 0);
}

void bringup_send_msg(uint32_t rshim_x, uint32_t rshim_y, uint32_t msg_type, uint32_t value)
{

  // Wait for host to indicate it is ready to receive another message
  uint32_t data = send_config_read(rshim_x, rshim_y, CFG_TAG, HPI_CHANNEL, 0xc);
  while (data != 0) 
  {
    data = send_config_read(rshim_x, rshim_y, CFG_TAG, HPI_CHANNEL, 0xc);
  }
  
  // Write the message value
  send_config_write(rshim_x, rshim_y, CFG_TAG, RSHIM_CHANNEL, 0xc, value);
  
  // Write the message type
  send_config_write(rshim_x, rshim_y, CFG_TAG, HPI_CHANNEL, 0xc, (MY_COORD | msg_type));
}

void bringup_msg(uint32_t rshim_x, uint32_t rshim_y, uint32_t msg_type, uint32_t value)
{
  // Send a message to outside world to indicate that the test is done
  
  get_tile_semaphore(rshim_x, rshim_y);

  bringup_send_msg(rshim_x, rshim_y, msg_type, value);

  release_tile_semaphore(rshim_x, rshim_y);
}

void bringup_pass(uint32_t rshim_x, uint32_t rshim_y, uint32_t value)
{
  if (bringup_board()) bringup_msg(rshim_x, rshim_y, 1, value);
}

void bringup_fail(uint32_t rshim_x, uint32_t rshim_y, uint32_t value)
{
  if (bringup_board()) bringup_msg(rshim_x, rshim_y, 2, value);
}

void bringup_done(uint32_t rshim_x, uint32_t rshim_y, uint32_t value)
{
  if (bringup_board()) bringup_msg(rshim_x, rshim_y, 3, value);
}

void bringup_prompt(uint32_t rshim_x, uint32_t rshim_y, uint32_t value)
{
  bringup_msg(rshim_x, rshim_y, 6, value);
}

void bringup_send_msg_string(uint32_t rshim_x, uint32_t rshim_y, char *msg_string, uint32_t lock) {

  uint32_t index = 0;
  uint32_t msg = 0;
  
  if (lock != 0) 
    get_tile_semaphore(rshim_x, rshim_y);

  while (1) {
    
    if ((index & 3) == 0) {
      msg = 0;
      msg = msg_string[index];
    }

    if ((index & 3) == 1) {
      msg |= (msg_string[index] << 8);
    }

    if ((index & 3) == 2) {
      msg |= (msg_string[index] << 16);
    }

    if ((index & 3) == 3) {
      msg |= (msg_string[index] << 24);
    }

    if (((index & 3) == 3) || (msg_string[index] == '\0')) {
      bringup_send_msg(rshim_x, rshim_y, 8, msg);
    }

    if (msg_string[index] == '\0') {
      break;
    }

    index++;
  } 

  if (lock != 0) 
    release_tile_semaphore(rshim_x, rshim_y);
}

int bringup_board(void)
{
#ifdef BRINGUP
  return 1;
#else
  return 0;
#endif
}


void hex_to_ascii(char *string, uint32_t value) {
#ifdef BRINGUP
  uint32_t i;
  string[0] = 48;
  string[1] = 120;
  string[2] = ((value & 0xf0000000) >> 28);
  string[3] = ((value & 0x0f000000) >> 24);
  string[4] = ((value & 0x00f00000) >> 20);
  string[5] = ((value & 0x000f0000) >> 16);
  string[6] = ((value & 0x0000f000) >> 12);
  string[7] = ((value & 0x00000f00) >>  8);
  string[8] = ((value & 0x000000f0) >>  4);
  string[9 ] = ((value & 0x0000000f) >>  0);
  for (i=2; i<10; i++)
    string[i] = (string[i] < 0xa) ? (string[i] + 48) : (string[i] + 87);
  string[10] = '\0';
#endif
}

void str_append(char *base_str, char *append_str) {
#ifdef BRINGUP
  uint32_t base_index = 0;
  uint32_t append_index = 0;

  while (base_str[base_index++] != '\0');
  base_index--;
  while (append_str[append_index] != '\0')
    base_str[base_index++] = append_str[append_index++];
  base_str[base_index] = '\0';
#endif
}

void str_append_int(char *base_str, uint32_t value) {
#ifdef BRINGUP
  char temp_str[32] = " ";
  hex_to_ascii(temp_str,value);
  str_append(base_str,temp_str);
#endif
}

void print(char *msg_str) {
#ifdef BRINGUP
  TEST_MSG_STRING(msg_str, 1);
#endif
}

void print2(char *msg_str, uint32_t value) {
#ifdef BRINGUP
  char base_str[1024];
  base_str[0] = '\0';
  str_append(base_str,msg_str);
  str_append_int(base_str,value);
  TEST_MSG_STRING(base_str, 1);
#endif
}

void print4(char *msg_str0, uint32_t value0, char *msg_str1, uint32_t value1) {
#ifdef BRINGUP
  char base_str[1024];
  base_str[0] = '\0';
  str_append(base_str,msg_str0);
  str_append_int(base_str,value0);
  str_append(base_str,msg_str1);
  str_append_int(base_str,value1);
  TEST_MSG_STRING(base_str, 1);
#endif
}

void print6(char *msg_str0, uint32_t value0, char *msg_str1, uint32_t value1, char *msg_str2, uint32_t value2) {
#ifdef BRINGUP
  char base_str[1024];
  base_str[0] = '\0';
  str_append(base_str,msg_str0);
  str_append_int(base_str,value0);
  str_append(base_str,msg_str1);
  str_append_int(base_str,value1);
  str_append(base_str,msg_str2);
  str_append_int(base_str,value2);
  TEST_MSG_STRING(base_str, 1);
#endif
}
//<MLS 20100923>

void print8(char *msg_str0, uint32_t value0, char *msg_str1, uint32_t value1, char *msg_str2, uint32_t value2, char *msg_str3, uint32_t value3) {
#ifdef BRINGUP
  char base_str[1024];
  base_str[0] = '\0';
  str_append(base_str,msg_str0);
  str_append_int(base_str,value0);
  str_append(base_str,msg_str1);
  str_append_int(base_str,value1);
  str_append(base_str,msg_str2);
  str_append_int(base_str,value2);
  str_append(base_str,msg_str3);
  str_append_int(base_str,value3);
  TEST_MSG_STRING(base_str, 1);

#endif
}

void print10(char *msg_str0, uint32_t value0, char *msg_str1, uint32_t value1, char *msg_str2, uint32_t value2, char *msg_str3, uint32_t value3, char *msg_str4, uint32_t value4) {
#ifdef BRINGUP
  char base_str[1024];
  base_str[0] = '\0';
  str_append(base_str,msg_str0);
  str_append_int(base_str,value0);
  str_append(base_str,msg_str1);
  str_append_int(base_str,value1);
  str_append(base_str,msg_str2);
  str_append_int(base_str,value2);
  str_append(base_str,msg_str3);
  str_append_int(base_str,value3);
  str_append(base_str,msg_str4);
  str_append_int(base_str,value4);
  TEST_MSG_STRING(base_str, 1);
#endif
}
//<M/LS 20100923>
void print_nolock(char *msg_str) {
#ifdef BRINGUP
  TEST_MSG_STRING(msg_str, 0);
#endif
}

void print2_nolock(char *msg_str, uint32_t value) {
#ifdef BRINGUP
  char base_str[1024];
  base_str[0] = '\0';
  str_append(base_str,msg_str);
  str_append_int(base_str,value);
  TEST_MSG_STRING(base_str, 0);
#endif
}

void print4_nolock(char *msg_str0, uint32_t value0, char *msg_str1, uint32_t value1) {
#ifdef BRINGUP
  char base_str[1024];
  base_str[0] = '\0';
  str_append(base_str,msg_str0);
  str_append_int(base_str,value0);
  str_append(base_str,msg_str1);
  str_append_int(base_str,value1);
  TEST_MSG_STRING(base_str, 0);
#endif
}

void print6_nolock(char *msg_str0, uint32_t value0, char *msg_str1, uint32_t value1, char *msg_str2, uint32_t value2) {
#ifdef BRINGUP
  char base_str[1024];
  base_str[0] = '\0';
  str_append(base_str,msg_str0);
  str_append_int(base_str,value0);
  str_append(base_str,msg_str1);
  str_append_int(base_str,value1);
  str_append(base_str,msg_str2);
  str_append_int(base_str,value2);
  TEST_MSG_STRING(base_str, 0);
#endif
}

#endif //__DVLIB_C__
