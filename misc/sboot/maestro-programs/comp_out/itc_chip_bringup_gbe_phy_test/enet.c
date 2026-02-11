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

//
// A collection of functions related Ethernet/MDIO.
//
#include "dvlib.h"
#include "enet_defs.h"
#include "gsh_defs.h"
#include "enet.h"
#include "tb1_rsh_jtag.h"

//
// TLR Ethernet functions.
//
void gen_and_send_pkt(uint32_t enet_if,  
                      unsigned char * txBuf, 
                      uint32_t pktSize) {

  DmaSendEgNoNotifPkt eDmaPkt;
  uint32_t i;
  static char startByte;
  uint32_t x, y, channel;
  volatile uint32_t tmpData;

  // Enet interface.
  if (enet_if == ENET_XGBE_0_IF) {
    x = XGBE0_IDN1_X;
    y = XGBE0_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_XGBE_1_IF) {
    x = XGBE1_IDN1_X;
    y = XGBE1_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_GBE_0_IF) {
    x = GBE0_IDN1_X;
    y = GBE0_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_GBE_1_IF) {
    x = GBE1_IDN1_X;
    y = GBE1_IDN1_Y;
    channel = 1;
  } else {
    print("gen_and_send_pkt(): invalid Ethernet interface specified");
    TEST_FAIL(0xdead0000);
  }

  // Route header.
  eDmaPkt.bits.routeHdr.length = 5;
  eDmaPkt.bits.routeHdr.dest_x = x;
  eDmaPkt.bits.routeHdr.dest_y = y;
  eDmaPkt.bits.routeHdr.fbit = 1;
  eDmaPkt.bits.routeHdr.rsvd = 0;
  
  // Shim header
  eDmaPkt.bits.channel = channel;
  eDmaPkt.bits.iodn_type = IODN_EGRESS_SEND_PKT;
  eDmaPkt.bits.ack = 1;
  eDmaPkt.bits.rsvd1 = 0;
  eDmaPkt.bits.notify = 0;
  eDmaPkt.bits.boundary = 1;
  eDmaPkt.bits.rsvd2 = 0;
  eDmaPkt.bits.m = 1;
  eDmaPkt.bits.rsvd3 = 0;
  eDmaPkt.bits.xsize = pktSize;
  
  eDmaPkt.bits.lo_addr =  (uint32_t) txBuf;
  
  eDmaPkt.bits.hi_addr = 0;
  eDmaPkt.bits.dsize = 0;
  eDmaPkt.bits.dctl = 0;
  
  // From header
  eDmaPkt.bits.s_channel = channel;
  eDmaPkt.bits.source_x = MY_X;
  eDmaPkt.bits.source_y = MY_Y;
  eDmaPkt.bits.source_fbit = 0;
  eDmaPkt.bits.rsvd5 = 0;
  
  eDmaPkt.bits.s_source_tag = ENET_IDN_TAG;

  // Create the packet data pattern
  for (i=0; i<pktSize; i++) {
    txBuf[i] = startByte+i;
  }
  startByte++;

  // Write the pattern from cache to memory
  for (i=0; i<pktSize; i++) {
    if (((i % 16) == 0) || (i == (pktSize-1))) {
      __insn_finv(txBuf+i);
    }
  }

  // Wait for the memory operations to complete
  __insn_mf();

  // Additional operations for Wachusett to guarantee
  // memory operations are complete.
  if (MY_REV == WACHUSETT) {
    tmpData = txBuf[0];
    __insn_mf();
  }

  // Send the eDMA command
  iodn_send_pkt(eDmaPkt.word, 5);  
}

void sync_and_check_pkt(uint32_t enet_if, 
                        unsigned char * rxBuf, 
                        unsigned char * txBuf, 
                        uint32_t pktSize) {

  uint32_t i;
  uint32_t idnType, idnStatus, idnLen, idnData, idnHdrBytes;
  uint32_t x, y, channel; 
  uint32_t noAck, noNotif, noAvail;

  DmaSendInPkt iDmaPkt;

  // Enet interface.
  if (enet_if == ENET_XGBE_0_IF) {
    x = XGBE0_IDN1_X;
    y = XGBE0_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_XGBE_1_IF) {
    x = XGBE1_IDN1_X;
    y = XGBE1_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_GBE_0_IF) {
    x = GBE0_IDN1_X;
    y = GBE0_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_GBE_1_IF) {
    x = GBE1_IDN1_X;
    y = GBE1_IDN1_Y;
    channel = 1;
  } else {
    print("sync_and_check_pkt(): invalid Ethernet interface specified");
    TEST_FAIL(0xdead0001);
  }

  // Route header.
  iDmaPkt.bits.routeHdr.length = 8;
  iDmaPkt.bits.routeHdr.dest_x = x;
  if ((enet_if == ENET_XGBE_0_IF) || (enet_if == ENET_GBE_0_IF) || (enet_if == ENET_GBE_1_IF))
    iDmaPkt.bits.routeHdr.dest_y = (y - 1);
  else if (enet_if == ENET_XGBE_1_IF)
    iDmaPkt.bits.routeHdr.dest_y = (y + 1);
  iDmaPkt.bits.routeHdr.fbit = 1;
  iDmaPkt.bits.routeHdr.rsvd = 0;
  
  // Shim header.
  iDmaPkt.bits.channel = channel;
  iDmaPkt.bits.iodn_type = IODN_INGRESS_SEND_PKT;
  iDmaPkt.bits.ack = 1;
  iDmaPkt.bits.rsvd1 = 0;
  iDmaPkt.bits.notify = 1;
  iDmaPkt.bits.boundary = 1;
  iDmaPkt.bits.d = 2;
  iDmaPkt.bits.xsize = pktSize;
  
  iDmaPkt.bits.lo_addr = (uint32_t) rxBuf;
  iDmaPkt.bits.hi_addr = 0;

  iDmaPkt.bits.dsize = 0;
  iDmaPkt.bits.dctl = 0;
  
  // Notification header.
  iDmaPkt.bits.n_s_channel = channel;
  iDmaPkt.bits.n_source_x = MY_X;
  iDmaPkt.bits.n_source_y = MY_Y;
  iDmaPkt.bits.n_source_fbit = 0;
  iDmaPkt.bits.rsvd2 = 0;
  
  iDmaPkt.bits.n_source_tag = ENET_IDN_TAG;
  iDmaPkt.bits.n_trans_id = 0;
  
  // From header.
  iDmaPkt.bits.s_channel = channel;
  iDmaPkt.bits.source_x = MY_X;
  iDmaPkt.bits.source_y = MY_Y;
  iDmaPkt.bits.source_fbit = 0;
  iDmaPkt.bits.rsvd3 = 0;
  
  iDmaPkt.bits.s_source_tag = ENET_IDN_TAG;

  // print("Wait for an edma ack and an avail");
  // Wait for an edma ack and an avail, they can come 
  // out of order
  noAck = 1;
  noAvail = 1;
  while (noAck || noAvail) {

    idnData = iodn0_receive();
    idnType = (idnData >> 7) & 0xf;

    if (idnType == IODN_FC_PKT) {
      noAck = 0;
    } else if (idnType == IODN_AVAIL_PKT) {
      idnLen = (idnData >> 16) & 0x1ff;
      idnHdrBytes = 0;
      while (idnHdrBytes < idnLen) {
        idnData = iodn0_receive();    
        idnHdrBytes += 4;
      }
      noAvail = 0;
    } else {
      TEST_PASS(idnType);
      TEST_FAIL(0xdead0002);
    }
  }

  // Invalidate the current receive packet in cache
  // so that the iDMA packet is read from memory
  for (i=0; i<pktSize; i++) {
    if (((i % 16) == 0) || (i == (pktSize - 1))) {
      __insn_finv(rxBuf+i);
    }
  }

  // Wait for the memory operations to complete
  __insn_mf();

  // print("Send an iDMA");
  // Send an iDMA
  // print6("Route x ", iDmaPkt.bits.routeHdr.dest_x, ", Route y ", iDmaPkt.bits.routeHdr.dest_y, ", Route fbit ", iDmaPkt.bits.routeHdr.fbit);
  // print6("Source x ", iDmaPkt.bits.source_x, ", Source y ", iDmaPkt.bits.source_y, ", Source fbit ", iDmaPkt.bits.source_fbit);  

  iodn_send_pkt(iDmaPkt.word, 8);  

  // print("Wait for an idma ack and notif");
  // Wait for an idma ack and notif, they can come 
  // out of order
  noAck = 1;
  noNotif = 1;  
  while (noAck || noNotif) {

    idnData = iodn0_receive();
    //print2("Got idn header", idnData);
    idnType = (idnData >> 7) & 0xf;
  
    if (idnType == IODN_FC_PKT) {
      noAck = 0;
    } else if (idnType == IODN_NOTIFY_PKT) {
      idnStatus = (idnData >> 14) & 0x3;
      if (idnStatus != 0) {
        uint32_t fcs_errors;
        print2("iDMA notification received with status = ", idnStatus);
        if ((enet_if == ENET_GBE_0_IF) || (enet_if == ENET_GBE_1_IF)) 
          fcs_errors = send_config_read(x, y, CFG_TAG, channel, GEM_FCS_ERRORS);
        else if ((enet_if == ENET_XGBE_0_IF) || (enet_if == ENET_XGBE_1_IF)) 
          fcs_errors = send_config_read(x, y, CFG_TAG, channel, XGM_RX_CRC_ERRORS);
        print2("XGM FCS error count = ", fcs_errors);
        TEST_FAIL(0xdead0003);
      }
      idnData = iodn0_receive();
      // print2("Got idn notif", idnData);
      noNotif = 0;
    } else {
      TEST_PASS(idnType);
      TEST_FAIL(0xdead0004);
    }
  }

  // Compare the packet
  for (i=0; i<pktSize; i++) {
    if (rxBuf[i] != txBuf[i]) {
      TEST_PASS(txBuf[i]);
      TEST_PASS(rxBuf[i]);
      print4("Packet compare failure: exp_data=", txBuf[i], ", rec_data=", rxBuf[i]);
      TEST_FAIL(0xdead0005);
    }
  }
}

