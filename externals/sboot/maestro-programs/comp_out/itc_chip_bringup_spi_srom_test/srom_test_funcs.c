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


////////////////////////////////////////////////////////////////////////
//
// This file should NOT include testbench_defs.h.
//
// Any testbench-specific parameters should be passed to functions as
// arguments to functions, not by #defines
//
////////////////////////////////////////////////////////////////////////

#include "itc_dvlib.h"
#include "srom_test_funcs.h"

void wait_for_srom_wfifo_empty(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t flags = send_config_read(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x24);
  while ((flags & 0x8) == 0)
  {
    flags = send_config_read(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x24);
  }
}

void wait_for_srom_not_busy(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t flags = send_config_read(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x24);
  while ((flags & 0x1) != 0)
  {
    flags = send_config_read(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x24);
  }
}

void wait_for_srom_rfifo_not_empty(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t flags = send_config_read(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x24);
  while ((flags & 0x4) != 0)
  {
    flags = send_config_read(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x24);
  }
}

uint32_t srom_get_read_data(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t rd_data;
  wait_for_srom_rfifo_not_empty(rshim_x, rshim_y);
  rd_data = send_config_read(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x44);
  return(rd_data);
}

void srom_do_instr(uint32_t rshim_x, uint32_t rshim_y, uint32_t instr)
{
  wait_for_srom_not_busy(rshim_x, rshim_y);
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x50, instr);
}

uint32_t srom_read_status(uint32_t rshim_x, uint32_t rshim_y)
{
  srom_do_instr(rshim_x, rshim_y, 5);
  return(srom_get_read_data(rshim_x, rshim_y));
}; // read_srom_statu...

void srom_wait_for_wip_clear(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t status = srom_read_status(rshim_x, rshim_y);

  while ((status & 1) == 1)
  {
    status = srom_read_status(rshim_x, rshim_y);
  }
}
   
void srom_exec_inst(uint32_t rshim_x, uint32_t rshim_y, uint32_t inst)
{
  srom_wait_for_wip_clear(rshim_x, rshim_y);
  srom_do_instr(rshim_x, rshim_y, inst);
};

uint32_t srom_read_id(uint32_t rshim_x, uint32_t rshim_y)
{
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x4c, 3); // byte count
  srom_exec_inst(rshim_x, rshim_y, 0x9f);
  return(srom_get_read_data(rshim_x, rshim_y));
}

void srom_write_enable(uint32_t rshim_x, uint32_t rshim_y)
{   
  srom_exec_inst(rshim_x, rshim_y, 6);
}
      
uint32_t srom_read_sig(uint32_t rshim_x, uint32_t rshim_y)
{
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x4c, 3); // byte count
  srom_do_instr(rshim_x, rshim_y, 0xab);
  return(srom_get_read_data(rshim_x, rshim_y));
}

void srom_sector_erase(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr)
{
  srom_wait_for_wip_clear(rshim_x, rshim_y);

  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x48, addr);

  srom_exec_inst(rshim_x, rshim_y, 0xd8);
}      
      
void srom_write(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t data)
{
      
  srom_wait_for_wip_clear(rshim_x, rshim_y);
         
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x48, addr);

  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x4c, 4); // byte count

  srom_exec_inst(rshim_x, rshim_y, 2);
      
  wait_for_srom_wfifo_empty(rshim_x, rshim_y);
         
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x40, data);
}

void srom_write_block(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t* data, uint32_t size)
{
      
  srom_wait_for_wip_clear(rshim_x, rshim_y);
         
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x48, addr);

  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x4c, size*4);

  srom_exec_inst(rshim_x, rshim_y, 2);

  for (int i = 0; i < size; i++)
  {
    wait_for_srom_wfifo_empty(rshim_x, rshim_y);
    send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x40, data[i]);
  }
}

uint32_t srom_read(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr)
{
  srom_wait_for_wip_clear(rshim_x, rshim_y);            
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x48, addr);
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x4c, 4); // byte count
  srom_exec_inst(rshim_x, rshim_y, 3);

  return(srom_get_read_data(rshim_x, rshim_y));
}
   

void srom_read_block(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t size, uint32_t* rd_data_p)
{
  srom_wait_for_wip_clear(rshim_x, rshim_y);
            
  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x48, addr);

  send_config_write(rshim_x, rshim_y, CFG_TAG, SROM_CHANNEL, 0x4c, size*4);

  srom_exec_inst(rshim_x, rshim_y, 3);

  for (int i = 0; i < size; i++)
  {
    rd_data_p[i] = srom_get_read_data(rshim_x, rshim_y);
    // print4("DBG:srom_read_block rd_data_p loop ",i ," value = ", rd_data_p[i] );
  }
}


