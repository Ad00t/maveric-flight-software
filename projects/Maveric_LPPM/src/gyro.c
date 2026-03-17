#include "gyro.h"
#include "spi.h"
#include "common.h"
#include <stdint.h>

#module

#define DEG_PER_SEC_PER_BIT 	0.018315	// For +/- 80 degrees per second
#define DEG_PER_BIT 			0.1453		// Temperatuer Sensor
#define DEG_OFFSET				25.0		// The zero value for the temperature is 25.0C
#define RAD_PER_SEC_PER_BIT		0.00031974431896536100 // For +/- 80 degrees per second range, taken by converting 0.01832 to rad/sec/LSB

// HELPERS

// Perform sign extension from 14 to 16 bits 
int16_t sign_ext_14_16(uint16_t i) {
	if (bit_test(i, 13)) i = i | 0b1100000000000000; // set the two highest bits via |
	else i = i & 0b0011111111111111; // clear the two highest bits using &
    return i;
}

// Perform sign extension from 12 to 16 bits
int16_t sign_ext_12_16(uint16_t i) {
	if (bit_test(i, 11)) i = i | 0b1111000000000000; // set the four highest bits via |
	else i = i & 0b0000111111111111; // clear the four highest bits using &
    return i;
}

// Convert rate counts to degrees per sec
float counts_to_dps(uint16_t cnt) {
	return cnt*DEG_PER_SEC_PER_BIT;
}

// Convert degrees per sec to rate counts
uint16_t dps_to_counts(float x) {
	x=x/DEG_PER_SEC_PER_BIT;	// Convert to counts
	return (x >= 0) ? (int)(x+0.5) : (int)(x-0.5);
}

// Convert rate counts to radians per sec
float counts_to_rps(uint16_t cnt) {
	return cnt*RAD_PER_SEC_PER_BIT;
}

// Convert temperature counts to degrees Celsius
float counts_to_C(uint16_t cnt) {
	return cnt*DEG_PER_BIT + DEG_OFFSET;
}

// API

void gyro_init(gyro_s* gyro, uint8_t cs_x, uint8_t cs_y, uint8_t cs_z, uint8_t on) {
    gyro->cs_x = cs_x;
    gyro->cs_y = cs_y;
    gyro->cs_z = cs_z;
    gyro->on = on;
    gyro_set_power(gyro, TRUE);
    output_high(gyro->cs_x);
    output_high(gyro->cs_y);
    output_high(gyro->cs_z);
    sprintf(LOGBUF, "gyro_init: x=%u y=%u z=%u en=%u", gyro->cs_x, gyro->cs_y, gyro->cs_z, gyro->on); log_flush(LL_INFO); 
}	

void gyro_clear(gyro_s* gyro) {
    memset(gyro, 0, sizeof(gyro_s));
}

void gyro_set_power(gyro_s* gyro, int1 on) {
    if (on) output_high(gyro->on);
    else    output_low(gyro->on);
}

void gyro_read_reg(gyro_s* gyro, uint8_t reg, uint16_t* res) {
    uint16_t req = (0 << 15) | ((uint16_t)reg << 8);
    sprintf(LOGBUF, "gyro_read_reg: 0x%02X%02X", req >> 8, req & 0x00FF); log_flush(LL_TRACE);
	// spi_set_mode(GYRO_SPI_MODE);

    output_low(gyro->cs_x); 
    // spi_write(req >> 8); 			      
    // spi_write(req & 0x00FF); 		     
    // uint16_t d0 = spi_xfer(SPI_1, req, 16);
    // res[1] = spi_xfer(SPI_1, 0, 16);
    // spi_xfer(SPI_1, req);
    output_high(gyro->cs_x); 
   
    delay_us(15);

    output_low(gyro->cs_x); 
    // res[0] = make16(spi_read(req >> 8), spi_read(req & 0x00FF));	            
    // res[0] = make16(spi_read(0), spi_read(0));
    // res[0] = spi_xfer(SPI_1, 0xFFFF);
    output_high(gyro->cs_x);

    delay_us(15);

    // output_low(gyro->cs_y); 				        
    // // res[0] = make16(spi_read(req >> 8), spi_read(req & 0x00FF));	            
    // spi_xfer(SPI_1, req, 16);
    // output_high(gyro->cs_y);
    //
    // delay_us(13);
    //
    // output_low(gyro->cs_y); 				    
    // res[1] = spi_xfer(SPI_1, 0, 16);
    // output_high(gyro->cs_y);
    //
    // delay_us(13);
    //
    // output_low(gyro->cs_z); 				        
    // // res[1] = make16(spi_read(req >> 8), spi_read(req & 0x00FF));	            
    // // res[2] = make16(spi_read(0), spi_read(0));	            
    // spi_xfer(SPI_1, req, 16);
    // output_high(gyro->cs_z);
    //
    // delay_us(13);
    //
    // output_low(gyro->cs_y); 				    
    // res[2] = spi_xfer(SPI_1, 0, 16);
    // output_high(gyro->cs_y);
    //
    // delay_us(13);
}

