#include "cmd.h"

//void cmd_get_command(unsigned char *command_str,
//					 unsigned char *origin,
//					 unsigned char *destination,
//					 unsigned char *echo, 
//					 unsigned char *command, 
//					 unsigned char *params) // Variables

void cmd_get_command(unsigned char *command_str,
					 unsigned int8 *origin,
					 unsigned int8 *destination,
					 unsigned int8 *echo, 
					 unsigned char *command, 
					 unsigned char *params) // Variables
{
	//General Purpose get Command
	// char(pointer) char(pointer) char(pointer)  -> none
	//eps_get_command separates the comand from the parameters
	//example: eps_get_command("epsout 0 0", command, params)
	//places command=="epsout" and params=="0 0"

	unsigned char *command_ptr = command_str;
	//***** Decoding Routes *******
	unsigned int8 origin_value = 0;
	unsigned int8 dest_value = 0;
	unsigned int8 echo_value = 0;

	origin_value = *command_ptr - 0x30;
	command_ptr++;
	dest_value =  *command_ptr - 0x30;
	command_ptr++;
	echo_value = *command_ptr - 0x30;
	command_ptr++;

	*command = *command_ptr;
	while (*command != 0)
	{	
		switch(*command_ptr)
		{
			case 0x20:
				*command = 0;
				*params = 0;
				command_ptr++;
				//delay_ms(1);
				break;
			default:
				command_ptr++;
				command++;
				*command = *command_ptr;
				//delay_ms(1);
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
		//delay_ms(1);
	}
	*params = *command_ptr;
	//delay_ms(1);

	*origin = origin_value;
	*destination = dest_value;
	*echo = echo_value; 
}

void cmd_get_prmts(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2)
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

void cmd_get_prmts3(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2, unsigned int8 *pmt3)
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
