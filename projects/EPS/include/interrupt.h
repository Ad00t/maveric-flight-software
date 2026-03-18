#ifndef __INTERRUPT__
#define __INTERRUPT__

BYTE start_flag, cmd_flag;
BYTE address, buffer[0x10];

unsigned char rcv_cmd[256];
unsigned char buffer_cmd[128];
unsigned char buffer2_cmd[128];
unsigned int8 max;
unsigned int8 len;
unsigned int8 counter=0;
unsigned int8 counter2=0;

void enable_all_interrupts(void);
void disable_all_interrupts(void);

#INT_RDA
void RDA_isr()
{
	BYTE state;
   	char c;
	//disable_all_interrupts();
	max = 255;
   	len=0;
	state = kbhit(COM_A);
	if(state&&start_flag)
	{
		do 
		{
			c=fgetc(COM_A);
	     	if(c==8) 
			{  // Backspace
	        	if(len>0) 
				{
	          	len--;
	        	}
	     	} 
			else if ((c>=' ')&&(c<='~'))
	       	{
				if(len<=max) 
				{
			 		rcv_cmd[len++]=c;
	       		}
				else
				{
					break;
				}
			}
			else if (c!=13) 
			{
				if(len<=max) 
				{
			 		len++;
	       		}
				else
				{
					break;
				}
			}
		} while((c!=13));
		rcv_cmd[len]=0;
	}
	cmd_flag = TRUE;
	fprintf(COM_A,"\033[33m[COM_A] Data received: %s\n\r",rcv_cmd);
	delay_ms(1);
	//enable_all_interrupts();
}

#bit CKP = getenv("BIT:CKP")

#INT_SI2C3
void i2c3_isr()
{
	BYTE incoming, state;
   	//char c;
	//disable_all_interrupts();
	//max = 255;
   	//len=0;
	
	//state = i2c_isr_state(I2C_3);
	state = i2c_isr_state(I2C_3);
	//fprintf(COM_A,"State %x \n\r",state);
	if (state<=0x80)
	{
		//incoming = i2c_read(I2C_3);
		if (state==0x00)
		{
			counter = 0;
			counter2 = 0;
			//fprintf(COM_A,"reading \n\r");
			
			//fprintf(COM_A,"State %x \n\r",state);
			if (i2c_poll(I2C_3))
			{
				//fprintf(COM_A,"Reading \n\r");
				incoming = i2c_read(I2C_3);
				
				//fprintf(COM_A,"received %x \n\r",incoming);
			}			
		}
		else if (state==0x80)
		{
			incoming = i2c_read(I2C_3);
			//incoming = i2c_read(I2C_3,2);
		}
		else
		{
			incoming = i2c_read(I2C_3);
			//if (state>0)
			//{
				
			if (incoming==13)
			{
				cmd_flag = TRUE;
				rcv_cmd[state-1] = 0;
			}
			else
			{
				rcv_cmd[state-1] = incoming;
			}
			//}
		}	
	}
	if (state>=0x80)
	{
		if (counter2)
		{
			if (buffer2_cmd[counter] == 0)
			{
				i2c_write(I2C_3,0);
			}
			else
			{
				i2c_write(I2C_3,buffer2_cmd[counter]);
				counter++;
				if (counter == 128)
				{
					counter = 0;
					counter2 = 0;
				}
			}
		}
		else
		{
			if (buffer_cmd[counter] == 0)
			{
				counter = 0;
				counter2 = 1;
				i2c_write(I2C_3,buffer2_cmd[counter]);
			}
			else
			{
				i2c_write(I2C_3,buffer_cmd[counter]);
			}
			counter++;
			if (counter == 128)
			{
				counter = 0;
				counter2 = 1;
			}
		}
		//i2c_write(I2C_3,counter);
		//counter++;
		//CKP = TRUE;	
		//break;
	}
	//cmd_flag = TRUE;
	//delay_ms(1);

}