void sync_and_discard_pkt(uint32_t enet_if) {

  uint32_t idnType;
  uint32_t idnLen;
  uint32_t idnData;
  uint32_t idnHdrBytes;
  uint32_t x, y, channel; 

  DmaSendInPkt iDmaPkt;

  // Enet specific information
  if (enet_if == ENET_XGBE_0_IF) {
    x = XGBE0_IDN1_X;
    y = XGBE0_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_XGBE_1_IF) {
    x = XGBE1_IDN1_X;
    y = XGBE1_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_GBE_0_IF) {
    x = GBE0_IDN1_X;
    y = GBE0_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_GBE_1_IF) {
    x = GBE1_IDN1_X;
    y = GBE1_IDN1_Y;
    channel = 1;
  } else {
    print("sync_and_discard_pkt(): invalid Ethernet interface specified");
    TEST_FAIL(0xdead0006);
  }

  // Route header.
  iDmaPkt.bits.routeHdr.length = 1;
  iDmaPkt.bits.routeHdr.dest_x = x;
  if ((enet_if == ENET_XGBE_0_IF) || (enet_if == ENET_GBE_0_IF) || (enet_if == ENET_GBE_1_IF))
    iDmaPkt.bits.routeHdr.dest_y = (y - 1);
  else if (enet_if == ENET_XGBE_1_IF)
    iDmaPkt.bits.routeHdr.dest_y = (y + 1);
  iDmaPkt.bits.routeHdr.fbit = 1;
  iDmaPkt.bits.routeHdr.rsvd = 0;
  
  // Shim header.
  iDmaPkt.bits.channel = channel;
  iDmaPkt.bits.iodn_type = IODN_INGRESS_SEND_PKT;
  iDmaPkt.bits.ack = 0;
  iDmaPkt.bits.rsvd1 = 0;
  iDmaPkt.bits.notify = 0;
  iDmaPkt.bits.boundary = 1;
  iDmaPkt.bits.d = 0;
  iDmaPkt.bits.xsize = 2000;

  // Wait for an avail.
  idnData = iodn0_receive();
  idnType = (idnData >> 7) & 0xf;
  if (idnType != IODN_AVAIL_PKT) 
      TEST_FAIL(0xdead0007);
  idnLen = (idnData >> 16) & 0x1ff;
  idnHdrBytes = 0;
  while (idnHdrBytes < idnLen) {
    idnData = iodn0_receive();    
    idnHdrBytes += 4;
  };

  iodn_send_pkt(iDmaPkt.word, 1);  
}

void sync_and_send_pkt(uint32_t enet_if, 
                       unsigned char * buf, 
                       uint32_t pktSize) {

  int idnType, idnStatus, idnLen, idnData, idnHdrBytes;
  uint32_t x, y, channel; 
  DmaSendInPkt iDmaPkt;
  DmaSendEgNoNotifPkt eDmaPkt;

  // Enet specific information
  if (enet_if == ENET_XGBE_0_IF) {
    x = XGBE0_IDN1_X;
    y = XGBE0_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_XGBE_1_IF) {
    x = XGBE1_IDN1_X;
    y = XGBE1_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_GBE_0_IF) {
    x = GBE0_IDN1_X;
    y = GBE0_IDN1_Y;
    channel = 0;
  } else if (enet_if == ENET_GBE_1_IF) {
    x = GBE1_IDN1_X;
    y = GBE1_IDN1_Y;
    channel = 1;
  } else {
    print("sync_and_send_pkt(): invalid Ethernet interface specified");
    TEST_FAIL(0xdead0008);
  }

  // Route header.
  iDmaPkt.bits.routeHdr.length = 8;
  iDmaPkt.bits.routeHdr.dest_x = x;
  if ((enet_if == ENET_XGBE_0_IF) || (enet_if == ENET_GBE_0_IF) || (enet_if == ENET_GBE_1_IF))
    iDmaPkt.bits.routeHdr.dest_y = (y - 1);
  else if  (enet_if == ENET_XGBE_1_IF)
    iDmaPkt.bits.routeHdr.dest_y = (y + 1);
  iDmaPkt.bits.routeHdr.fbit = 1;
  iDmaPkt.bits.routeHdr.rsvd = 0;

  // Shim header.
  iDmaPkt.bits.channel = channel;
  iDmaPkt.bits.iodn_type = IODN_INGRESS_SEND_PKT;
  iDmaPkt.bits.ack = 1;
  iDmaPkt.bits.rsvd1 = 0;
  iDmaPkt.bits.notify = 1;
  iDmaPkt.bits.boundary = 1;
  iDmaPkt.bits.d = 2;
  iDmaPkt.bits.xsize = pktSize;
  
  iDmaPkt.bits.lo_addr = (uint32_t) buf;
  iDmaPkt.bits.hi_addr = 0;

  iDmaPkt.bits.dsize = 0;
  iDmaPkt.bits.dctl = 0;
  
  // Notification header.
  iDmaPkt.bits.n_s_channel = channel;
  iDmaPkt.bits.n_source_x = MY_X;
  iDmaPkt.bits.n_source_y = MY_Y;
  iDmaPkt.bits.n_source_fbit = 0;
  iDmaPkt.bits.rsvd2 = 0;
  
  iDmaPkt.bits.n_source_tag = ENET_IDN_TAG;
  iDmaPkt.bits.n_trans_id = 0;
  
  // From header.
  iDmaPkt.bits.s_channel = channel;
  iDmaPkt.bits.source_x = MY_X;
  iDmaPkt.bits.source_y = MY_Y;
  iDmaPkt.bits.source_fbit = 0;
  iDmaPkt.bits.rsvd3 = 0;
  
  iDmaPkt.bits.s_source_tag = ENET_IDN_TAG;

  // Wait for an avail packet
  idnData = iodn0_receive();
  idnType = (idnData >> 7) & 0xf;
  if (idnType != IODN_AVAIL_PKT) 
      TEST_FAIL(0xdead0009);
  idnLen = (idnData >> 16) & 0x1ff;
  idnHdrBytes = 0;
  while (idnHdrBytes < idnLen) {
    idnData = iodn0_receive();    
    idnHdrBytes += 4;
  };

  // Send an iDMA
  iodn_send_pkt(iDmaPkt.word, 8);  

  // Wait for the ack
  idnData = iodn0_receive();
  idnType = (idnData >> 7) & 0xf;
  if (idnType != IODN_FC_PKT) 
      TEST_FAIL(0xdead000a);

  // Wait for the notification
  idnData = iodn0_receive();
  idnType = (idnData >> 7) & 0xf;
  if (idnType == IODN_NOTIFY_PKT) {
    idnStatus = (idnData >> 14) & 0x3;
    if (idnStatus != 0) {
      print2("iDMA notification received with status = ", idnStatus);
      TEST_FAIL(0xdead000b);
    }
    idnData = iodn0_receive();
  } else {
    TEST_FAIL(0xdead000c);
  }

  // Now eDMA this packet

  // Route header.
  eDmaPkt.bits.routeHdr.length = 5;
  eDmaPkt.bits.routeHdr.dest_x = x;
  eDmaPkt.bits.routeHdr.dest_y = y;
  eDmaPkt.bits.routeHdr.fbit = 1;
  eDmaPkt.bits.routeHdr.rsvd = 0;
  
  // Shim header
  eDmaPkt.bits.channel = channel;
  eDmaPkt.bits.iodn_type = IODN_EGRESS_SEND_PKT;
  eDmaPkt.bits.ack = 1;
  eDmaPkt.bits.rsvd1 = 0;
  eDmaPkt.bits.notify = 0;
  eDmaPkt.bits.boundary = 1;
  eDmaPkt.bits.rsvd2 = 0;
  eDmaPkt.bits.m = 1;
  eDmaPkt.bits.rsvd3 = 0;
  eDmaPkt.bits.xsize = pktSize;
  
  eDmaPkt.bits.lo_addr =  (uint32_t) buf;
  
  eDmaPkt.bits.hi_addr = 0;
  eDmaPkt.bits.dsize = 0;
  eDmaPkt.bits.dctl = 0;
  
  // From header
  eDmaPkt.bits.s_channel = channel;
  eDmaPkt.bits.source_x = MY_X;
  eDmaPkt.bits.source_y = MY_Y;
  eDmaPkt.bits.source_fbit = 0;
  eDmaPkt.bits.rsvd5 = 0;
  
  eDmaPkt.bits.s_source_tag = ENET_IDN_TAG;

  // Send the eDMA command
  iodn_send_pkt(eDmaPkt.word, 5);  

  //Wait for the ack
  idnData =  iodn0_receive();
  idnType = (idnData >> 7) & 0xf;
  if (idnType != IODN_FC_PKT) 
      TEST_FAIL(0xdead000d);
}

