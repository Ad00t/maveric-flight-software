#include "m41t81s.h"
#include "i2c.h"
#include <stdint.h>

#module

// HELPERS

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

// ERTC FUNCTIONS

//		memcpy(buf,&globals.T_Init,7);			// get fred's default time
// 
void ertc_init(ertc_s* ertc, uint8_t wday, uint8_t month, uint8_t mday, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec) {
    // Assume Failure and set global
    //I2C_Clock_OK=FALSE; 

    i2c_start();							// set the FPM rtc clock with it
    i2c_write(0xD0);
    i2c_write(0x01);
    i2c_write(hextobcd(sec));	// seconds
    i2c_write(hextobcd(min));  // minutes
    i2c_write(hextobcd(hour));	// hour
    i2c_write(hextobcd(wday)+1);  // weekday (rtc stores 1 to 7 code is 0to6)
    i2c_write(hextobcd(mday));  // month day
    i2c_write(hextobcd(month));  // month
    i2c_write(hextobcd(year));  // year
    i2c_write(0x0);  	// Cal
    i2c_write(0x0);  	// Watchdog
    i2c_write(0x0); 	// Alm Month
    i2c_write(0x0);  	// Alm Date
    i2c_write(0x0); 	// Alm Hour
    i2c_write(0x0); 	// Alm Min
    i2c_write(0x0);		// Alm Sec
    i2c_stop();

    // Reset Global to OK
    //I2C_Clock_OK=TRUE;
    
    ertc_get_time(ertc);
}

void ertc_get_time(ertc_s* ertc) {
    // Assume Failure and set global
    //I2C_Clock_OK=FALSE;

    i2c_start();					// RTC READ SEQ
    i2c_write(0xD0);  				// Device address/write mode
    i2c_write(0x01);				// Device address pointer write
    i2c_start();					// start
    i2c_write(0xD1);  				// Device address/read mode 

    ertc->sec = bcdtohex(i2c_read()& 0x7f); 	// seconds, w/ACK
    ertc->min = bcdtohex(i2c_read()& 0x7f); 	// minutes, w/ACK
    ertc->hour = bcdtohex(i2c_read()& 0x3f); 	// hour, w/ACK
    ertc->wday = bcdtohex(i2c_read()& 0x07) - 1; 	// weekday, w/ACK (rtc is 1-7, Fred's code is 0-6)
    ertc->mday = bcdtohex(i2c_read()& 0x3f); 	// Month day, w/ACK
    ertc->month = bcdtohex(i2c_read()& 0x1f); 	// month, w/ACK
    ertc->year = bcdtohex(i2c_read(0)& 0xff); 	// year, w/NOACK

    i2c_stop();

    // Reset Global to OK
    //I2C_Clock_OK=TRUE;
}

void ertc_set_time(ertc_s* ertc, uint8_t wday, uint8_t month, uint8_t mday, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec) {
    //// Assume Failure and set global
    //I2C_Clock_OK=FALSE; 

    //memcpy(time, buf, 7);						// set the RAM version of time now
    i2c_start();
    i2c_write(0xD0);
    i2c_write(0x01);
    i2c_write(hextobcd(sec));	//seconds
    i2c_write(hextobcd(min));  //minutes
    i2c_write(hextobcd(hour));	//hour
    i2c_write(hextobcd(wday)+1);  //weekday (rtc is 1-7 code is 0-6)
    i2c_write(hextobcd(mday));  //Month day
    i2c_write(hextobcd(month));  //month
    i2c_write(hextobcd(year));  //year
    i2c_stop();

    //// Reset Global to OK
    //I2C_Clock_OK=TRUE;

    ertc_get_time(ertc);
}

