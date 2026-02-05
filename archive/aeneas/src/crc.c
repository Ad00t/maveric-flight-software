// crc.c

#include "crc.h"
#include <stdint.h>

/*=================== Compute CRC ========================
  PURPOSE: Creates a unique checksum for a specified length of unsigned char buffer.
  UNMODIFIED VARIABLES: len, buf
  MODIFIED VARIABLES: value
  GLOBALS: none
  RETURN VALUE: none (returned in *value)
  NOTES: This function assumes host endianness is little
         and will therefore not work on big endian machines.
         It assumes the caller knows the return value is
         a 16 bit unsigned short. This is dangerous if a
         char array of size 1 is passed in.
-----------------------------------------------------------
*/
void getCRC(unsigned char* buf, int len, unsigned char* value) {
    uint16_t crcval, t;
    int i;
    crcval = 0xffff; // CCITT CRC
    for (i = 0; i < len; ++i) {
        t = crcval ^ (uint16_t) buf[i];
        t = (t ^ (t << 4)) & 0xff;
        crcval = (crcval >> 8) ^ (t << 8) ^ (t << 3) ^ (t >> 4);
    }
    crcval = ~crcval;
    value[0] = (unsigned char)(crcval & 0x00ff); // return LSB (sent first)
    value[1] = (unsigned char)((crcval & 0xff00) >> 8); // return MSB (sent second)
}

/*=================== get CRC Continuous ========================
  PURPOSE: Creates a unique checksum for a specified length of unsigned char buffer,
           can be called multiple times and will continue to compute a rolling
           CRC value.
  UNMODIFIED VARIABLES: len, buf, restart
  MODIFIED VARIABLES: value
  GLOBALS: none
  RETURN VALUE: none (returned in *value)
  NOTES:
      Pass this function a nonzero restart to make it restart/
      If you pass it a restart of 0, it will continue computing the CRC
      as if the last unsigned char buffer and the current unsigned char buffer were concatenated.
-----------------------------------------------------------
*/
void getCRCCont(unsigned char restart, unsigned char* buf, int len, unsigned char* value) {
    static uint16_t crcval = 0;
    uint16_t t;
    int i;

    if (restart == CRC_RESTART) crcval = 0xffff; // restarts the crc
    else crcval = ~crcval;	     // continue where we left off by reversing again

    for (i = 0; i < len; ++i) {
        t = crcval ^ (uint16_t) buf[i];
        t = (t ^ (t << 4)) & 0xff;
        crcval = (crcval >> 8) ^ (t << 8) ^ (t << 3) ^ (t >> 4);
    }

    crcval = ~crcval;			 // Flip the bits
    value[0] = (unsigned char)(crcval & 0x00ff); // return LSB (sent first)
    value[1] = (unsigned char)((crcval & 0xff00) >> 8); // return MSB (sent second)
}

// Checks the CRC of a Message.  1 for Good CRC, 0 for bad.
unsigned char checkCRC(unsigned char* buf, int len, unsigned char* bufcrc) {
    unsigned char CRC[2];

    getCRC(buf, len, CRC);

    if (CRC[0] == bufcrc[0] && CRC[1] == bufcrc[1]) {
        return 1; 		// Valid
    } else {
        return 0;		// Invalid
    }
}