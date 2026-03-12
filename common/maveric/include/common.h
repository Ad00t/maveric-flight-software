#ifndef __COMMON_H__
#define __COMMON_H__

#include <time.h>
#include <stdint.h>

#define STATUS_OK               1
#define STATUS_ERR              0

#define KNRM  "\033[0m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"

uint8_t bcdtohex(uint8_t bcd);
uint8_t hextobcd(uint8_t hex);

#endif // !__COMMON_H__