void ertc_enable_fpm(ertc_s* ertc, uint8_t* halted_time, uint8_t* time) {
    int i;
    uint8_t test_buf[16];

    // Enable interrupts (???? you mean disable?)
    disable_interrupts(INT_MI2C);

    // Assume Failure and set global
    // I2C_Clock_OK=FALSE;

    i2c_start();					// RTC READ SEQ - reads ST bit & Seconds, 10s and 1s
    i2c_write(0xD0);  				// Device address/write mode
    i2c_write(0x01);				// Device address pointer write (ST & Secs)
    i2c_start();
    i2c_write(0xD1);  				// Device address/read mode (get ST & Secs)
    test_buf[1] = i2c_read(0); 		// Read w/NOACK - seconds and ST bit
    i2c_stop();

    i2c_start();					// RTC READ SEQ - HT & Alarms
    i2c_write(0xD0);  				// Device address/write mode
    i2c_write(0x0C);				// Device address pointer write - HT address
    i2c_start();					//
    i2c_write(0xD1);  				// Device address/read mode 
    test_buf[0xC] = i2c_read(0); 	// Read w/NOACK
    i2c_stop();

    if( test_buf[1] & 0x80 ){		// ST bit set means first time powered up
        i2c_start();   				// zero out all registers (first time ever board bringup or dead batt)
        i2c_write(0xD0);
        i2c_write(0x00);
        for(i=0; i<16; i++){
            i2c_write(0x00);
        }
        i2c_stop();
    }
    else if(test_buf[0xC] & 0x40 ){
        // We've lost power and been on battery backtup, save halted time, enable updates, reload time[]
        //save halted time
        i2c_start();					// RTC READ SEQ
        i2c_write(0xD0);  				// Device address/write mode
        i2c_write(0x01);				// Device address pointer write
        i2c_start();					// start
        i2c_write(0xD1);  				// Device address/read mode 

        test_buf[6] = bcdtohex(i2c_read()& 0x7f); 	// seconds, w/ACK
        test_buf[5] = bcdtohex(i2c_read()& 0x7f); 	// minutes, w/ACK
        test_buf[4] = bcdtohex(i2c_read()& 0x3f); 	// hour, w/ACK
        test_buf[3] = bcdtohex(i2c_read()& 0x07)-1 ; 	// weekday, w/ACK (rtc is 1-7 code is 0-6)
        test_buf[1] = bcdtohex(i2c_read()& 0x3f); 	// Month day, w/ACK
        test_buf[0] = bcdtohex(i2c_read()& 0x1f); 	// month, w/ACK
        test_buf[2] = bcdtohex(i2c_read(0)& 0xff); 	// year, w/NOACK
        i2c_stop();
        memcpy( halted_time, test_buf, 7 );
        // end save halted time

        // reenable time updates
        i2c_start();					// RTC WRITE SEQ - reset HT bit (enabling updates) zero alarms
        i2c_write(0xD0);
        i2c_write(0x0C);
        i2c_write(0x00);				//HT bit lo enables updates, and zeros alarm
        i2c_stop();

        for( i=1; i<32; i++){			// delay a bit to allow new update (necessary?)
            test_buf[0] = test_buf[7];
        }
        // end reenable time updates

        //copy the now updating time from batt backed rtc to time struct (preventing a default time load)
        i2c_start();					// RTC READ SEQ
        i2c_write(0xD0);  				// Device address/write mode
        i2c_write(0x01);				// Device address pointer write
        i2c_start();					// start
        i2c_write(0xD1);  				// Device address/read mode 
        test_buf[6] = bcdtohex(i2c_read()& 0x7f); 	// seconds, w/ACK
        test_buf[5] = bcdtohex(i2c_read()& 0x7f); 	// minutes, w/ACK
        test_buf[4] = bcdtohex(i2c_read()& 0x3f); 	// hour, w/ACK
        test_buf[3] = bcdtohex(i2c_read()& 0x07)-1; 	// weekday, w/ACK (rtc is 1-7 code is 0-6)
        test_buf[1] = bcdtohex(i2c_read()& 0x3f); 	// Month day, w/ACK
        test_buf[0] = bcdtohex(i2c_read()& 0x1f); 	// month, w/ACK
        test_buf[2] = bcdtohex(i2c_read(0)& 0xff); 	// year, w/NOACK
        i2c_stop();
        memcpy(time, test_buf, 7);
        // end of recovered time from rtc during batt backup
    }
    else{
        // just another reboot, nothing to save, time[] should be ok, rtc should still be running
        // do nothing meaningful, just leave some tracks to read on the analyzer

        i2c_start();					// RTC WRITE SEQ - reset ST bit, preserving seconds
        i2c_write(0xD0);
        i2c_write(0x01);
        i2c_write(test_buf[1] & 0x7F);	// ST bit lo enables oscillator, and set seconds
        i2c_stop();
    }


    // Reset Global to OK
    // I2C_Clock_OK=TRUE;
}

