/*
 * 
 *  RUN: itc_chip_i2c_temp_sense_test(compile_test=True)
 */


#include "testbench_defs.h"
#include <sys/archlib.h>
#include "dvlib.h"
#include "i2c_test_funcs.h"

#define PAGE_SIZE (128)

//<MLS> #define ROM_SIZE (64*1024)
#define ROM_SIZE (512*128)

#define START_ADDR 0
// #define NUM_WORDS ((64*1024/4) - 0)
#define NUM_WORDS 64
#define NUM_PASSES 2
#define MAX_ERRORS 5

#define MANUFACTURE_ID_ADDR 0xFE
#define REVISION_ADDR 0xFF

#define MANUFACTURE_ID_VALUE 0x01
#define REVISION_VALUE 0xB1

uint32_t data_id;
uint32_t data_rev;

int main()
{
  // Receive arguments in a raw IDN packet
  // Must consume the whole packet before trying to print anything
  uint32_t foo = iodn0_receive(); // throw away raw pkt header
  uint32_t slave_addr = iodn0_receive();
  print2("Testing I2C Tempurature sensor using slave address ", slave_addr);

  uint32_t fail_count = 0;
  uint32_t temp_val = 0;

  if (i2cm_probe_device(RSH0_IDN_X, RSH0_IDN_Y, slave_addr, 0, 1, 0)) {
    print4("i2cm_probe_device found R_X ", RSH0_IDN_X , " R_Y ", RSH0_IDN_Y);

    // Read Part info
    // Setup 8 bit addressing
    send_config_write(RSH0_IDN_X, RSH0_IDN_Y, CFG_TAG, I2CM_CHANNEL, 0x64, 0x0);

    temp_val = send_config_read(RSH0_IDN_X, RSH0_IDN_Y, CFG_TAG, I2CM_CHANNEL, 0x68);

 
    data_id = i2cm_read_byte(RSH0_IDN_X, RSH0_IDN_Y, MANUFACTURE_ID_ADDR);


    print4("MANUFACTURE_ID ", data_id, " from address ", MANUFACTURE_ID_ADDR);
    if ( data_id != MANUFACTURE_ID_VALUE) {
      fail_count++;
    }

    data_rev = i2cm_read_byte(RSH0_IDN_X, RSH0_IDN_Y, REVISION_ADDR);
    print4("REVISION ", data_rev, " from address " , REVISION_ADDR);  
    if ( data_rev != REVISION_VALUE) {
      fail_count++;
    }
  }
  else
  {
    // Probe failed
    fail_count++;
    print2(" fail_count_inc ", fail_count);
  }

  TEST_DONE(fail_count);
  
  idle();
}

