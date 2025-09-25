#include "BQ25672.h"

#module					// !!! Important: This command makes everything below scoped only to this file.

int1 bq25672_init(unsigned int8 add = BQ_ADDR)
{
	//int8(i2c address) -> bool
	//Check if the ina226 device is connected in the indicated address (add)
	//example: ina226_init(INA226_ADDRESS_12) returns TRUE if there is a INA226 connected in that direction, otherwise return FALSE
	unsigned int8 rdata;
			
	rdata = i2c1_read_8(add, REG48_Part_Infotmation);

	if (rdata ==0x21)
	{
		return 1;
	} 
	else
	{
		return 0;
	}	
}

void bq25672_state(unsigned int8 add = BQ_ADDR, unsigned int16 reg = 0x00, unsigned int1 text=0)
{
	switch (reg)
	{
		case 0x00:
		{	
			if (text)
			{
				printf("%Lu", 2500 + 250 * i2c1_read_8(add, REG00_Minimal_System_Voltage));
			}
			else
			{
				printf("REG00_Minimal_System_Voltage %Lu mV\n\r", 2500 + 250 * i2c1_read_8(add, REG00_Minimal_System_Voltage));
			}
			break;
		}
		case 0x01:
		{
			if (text)
			{
				printf("%Lu", 10 * i2c1_read_16(add, REG01_Charge_Voltage_Limit));
			}
			else
			{
				printf("REG01_Charge_Voltage_Limit %Lu mV\n", 10 * i2c1_read_16(add, REG01_Charge_Voltage_Limit));
			}
			break;
		}
		case 0x03:
		{
			if (text)
			{
				printf("%Lu", 10 * i2c1_read_16(add, REG03_Charge_Current_Limit));		
			}
			else
			{
				printf("REG03_Charge_Current_Limit %Lu mA\n", 10 * i2c1_read_16(add, REG03_Charge_Current_Limit));		
			}
			break;
		}
	
		case 0x05:
		{
			if (text)
			{
				printf("%Lu", 100 * i2c1_read_8(add, REG05_Input_Voltage_Limit));	
			}
			else
			{
				printf("REG05_Input_Voltage_Limit %Lu mV\n", 100 * i2c1_read_8(add, REG05_Input_Voltage_Limit));	
			}
			break;
		}
		
		case 0x06:
		{
			if (text)
			{
				printf("%Lu"   , 10 * i2c1_read_16(add, REG06_Input_Current_Limit));
			}
			else
			{
				printf("REG06_Input_Current_Limit %Lu mA\n", 10 * i2c1_read_16(add, REG06_Input_Current_Limit));
			}
			break;
		}
	    
	
	    case 0x08:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG08_Precharge_Control));
			}
			else
			{
				printf("REG08_Precharge_Control %X\n", i2c1_read_8(add, REG08_Precharge_Control));
			}
		    break;
		}
	
		case 0x09:
		{
			if (text)
			{
				printf("%Lu"  , 40 * i2c1_read_8(add, REG09_Termination_Control));
			}
			else
			{
				printf("REG09_Termination_Control* %Lu mA\n"  , 40 * i2c1_read_8(add, REG09_Termination_Control));
			}
		    break;
		}
	    
		case 0x0A:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG0A_Recharge_Control));
			}
			else
			{
				printf("REG0A_Recharge_Control %X\n", i2c1_read_8(add, REG0A_Recharge_Control));
			}
		    break;
		}
	    
		case 0x0B:
		{
			if (text)
			{
				printf("%Lu", 2800 + 10 * i2c1_read_16(add, REG0B_VOTG_regulation));
			}
			else
			{
				printf("REG0B_VOTG_regulation %Lu mV\n", 2800 + 10 * i2c1_read_16(add, REG0B_VOTG_regulation));
			}
		    break;
		}
	    
		case 0x0D:
		{
			if (text)
			{
				printf("%Lu", 40 * i2c1_read_8(add, REG0D_IOTG_regulation));
			}
			else
			{
				printf("REG0D_IOTG_regulation* %Lu mA\n", 40 * i2c1_read_8(add, REG0D_IOTG_regulation));
			}
		    break;
		}
	    
		case 0x0E:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG0E_Timer_Control));
			}
			else
			{
				printf("REG0E_Timer_Control %X\n", i2c1_read_8(add, REG0E_Timer_Control));
			}
		    break;
		}
	    
		case 0x0F:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG0F_Charger_Control_0));
			}
			else
			{
				printf("REG0F_Charger_Control_0 %X\n", i2c1_read_8(add, REG0F_Charger_Control_0));
			}
		    break;
		}
	    
		case 0x10:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG10_Charger_Control_1));
			}
			else
			{
				printf("REG10_Charger_Control_1 %X\n", i2c1_read_8(add, REG10_Charger_Control_1)); 
			}
		    break;
		}
		
		case 0x11:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG11_Charger_Control_2));
			}
			else
			{
				printf("REG11_Charger_Control_2 %X\n", i2c1_read_8(add, REG11_Charger_Control_2));
			}
		    break;
		}
	    
		case 0x12:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG12_Charger_Control_3));
			}
			else
			{
				printf("REG12_Charger_Control_3 %X\n", i2c1_read_8(add, REG12_Charger_Control_3));
			}
		    break;
		}
	    
		case 0x13:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG13_Charger_Control_4));
			}
			else
			{
				printf("REG13_Charger_Control_4 %X\n", i2c1_read_8(add, REG13_Charger_Control_4));
			}
		    break;
		}
	    
		case 0x14:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG14_Charger_Control_5));
			}
			else
			{
				printf("REG14_Charger_Control_5 %X\n", i2c1_read_8(add, REG14_Charger_Control_5));
			}
		    break;
		}
	    
		case 0x15:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG15_MPPT_Control));
			}
			else
			{
				printf("REG15_MPPT_Control %X\n", i2c1_read_8(add, REG15_MPPT_Control));
			}
		    break;
		}
	    
		case 0x16:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG16_Temperature_Control));
			}
			else
			{
				printf("REG16_Temperature_Control %X\n", i2c1_read_8(add, REG16_Temperature_Control));
			}
		    break;
		}
	    
		case 0x17:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG17_NTC_Control_0));
			}
			else
			{
				printf("REG17_NTC_Control_0 %X\n", i2c1_read_8(add, REG17_NTC_Control_0));
			}
		    break;
		}
	    
		case 0x18:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG18_NTC_Control_1));
			}
			else
			{
				printf("REG18_NTC_Control_1 %X\n", i2c1_read_8(add, REG18_NTC_Control_1));
			}
		    break;
		}
	    
		case 0x19:
		{
			if (text)
			{
				printf("%Lu", 10 * i2c1_read_8(add, REG19_ICO_Current_Limit));
			}
			else
			{
				printf("REG19_ICO_Current_Limit %Lu mA\n", 10 * i2c1_read_8(add, REG19_ICO_Current_Limit));
			}
		    break;
		}
	    
		case 0x1B:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG1B_Charger_Status_0));
			}
			else
			{
				printf("REG1B_Charger_Status_0 %X\n", i2c1_read_8(add, REG1B_Charger_Status_0));
			}
		    break;
		}
	    
		case 0x1C:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG1C_Charger_Status_1));
			}
			else
			{
				printf("REG1C_Charger_Status_1 %X\n", i2c1_read_8(add, REG1C_Charger_Status_1));
			}
		    break;
		}
	    
		case 0x1D:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG1D_Charger_Status_2));
			}
			else
			{
				printf("REG1D_Charger_Status_2 %X\n", i2c1_read_8(add, REG1D_Charger_Status_2));
			}
		    break;
		}
	    
		case 0x1E:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG1E_Charger_Status_3));
			}
			else
			{
				printf("REG1E_Charger_Status_3 %X\n", i2c1_read_8(add, REG1E_Charger_Status_3));
			}
		    break;
		}
	    
		case 0x1F:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG1F_Charger_Status_4));
			}
			else
			{
				printf("REG1F_Charger_Status_4 %X\n", i2c1_read_8(add, REG1F_Charger_Status_4));
			}
		    break;
		}
	    
		case 0x20:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG20_FAULT_Status_0));
			}
			else
			{
				printf("REG20_FAULT_Status_0 %X\n", i2c1_read_8(add, REG20_FAULT_Status_0));
			}
		    break;
		}
	
	    case 0x21:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG21_FAULT_Status_1));
			}
			else
			{
				printf("REG21_FAULT_Status_1 %X\n", i2c1_read_8(add, REG21_FAULT_Status_1));
			}
		    break;
		}
	
		case 0x22:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG22_Charger_Flag_0));
			}
			else
			{
				printf("REG22_Charger_Flag_0 %X\n", i2c1_read_8(add, REG22_Charger_Flag_0));
			}
			break;
		}
	    
		case 0x23:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG23_Charger_Flag_1));
			}
			else
			{
				printf("REG23_Charger_Flag_1 %X\n", i2c1_read_8(add, REG23_Charger_Flag_1));
			}
			break;
		}
	    
		case 0x24:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG24_Charger_Flag_2));
			}
			else
			{
				printf("REG24_Charger_Flag_2 %X\n", i2c1_read_8(add, REG24_Charger_Flag_2));
			}
			break;
		}
	    
		case 0x25:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG25_Charger_Flag_3));
			}
			else
			{
				printf("REG25_Charger_Flag_3 %X\n", i2c1_read_8(add, REG25_Charger_Flag_3));
			}
			break;
		}
	    
		case 0x26:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG26_FAULT_Flag_0));
			}
			else
			{
				printf("REG26_FAULT_Flag_0 %X\n", i2c1_read_8(add, REG26_FAULT_Flag_0));
			}
			break;
		}
	    
		case 0x27:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG27_FAULT_Flag_1));
			}
			else
			{
				printf("REG27_FAULT_Flag_1 %X\n", i2c1_read_8(add, REG27_FAULT_Flag_1));
			}
			break;
		}
	    
		case 0x28:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG28_Charger_Mask_0));
			}
			else
			{
				printf("REG28_Charger_Mask_0 %X\n", i2c1_read_8(add, REG28_Charger_Mask_0));
			}
		    break;
		}
	
		case 0x29:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG29_Charger_Mask_1));
			}
			else
			{
				printf("REG29_Charger_Mask_1 %X\n", i2c1_read_8(add, REG29_Charger_Mask_1));
			}
		    break;
		}
	
		case 0x2A:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG2A_Charger_Mask_2));
			}
			else
			{
				printf("REG2A_Charger_Mask_2 %X\n", i2c1_read_8(add, REG2A_Charger_Mask_2));
			}
			break;
		}
	    
		case 0x2B:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG2B_Charger_Mask_3));
			}
			else
			{
				printf("REG2B_Charger_Mask_3 %X\n", i2c1_read_8(add, REG2B_Charger_Mask_3));
			}
			break;
		}
	    
		case 0x2C:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG2C_FAULT_Mask_0));
			}
			else
			{
				printf("REG2C_FAULT_Mask_0 %X\n", i2c1_read_8(add, REG2C_FAULT_Mask_0));
			}
			break;
		}
	    
		case 0x2D:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG2D_FAULT_Mask_1));
			}
			else
			{
				printf("REG2D_FAULT_Mask_1 %X\n", i2c1_read_8(add, REG2D_FAULT_Mask_1));
			}
		    break;
		}
	
		case 0x2E:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG2E_ADC_Control));
			}
			else
			{
				printf("REG2E_ADC_Control %X\n", i2c1_read_8(add, REG2E_ADC_Control));
			}
			break;
		}
	    
		case 0x2F:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG2F_ADC_Function_Disable_0));
			}
			else
			{
				printf("REG2F_ADC_Function_Disable_0 %X\n", i2c1_read_8(add, REG2F_ADC_Function_Disable_0));
			}
			break;
		}
	    
		case 0x30:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG30_ADC_Function_Disable_1));
			}
			else
			{
				printf("REG30_ADC_Function_Disable_1 %X\n", i2c1_read_8(add, REG30_ADC_Function_Disable_1));
			}
		    break;
		}
	
		case 0x31:
		{
			if (text)
			{
				printf("%Ld", i2c1_read_16(add, REG31_IBUS_ADC));
			}
			else
			{
				printf("REG31_IBUS_ADC %Ld mA\n", i2c1_read_16(add, REG31_IBUS_ADC));
			}
			break;
		}    
		case 0x33:
		{
			if (text)
			{
				printf("%Ld", i2c1_read_16(add, REG33_IBAT_ADC));
			}
			else
			{
				printf("REG33_IBAT_ADC %Ld mA\n", i2c1_read_16(add, REG33_IBAT_ADC));
			}
		    break;
		}
	
		case 0x35:
		{
			if (text)
			{
				printf("%Ld", i2c1_read_16(add, REG35_VBUS_ADC));
			}
			else
			{
				printf("REG35_VBUS_ADC %Ld mV\n", i2c1_read_16(add, REG35_VBUS_ADC));
			}
			break;
		}
	    
		case 0x37:
		{
			if (text)
			{
				printf("%Ld", i2c1_read_16(add, REG37_VAC1_ADC));
			}
			else
			{
				printf("REG37_VAC1_ADC %Ld mV\n", i2c1_read_16(add, REG37_VAC1_ADC));
			}
		    break;
		}
	
		case 0x39:
		{
			if (text)
			{
				printf("%Ld", i2c1_read_16(add, REG39_VAC2_ADC));
			}
			else
			{
				printf("REG39_VAC2_ADC %Ld mV\n", i2c1_read_16(add, REG39_VAC2_ADC));
			}
			break;
		}
	    
		case 0x3B:
		{
			if (text)
			{
				printf("%Lu", i2c1_read_16(add, REG3B_VBAT_ADC));
			}
			else
			{
				printf("REG3B_VBAT_ADC %Lu mV\n", i2c1_read_16(add, REG3B_VBAT_ADC));
			}
			break;
		}
	    
		case 0x3D:
		{
			if (text)
			{
				printf("%Ld", i2c1_read_16(add, REG3D_VSYS_ADC));
			}
			else
			{
				printf("REG3D_VSYS_ADC %Ld mV\n", i2c1_read_16(add, REG3D_VSYS_ADC));
			}			
			break;
		}
	    
		case 0x3F:
		{
			if (text)
			{
				printf("%f", 0.0976563 * i2c1_read_16(add, REG3F_TS_ADC));
			}
			else
			{
				printf("REG3F_TS_ADC %f \n", 0.0976563 * i2c1_read_16(add, REG3F_TS_ADC));
			}	
			break;
		}
	    
		case 0x41:
		{
			if (text)
			{
				printf("%f", 0.5 * i2c1_read_16(add, REG41_TDIE_ADC));
			}
			else
			{
				printf("REG41_TDIE_ADC %f C\n", 0.5 * i2c1_read_16(add, REG41_TDIE_ADC));
			}			
		    break;
		}
	
		case 0x43:
		{
			if (text)
			{
				printf("%Lu", i2c1_read_16(add, REG43_Dp_ADC));
			}
			else
			{
				printf("REG43_Dp_ADC %Lu mV\n", i2c1_read_16(add, REG43_Dp_ADC));
			}
		    break;
		}
	
		case 0x45:
		{
			if (text)
			{
				printf("%Lu", i2c1_read_16(add, REG45_Dm_ADC));
			}
			else
			{
				printf("REG45_Dm_ADC %Lu mV\n", i2c1_read_16(add, REG45_Dm_ADC));
			}
		    break;
		}
	
		case 0x47:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG47_DPDM_Driver));
			}
			else
			{
				printf("REG47_DPDM_Driver %X\n", i2c1_read_8(add, REG47_DPDM_Driver));
			}
		    break;
		}
	
		case 0x48:
		{
			if (text)
			{
				printf("%X", i2c1_read_8(add, REG48_Part_Infotmation)); 
			}
			else
			{
				printf("REG48_Part_Infotmation %X\n", i2c1_read_8(add, REG48_Part_Infotmation)); 
			}
			break;
		}
	
		default:
		{
			break;
		}
	}
}

void bq25672_update(unsigned int8 add = BQ_ADDR)
{
	i2c1_write_8(add, REG2E_ADC_Control, 0xB0);
}