/*
#INT_SI2C3
void i2c3_isr()
{
	BYTE incoming, state;
   	char c;
	disable_all_interrupts();
	max = 255;
   	len=0;
	
	//printf("I2C interrupt activated \n\r");
	state = i2c_isr_state(I2C_3);
	
	if ((state <= 0x80) && start_flag)
		{
		
		//while(!i2c_poll(I2C_3));
		incoming = i2c_read(I2C_3);
		incoming = i2c_read(I2C_3);
		incoming = i2c_read(I2C_3);
		i2c_write(I2C_3, 0xca);
		if(state == 1)
			address = incoming;
		if(state == 2)
			buffer[address] = incoming;
		}
	if (state == 0x80)
		{
		//fprintf(COM_A,"no incoming byte \n\r");
		i2c_write(I2C_3, buffer[address]);
		}
	cmd_flag = TRUE;
	delay_ms(1);
	enable_all_interrupts();
	//fprintf(COM_A,"state %u \n\r", state);
	//fprintf(COM_A,"byte to read:%x \n\r", incoming);
}

#INT_RDA2
void RDA2_isr()
{
	BYTE state;
   	char c;
	disable_all_interrupts();
	max = 255;
   	len=0;
	state = kbhit(COM_B);
	if(state&&start_flag)
	{
	   do 
		{
			c=fgetc(COM_B);
	     	if(c==8) 
			{  // Backspace
	        	if(len>0) 
				{
	          	len--;
	        	}
	     	} 
			else if ((c>=' ')&&(c<='~'))
	       	{
				if(len<=max) 
				{
			 		rcv_cmd[len++]=c;
	       		}
				else
				{
					break;
				}
			}
			else if (c!=13)
			{
				if(len<=max) 
				{
			 		len++;
	       		}
				else
				{
					break;
				}
			}
		} while((c!=13));
	   	//strB[len]=0;
		rcv_cmd[len]=0;
	}
	cmd_flag = TRUE;
	fprintf(COM_B,"\033[33m[COM_B] Data received: %s\n\r",rcv_cmd);
	delay_ms(1);
	enable_all_interrupts();
}


#INT_RDA3
void RDA3_isr()
{
	BYTE state;
   	char c;
	disable_all_interrupts();
	max = 255;
   	len=0;

	state = kbhit(COM_C);
	if(state&&start_flag)
	{
	   do 
		{
			c=fgetc(COM_C);
	     	if(c==8) 
			{  // Backspace
	        	if(len>0) 
				{
	          	len--;
	        	}
	     	} 
			else if ((c>=' ')&&(c<='~'))
	       	{
				if(len<=max) 
				{
			 		rcv_cmd[len++]=c;
	       		}
				else
				{
					break;
				}
			}
			else if (c!=13)
			{
				if(len<=max) 
				{
			 		len++;
	       		}
				else
				{
					break;
				}
			}
		} while((c!=13));
		rcv_cmd[len]=0;
	}
	cmd_flag = TRUE;
	//fprintf(COM_C,"\033[33m[COM_C] Data received: %s\n\r",rcv_cmd);
	delay_ms(1);
	enable_all_interrupts();
}

#INT_RDA4
void RDA4_isr()
{
	BYTE state;
   	char c;
	disable_all_interrupts();
	max = 255;
   	len=0;

	state = kbhit(COM_D);
	if(state&&start_flag)
	{
	   do 
		{
			c=fgetc(COM_D);
	     	if(c==8) 
			{  // Backspace
	        	if(len>0) 
				{
	          	len--;
	        	}
	     	} 
			else if ((c>=' ')&&(c<='~'))
	       	{
				if(len<=max) 
				{
			 		rcv_cmd[len++]=c;
	       		}
				else
				{
					break;
				}
			}
			else if (c!=13)
			{
				if(len<=max) 
				{
			 		len++;
	       		}
				else
				{
					break;
				}
			}
		} while((c!=13));
		rcv_cmd[len]=0;
	}
	cmd_flag = TRUE;
	fprintf(COM_D,"\033[33m[COM_D] Data received: %s\n\r",rcv_cmd);
	delay_ms(1);
	enable_all_interrupts();	
}
*/
#endif
#include "interrupt.c"