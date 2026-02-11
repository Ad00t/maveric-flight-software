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

#include "dvlib.h"
#include "i2c_test_funcs.h"

void i2cm_wait_for_not_busy(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
  while (flag & 1)
  {
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
  }
}

void i2cm_wait_for_wfifo_not_full(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
  while (flag & 0x40)
  {
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
  }
}

void i2cm_wait_for_rfifo_not_empty(uint32_t rshim_x, uint32_t rshim_y)
{
  uint32_t flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
  while (flag & 0x4)
  {
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
  }
}



void i2c_config(uint32_t rshim_x, uint32_t rshim_y)
{
  // Change the write cycle time to be fast write cycle
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x6c, 0);
  
  // Configure a slave address for the master to use
  // Should be same as the slave's powerup value
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x60, 0x08 << 1);

  // Configure the prescalers to speed things up
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x68, 5);
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x68, 5);
  
  // Write the byte register
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x4c, 4);
  
  // Write the address register
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x48, (I2CS_CHANNEL << 13) | 0x40); // I2C slave receive fifo
  
  // Write the instruction register
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x50, 1); // write
}


void i2c_send_word(uint32_t rshim_x, uint32_t rshim_y, uint32_t data)
{
  // Wait for the write fifo to not be full
  uint32_t flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
  while (flag & 0x40)
  {
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
  }
  
  // Write the write fifo
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x40, data);
}


uint32_t i2c_check_word(uint32_t rshim_x, uint32_t rshim_y, uint32_t data)
{
  uint32_t fail_count = 0;

  // Wait for receive buffer to be non-empty
  uint32_t flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x24);
  while ((flag & 0x2) != 0)
  {
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x24);
  }
  
  // Get it
  print("Testing I2C i2c check word reading 0x44");
  uint32_t rd_data = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x44);
  print2("Testing I2C i2c check word read 0x44", rd_data);
  
  // Check it
  if (rd_data != data)
  {
    TEST_FAIL(rd_data);
    fail_count++;
  }

  return(fail_count);
}

void i2cm_start_write(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t size)
{
    i2cm_wait_for_not_busy(rshim_x, rshim_y);

    // Write the byte register
    send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x4c, size);
    
    // Write the address register
    send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x48, addr);
    
    // Write the instruction register
    send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x50, 1); // write
}

void i2cm_write_bytes(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t data, uint32_t size)
{
  i2cm_start_write(rshim_x, rshim_y, addr, size);
  i2cm_wait_for_wfifo_not_full(rshim_x, rshim_y);
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x40, data << ((addr & 3) * 8));
}

void i2cm_write_byte(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, unsigned char data)
{
  i2cm_write_bytes(rshim_x, rshim_y, addr, data, 1);
}

void i2cm_write(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t* wr_data, uint32_t num_words, uint32_t page_size)
{

  uint32_t page_addr_mask = (page_size*4) - 1;

  uint32_t next_index = 0;
  uint32_t next_addr = addr;
  int words_left = num_words;

  uint32_t words_to_end_of_page;
  uint32_t wr_size;

  while (words_left > 0)
  {
    // Calculate the size of this write
    // It will be either a full page or up to the page boundary
    words_to_end_of_page = page_size - ((next_addr & page_addr_mask) >> 2);
    wr_size = (page_size > words_to_end_of_page) ? words_to_end_of_page : page_size;
    if (wr_size > words_left)
    {
      wr_size = words_left;
    }
    
    i2cm_start_write(rshim_x, rshim_y, next_addr, wr_size*4);

    // Write the write fifo
    for (int i = 0; i < wr_size; i++)
    {
      i2cm_wait_for_wfifo_not_full(rshim_x, rshim_y);
      send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x40, wr_data[next_index + i]);
    }

    // Update the number of words left and the next start address
    words_left = words_left - wr_size;
    next_addr = next_addr + (wr_size*4);
    next_index = next_index + wr_size;
  }
}

void i2cm_start_read(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t size)
{
  i2cm_wait_for_not_busy(rshim_x, rshim_y);
  
  // Write the byte register
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x4c, size);
  
  // Write the address register
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x48, addr);
  
  // Change the instruction to a read
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x50, 0); // read
}

uint32_t i2cm_read_bytes(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t size)
{
  i2cm_start_read(rshim_x, rshim_y, addr, size);
  i2cm_wait_for_rfifo_not_empty(rshim_x, rshim_y);
  return(send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x44) >> ((addr & 3)*8));
}

unsigned char i2cm_read_byte(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr)
{
  return i2cm_read_bytes(rshim_x, rshim_y, addr, 1);
}

void i2cm_read(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t* rd_data, uint32_t num_words)
{
  uint32_t words_left = num_words;
  uint32_t next_addr = addr;
  uint32_t next_index = 0;

  uint32_t rd_size;

  while (words_left)
  {
    rd_size = (words_left < (8*1024)) ? words_left : (8*1024);

    i2cm_start_read(rshim_x, rshim_y, next_addr, rd_size*4);

    for (int i = 0; i < rd_size; i++)
    {
      i2cm_wait_for_rfifo_not_empty(rshim_x, rshim_y);
      rd_data[next_index + i] = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x44);
    }

    words_left = words_left - rd_size;
    next_index = next_index + rd_size;
    next_addr = next_addr + (rd_size*4);
  }

}