void xaui_set_pll_divisor(uint32_t enet_if, 
                          uint32_t value) {

  uint32_t rdData, x, y, retries, clock_ready;

  if (value > 3) {
    print("ERROR: invalid XAUI pll divisor");
    TEST_FAIL(0xdead000e);
  }

  // Enet interface.
  if (enet_if == ENET_XGBE_1_IF) {
    x = XGBE1_IDN1_X;
    y = XGBE1_IDN1_Y;
  } else if (enet_if == ENET_XGBE_0_IF) {
    x = XGBE0_IDN1_X;
    y = XGBE0_IDN1_Y;
  } else {
    print("xaui_set_pll_divisor(): invalid Ethernet interface specified");
    TEST_FAIL(0xdead000f);
  }

  // Disable the SERDES.
  rdData = 0;
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_SHIMX_CTL2, rdData);
  
  // Enable the serdes with the  new divisor value.
  rdData = 1;
  rdData |= (value << 8);
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_SHIMX_CTL2, rdData);
  
  // Wait for clock ready.
  clock_ready = 0;
  for (retries=0; retries<40000000; retries++) {
    if ((send_config_read(x, y, CFG_TAG, 0, GSH_REG_SHIMX_STS0) & 0x01000000) == 0x01000000) {
      clock_ready = 1;
      break;
    }
  }
  if (clock_ready == 0) {
    print("ERROR: XAUI CLOCK_READY failed to assert after setting PLL divisor");
    TEST_FAIL(0xdead0010);
  } else {
    print2("XAUI PLL divisor set to ", value);
  }
}

