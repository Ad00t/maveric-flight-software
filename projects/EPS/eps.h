#ifndef __EPS__
#define __EPS__


extern void eps_read_command(unsigned char *string_pointer);
extern void eps_get_command(unsigned char *command_str, unsigned char *command, unsigned char *params);
extern void eps_get_prmts(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2);
extern void eps_get_prmts3(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2, unsigned int8 *pmt3);
//extern long my_atoul(char *s, char *endptr, int base);
//extern float my_atof(char * s, char *endptr);
//extern int8 sscanf(char  *buf, char  *fmt, ...);

#endif