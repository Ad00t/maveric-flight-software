#include "m41t81s.h"
#include "common.h"
#include "systime.h"
#include "i2c.h"
#include <stdint.h>
#include <time.h>

#module

// ERTC FUNCTIONS

status_e ertc_init(ertc_s* ertc, rtc_time_t* init_time) {
    ertc->is_init = TRUE;
    ertc_clear(ertc);
    memcpy(&ertc->init_time, init_time, sizeof(rtc_time_t));
    ertc->is_using_ertc = TRUE;
    rtc_write(init_time);
    ertc_enable_fpm(ertc);
    return SUCCESS;
}

void ertc_clear(ertc_s* ertc) {
    if (!ertc->is_init) return;
    memset(&ertc->time, 0, sizeof(rtc_time_t));
    memset(&ertc->halted_time, 0, sizeof(rtc_time_t));
}

void ertc_get_time(ertc_s* ertc) {
    if (!ertc->is_init) return;
    if (ertc->is_using_ertc) {
        i2c_start();					// RTC READ SEQ
        i2c_write(0xD0);  				// Device address/write mode
        i2c_write(0x01);				// Device address pointer write
        i2c_start();					// start
        i2c_write(0xD1);  				// Device address/read mode 
        ertc->time.tm_sec = bcdtohex(i2c_read()& 0x7f); 	// seconds, w/ACK
        ertc->time.tm_min = bcdtohex(i2c_read()& 0x7f); 	// minutes, w/ACK
        ertc->time.tm_hour = bcdtohex(i2c_read()& 0x3f); 	// hour, w/ACK
        ertc->time.tm_wday = bcdtohex(i2c_read()& 0x07) - 1; 	// weekday, w/ACK (rtc is 1-7, Fred's code is 0-6)
        ertc->time.tm_mday = bcdtohex(i2c_read()& 0x3f); 	// Month day, w/ACK
        ertc->time.tm_mon = bcdtohex(i2c_read()& 0x1f); 	// month, w/ACK
        ertc->time.tm_year = bcdtohex(i2c_read(0)& 0xff); 	// year, w/NOACK
        i2c_stop();
        
        int1 valid = (ertc->time.tm_hour <= 23 && ertc->time.tm_min <= 59 && ertc->time.tm_sec <= 59
                        && ertc->time.tm_mon > 0 && ertc->time.tm_mday > 0); 
        if (valid) {
            rtc_write(&ertc->time);
        } else {
            ertc->is_using_ertc = FALSE;
        }
    }

    if (!ertc->is_using_ertc) {
        rtc_read(&ertc->time);
    }
}

void ertc_set_time(ertc_s* ertc, rtc_time_t time) {
    if (!ertc->is_init) return;
    if (ertc->is_using_ertc) {
        i2c_start();
        i2c_write(0xD0);
        i2c_write(0x01);
        i2c_write(hextobcd(time.tm_sec));	//seconds
        i2c_write(hextobcd(time.tm_min));  //minutes
        i2c_write(hextobcd(time.tm_hour));	//hour
        i2c_write(hextobcd(time.tm_wday)+1);  //weekday (rtc is 1-7 code is 0-6)
        i2c_write(hextobcd(time.tm_mday));  //Month day
        i2c_write(hextobcd(time.tm_mon));  //month
        i2c_write(hextobcd(time.tm_year));  //year
        i2c_stop();
    }
   
    rtc_write(&time);
    ertc_get_time(ertc);
}

int1 ertc_heartbeat(ertc_s* ertc) {
    if (!ertc->is_init) return 0;
    if (!ertc->is_using_ertc) {
        sprintf(LOGBUF, "ertc_heartbeat: flatlined. resetting..."); log_error();
        rtc_time_t init_time;
        memcpy(&init_time, &ertc->time, sizeof(rtc_time_t));
        ertc_init(ertc, &init_time); 
    }
    return ertc->is_using_ertc;
}

void ertc_enable_fpm(ertc_s* ertc) {
    if (!ertc->is_init) return;
    int i;
    uint8_t test_buf[16];

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

    if( test_buf[1] & 0x80 ) {		// ST bit set means first time powered up
        i2c_start();   				// zero out all registers (first time ever board bringup or dead batt)
        i2c_write(0xD0);
        i2c_write(0x00);
        for(i=0; i<16; i++){
            i2c_write(0x00);
        }
        i2c_stop();
    } else if(test_buf[0xC] & 0x40 ){
        // We've lost power and been on battery backtup, save halted time, enable updates, reload time[]
        //save halted time
        i2c_start();					// RTC READ SEQ
        i2c_write(0xD0);  				// Device address/write mode
        i2c_write(0x01);				// Device address pointer write
        i2c_start();					// start
        i2c_write(0xD1);  				// Device address/read mode 

        ertc->halted_time.tm_sec = bcdtohex(i2c_read()& 0x7f); 	// seconds, w/ACK
        ertc->halted_time.tm_min = bcdtohex(i2c_read()& 0x7f); 	// minutes, w/ACK
        ertc->halted_time.tm_hour = bcdtohex(i2c_read()& 0x3f); 	// hour, w/ACK
        ertc->halted_time.tm_wday = bcdtohex(i2c_read()& 0x07) - 1; 	// weekday, w/ACK (rtc is 1-7, Fred's code is 0-6)
        ertc->halted_time.tm_mday = bcdtohex(i2c_read()& 0x3f); 	// Month day, w/ACK
        ertc->halted_time.tm_mon = bcdtohex(i2c_read()& 0x1f); 	// month, w/ACK
        ertc->halted_time.tm_year = bcdtohex(i2c_read(0)& 0xff); 	// year, w/NOACK
        i2c_stop();
        // end save halted time

        // reenable time updates
        i2c_start();					// RTC WRITE SEQ - reset HT bit (enabling updates) zero alarms
        i2c_write(0xD0);
        i2c_write(0x0C);
        i2c_write(0x00);				//HT bit lo enables updates, and zeros alarm
        i2c_stop();

        // end reenable time updates

        // end of recovered time from rtc during batt backup
    } else {
        // just another reboot, nothing to save, time[] should be ok, rtc should still be running
        // do nothing meaningful, just leave some tracks to read on the analyzer

        i2c_start();					// RTC WRITE SEQ - reset ST bit, preserving seconds
        i2c_write(0xD0);
        i2c_write(0x01);
        i2c_write(test_buf[1] & 0x7F);	// ST bit lo enables oscillator, and set seconds
        i2c_stop();
    }
    
    ertc_get_time(ertc);
}

