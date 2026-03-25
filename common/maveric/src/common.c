#include "common.h"
#include <math.h>
#include <stdint.h>

#module

uint8_t bcdtohex(uint8_t bcd) {
    uint8_t zerosb, onesb, twosb, threesb;
    int retval;

    zerosb = bcd & 0x000f;
    onesb = (bcd & 0x00f0)>>4;
    twosb = (bcd & 0x0f00)>>8;
    threesb = (bcd & 0xf000)>>12;
    retval = threesb*1000 + twosb*100 + onesb*10 + zerosb;
    return retval;
}

uint8_t hextobcd(uint8_t hex) {
    uint8_t y;
    y = (hex / 10) <<4;
    y = y | (hex %10);
    return (y);
}

uint8_t maxu8(uint8_t a, uint8_t b) {
    return a >= b ? a : b;
}

uint8_t minu8(uint8_t a, uint8_t b) {
    return a <= b ? a : b;
}

float rad2deg(float rad) {
    return rad * (180.0f / PI);
}

float deg2rad(float deg) {
    return deg * (PI / 180.0f);
}

//  FUNCTION: ftoa
//  AUTHOR = TRAMPAS STERN
//  FILE = strio.c
//  DATE = 2/6/2003  4:27:14 PM
//
//  PARAMETERS: long,*str, int count
//
//  DESCRIPTION: Convets an float to string
//     format 'f', 'E', or 'e'
//
//  RETURNS:
//
// NOTE this code was found on the web and modified
//  to actually work.
//-----------------------------------------------------------

uint8_t ftoa(float x, char* str, uint8_t prec, char format) {
    int k, fstyle;

    int8_t ie, i, ndig;

    //double y;
    //float y;
       
    char* start;

    start = str;

    // Based on precision, set the number of digits.
    ndig = prec + 1;

    // if(prec < 0)
    //    ndig = 7;

    if(prec > 22)
       ndig = 23;

    fstyle = 0;  // Exponent 'e'

    if(format == 'f' || format == 'F')
       fstyle = 1;  // Normal 'f' style

    if(format == 'g' || format == 'G')
       fstyle=2;

    ie = 0;

    // If x is negative, write minus sign and reverse.
    if(x < 0)
      {
       *str++ = '-';
       x = -x;
      }

    // If (x < 0.0) then increment by 10 until between 1.0 and 10.0.
    if(x != 0.0)
      {
       while (x < 1.0)
         {
          x =x* 10.0;
          ie--;
         }
      }

    // If x > 10 then let's shift it down.
    while(x >= 10.0)
      {
       x = x * (1.0/10.0);
       ie++;
      }

    /*
    if(ABS(ie) > MAX_MANTISA)
      {
       if(fstyle==1)
         {
          fstyle=0;
          format='e';
          // ie=2;
         }
      }
    */

    // In f format, the number of digits is related to size.

    if(fstyle)
       ndig = ndig + ie;

    if(prec == 0 && (ie > ndig) && fstyle)
      {
       ndig=ie;
      }


    // Round. x is between 1 and 10 and ndig will be printed to
    // the right of the decimal point so rounding is...
    /*
    y = 1;

    for(i = 1; i < ndig; i++)  // Find least significant digit
        y = y * (1.0/10.0);    // Multiplying by 1/10 is faster
                               // than dividing

    x = x + y * (1.0/2.0);     // Add rounding

    // Repair rounding disasters.
    if(x >= 10.0)
      {
       x = 1.0;
       ie++;
       ndig++;
      }
    */

    // Check and see if the number is less than 1.0
    if(fstyle && ie<0)
      {
       *str++ = '0';

       if(prec!=0)
          *str++ = '.';

       if(ndig < 0)
          ie = ie-ndig;  // Limit zeros if underflow

       for(i = -1; i > ie; i--)
           *str++ = '0';
      }


    // For each digit.
    for(i=0; i < ndig; i++)
       {
        float b;
        k = x;             // k = most significant digit
        *str++ = k + '0';  // Output the char representation

        if(((!fstyle && i==0) || (fstyle && i==ie)) && prec!=0)
           *str++ = '.';   // Output a decimal point

        b = (float)k;

        // Multiply by 10 before subtraction to remove
        // errors from limited number of bits in float.
        b = b*10.0;
        x = x*10.0;
        x = x - b;      // Subtract k from x

       // b=x+b;
       // x =x* 10.0;   // Get next digit
      }


    // Now, in e style, put out the exponent if not zero.
    if(!fstyle && (ie != 0))
      {
       *str++ = format;

       if(ie < 0)  // If number has a negative exponent
         {
          ie = -ie;
          *str++ = '-';
         }

       // Now we need to convert the exponent to a string.

       for(k = 1000; k > ie; k = k/10); // Find the decade of exponent

       for(   ; k > 0; k = k/10)
          {
           char t;
           t = div(ie, k);
           *str++ = t + '0';
           ie = ie -(t * k);
          }

      }
    *str++ = '\0';
    return (str - start - 1);  // Return string length
}
