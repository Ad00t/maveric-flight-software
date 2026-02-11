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
//   This test configures gbe_phy and gbe shim for channels 0 and 1.
// 
// Loopback packets in the external PHYs.
// run >> execfile("itc_bringup_gbe_phy_test.py")
//  
#include <sys/archlib.h>  
#include "enet.h"
#include "enet_defs.h"
#include "itc_dvlib.h"
//<MLS-DEBUG> Added for debug addresses
#include "gsh_defs.h"
//</MLS-DEBUG>
#define PACKET_COUNT 0x2000

#define GBE0_TILE_X (CORNER_X + 2)
#define GBE0_TILE_Y (CORNER_Y + 2)
#define GBE1_TILE_X (CORNER_X + 5)
#define GBE1_TILE_Y (CORNER_Y + 5)

int main() {

  uint32_t loopCount0, loopCount1;
  uint32_t phySpeed0, phySpeed1;
  uint32_t phyLoopback0, phyLoopback1;
  uint32_t phyDelay0, phyDelay1;
  uint32_t phyType0, phyType1;
  uint32_t phyAddr0, phyAddr1;
  uint32_t pktSize0 = 96;
  uint32_t pktSize1 = 96; 
  uint32_t ctlWord, ctlWord0, ctlWord1;
  uint32_t mshim_x = X_FROM_COORD(__insn_mfspr(SPR_CBOX_MMAP_0));
  uint32_t mshim_y = Y_FROM_COORD(__insn_mfspr(SPR_CBOX_MMAP_0));

  // Carve out some memory for packet buffers.
  unsigned char *txBuf0 = (unsigned char *) 0x400000;
  unsigned char *rxBuf0 = (unsigned char *) 0x410000;
  unsigned char *txBuf1 = (unsigned char *) 0x420000;
  unsigned char *rxBuf1 = (unsigned char *) 0x430000;

  TEST_BEGIN;

  switch (MY_ID) {

  case MASTER_ID: 

    // Control word passed to test:
    //
    //   ctlWord[1:0]   = Gbe_0 PHY loopback_mode (0=none, 1=PHY_LOOPBACK)
    //   ctlWord[3:2]   = Gbe_0 PHY speed (0=auto, 1=10M, 2=100M, 3=1G)
    //   ctlWord[5:4]   = Gbe_0 PHY signal delay (0=none, 1=Rx, 2=Tx, 3=Rx/Tx)
    //   ctlWord[9:6]   = Gbe_0 PHY type (1=MV88E1111, 2=BCM5481)
    //   ctlWord[15:10] = Gbe_0 PHY addr
    //
    //   ctlWord[17:16] = Gbe_1 PHY loopback_mode (0=none, 1=PHY_LOOPBACK)
    //   ctlWord[19:18] = Gbe_1 PHY loopback_mode (0=none, 1=10M, 2=100M, 3=1G)
    //   ctlWord[21:20] = Gbe_1 PHY signal delay (0=none, 1=Rx, 2=Tx, 3=Rx/Tx)
    //   ctlWord[25:22] = Gbe_1 PHY type (1=MV88E1111, 2=BCM5481)
    //   ctlWord[31:26] = Gbe_1 PHY addr
    print("Master tile 1, 4 start the gbe test");
    ctlWord = iodn0_receive();
    ctlWord = iodn0_receive();
    udn_send_msg(GBE0_TILE_X, GBE0_TILE_Y, UDN_TAG_1, (ctlWord & 0xffff));
    udn_send_msg(GBE1_TILE_X, GBE1_TILE_Y, UDN_TAG_1, (ctlWord >> 16));
    break;   

  //
  // Start Gbe channel 0
  //
  case TILE_ID(GBE0_TILE_X, GBE0_TILE_Y): 

    gbe_config(ENET_GBE_0_IF);
    // Get the control word from the master.
    ctlWord0 = udn1_receive();
    phyLoopback0 = ((ctlWord0 >> 0) & 0x3);
    phySpeed0 = ((ctlWord0 >> 2) & 0x3);
    phyDelay0 = ((ctlWord0 >> 4) & 0x3);
    phyType0 = ((ctlWord0 >> 6) & 0xf);
    phyAddr0 = ((ctlWord0 >> 10) & 0x1f);
    
    if (phySpeed0 == ENET_1G) 
      send_config_write(GBE0_IDN1_X, GBE0_IDN1_Y, CFG_TAG, 0, GEM_NETWORK_CONFIG,  0x00120412);

    else if (phySpeed0 == ENET_100M) 
      send_config_write(GBE0_IDN1_X, GBE0_IDN1_Y, CFG_TAG, 0, GEM_NETWORK_CONFIG,  0x00120013);

    else if (phySpeed0 == ENET_10M) 
      send_config_write(GBE0_IDN1_X, GBE0_IDN1_Y, CFG_TAG, 0, GEM_NETWORK_CONFIG,  0x00120012);

    else
      break;

    // Configure the PHY.
    gbe_phy_config(ENET_GBE_0_IF, phyLoopback0, phySpeed0, phyDelay0, phyType0, phyAddr0);

    // If we get here then nothing stuck in PHY communication, we are done.
    TEST_PASS(0);

    break;

  //
  // Start Gbe channel 1.
  //
  case TILE_ID(GBE1_TILE_X, GBE1_TILE_Y): 

    print("Gbe 1: loopback test started");
    gbe_config(ENET_GBE_1_IF);
    print("Gbe 1: gbe_config completed");

    // Get the control word from the master.
    ctlWord1 = udn1_receive();
    phyLoopback1 = ((ctlWord1 >> 0) & 0x3);
    phySpeed1 = ((ctlWord1 >> 2) & 0x3);
    phyDelay1 = ((ctlWord1 >> 4) & 0x3);
    phyType1 = ((ctlWord1 >> 6) & 0xf);
    phyAddr1 = ((ctlWord1 >> 10) & 0x1f);

    print2("Gbe 1: phySpeed1 = ", phySpeed1);
    print2("Gbe 1: phyType1 = ", phyType1);
    if (phySpeed1 == ENET_1G) 
      send_config_write(GBE1_IDN1_X, GBE1_IDN1_Y, CFG_TAG, 1, GEM_NETWORK_CONFIG,  0x00120412);

    else if (phySpeed1 == ENET_100M) 
      send_config_write(GBE1_IDN1_X, GBE1_IDN1_Y, CFG_TAG, 1, GEM_NETWORK_CONFIG,  0x00120013);

    else if (phySpeed1 == ENET_10M) 
      send_config_write(GBE1_IDN1_X, GBE1_IDN1_Y, CFG_TAG, 1, GEM_NETWORK_CONFIG,  0x00120012);

    else 
      break;

    print("Gbe 1: gbe_phy_config started");
    // Configure the PHY.
    gbe_phy_config(ENET_GBE_1_IF, phyLoopback1, phySpeed1, phyDelay1, phyType1, phyAddr1);

    // If we get here then nothing stuck in PHY communication, we are done.
    TEST_PASS(0);


    break;
  }

  TEST_END(0);
}
