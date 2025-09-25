#include "eps.h"

#module					// !!! Important: This command makes everything below scoped only to this file.

void eps_read_command(unsigned char *string_pointer)
{
	//EPS UART Get Command
	// char(pointer)  -> none
	//reads a character chain send via UART
	//example: 
	
	while (*string_pointer != 0)
	{	
		*string_pointer = fgetc(SERIAL);				
		
		if (*string_pointer == 0x0D)
		{
			*string_pointer = 0;
		}
		else
		{
			string_pointer++;
			*string_pointer = 0x21;
		}
	}		
}

void eps_get_command(unsigned char *command_str,
					unsigned char *command,
  					unsigned char *params) // Variables
{
	//EPS get Command
	// char(pointer) char(pointer) char(pointer)  -> none
	//eps_get_command separates the comand from the parameters
	//example: eps_get_command("epsout 0 0", command, params)
	//places command=="epsout" and params=="0 0"
	
	unsigned char *command_ptr = command_str;
	
	while (*command != 0)
	{	
		*command = *command_ptr;
		switch(*command_ptr)
		{
			case 0x20:
				*command = 0;
				*params = 0;
				command_ptr++;
				break;
			default:
				command_ptr++;
				command++;
				break;
		}
	}
	
	if (*command_ptr == 0x20)
	{
		command_ptr++;
	}
	while (*command_ptr != 0)
	{	
		*params = *command_ptr;
		params++;
		command_ptr++;					
	}
	*params = *command_ptr;
}

void eps_get_prmts(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2)
{
	unsigned char *params_ptr = params;
	unsigned int8 value1 = 0;
	unsigned int8 value2 = 0;

	while (*params_ptr != 0x20)
	{
		value1 =10*value1 + *params_ptr-0x30;
		params_ptr++;
	}
	
	params_ptr++;
	
	while (*params_ptr != 0)
	{
		value2 =10*value2 + *params_ptr-0x30;
		params_ptr++;
	}

	*pmt1 = value1;
	*pmt2 = value2;
}

void eps_get_prmts3(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2, unsigned int8 *pmt3)
{
	unsigned char *params_ptr = params;
	unsigned int8 value1 = 0;
	unsigned int8 value2 = 0;
	unsigned int8 value3 = 0;

	while (*params_ptr != 0x20)
	{
		value1 =10*value1 + *params_ptr-0x30;
		params_ptr++;
	}
	
	params_ptr++;
	
	while (*params_ptr != 0x20)
	{
		value2 =10*value2 + *params_ptr-0x30;
		params_ptr++;
	}
	
	params_ptr++;

	while (*params_ptr != 0)
	{
		value3 =10*value3 + *params_ptr-0x30;
		params_ptr++;
	}

	*pmt1 = value1;
	*pmt2 = value2;
	*pmt3 = value3;
}