void xaui_config(uint32_t enet_if, 
                 uint32_t serdes_loopback) {

  uint32_t rdData;
  uint32_t srcId0, srcId1; 
  uint32_t x, y;
  uint32_t mshim_x, mshim_y, xgbe_mdn0_x, xgbe_mdn0_y, xgbe_mdn1_x, xgbe_mdn1_y;
  uint32_t restarts, retries, link_up, serdes;

  // Get the western mshim coordinates
  mshim_y = Y_FROM_COORD(__insn_mfspr(SPR_CBOX_MMAP_0));
  mshim_x = X_FROM_COORD(__insn_mfspr(SPR_CBOX_MMAP_0));
  mshim_x = western_mdn_port_x(mshim_x, mshim_y);

  // Instance specific information
  if (enet_if == ENET_XGBE_1_IF) {
    print("XAUI 1 SERDES enable");
    srcId0 = XGBE1_0_SRC_ID;
    srcId1 = XGBE1_1_SRC_ID;
    x = XGBE1_IDN1_X;
    y = XGBE1_IDN1_Y;
    xgbe_mdn0_x = XGBE1_MDN0_X;
    xgbe_mdn0_y = XGBE1_MDN0_Y;
    xgbe_mdn1_x = XGBE1_MDN1_X;
    xgbe_mdn1_y = XGBE1_MDN1_Y;
    serdes = XGBE1_SERDES_CTL;
  } else if (enet_if == ENET_XGBE_0_IF) {
    print("XAUI 0 SERDES enable");
    srcId0 = XGBE0_0_SRC_ID;
    srcId1 = XGBE0_1_SRC_ID;
    x = XGBE0_IDN1_X;
    y = XGBE0_IDN1_Y;
    xgbe_mdn0_x = XGBE0_MDN0_X;
    xgbe_mdn0_y = XGBE0_MDN0_Y;
    xgbe_mdn1_x = XGBE0_MDN1_X;
    xgbe_mdn1_y = XGBE0_MDN1_Y;
    serdes = XGBE0_SERDES_CTL;
  } else {
    print("xaui_config(): invalid Ethernet interface specified");
    TEST_FAIL(0xdead0011);
  }

  // Try the link bringup sequence. 
  link_up = 0;
  for (restarts=0; restarts<2; restarts++) {

    if (link_up == 1) {
      break;
    }

    // Get the PLL clock divisor.
    rdData = (send_config_read(x, y, CFG_TAG, 0, GSH_REG_SHIMX_CTL2) & 0x300);

    // Enable the serdes and hold the PCS layer in reset.
    rdData |= (1<<0);
    rdData |= (1<<10);
    send_config_write(x, y, CFG_TAG, 0, GSH_REG_SHIMX_CTL2, rdData);

    // Clear the elasticity buffer via JTAG if we use a non-zero clock divisor.
    if ((rdData & 0x300) != 0) {
      //serdes_eb_reset(serdes, 1, serdes_loopback); 
      //serdes_eb_reset(serdes, 0, serdes_loopback);    
    }
    
    // Take the PCS layer out of reset.
    rdData |= (3<<10);
    send_config_write(x, y, CFG_TAG, 0, GSH_REG_SHIMX_CTL2, rdData);

    // Wait for PCS_UNALIGNED to deassert.
    for (retries=0; retries<1000000; retries++) {
      if ((send_config_read(x, y, CFG_TAG, 0, GSH_REG_SHIMX_STS0) & 0x8000) == 0) {
        link_up = 1;
        break;
      }
    }
  }  
  if (link_up == 0) {
    print("ERROR: XAUI PCS_UNALINGED failed to deassert");
    TEST_FAIL(0xdead0012);
  } else {
    print("XAUI PCS_UNALINGED deasserted");
  }

  if (enet_if == ENET_XGBE_1_IF) {
    print("XAUI 1 MAC configure"); 
  } else {
    print("XAUI 0 MAC configure"); 
  };

  // Clear out errors that occured during link bringup.
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_SINT_STS, 0xffffffff);  

  // iDMA to Mshim
  if (MY_REV == TILE64) 
    send_config_write(x, y, CFG_TAG, 0, GSH_REG_MEM0_CFG, srcId0);  
  else 
    send_config_write(x, y, CFG_TAG, 0, GSH_REG_MEM0_CFG, (((xgbe_mdn0_x & 0xf) << 4) | (xgbe_mdn0_y & 0xf)));  

  send_config_write(x, y, CFG_TAG, 0, GSH_REG_M0MAP0, ((mshim_x << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_M0MAP1, ((mshim_x << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_M0MAP2, ((mshim_x << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_M0MAP3, ((mshim_x << 18) | (mshim_y << 7)));  

  // eDMA to Mshim
  if (MY_REV == TILE64) 
    send_config_write(x, y, CFG_TAG, 0, GSH_REG_MEM1_CFG, srcId1);  
  else
    send_config_write(x, y, CFG_TAG, 0, GSH_REG_MEM1_CFG,  (((xgbe_mdn1_x & 0xf) << 4) | (xgbe_mdn1_y & 0xf)));

  send_config_write(x, y, CFG_TAG, 0, GSH_REG_M1MAP0, ((mshim_x+1 << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_M1MAP1, ((mshim_x+1 << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_M1MAP2, ((mshim_x+1 << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_M1MAP3, ((mshim_x+1 << 18) | (mshim_y << 7)));  

  // iDMA binding
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_INGR_BND0, 3);
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_INGR_BND1, ((MY_X << 18) | (MY_Y << 7))); 
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_INGR_BND2, ENET_IDN_TAG); 

  // iDMA header sizing
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_IHDR_SZ, ENET_HDR_SIZE);

  // Enable the shim
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_ICTL, 3);
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_ECTL, 1);
  send_config_write(x, y, CFG_TAG, 0, GSH_REG_BASELINE_CTL, 1);

  // Enable the xgm
  send_config_write(x, y, CFG_TAG, 0, XGM_TX_CONTROL, 1);
  send_config_write(x, y, CFG_TAG, 0, XGM_RX_CONTROL, 1);
  send_config_write(x, y, CFG_TAG, 0, XGM_RX_CONFIGURATION, 0x41);
}

uint32_t xaui_mdio_rd(uint32_t mgmt_port, // MDIO management port
                      uint32_t port_addr, // Board level hard-wired PHY address
                      uint32_t dev_addr,  // Device within the PHY
                      uint32_t reg_addr)  // Register withing the device
{
  uint32_t x, y, time_out, rd_data;

  // MDIO structure designed for clause 22 and 
  // recycled for clause 45.
  MdioControlReg mdioControlReg;

  // Management port.
  if (mgmt_port == ENET_XGBE_0_MGMT_PORT) {
    x = XGBE0_IDN1_X;
    y = XGBE0_IDN1_Y;
  } else if (mgmt_port == ENET_XGBE_1_MGMT_PORT) {
    x = XGBE1_IDN1_X;
    y = XGBE1_IDN1_Y;
  } else {
    print("xaui_mdio_rd(): invalid Ethernet MDIO management port specified");
    TEST_FAIL(0xdead0013);
  }

  // Enable the PHY frame complete interrupt.
  rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_ENABLE);
  rd_data |=  0x00000100;
  send_config_write(x, y, CFG_TAG, 0, XGM_INTERRUPT_ENABLE, rd_data);

  // Clause 45.
  mdioControlReg.bits.phy_clause = 0; 

  // MDIO turn around bits.
  mdioControlReg.bits.mb_10 = 2;  

  // Clause 45 address cycle to set the register address.
  mdioControlReg.bits.phy_opp = 0;
  mdioControlReg.bits.phy_addr = port_addr;
  mdioControlReg.bits.phy_reg = dev_addr;
  mdioControlReg.bits.phy_data = reg_addr;

  // Write the MDIO control register.
  send_config_write(x, y, CFG_TAG, 0, XGM_MDIO_REGISTER, mdioControlReg.word);

  // Wait for the MDIO operation to finish.
  time_out = 0;
  rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_STATUS);
  while ((rd_data & 0x00000100) == 0) {
    rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_STATUS);
    time_out++;
    if (time_out > 200)
      FAIL(0xdead0014);
  };

  // Clause 45 read cycle.
  mdioControlReg.bits.phy_opp = 3;
  mdioControlReg.bits.phy_data = 0;

  // Write the MDIO control register.
  send_config_write(x, y, CFG_TAG, 0, XGM_MDIO_REGISTER, mdioControlReg.word);

  // Wait for the MDIO operation to finish.
  time_out = 0;
  rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_STATUS);
  while ((rd_data & 0x00000100) == 0) {
    rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_STATUS);
    time_out++;
    if (time_out > 200)
      FAIL(0xdead0015);
  };

  // Return the MDIO read data.
  rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_MDIO_REGISTER);
  return(rd_data & 0x0000ffff);
}

void xaui_mdio_wr(uint32_t mgmt_port,  // MDIO management port
                  uint32_t port_addr,  // Board level hard-wired PHY address
                  uint32_t dev_addr,   // Device within the PHY
                  uint32_t reg_addr,   // Register withing the device
                  uint32_t wr_data)
{
  uint32_t x, y, time_out, rd_data;

  // MDIO structure designed for clause 22 and 
  // recycled for clause 45.
  MdioControlReg mdioControlReg;

  // Management port.
  if (mgmt_port == ENET_XGBE_0_MGMT_PORT) {
    x = XGBE0_IDN1_X;
    y = XGBE0_IDN1_Y;
  } else if (mgmt_port == ENET_XGBE_1_MGMT_PORT) {
    x = XGBE1_IDN1_X;
    y = XGBE1_IDN1_Y;
  } else {
    print("xaui_mdio_wr(): invalid Ethernet MDIO management port specified");
    TEST_FAIL(0xdead0016);
  }

  // Enable the PHY frame complete interrupt.
  rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_ENABLE);
  rd_data |=  0x00000100;
  send_config_write(x, y, CFG_TAG, 0, XGM_INTERRUPT_ENABLE, rd_data);

  // Clause 45.
  mdioControlReg.bits.phy_clause = 0; 

  // MDIO turn around bits.
  mdioControlReg.bits.mb_10 = 2;  

  // Clause 45 address cycle to set the register address.
  mdioControlReg.bits.phy_opp = 0;
  mdioControlReg.bits.phy_addr = port_addr;
  mdioControlReg.bits.phy_reg = dev_addr;
  mdioControlReg.bits.phy_data = reg_addr;

  // Write the MDIO control register.
  send_config_write(x, y, CFG_TAG, 0, XGM_MDIO_REGISTER, mdioControlReg.word);

  // Wait for the MDIO operation to finish.
  time_out = 0;
  rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_STATUS);
  while ((rd_data & 0x00000100) == 0) {
    rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_STATUS);
    time_out++;
    if (time_out > 200)
      FAIL(0xdead0017);
  };

  // Clause 45 write cycle.
  mdioControlReg.bits.phy_opp = 1;
  mdioControlReg.bits.phy_data = wr_data;

  // Write the MDIO control register.
  send_config_write(x, y, CFG_TAG, 0, XGM_MDIO_REGISTER, mdioControlReg.word);

  // Wait for the MDIO operation to finish.
  time_out = 0;
  rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_STATUS);
  while ((rd_data & 0x00000100) == 0) {
    rd_data = send_config_read(x, y, CFG_TAG, 0, XGM_INTERRUPT_STATUS);
    time_out++;
    if (time_out > 200)
      FAIL(0xdead0018);
  }
}