uint32_t srom_test(uint32_t rshim_x, uint32_t rshim_y, uint32_t passes)
{
  uint32_t fail_count = 0;
  uint32_t rd_data;

  //
  // Read the electronic signature
  //
  uint32_t sig_value;

#ifdef BRINGUP_BOARD
  sig_value = 0x16;
#else
  sig_value = 0x05;
#endif
  print("Start srom_read_sig");
  rd_data = srom_read_sig(rshim_x, rshim_y);
  TEST_PASS(rd_data);
  if (rd_data != sig_value)
  {
    //    TEST_PASS(0xbaddeed1);
    //    fail_count++;
  }

  //
  // Read the ID
  //
  uint32_t id_value;

#ifdef BRINGUP_BOARD
  id_value = 0x10216;
#else
  id_value = 0x00202010;
#endif

  print("Start srom_read_id");
  rd_data = srom_read_id(rshim_x, rshim_y);
  TEST_PASS(rd_data);
  if (rd_data != id_value)
  {
    //    TEST_PASS(0xbaddeed2);
    //    fail_count++;
  }

  // Figure out if the ROM contains valid boot segments
  // Start by assuming that it does
  uint32_t valid_boot_segments = 1;

  // Read the first word and figure out if this might be a revision number with reserved field
  uint32_t rd_data_block[2];
  uint32_t addr = 0;
  print("Start srom_read_block");
  srom_read_block(rshim_x, rshim_y, addr, 1, rd_data_block);
  TEST_PASS(rd_data_block[0]);
  addr += 4;

  // If the reserved fields of the first word are 0, this might be a bootable image
  // FIXME - double check that we will really zero out the reserved field
  if ((rd_data_block[0] & 0xffffff00) == 0)
  {
    // Keep checking for boot segments until we find something invalid
    // or find the final segment
    uint32_t done = 0;
    while (valid_boot_segments & !done)
    {
      srom_read_block(rshim_x, rshim_y, addr, 1, rd_data_block);
      TEST_PASS(rd_data_block[0]);
      // If the reserved fields are 0, this might be a valid segment header
      // FIXME - double check that we will really zero out the reserved field
      if ((rd_data_block[0] & 0x00008000) == 0)
      {
        // If the word count is greater than 1, this might be a valid header
        // FIXME - double check that there will never be a segment header with no data
        uint32_t word_count = rd_data_block[0] & 0x00007fff;
        if (word_count > 1)
        {
          // If the last segment bit is set, we're done
          if ((rd_data_block[0] & 0x80000000) == 0x80000000)
          {
            done = 1;
          }
          else
          {
            addr += (word_count*4);
          }
        }
        else
        {
          done = 1;
          print2("Found impossible word count ", word_count);
          valid_boot_segments = 0;
        }
      }
      else
      {
        done = 1;
        print("Found non-zero reserved field in segment header");
        valid_boot_segments = 0;
      }
    }
  }
  else
  {
    print("Found non-zero reserved field in boot block header");
    valid_boot_segments = 0;
  }

  if (valid_boot_segments)
  {
    print("Found valid boot segments in SPI SROM.  Destructive test will NOT be run.");
  }
  else
  {
    print("Did NOT find valid boot segments in SPI SROM.  Destructive test will be run.");

    //
    // Write and read the srom
    //
    addr = 0;
    uint32_t mask = 0;
    
    uint32_t wr_data_block[2];
    
    for (int pass = 0; pass < passes; pass++)
    {
      // Erase the sector 
      srom_write_enable(rshim_x, rshim_y);
      srom_sector_erase(rshim_x, rshim_y, 0);
      
      // Read after erase and see that things are erased
      srom_read_block(rshim_x, rshim_y, addr, 2, rd_data_block);
      for (int i = 0; i < 2; i++)
      {
        if (rd_data_block[i] != 0xffffffff)
        {
          TEST_PASS(0xbaddeed3);
          TEST_PASS(addr);
          TEST_PASS(0xffffffff);
          TEST_PASS(rd_data_block[i]);
          fail_count++;
        }
      }
      
      // Write some locations
      wr_data_block[0] = 0x08040201 ^ mask;
      wr_data_block[1] = 0x80402010 ^ mask;
      srom_write_enable(rshim_x, rshim_y);
      srom_write_block(rshim_x, rshim_y, addr, wr_data_block, 2);
      
      // Read those locations
      srom_read_block(rshim_x, rshim_y, addr, 2, rd_data_block);
      
      for (int i = 0; i < 2; i++)
      {
        if (rd_data_block[i] != wr_data_block[i])
        {
          TEST_PASS(0xbaddeed3);
          TEST_PASS(addr);
          TEST_PASS(wr_data_block[i]);
          TEST_PASS(rd_data_block[i]);
          fail_count++;
        }
      }
      
      mask = mask ^ 0xffffffff;
    }

  }

  return(fail_count);
}
