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

#ifndef __MAESTRO_RSHIM_REGS_H
#define __MAESTRO_RSHIM_REGS_H

// rshim register definitions from rshim.vh

#define RSHIM_REG_BASELINE         0x000 
#define RSHIM_REG_BASELINE_CTL     0x004 
#define RSHIM_REG_CONFIG_INTFC_STS 0x008 
#define RSHIM_REG_SCRATCHPAD       0x00C 
#define RSHIM_REG_IDN_CONFIG       0x010

#define RSHIM_REG_INT_STS          0x300
#define RSHIM_REG_INT_MASK         0x304
#define RSHIM_REG_INT_MODE         0x308
#define RSHIM_REG_INT_DEST0        0x30C
#define RSHIM_REG_INT_DEST1        0x310

#define RSHIM_REG_BND0_0           0x400 
#define RSHIM_REG_BND0_1           0x404 
#define RSHIM_REG_BND0_2           0x408 
#define RSHIM_REG_BND1_0           0x40c 
#define RSHIM_REG_BND1_1           0x410 
#define RSHIM_REG_BND1_2           0x414 
#define RSHIM_REG_BND2_0           0x418 
#define RSHIM_REG_BND2_1           0x41C 
#define RSHIM_REG_BND2_2           0x420 
#define RSHIM_REG_BND3_0           0x424 
#define RSHIM_REG_BND3_1           0x428 
#define RSHIM_REG_BND3_2           0x42C 

#define RSHIM_REG_IDN_THRESH       0x500
#define RSHIM_REG_STN_DATA         0x600
#define RSHIM_REG_STN_CTL          0x604
#define RSHIM_REG_IDN_DATA         0x700
#define RSHIM_REG_IDN_CTL          0x740

#define RSHIM_REG_SWINT            0x800

#define RSHIM_REG_SEM              0x810
#define RSHIM_REG_SEM_STS          0x814

#define RSHIM_REG_DCNT0_CUR        0x820
#define RSHIM_REG_DCNT0_REF        0x824
#define RSHIM_REG_DCNT0_CTL        0x828
#define RSHIM_REG_DCNT1_CUR        0x830
#define RSHIM_REG_DCNT1_REF        0x834
#define RSHIM_REG_DCNT1_CTL        0x838
#define RSHIM_REG_DCNT2_CUR        0x840
#define RSHIM_REG_DCNT2_REF        0x844
#define RSHIM_REG_DCNT2_CTL        0x848

#define RSHIM_REG_DEVID0           0x850
#define RSHIM_REG_DEVID1           0x854

#define RSHIM_REG_P_RAND           0x860
#define RSHIM_REG_P_RAND_CTL       0x864

#define RSHIM_REG_DIAG_BCST        0x868

#define RSHIM_REG_JTAG_CTL         0x870
#define RSHIM_REG_JTAG_SETUP       0x874
#define RSHIM_REG_JTAG_DATA        0x878

#define RSHIM_REG_SHIM_DISABLE     0x880

#define RSHIM_REG_CCNT             0x884

#define RSHIM_REG_PIN_MUX          0x888

#define RSHIM_REG_RESET_CTL        0x88C

#define RSHIM_REG_CLOCK_CTL        0x890

#define RSHIM_REG_PROTECT          0x894

#endif // __MAESTRO_RSHIM_REGS_H