void gbe_config(uint32_t channel) {

  uint32_t x = GBE0_IDN1_X;
  uint32_t y = GBE0_IDN1_Y;
  uint32_t read_data;
  uint32_t mshim_x, mshim_y, gbe_mdn0_x, gbe_mdn0_y, gbe_mdn1_x, gbe_mdn1_y;

  if (channel == 0) {
    print("Gbe 0: configuring GSH and MAC");
    gbe_mdn0_x = GBE0_MDN0_X;
    gbe_mdn0_y = GBE0_MDN0_Y;
    gbe_mdn1_x = GBE0_MDN1_X;
    gbe_mdn1_y = GBE0_MDN1_Y;
  } else if (channel == 1) {
    print("Gbe 1: configuring GSH and MAC");
    gbe_mdn0_x = GBE1_MDN0_X;
    gbe_mdn0_y = GBE1_MDN0_Y;
    gbe_mdn1_x = GBE1_MDN1_X;
    gbe_mdn1_y = GBE1_MDN1_Y;
  } else {
    print("gbe_config(): invalid channel specified");
    TEST_FAIL(0xdead0019);
  }

  // Get the western mshim coordinates
  // It's safe enough to use the function that will do a config read
  // to the mshim because only 1 or 2 of these could be happening
  // concurrently and the mshim can handle that.
  mshim_y = Y_FROM_COORD(__insn_mfspr(SPR_CBOX_MMAP_0));
  mshim_x = X_FROM_COORD(__insn_mfspr(SPR_CBOX_MMAP_0));
  mshim_x = western_mdn_port_x(mshim_x, mshim_y);


  // Set the pin electrical controls
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_SHIMX_CTL1, 0x06060606);


  // Set the external clock and MAC speed select
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_SHIMX_CTL0, 0x80000010);

  // GEM network config:
  // - full duplex
  // - copy all frames
  // - mdio clock divisor
  // - remove FCS
  // - gigbit mode
  send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONFIG,  0x00120412);

  // GEM interrupt enable:
  // - management frame sent
  send_config_write(x, y, CFG_TAG, channel, GEM_INT_ENABLE, 0x00000001);
  
  // GEM network control:
  // - receive enable
  // - transmit enable
  // - management port (MDIO) enable
  send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONTROL, 0x0000001c);
  read_data = send_config_read(x, y, CFG_TAG, channel, GEM_NETWORK_CONTROL);
  print2("enet.gbe_config: gem_network_control = ", read_data);


  if(0){
  // iDMA to Mshim
  if (MY_REV == TILE64) 
    send_config_write(x, y, CFG_TAG, channel, GSH_REG_MEM0_CFG, GBE0_0_SRC_ID);  
  else 
    send_config_write(x, y, CFG_TAG, channel, GSH_REG_MEM0_CFG, (((gbe_mdn0_x & 0xf) << 4) | (gbe_mdn0_y & 0xf)));

  print("gbe area 1");
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_M0MAP0, ((mshim_x << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_M0MAP1, ((mshim_x << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_M0MAP2, ((mshim_x << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_M0MAP3, ((mshim_x << 18) | (mshim_y << 7)));  

  // eDMA to Mshim
  if (MY_REV == TILE64) 
    send_config_write(x, y, CFG_TAG, channel, GSH_REG_MEM1_CFG, GBE0_1_SRC_ID);  
  else 
    send_config_write(x, y, CFG_TAG, channel, GSH_REG_MEM1_CFG, (((gbe_mdn1_x & 0xf) << 4) | (gbe_mdn1_y & 0xf)));
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_M1MAP0, ((mshim_x+1 << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_M1MAP1, ((mshim_x+1 << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_M1MAP2, ((mshim_x+1 << 18) | (mshim_y << 7)));  
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_M1MAP3, ((mshim_x+1 << 18) | (mshim_y << 7)));  
  print("gbe area 2");

  // iDMA binding
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_INGR_BND0, 3);
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_INGR_BND1, ((MY_X << 18) | (MY_Y << 7) | channel)); 
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_INGR_BND2, ENET_IDN_TAG); 
  
  print("gbe area 3");
  // iDMA header sizing
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_IHDR_SZ, ENET_HDR_SIZE);
  
  print("gbe area 3.5");
  
  // Enable the shim
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_ICTL,3 );
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_ECTL, 1);
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_BASELINE_CTL, 1);
  print("gbe area 4");
  }
}

void gbe_mdio_config(uint32_t channel) {

  uint32_t x;
  uint32_t y;

  if (channel == 0) {
    x = GBE0_IDN1_X;
    y = GBE0_IDN1_Y;
  } else if (channel == 1) {
    x = GBE1_IDN1_X;
    y = GBE1_IDN1_Y;
  } else {
    print("gbe_mac_loopback(): invalid channel specified");
    TEST_FAIL(0xdead001a);
  }

  // Set the pin electrical controls
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_SHIMX_CTL1, 0x06060606);

  // Set the external clock and MAC speed select
  send_config_write(x, y, CFG_TAG, channel, GSH_REG_SHIMX_CTL0, 0x80000010);

  // GEM network config:
  // - full duplex
  // - copy all frames
  // - mdio clock divisor
  // - remove FCS
  // - gigbit mode
  send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONFIG,  0x00120412);

  // GEM interrupt enable:
  // - management frame sent
  send_config_write(x, y, CFG_TAG, channel, GEM_INT_ENABLE, 0x00000001);
  
  // GEM network control:
  // - management port (MDIO) enable
  send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONTROL, 0x00000010);
}

void gbe_mac_loopback(uint32_t channel, 
                      uint32_t enable) {

  uint32_t x, y;

  if (MY_REV == TILE64) {
    print("Error: MAC loopback not supported on Tile64");
    TEST_FAIL(0xdead001b);
  }

  if (channel == 0) {
    x = GBE0_IDN1_X;
    y = GBE0_IDN1_Y;
  } else if (channel == 1) {
    x = GBE1_IDN1_X;
    y = GBE1_IDN1_Y;
  } else {
    print("gbe_mac_loopback(): invalid channel specified");
    TEST_FAIL(0xdead001c);
  }

  if (enable == 1) {

    // Disable the transmit/receive circuits.
    send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONTROL, 0x00000000); 
    
    // Enable the MAC loopback.
    send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONTROL, 0x00000002); 
    
    // Re-enable the transmit/receive circuits.
    send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONTROL, 0x0000001e); 

  } else {
    // Disable the transmit/receive circuits and MAC loopback.
    send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONTROL, 0x00000000); 
    
    // Re-enable the transmit/receive circuits.
    send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONTROL, 0x0000001c); 
  }
}

//
// TLR MDIO functions.
//
uint32_t gbe_mdio_rd(uint32_t channel, 
                     uint32_t phyAddr, 
                     uint32_t phyReg)
{
  MdioControlReg mdioControlReg;
  uint32_t timeOut;
  uint32_t rdData;
  uint32_t read_data;
  uint32_t x;
  uint32_t y;

  if (channel == 0) {
    x = GBE0_IDN1_X;
    y = GBE0_IDN1_Y;
    //x = 8;
    //y = GBE0_IDN1_Y;
  } else if (channel == 1) {
    x = GBE1_IDN1_X;
    y = GBE1_IDN1_Y;
  } else {
    print("gbe_mdio_rd(): invalid channel specified");
    TEST_FAIL(0xdead001d);
  }

  // Set the MDIO control register fields for a read operation.
  mdioControlReg.bits.mb_10 = 2;  
  mdioControlReg.bits.phy_data = 0;
  mdioControlReg.bits.phy_reg = phyReg;
  mdioControlReg.bits.phy_addr = phyAddr;
  mdioControlReg.bits.phy_opp = 2;
  mdioControlReg.bits.phy_clause = 1; // Clause 22

  // Write the GEM PHY maintenance register.
  

  send_config_write(x, y, CFG_TAG, channel, GEM_PHY_MANAGEMENT, mdioControlReg.word);

  read_data =   send_config_read(x, y, CFG_TAG, channel, GEM_PHY_MANAGEMENT);

  // Wait for the MDIO operation to finish.
  timeOut = 0;
  rdData = send_config_read(x, y, CFG_TAG, channel, GEM_INT_STATUS);
  while ((rdData & 0x00000001) == 0) {
    rdData = send_config_read(x, y, CFG_TAG, channel, GEM_INT_STATUS);
    timeOut++;
    if (timeOut > 1000) {
      print("Timed out waiting for TLR GBe MDIO Rd to complete");
      TEST_FAIL(0xdead001e);
    }
  };

  // Return the MDIO read data.
  rdData = send_config_read(x, y, CFG_TAG, channel, GEM_PHY_MANAGEMENT);
  return(rdData & 0x0000ffff);
}

