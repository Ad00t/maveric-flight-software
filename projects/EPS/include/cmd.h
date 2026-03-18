#ifndef __CMD__
#define __CMD__

unsigned int8 origin;
unsigned int8 destination;
unsigned int8 echo;
unsigned char command[256];
unsigned char params[256];
			
unsigned char *cmd=command;
unsigned char *prms=params;
unsigned int8 *orgn=origin;
unsigned int8 *dest=destination;
unsigned int8 *ech=echo;


//extern void cmd_get_command(unsigned char *command_str,unsigned char *origin,unsigned char *destination,unsigned char *echo, unsigned char *command, unsigned char *params);
extern void cmd_get_command(unsigned char *command_str,unsigned int8 *origin,unsigned int8 *destination,unsigned int8 *echo, unsigned char *command, unsigned char *params);
extern void cmd_get_prmts(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2);
extern void cmd_get_prmts3(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2, unsigned int8 *pmt3);

#endif

#include "cmd.c"