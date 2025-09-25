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

#ifndef __MAESTRO_MSH_REGS_H
#define __MAESTRO_MSH_REGS_H

// MSH regs from msh.vh

#define MSH_REG_BASELINE_STS 0x000 
#define MSH_REG_BASELINE_CTL 0x004 
#define MSH_REG_CFGIF_STS    0x008 
#define MSH_REG_SCRATCH      0x00c 
#define MSH_REG_IDN_CFG      0x010
#define MSH_REG_MDN_CFG      0x014
#define MSH_REG_MM_CFG       0x018

#define MSH_REG_INT_STS      0x080
#define MSH_REG_INT_MASK     0x084
#define MSH_REG_INT_MODE     0x088
#define MSH_REG_INT_DEST     0x08c
#define MSH_REG_BND0_CTL     0x0a0
#define MSH_REG_BND0_RTE     0x0a4
#define MSH_REG_BND0_TAG     0x0a8
#define MSH_REG_BND1_CTL     0x0ac
#define MSH_REG_BND1_RTE     0x0b0
#define MSH_REG_BND1_TAG     0x0b4

#define MSH_REG_CTL          0x100
#define MSH_REG_STARVE_MAX   0x104
#define MSH_REG_ACTIVE_MAX   0x108
#define MSH_REG_ADDR_RANGE   0x10c
                               
#define MSH_REG_EBUF_LVL     0x110 
#define MSH_REG_EMPTY        0x114
#define MSH_REG_FULL         0x118
#define MSH_REG_CREDIT       0x11c
#define MSH_REG_RH_CNT       0x120
#define MSH_REG_RD_CNT       0x124
#define MSH_REG_ORD_CNT      0x128 
#define MSH_REG_WH_CNT       0x130
#define MSH_REG_ROH_CNT      0x134 
#define MSH_REG_R2O_CNT      0x138 
#define MSH_REG_ERR_LOG0     0x140 
#define MSH_REG_ERR_LOG1     0x144 
#define MSH_REG_ERR_LOG2     0x148 
#define MSH_REG_ERR_LOG3     0x14c
#define MSH_REG_DIAG_CTL     0x150
#define MSH_REG_DIAG_ADDR    0x154
#define MSH_REG_DIAG_WDAT    0x158
#define MSH_REG_DIAG_RDAT    0x15c
#define MSH_REG_CCNT         0x160
#define MSH_REG_ID           0x164
#define MSH_REG_MODE         0x170

// DDR registers
#define DDR_CFG_BASELINE   0x400 
#define DDR_CFG_CTL        0x404 
#define DDR_CFG_DEVICE     0x408
#define DDR_CFG_TIM_ROW    0x410
#define DDR_CFG_TIM_COL    0x414
#define DDR_CFG_TIM_MISC   0x418
#define DDR_CFG_TIM_REF0   0x41c
#define DDR_CFG_TIM_REF1   0x420
#define DDR_CFG_EMR        0x424
#define DDR_CFG_USER_INIT0 0x428
#define DDR_CFG_USER_INIT1 0x42c
#define DDR_CFG_ODT_RD     0x430
#define DDR_CFG_ODT_WR     0x434
#define DDR_CFG_DLL0       0x43c
#define DDR_CFG_DLL1       0x440
#define DDR_CFG_MPHY_DELAY 0x444
#define DDR_CFG_PAD_ELEC0  0x448
#define DDR_CFG_PAD_ELEC1  0x44c
#define DDR_CFG_PAD_ENABLE 0x450
#define DDR_CFG_PAD_COMP_DRV 0x454
#define DDR_CFG_PAD_COMP_RTT 0x458
#define DDR_CFG_PLL          0x45c
#define DDR_CFG_PAD_COMP_MO0 0x460
#define DDR_CFG_PAD_COMP_MO1 0x464

#define ROW_BITS_13 2
#define ROW_BITS_14 3

#define COL_BITS_10 5

#define BANK_BITS_2 0

#endif // __MAESTRO_MSH_REGS_H