void gbe_mdio_wr(uint32_t channel, 
                 uint32_t phyAddr, 
                 uint32_t phyReg, 
                 uint32_t phyData)
{
  MdioControlReg mdioControlReg;
  uint32_t timeOut;
  uint32_t rdData;
  uint32_t x;
  uint32_t y;

  if (channel == 0) {
    x = GBE0_IDN1_X;
    y = GBE0_IDN1_Y;
  } else if (channel == 1) {
    x = GBE1_IDN1_X;
    y = GBE1_IDN1_Y;
  } else {
    print("gbe_mdio_wr(): invalid channel specified");
    TEST_FAIL(0xdead001f);
  }

  // Set the MDIO control register fields for a write operation.
  mdioControlReg.bits.mb_10 = 2;  
  mdioControlReg.bits.phy_data = phyData;
  mdioControlReg.bits.phy_reg = phyReg;
  mdioControlReg.bits.phy_addr = phyAddr;
  mdioControlReg.bits.phy_opp = 1;
  mdioControlReg.bits.phy_clause = 1; // Clause 22

  // Write the GEM PHY maintenance register.
  send_config_write(x, y, CFG_TAG, channel, GEM_PHY_MANAGEMENT, mdioControlReg.word);

  // Wait for the MDIO operation to finish.
  timeOut = 0;
  rdData = send_config_read(x, y, CFG_TAG, channel, GEM_INT_STATUS);
  while ((rdData & 0x00000001) == 0) {
    rdData = send_config_read(x, y, CFG_TAG, channel, GEM_INT_STATUS);
    timeOut++;
    if (timeOut > 1000) {
      print("Timed out waiting for TLR GBe MDIO Wr to complete");
      TEST_FAIL(0xdead0020);
    }
  };
}