void gyro_write_reg(gyro_s* gyro, uint8_t reg, uint16_t data) {
    uint16_t req = (1 << 15) | ((uint16_t)reg << 8);
    sprintf(LOGBUF, "gyro_write_reg: 0x%02X%02X", req >> 8, req & 0x00FF); log_flush(LL_TRACE);
	// spi_set_mode(GYRO_SPI_MODE);
	//
	//    output_low(gyro->cs_x); 				        // Select chip for given gyro
	//    delay_us(5); 						            // give it a second to engage.	
	//    spi_write(req >> 8); 			                // Send in two parts
	//    spi_write(req & 0x00FF); 		                // 2nd part
	//    output_high(gyro->cs_x);
	//
	//    output_low(gyro->cs_y); 				        
	//    delay_us(5); 						        
	//    spi_write(req >> 8); 			                
	//    spi_write(req & 0x00FF); 		               
	//    output_high(gyro->cs_y);
	//
	//    output_low(gyro->cs_z); 				        
	//    delay_us(5); 						       
	//    spi_write(req >> 8); 			              
	//    spi_write(req & 0x00FF); 		             
	//    output_high(gyro->cs_z);
}

int1 gyro_heartbeat(gyro_s* gyro) {
    uint16_t res[NUM_GYROS];
    memset(res, 0, NUM_GYROS * sizeof(uint16_t));
    gyro_read_reg(gyro, GYRO_PRODUCT_ID, res);
    // sprintf(LOGBUF, "gyro_heartbeat: x=0x%02X%02X y=0x%02X%02X z=0x%02X%02X", 
    //         res[0] >> 8, res[0] & 0x00FF, res[1] >> 8, res[1] & 0x00FF, res[2] >> 8, res[2] & 0x00FF);
    uint8_t i;
    int1 hb = TRUE;
    for (i = 0; i < NUM_GYROS; i++) {
        if (res[i] != 0x3F84) {
            hb = FALSE;
            break;
        }
    }
    if (!hb) {
        sprintf(LOGBUF, "gyro_heartbeat: flatlined. resetting..."); log_flush(LL_ERROR);
        gyro_set_power(gyro, FALSE);
        delay_ms(500);
        uint8_t cs_x = gyro->cs_x;
        uint8_t cs_y = gyro->cs_y;
        uint8_t cs_z = gyro->cs_z;
        uint8_t on = gyro->on;
        gyro_clear(gyro);
        gyro_init(gyro, cs_x, cs_y, cs_z, on);
    }
    return hb;
}

void gyro_read_all(gyro_s* gyro) {
    gyro_read_reg(gyro, GYRO_GYRO_OUT, gyro->rate_raw);
    gyro_read_reg(gyro, GYRO_TEMP_OUT, gyro->temp_raw);
    gyro_read_reg(gyro, GYRO_DIAG_STAT, gyro->error);
    sprintf(LOGBUF, "rate %u %u %u", gyro->rate_raw[0], gyro->rate_raw[1], gyro->rate_raw[2]); log_flush(LL_INFO);
    sprintf(LOGBUF, "temp %u %u %u", gyro->temp_raw[0], gyro->temp_raw[1], gyro->temp_raw[2]); log_flush(LL_INFO);
    sprintf(LOGBUF, "error %u %u %u", gyro->error[0], gyro->error[1], gyro->error[2]); log_flush(LL_INFO);
    uint8_t i;
    for (i = 0; i < NUM_GYROS; i++) {
        // Check for new data (MSB or 15th bit) and error alarm (14th bit)
        gyro->rate_nd[i] = bit_test(gyro->rate_raw[i], 15);
        gyro->rate_ea[i] = bit_test(gyro->rate_raw[i], 14);
        // Sign extend
        gyro->rate_cnt[i] = sign_ext_14_16(gyro->rate_raw[i]);
        // Convert the readings and store
        gyro->rate_dps[i] = counts_to_dps(gyro->rate_cnt[i]);
        gyro->rate_rps[i] = counts_to_rps(gyro->rate_cnt[i]);

        // Check for new data (MSB or 15th bit) and error alarm (14th bit)
        gyro->temp_nd[i] = bit_test(gyro->temp_raw[i], 15);
        gyro->temp_ea[i] = bit_test(gyro->temp_raw[i], 14);
        // Sign extend
        gyro->temp_cnt[i] = sign_ext_12_16(gyro->temp_raw[i]);
        // Convert the readings and store
        gyro->temp[i] = counts_to_C(gyro->temp_cnt[i]);
    }
}			