uint32_t i2cm_probe_device(uint32_t rshim_x, uint32_t rshim_y, uint32_t slave_addr, uint32_t addr, uint32_t size, uint32_t use_16_bit_addr)
{
  uint32_t reg;
  uint32_t flag;
  uint32_t timeout;
  uint32_t timeout_init = 10000;

  // Start by assuming the probe will be successful
  // If anything fails, this will be cleared below
  uint32_t pass = 1;

  // Poll waiting for I2CM to not be busy
  flag = 0xffffffff;
  timeout = 1000;
  while ((flag & 1) && (timeout > 0)) {
    print("waiting for I2CM");
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
    timeout = timeout - 1;
  }
  
  // If we timed out, that's an error
  if (timeout == 0) {
    pass = 0;
  } else {
    
    // Clear the interrupt status register and make sure it clears
    send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x80, 0xffffffff);
    reg = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x80);
    if (reg != 0) {
      pass == 0;
    } else {
      
      // Set up 16-bit addressing
      send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x64, use_16_bit_addr);
      
      // Set up the slave address
      send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x60, slave_addr << 1);
      
      // Set up the prescaler to run at 200 KHz
      send_config_write(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x68, 250);
      
      // Start a read
      i2cm_start_read(rshim_x, rshim_y, addr, size);
      

      // Poll waiting for read data to return
      flag = 0xffffffff;
      timeout = timeout_init;
      while ((flag & 4) && (timeout > 0)) {
        flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x24);
        timeout = timeout - 1;
      }
      
      // If we timed out, that's an error
      if (timeout == 0) {
        pass = 0;
      } else {
        // Get the data
        reg = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x44);
      }

      // Read the interrupt status register and make sure that only the bits we expect to be set are set
      reg = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CM_CHANNEL, 0x80);
      if (reg != 0x140) {
        pass == 0;
      }
    }
  }

  return pass;
}

// MLS
// i2cs
//
//
/************************************************
       I2CS  READ

************************************************/

void i2cs_read(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t* rd_data, uint32_t num_words) {
  uint32_t words_left = num_words;
  uint32_t next_addr = addr;
  uint32_t next_index = 0;

  uint32_t rd_size;

  while (words_left) {
    rd_size = (words_left < (8*1024)) ? words_left : (8*1024);

    i2cs_start_read(rshim_x, rshim_y, next_addr, rd_size*4);

    for (int i = 0; i < rd_size; i++) {
      i2cs_wait_for_rfifo_not_empty(rshim_x, rshim_y);
      rd_data[next_index + i] = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x44);
    }

    words_left = words_left - rd_size;
    next_index = next_index + rd_size;
    next_addr = next_addr + (rd_size*4);
  }

}

void i2cs_start_read(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t size) {
  i2cs_wait_for_not_busy(rshim_x, rshim_y);
  
  // Write the byte register
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x4c, size);
  
  // Write the address register
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x48, addr);
  
  // Change the instruction to a read
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x50, 0); // read
}

void i2cs_wait_for_not_busy(uint32_t rshim_x, uint32_t rshim_y) {
  uint32_t flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x24);

  while (flag & 1) {
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x24);
  }
}

void i2cs_wait_for_rfifo_not_empty(uint32_t rshim_x, uint32_t rshim_y) {
  uint32_t flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x24);
  while (flag & 0x4) {
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x24);
  }
}
uint32_t i2cs_read_bytes(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t size) {
  i2cs_start_read(rshim_x, rshim_y, addr, size);
  i2cs_wait_for_rfifo_not_empty(rshim_x, rshim_y);
  return(send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x44) >> ((addr & 3)*8));
}

unsigned char i2cs_read_byte(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr) {
  return i2cs_read_bytes(rshim_x, rshim_y, addr, 1);
}


/************************************************
       I2CS  WRITE

************************************************/
void i2cs_write(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t* wr_data, uint32_t num_words, uint32_t page_size) {

  uint32_t page_addr_mask = (page_size*4) - 1;

  uint32_t next_index = 0;
  uint32_t next_addr = addr;
  int words_left = num_words;

  uint32_t words_to_end_of_page;
  uint32_t wr_size;

  while (words_left > 0)
  {
    // Calculate the size of this write
    // It will be either a full page or up to the page boundary
    words_to_end_of_page = page_size - ((next_addr & page_addr_mask) >> 2);
    wr_size = (page_size > words_to_end_of_page) ? words_to_end_of_page : page_size;
    if (wr_size > words_left) {
      wr_size = words_left;
    }
    
    i2cs_start_write(rshim_x, rshim_y, next_addr, wr_size*4);

    // Write the write fifo
    for (int i = 0; i < wr_size; i++) {
      i2cs_wait_for_wfifo_not_full(rshim_x, rshim_y);
      send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x40, wr_data[next_index + i]);
    }

    // Update the number of words left and the next start address
    words_left = words_left - wr_size;
    next_addr = next_addr + (wr_size*4);
    next_index = next_index + wr_size;
  }
}


void i2cs_start_write(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t size) {
    i2cs_wait_for_not_busy(rshim_x, rshim_y);

    // Write the byte register
    send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x4c, size);
    
    // Write the address register
    send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x48, addr);
    
    // Write the instruction register
    send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x50, 1); // write
}


void i2cs_wait_for_wfifo_not_full(uint32_t rshim_x, uint32_t rshim_y) {
  uint32_t flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x24);
  while (flag & 0x40) {
    flag = send_config_read(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x24);
  }
}


void i2cs_write_bytes(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, uint32_t data, uint32_t size) {
  i2cs_start_write(rshim_x, rshim_y, addr, size);
  i2cs_wait_for_wfifo_not_full(rshim_x, rshim_y);
  send_config_write(rshim_x, rshim_y, CFG_TAG, I2CS_CHANNEL, 0x40, data << ((addr & 3) * 8));
}

void i2cs_write_byte(uint32_t rshim_x, uint32_t rshim_y, uint32_t addr, unsigned char data) {
  i2cs_write_bytes(rshim_x, rshim_y, addr, data, 1);
}