//
// TLR misc Ethernet functions.
//
void gbe_phy_config(uint32_t channel,
                    uint32_t phy_loopback,
                    uint32_t phy_speed, 
                    uint32_t phy_delay, 
                    uint32_t phy_type, 
                    uint32_t phy_addr) 
{

  uint32_t phy_id, phy_data;
  uint32_t phy_id_r2, phy_id_r3;

  //MLS-MDB Required to OR these together
  phy_id_r2 = gbe_mdio_rd(channel, phy_addr, 2);
  // print2("MLS-DBG: phy_id reg 2 = ", phy_id_r2);
  phy_id_r3 = gbe_mdio_rd(channel, phy_addr, 3);
  // print2("MLS-DBG: phy_id reg 3 = ", phy_id_r3);

  phy_id = phy_id_r2 | (phy_id_r3 << 16);
  print2("MLS-DBG: phy_id = ", phy_id);

  if ((channel != 0) && (channel != 1)) {
    print("gbe_mdio_wr(): invalid channel specified");
    TEST_FAIL(0xdead0021);
  }
  
  switch (phy_type) {

  case ENET_MV88E1111_COPPER: {

    if (phy_id != ENET_MV88E1111_ID) {
      print2("ERROR: unable to read MV88E1111 ID", phy_id);
      TEST_FAIL(0xdead0022);
    }

    // PHY errata workarounds.
    gbe_88e1111_rxclk_errata(channel, phy_addr);
    gbe_88e1111_clk125_jitter_errata(channel, phy_addr);

    // PHY delay.
    if (phy_delay == ENET_NO_DELAY) {
      gbe_mdio_wr(channel, phy_addr, 20, 0x0c60);
    } 
    else if (phy_delay == ENET_TX_DELAY) {
      gbe_mdio_wr(channel, phy_addr, 20, 0x0c62);
      print("MV88E1111 PHY Tx delay enabled");
    } 
    else if (phy_delay == ENET_RX_DELAY) {
      gbe_mdio_wr(channel, phy_addr, 20, 0x0ce0);
      print("MV88E1111 PHY Rx delay enabled");
    } 
    else {
      gbe_mdio_wr(channel, phy_addr, 20, 0x0ce2);
      print("MV88E1111 PHY Tx/Rx delays enabled");
    }

    // PHY speed/loopback.
    if (phy_speed == ENET_1G) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8140);
      print("MV88E1111 PHY configured for 1 Gbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4140);
        print("MV88E1111 PHY loopback configured");
      }
    }

    if (phy_speed == ENET_100M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0xa100);
      print("MV88E1111 PHY configured for 100 Mbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x6100);
        print("MV88E1111 PHY loopback configured");
      }
    }

    if (phy_speed == ENET_10M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8100);
      print("MV88E1111 PHY configured for 10 Mbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4100);
        print("MV88E1111 PHY loopback configured");
      }
    }
    break;
  }
  
  case ENET_MV88E1121_COPPER: {

    if (phy_id != ENET_MV88E1121_ID) {
      print4("ERROR: phy_id != ENET_MV88E1121_ID phy_id =", phy_id, " expected ENET_MV88E1121_ID =", ENET_MV88E1121_ID);
      TEST_FAIL(0xdead0022);
    }
    //MLS-hack
    else {
      print("MLS-DBG: read phy_id MV88E1121 ID");
    }
    // PHY speed/loopback.
    if (phy_speed == ENET_1G) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8140);
      print("MV88E1121 PHY configured for 1 Gbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4140);
        print("MV88E1121 PHY loopback configured");
      }
    }

    if (phy_speed == ENET_100M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0xa100);
      print("MV88E1121 PHY configured for 100 Mbit/sec Full Duplex");
      if (phy_loopback == 1) {
        print("MV88E1121 PHY loopback configured");
        gbe_mdio_wr(channel, phy_addr, 22, 2);
        phy_data = gbe_mdio_rd(channel, phy_addr, 21);
        phy_data &= ~0x0040;
        phy_data |= 0x2000;
        gbe_mdio_wr(channel, phy_addr, 21, phy_data);
        gbe_mdio_wr(channel, phy_addr, 22, 0);
        gbe_mdio_wr(channel, phy_addr, 0, 0xa100);
        gbe_mdio_wr(channel, phy_addr, 0, 0x6100);
      }
    }

    if (phy_speed == ENET_10M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8100);
      print("MV88E1121 PHY configured for 10 Mbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 22, 2);
        phy_data = gbe_mdio_rd(channel, phy_addr, 21);
        phy_data &= ~0x2040;
        gbe_mdio_wr(channel, phy_addr, 21, phy_data);
        gbe_mdio_wr(channel, phy_addr, 22, 0);
        gbe_mdio_wr(channel, phy_addr, 0, 0x8100);
        gbe_mdio_wr(channel, phy_addr, 0, 0x4100);
        print("MV88E1121 PHY loopback configured");
      }
    }
  
    // LED 0 = link, LED 1 = activity.
    //  only for FTB, MDB uses sometign different
    //   USes regsiter 22 for page set to 3, and regsiter 16.
    gbe_mdio_wr(channel, phy_addr, 22, 3);
    phy_data = gbe_mdio_rd(channel, phy_addr, 16);
    //phy_data &= 0xff00;
    //phy_data |= 0x0040;
    // LED 1 force on, LED0 force blink.
    phy_data &= 0xff00;
    phy_data |= 0x009B;

    //phy_data |= 0x0099; // Always on
    gbe_mdio_wr(channel, phy_addr, 16, phy_data);
    gbe_mdio_wr(channel, phy_addr, 22, 0);

    break;
  }

  case ENET_MV88E1111_FIBER: {
    
    if (phy_id != ENET_MV88E1111_ID) {
      print("ERROR: unable to read MV88E1111 ID");
      TEST_FAIL(0xdead0023);
    }

    // PHY errata workarounds.
    gbe_88e1111_rxclk_errata(channel, phy_addr);
    gbe_88e1111_clk125_jitter_errata(channel, phy_addr);

    // PHY delay.
    if (phy_delay == ENET_NO_DELAY) {
      gbe_mdio_wr(channel, phy_addr, 20, 0x0c60);
    } 
    else if (phy_delay == ENET_TX_DELAY) {
      gbe_mdio_wr(channel, phy_addr, 20, 0x0c62);
      print("MV88E1111 PHY Tx delay enabled");
    } 
    else if (phy_delay == ENET_RX_DELAY) {
      gbe_mdio_wr(channel, phy_addr, 20, 0x0ce0);
      print("MV88E1111 PHY Rx delay enabled");
    } 
    else {
      gbe_mdio_wr(channel, phy_addr, 20, 0x0ce2);
      print("MV88E1111 PHY Tx/Rx delays enabled");
    }

    // PHY speed/loopback.
    if (phy_speed == ENET_1G) {
      gbe_mdio_wr(channel, phy_addr, 27, 0x8003);
      gbe_mdio_wr(channel, phy_addr, 0, 0x8000);
      gbe_mdio_wr(channel, phy_addr, 0, 0x8140);
      print("MV88E1111 PHY configured for 1 Gbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4140);
        print("MV88E1111 PHY loopback configured");
      }
    }

    if (phy_speed == ENET_100M) {
      gbe_mdio_wr(channel, phy_addr, 27, 0x800b);
      gbe_mdio_wr(channel, phy_addr, 0, 0x8000);
      gbe_mdio_wr(channel, phy_addr, 0, 0xa100);
      print("MV88E1111 PHY configured for 100 Mbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x6100);
        print("MV88E1111 PHY loopback configured");
      }
    }

    if (phy_speed == ENET_10M) {
      gbe_mdio_wr(channel, phy_addr, 27, 0x800b);
      gbe_mdio_wr(channel, phy_addr, 0, 0x8000);
      gbe_mdio_wr(channel, phy_addr, 0, 0x8100);
      print("MV88E1111 PHY configured for 10 Mbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4100);
        print("MV88E1111 PHY loopback configured");
      }
    }

    break;
  }

  case ENET_BCM5481: {

    if (phy_id != ENET_BCM5481_ID) {
      print("ERROR: unable to read BCM5481 ID");
      TEST_FAIL(0xdead0024);
    }

    // PHY speed/loopback.
    if (phy_speed == ENET_1G) {
      gbe_mdio_wr(channel, phy_addr, 4, 0x0001);
      gbe_mdio_wr(channel, phy_addr, 9, 0x0200);
      gbe_mdio_wr(channel, phy_addr, 0, 0x8140);
      print("BCM5481 PHY configured for 1 Gbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4140);
        print("BCM5481 PHY loopback configured");
      }
    }

    if (phy_speed == ENET_100M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x2100);
      print("BCM5481 PHY configured for 100 Mbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0x18, 0x8400);
        gbe_mdio_wr(channel, phy_addr, 0x18, 0x0014);
        print("BCM5481 PHY loopback configured");
      }
    }

    if (phy_speed == ENET_10M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x0100);
      print("BCM5481 PHY configured for 10 Mbit/sec Full Duplex");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0x18, 0x8400);
        gbe_mdio_wr(channel, phy_addr, 0x18, 0x0014);
        print("BCM5481 PHY loopback configured");
      }
    }

    // PHY delay.
    if (phy_delay != ENET_NO_DELAY) {
      gbe_mdio_wr(channel, phy_addr, 0x18, 0xf187);
      gbe_mdio_wr(channel, phy_addr, 0x18, 0x7007);
      gbe_mdio_wr(channel, phy_addr, 0x1c, 0x8e00);
      gbe_mdio_wr(channel, phy_addr, 0x1c, 0x0e00);
      print("BCM5481 PHY Tx/Rx delays enabled");
    }

    break;
  }

  case ENET_VSC8211_COPPER: {

    if (phy_id != ENET_VSC8211_ID) {
      print("ERROR: unable to read ENET_VCS8211_ID");
      TEST_FAIL(0xdead0025);
    }

    // Copper mode.
    gbe_mdio_wr(channel, phy_addr, 23, 0x24);

    // Enable auto mdix mode.
    vsc8211_auto_mdix(channel, phy_addr, 1);

    // PHY speed/loopback.
    if (phy_speed == ENET_1G) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8140);
      print("VCS8211 configured for 1 Gbit/sec Full Duplex (copper)");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4140);
        print("VCS8211 loopback configured");
      }
    }

    if (phy_speed == ENET_100M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8000);
      gbe_mdio_wr(channel, phy_addr, 0, 0x2100);
      print("VCS8211 configured for 100 Mbit/sec Full Duplex (copper)");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x6100);
        print("VCS8211 loopback configured");
      }
    }
    
    if (phy_speed == ENET_10M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8000);
      gbe_mdio_wr(channel, phy_addr, 0, 0x0100);
      print("VCS8211 configured for 10 Mbit/sec Full Duplex (copper)");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4100);
        print("VCS8211 loopback configured");
      }
    }
    break;
  }

  case ENET_VSC8211_FIBER: {

    // Fiber mode.
    gbe_mdio_wr(channel, phy_addr, 23, 0x32);

    // PHY speed/loopback.
    if (phy_speed == ENET_1G) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8000);
      gbe_mdio_wr(channel, phy_addr, 0, 0x8140);
      print("VCS8211 PHY configured for 1 Gbit/sec Full Duplex (fiber)");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4140);
        print("VCS8211 loopback configured");
      }
    }

    if (phy_speed == ENET_100M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8000);
      gbe_mdio_wr(channel, phy_addr, 0, 0x2100);
      print("VCS8211 configured for 100 Mbit/sec Full Duplex (fiber)");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x6100);
        print("VCS8211 loopback configured");
      }
    }

    if (phy_speed == ENET_10M) {
      gbe_mdio_wr(channel, phy_addr, 0, 0x8000);
      gbe_mdio_wr(channel, phy_addr, 0, 0x0100);
      print("VCS8211 configured for 10 Mbit/sec Full Duplex (fiber)");
      if (phy_loopback == 1) {
        gbe_mdio_wr(channel, phy_addr, 0, 0x4100);
        print("VCS8211 loopback configured");
      }
    }
    break;
  }
  }
}

void gbe_wait_for_link(uint32_t channel, 
                       uint32_t phyAddr,
                       uint32_t phyType)
{
  uint32_t timeOut;
  
  if ((channel != 0) && (channel != 1)) {
    print("gbe_wait_for_link(): invalid channel specified");
    TEST_FAIL(0xdead0026);
  }
  
  timeOut = 0;
  if (phyType == ENET_MV88E1121_COPPER) {
    while (((gbe_mdio_rd(channel, phyAddr, 17)) & 0x0400) != 0x0400) {
      timeOut++;
      if (timeOut > 500000) {
        print2("Gbe link down, channel=", channel);
        TEST_FAIL(0xbad);
        return;
      }
    }
  } else {
    while (((gbe_mdio_rd(channel, phyAddr, 1)) & 0x4) == 0) {
      timeOut++;
      if (timeOut > 500000) {
        print2("Gbe link down, channel=", channel);
        TEST_FAIL(0xbad);
        return;
      }
    }
  }
}

