#ifndef __BQ25672__
#define __BQ25672__

//BQ256772 Address
#define BQ_ADDR 0x6B

//BQ25672 Registers
#define REG00_Minimal_System_Voltage 0x00
#define REG01_Charge_Voltage_Limit   0x01
#define REG03_Charge_Current_Limit   0x03
#define REG05_Input_Voltage_Limit    0x05
#define REG06_Input_Current_Limit    0x06
#define REG08_Precharge_Control      0x08
#define REG09_Termination_Control    0x09
#define REG0A_Recharge_Control       0x0A
#define REG0B_VOTG_regulation        0x0B
#define REG0D_IOTG_regulation        0x0D
#define REG0E_Timer_Control          0x0E
#define REG0F_Charger_Control_0      0x0F
#define REG10_Charger_Control_1      0x10
#define REG11_Charger_Control_2      0x11
#define REG12_Charger_Control_3      0x12
#define REG13_Charger_Control_4      0x13
#define REG14_Charger_Control_5      0x14
#define REG15_MPPT_Control           0x15
#define REG16_Temperature_Control    0x16
#define REG17_NTC_Control_0          0x17
#define REG18_NTC_Control_1          0x18
#define REG19_ICO_Current_Limit      0x19
#define REG1B_Charger_Status_0       0x1B
#define REG1C_Charger_Status_1       0x1C
#define REG1D_Charger_Status_2       0x1D
#define REG1E_Charger_Status_3       0x1E
#define REG1F_Charger_Status_4       0x1F
#define REG20_FAULT_Status_0         0x20
#define REG21_FAULT_Status_1         0x21
#define REG22_Charger_Flag_0         0x22
#define REG23_Charger_Flag_1         0x23
#define REG24_Charger_Flag_2         0x24
#define REG25_Charger_Flag_3         0x25
#define REG26_FAULT_Flag_0           0x26
#define REG27_FAULT_Flag_1           0x27
#define REG28_Charger_Mask_0         0x28
#define REG29_Charger_Mask_1         0x29
#define REG2A_Charger_Mask_2         0x2A
#define REG2B_Charger_Mask_3         0x2B
#define REG2C_FAULT_Mask_0           0x2C
#define REG2D_FAULT_Mask_1           0x2D
#define REG2E_ADC_Control            0x2E
#define REG2F_ADC_Function_Disable_0 0x2F
#define REG30_ADC_Function_Disable_1 0x30
#define REG31_IBUS_ADC               0x31
#define REG33_IBAT_ADC               0x33
#define REG35_VBUS_ADC               0x35
#define REG37_VAC1_ADC               0x37
#define REG39_VAC2_ADC               0x39
#define REG3B_VBAT_ADC               0x3B
#define REG3D_VSYS_ADC               0x3D
#define REG3F_TS_ADC                 0x3F
#define REG41_TDIE_ADC               0x41
#define REG43_Dp_ADC                 0x43
#define REG45_Dm_ADC                 0x45
#define REG47_DPDM_Driver            0x47
#define REG48_Part_Infotmation       0x48
          
