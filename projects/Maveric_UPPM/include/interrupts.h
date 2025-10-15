#ifndef __INTERRUPT__
#define __INTERRUPT__

BYTE start_flag, cmd_flag;
unsigned char rcv_cmd[256];
unsigned int8 max;
unsigned int8 len;

void enable_all_interrupts(void);
void disable_all_interrupts(void);

#INT_RDA
void RDA_isr()
{
	BYTE state;
   	char c;
	disable_all_interrupts();
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
	enable_all_interrupts();
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
#endif
#include "interrupts.c"