void gbe_find_gcd(uint32_t channel, 
                  uint32_t phyAddr)
{
  uint32_t advSpeed;
  uint32_t partnerSpeed;
  uint32_t x = GBE0_IDN1_X;
  uint32_t y = GBE0_IDN1_Y;

  if ((channel != 0) && (channel != 1)) {
    print("gbe_find_gcd(): invalid channel specified");
    TEST_FAIL(0xdead0027);
  }

  // Determine if we negotiated 1G
  advSpeed =  gbe_mdio_rd(channel, phyAddr, 9);
  partnerSpeed =  gbe_mdio_rd(channel, phyAddr, 10);
  if ((advSpeed & 0x0200) && (partnerSpeed & 0x0800)) {
    send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONFIG,  0x00120412);
    TEST_PASS(1000);
    return;
  }
  
  // Determine if we negotiated 100M
  advSpeed =  gbe_mdio_rd(channel, phyAddr, 4);
  partnerSpeed =  gbe_mdio_rd(channel, phyAddr, 5);
  if ((advSpeed & 0x0100) && (partnerSpeed & 0x0100)) {
    send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONFIG,  0x00120013);  
    TEST_PASS(100);
    return;
  }
      
  // Determine if we negotiated 10M
  if ((advSpeed & 0x0040) && (partnerSpeed & 0x0040)) {
    send_config_write(x, y, CFG_TAG, channel, GEM_NETWORK_CONFIG,  0x00120012);  
    TEST_PASS(10);
    return;
  }

  TEST_PASS(advSpeed);
  TEST_PASS(partnerSpeed);
  TEST_FAIL(0xbadbad);
}

void gbe_verify_counters(uint32_t channel, uint32_t pktCount) {

  uint32_t x = GBE0_IDN1_X;
  uint32_t y = GBE0_IDN1_Y;
  uint32_t rdData;
  
  if ((channel != 0) && (channel != 1)) {
    print("gbe_verify_counters(): invalid channel specified");
    TEST_FAIL(0xdead0028);
  }

  rdData = send_config_read(x, y, CFG_TAG, channel, GEM_FRAMES_RXED_OK);
  if (rdData != pktCount) {
    TEST_PASS(pktCount);
    TEST_PASS(rdData);
    print6("Invalid Gbe Tx packet counter, chan=", channel,
           ", exp=", pktCount,
           ", rec=", rdData);
    TEST_FAIL(0xdead0029);
  }
  rdData = send_config_read(x, y, CFG_TAG, channel, GEM_FRAMES_TXED_OK);
  if (rdData != pktCount) {
    TEST_PASS(pktCount);
    TEST_PASS(rdData);
    print6("Invalid Gbe Rx packet counter, chan=", channel,
           ", exp=", pktCount,
           ", rec=", rdData);
    TEST_FAIL(0xdead002a);
  }
}

//
// 82E1111 errata 3.1 workaround. 
//
void gbe_88e1111_rxclk_errata(uint32_t channel,
                              uint32_t phyAddr) {

  uint16_t rdData;

  if ((channel != 0) && (channel != 1)) {
    print("gbe_88e1111_rxclk_errata(): invalid channel specified");
    TEST_FAIL(0xdead002b);
  }

  // Undocumented page selects and control bits.
  gbe_mdio_wr(channel, phyAddr, 29, 0x001b);
  gbe_mdio_wr(channel, phyAddr, 30, 0x418f);
  gbe_mdio_wr(channel, phyAddr, 29, 0x0016);
  gbe_mdio_wr(channel, phyAddr, 30, 0xa2da);

  // Bits take effect and are retained by soft reset.
  rdData = gbe_mdio_rd(channel, phyAddr, 0);
  rdData |= 0x8000;
  gbe_mdio_wr(channel, phyAddr, 0, rdData);
}

//
// 82E1111 errata 4.38 workaround. 
//
void gbe_88e1111_clk125_jitter_errata(uint32_t channel,
                                      uint32_t phyAddr) {

  uint16_t rdData;

  if ((channel != 0) && (channel != 1)) {
    print("gbe_88e1111_clk125_jitter_errata(): invalid channel specified");
    TEST_FAIL(0xdead002c);
  }

  // Force manual master mode for 1G autonegotiation.
  rdData = gbe_mdio_rd(channel, phyAddr, 9);
  rdData |= 0x1800;
  gbe_mdio_wr(channel, phyAddr, 9, rdData);

  // Bits take effect and are retained by soft reset.
  rdData = gbe_mdio_rd(channel, phyAddr, 0);
  rdData |= 0x8000;
  gbe_mdio_wr(channel, phyAddr, 0, rdData);
}

//
// Broadcom PHY functions.
//

void bcm8706_loopback_enable(uint32_t mgmt_port, uint32_t phy_addr) {

  uint32_t phy_id, phy_data;

  // Verify we can read the PHY by checking the vendor ID.
  phy_id = xaui_mdio_rd(mgmt_port, phy_addr, 1, 2);
  phy_id |= (xaui_mdio_rd(mgmt_port, phy_addr, 1, 3) << 16);
  if (phy_id != ENET_BCM8706_ID) {
    if (mgmt_port == 1)
      print("ERROR: XAUI 1 unable to read BCM8706 ID");
    else
      print("ERROR: XAUI 0 unable to read BCM8706 ID");
    TEST_FAIL(0xdead002d);
  }

  // Verify that an SFP is installed by verifying the 
  // firmware load checksum.
  phy_data = xaui_mdio_rd(mgmt_port, phy_addr, 1, 0xca1c);
  if (phy_data != 0xffff) {
    if (mgmt_port == 1)
      print("ERROR: XAUI 1 BCM8706 SPF transceiver firmware load failed");
    else
      print("ERROR: XAUI 0 BCM8706 SPF transceiver firmware load failed");
    TEST_FAIL(0xdead002e);
  }


  // Enable PHY loopback mode.
  phy_data = xaui_mdio_rd(mgmt_port, phy_addr, 3, 0);
  phy_data |= 0x4000;
  xaui_mdio_wr(mgmt_port, phy_addr, 3, 0, phy_data);
  if (mgmt_port == 1)
    print("XAUI 1 enabled BCM8706 PCS loopback mode");
  else
    print("XAUI 0 enabled BCM8706 PCS loopback mode");
}

void bcm8706_loopback_disable(uint32_t mgmt_port, uint32_t phy_addr) {

  uint32_t phy_data;
  phy_data = xaui_mdio_rd(mgmt_port, phy_addr, 3, 0);
  phy_data &= ~(0x4000);
  xaui_mdio_wr(mgmt_port, phy_addr, 3, 0, phy_data);
  if (mgmt_port == 1)
    print("XAUI 1 disabled BCM8706 PCS loopback mode");
  else
    print("XAUI 0 disabled BCM8706 PCS loopback mode");
}

void vsc8211_auto_mdix(uint32_t mgmt_port, uint32_t phy_addr, uint32_t enable) {

  if (enable == 1) {
    gbe_mdio_wr(mgmt_port, phy_addr, 31, 0x2a30);
    gbe_mdio_wr(mgmt_port, phy_addr,  8, 0x0212);
    gbe_mdio_wr(mgmt_port, phy_addr, 31, 0x52b5);
    gbe_mdio_wr(mgmt_port, phy_addr,  2, 0x0012);
    gbe_mdio_wr(mgmt_port, phy_addr,  1, 0x2803);
    gbe_mdio_wr(mgmt_port, phy_addr,  0, 0x87fa);
    gbe_mdio_wr(mgmt_port, phy_addr, 31, 0x2a30);
    gbe_mdio_wr(mgmt_port, phy_addr,  8, 0x0012);
    gbe_mdio_wr(mgmt_port, phy_addr, 31, 0x0000);
    print("VCS8211 Auto MDIX enabled");
  } else {
    gbe_mdio_wr(mgmt_port, phy_addr, 31, 0x2a30);
    gbe_mdio_wr(mgmt_port, phy_addr,  8, 0x0212);
    gbe_mdio_wr(mgmt_port, phy_addr, 31, 0x52b5);
    gbe_mdio_wr(mgmt_port, phy_addr,  2, 0x0012);
    gbe_mdio_wr(mgmt_port, phy_addr,  1, 0x3003);
    gbe_mdio_wr(mgmt_port, phy_addr,  0, 0x87fa);
    gbe_mdio_wr(mgmt_port, phy_addr, 31, 0x2a30);
    gbe_mdio_wr(mgmt_port, phy_addr,  8, 0x0012);
    gbe_mdio_wr(mgmt_port, phy_addr, 31, 0x0000);
    print("VCS8211 Auto MDIX disabled");
  }
}