extern bq25672_init(unsigned int8 add = BQ_ADDR);
extern void bq25672_state(unsigned int8 add = BQ_ADDR, unsigned int16 reg = 0x00, unsigned int1 text = 0);
extern void bq25672_update(unsigned int8 add = BQ_ADDR);
#endif
/*
def main():
    bbq = BQ25672(0x6B)
    
    #I2C Registers for BQ25672
    REG00_Minimal_System_Voltage = 0x00
    REG01_Charge_Voltage_Limit   = 0x01
    REG03_Charge_Current_Limit   = 0x03
    REG05_Input_Voltage_Limit    = 0x05
    REG06_Input_Current_Limit    = 0x06
    REG08_Precharge_Control      = 0x08
    REG09_Termination_Control    = 0x09
    REG0A_Recharge_Control       = 0x0A
    REG0B_VOTG_regulation        = 0x0B
    REG0D_IOTG_regulation        = 0x0D
    REG0E_Timer_Control          = 0x0E
    REG0F_Charger_Control_0      = 0x0F
    REG10_Charger_Control_1      = 0x10
    REG11_Charger_Control_2      = 0x11
    REG12_Charger_Control_3      = 0x12
    REG13_Charger_Control_4      = 0x13
    REG14_Charger_Control_5      = 0x14
    REG15_MPPT_Control           = 0x15
    REG16_Temperature_Control    = 0x16
    REG17_NTC_Control_0          = 0x17
    REG18_NTC_Control_1          = 0x18
    REG19_ICO_Current_Limit      = 0x19
    REG1B_Charger_Status_0       = 0x1B
    REG1C_Charger_Status_1       = 0x1C
    REG1D_Charger_Status_2       = 0x1D
    REG1E_Charger_Status_3       = 0x1E
    REG1F_Charger_Status_4       = 0x1F
    REG20_FAULT_Status_0         = 0x20
    REG21_FAULT_Status_1         = 0x21
    REG22_Charger_Flag_0         = 0x22
    REG23_Charger_Flag_1         = 0x23
    REG24_Charger_Flag_2         = 0x24
    REG25_Charger_Flag_3         = 0x25
    REG26_FAULT_Flag_0           = 0x26
    REG27_FAULT_Flag_1           = 0x27
    REG28_Charger_Mask_0         = 0x28
    REG29_Charger_Mask_1         = 0x29
    REG2A_Charger_Mask_2         = 0x2A
    REG2B_Charger_Mask_3         = 0x2B
    REG2C_FAULT_Mask_0           = 0x2C
    REG2D_FAULT_Mask_1           = 0x2D
    REG2E_ADC_Control            = 0x2E
    REG2F_ADC_Function_Disable_0 = 0x2F
    REG30_ADC_Function_Disable_1 = 0x30
    REG31_IBUS_ADC               = 0x31
    REG33_IBAT_ADC               = 0x33
    REG35_VBUS_ADC               = 0x35
    REG37_VAC1_ADC               = 0x37
    REG39_VAC2_ADC               = 0x39
    REG3B_VBAT_ADC               = 0x3B
    REG3D_VSYS_ADC               = 0x3D
    REG3F_TS_ADC                 = 0x3F
    REG41_TDIE_ADC               = 0x41
    REG43_Dp_ADC                 = 0x43
    REG45_Dm_ADC                 = 0x45
    REG47_DPDM_Driver            = 0x47
    REG48_Part_Infotmation       = 0x48
    
    
    # reg = mag.read8(REG48_Part_Infotmation)
    reg = bbq.check_masurement()
    
    print('REG00_Minimal_System_Voltage ', 2500 + 250 * bbq.read8d(REG00_Minimal_System_Voltage), ' mV')
    print('REG01_Charge_Voltage_Limit '  , 10 * bbq.read16(REG01_Charge_Voltage_Limit), ' mV')
    print('REG03_Charge_Current_Limit '  , 10 * bbq.read16(REG03_Charge_Current_Limit), ' mA')
    print('REG05_Input_Voltage_Limit '   , 100 * bbq.read8d(REG05_Input_Voltage_Limit), ' mV')
    print('REG06_Input_Current_Limit '   , 10 * bbq.read16(REG06_Input_Current_Limit), ' mA')
    print('REG08_Precharge_Control '     , bbq.read8(REG08_Precharge_Control))
    print('REG09_Termination_Control* '  , 40 * bbq.read8d(REG09_Termination_Control), ' mA')
    print('REG0A_Recharge_Control '      , bbq.read8(REG0A_Recharge_Control))
    print('REG0B_VOTG_regulation '       , 2800 + 10 * bbq.read16(REG0B_VOTG_regulation), ' mV')
    print('REG0D_IOTG_regulation* '      , 40 * bbq.read8d(REG0D_IOTG_regulation), ' mA')
    print('REG0E_Timer_Control '         , bbq.read8(REG0E_Timer_Control))
    print('REG0F_Charger_Control_0 '     , bbq.read8(REG0F_Charger_Control_0))
    print('REG10_Charger_Control_1 '     , bbq.read8(REG10_Charger_Control_1))
    print('REG11_Charger_Control_2 '     , bbq.read8(REG11_Charger_Control_2))
    print('REG12_Charger_Control_3 '     , bbq.read8(REG12_Charger_Control_3))
    print('REG13_Charger_Control_4 '     , bbq.read8(REG13_Charger_Control_4))
    print('REG14_Charger_Control_5 '     , bbq.read8(REG14_Charger_Control_5))
    print('REG15_MPPT_Control '          , bbq.read8(REG15_MPPT_Control))
    print('REG16_Temperature_Control '   , bbq.read8(REG16_Temperature_Control))
    print('REG17_NTC_Control_0 '         , bbq.read8(REG17_NTC_Control_0))
    print('REG18_NTC_Control_1 '         , bbq.read8(REG18_NTC_Control_1))
    print('REG19_ICO_Current_Limit '     , 10 * bbq.read8d(REG19_ICO_Current_Limit), ' mA')
    print('REG1B_Charger_Status_0 '      , bbq.read8(REG1B_Charger_Status_0))
    print('REG1C_Charger_Status_1 '      , bbq.read8(REG1C_Charger_Status_1))
    print('REG1D_Charger_Status_2 '      , bbq.read8(REG1D_Charger_Status_2))
    print('REG1E_Charger_Status_3 '      , bbq.read8(REG1E_Charger_Status_3))
    print('REG1F_Charger_Status_4 '      , bbq.read8(REG1F_Charger_Status_4))
    print('REG20_FAULT_Status_0 '        , bbq.read8(REG20_FAULT_Status_0))
    print('REG21_FAULT_Status_1 '        , bbq.read8(REG21_FAULT_Status_1))
    print('REG22_Charger_Flag_0 '        , bbq.read8(REG22_Charger_Flag_0))
    print('REG23_Charger_Flag_1 '        , bbq.read8(REG23_Charger_Flag_1))
    print('REG24_Charger_Flag_2 '        , bbq.read8(REG24_Charger_Flag_2))
    print('REG25_Charger_Flag_3 '        , bbq.read8(REG25_Charger_Flag_3))
    print('REG26_FAULT_Flag_0 '          , bbq.read8(REG26_FAULT_Flag_0))
    print('REG27_FAULT_Flag_1 '          , bbq.read8(REG27_FAULT_Flag_1))
    print('REG28_Charger_Mask_0 '        , bbq.read8(REG28_Charger_Mask_0))
    print('REG29_Charger_Mask_1 '        , bbq.read8(REG29_Charger_Mask_1))
    print('REG2A_Charger_Mask_2 '        , bbq.read8(REG2A_Charger_Mask_2))
    print('REG2B_Charger_Mask_3 '        , bbq.read8(REG2B_Charger_Mask_3))
    print('REG2C_FAULT_Mask_0 '          , bbq.read8(REG2C_FAULT_Mask_0))
    print('REG2D_FAULT_Mask_1 '          , bbq.read8(REG2D_FAULT_Mask_1))
    print('REG2E_ADC_Control '           , bbq.read8(REG2E_ADC_Control))
    print('REG2F_ADC_Function_Disable_0 ', bbq.read8(REG2F_ADC_Function_Disable_0))
    print('REG30_ADC_Function_Disable_1 ', bbq.read8(REG30_ADC_Function_Disable_1))
    print('REG31_IBUS_ADC '              , bbq.read16(REG31_IBUS_ADC), ' mA')
    print('REG33_IBAT_ADC '              , bbq.read16(REG33_IBAT_ADC), ' mA')
    print('REG35_VBUS_ADC '              , bbq.read16(REG35_VBUS_ADC), ' mV')
    print('REG37_VAC1_ADC '              , bbq.read16(REG37_VAC1_ADC), ' mV')
    print('REG39_VAC2_ADC '              , bbq.read16(REG39_VAC2_ADC), ' mV')
    print('REG3B_VBAT_ADC '              , bbq.read16(REG3B_VBAT_ADC), ' mV')
    print('REG3D_VSYS_ADC '              , bbq.read16(REG3D_VSYS_ADC), ' mV')
    print('REG3F_TS_ADC '                , bbq.read16(REG3F_TS_ADC), ' mV')
    print('REG41_TDIE_ADC '              , 0.5 * bbq.read16(REG41_TDIE_ADC), ' C')
    print('REG43_Dp_ADC '                , bbq.read16(REG43_Dp_ADC), ' mV')
    print('REG45_Dm_ADC '                , bbq.read16(REG45_Dm_ADC), ' mV')
    print('REG47_DPDM_Driver '           , bbq.read8(REG47_DPDM_Driver))
    print('REG48_Part_Infotmation '      , bbq.read8(REG48_Part_Infotmation))
    
    #ADC Values Update
    update_adc = bbq.write8(REG2E_ADC_Control, 0xb0)
    
    print(update_adc)
    # print(bin(reg))
    # eh = mag.write8(RM3100_REGISTER_CMM, 0x79)
    # #print(eh)
    # ccx = mag.write16(RM3100_REGISTER_CMX, 800)
    # ccy = mag.write16(RM3100_REGISTER_CMY, 800)    
    # ccz = mag.write16(RM3100_REGISTER_CMZ, 800)
    # #print(ccx)
    # #print(ccy)
    # #print(ccz)
    # rccx = mag.read16(RM3100_REGISTER_CMX)
    # rccy = mag.read16(RM3100_REGISTER_CMY)    
    # rccz = mag.read16(RM3100_REGISTER_CMZ)
    # #print(rccx)
    # #print(rccy)
    # #print(rccz)
    # #print(mag.read8(RM3100_REGISTER_CMX))
    # #print(mag.read8(RM3100_REGISTER_CMX+1))
    # counter1 = 0
    
    # tstart = time.time()
    # tfinish = tstart +4* 3600
    
    # while(time.time() <= tfinish):
    # #while True:
        
    #     start = time.time()
    #     finish = start + 1
    #     x = 0
    #     y = 0
    #     z = 0
    #     counter2 = 0
    #     counter1 += 1
        
    #     while (time.time()<= finish):
    #         if mag.check_masurement():
    #             mag_data = mag.read_measurements()
    #             x += mag_data['x']
    #             y += mag_data['y']
    #             z += mag_data['z']
    #             counter2 += 1
    #             #print(mag_data['x'], mag_data['y'], mag_data['z'])
    #             #print(counter1, counter2, x, y, z)
    #             #time.sleep(0.027)
    #             while (mag.check_masurement()==True):
    #                 pass
    #         #else:
    #             #break
    
    #     print(counter1, counter2, x, y, z)
if __name__ == "__main__":
    main()
*/
