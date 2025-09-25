#ifndef __UART_INTERFACES__
#define __UART_INTERFACES__

//extern void uart_read_command(unsigned char *string_pointer);
extern int8 uart_read_command(unsigned char *string_pointer, unsigned int8 stream_id);
extern void uart_get_command(unsigned char *command_str, unsigned char *command, unsigned char *params);
extern void uart_get_prmts(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2);
extern void uart_get_prmts3(unsigned char *params, unsigned int8 *pmt1, unsigned int8 *pmt2, unsigned int8 *pmt3);
//extern long my_atoul(char *s, char *endptr, int base);
//extern float my_atof(char * s, char *endptr);
//extern int8 sscanf(char  *buf, char  *fmt, ...);

#endif
