#include "i2c.h"
#include <stdint.h>
//#include <stdio.h>

#module

uint8_t i2c_read_8(uint8_t port, uint8_t add, uint8_t reg){
	//I2C 1  byte reading routine
	//int8(i2c address), int8(i2c register)  -> int8
	//reads the bytes present in the i2c register of the addressed device and saves it on read_data 
	//example: i2c_write_8(ADDRESS, REG)
	
	uint8_t addw = add << 1;
	uint8_t addr = addw | 0x01;
	
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	i2c_stop(I2C_1);
	
    i2c_start(I2C_1);
	i2c_write(I2C_1, addr);
	uint8_t read_data = i2c_read(I2C_1, FALSE);
	//read_data = (read_data<<8);
	//read_data = read_data | i2c_read(I2C_1);
	i2c_stop(I2C_1);

	return read_data;		
}

void i2c_write_8(uint8_t port, uint8_t add, uint8_t reg, uint8_t write_data){
	//I2C writing byte routine
	//int8(i2c address), int8(i2c register), char(1 byte write register pointer)  -> none
	//Writes the bytes present in write_data into the i2c register of the addressed device 
	//example: i2c_write_16(ADDRESS, REG, setting)

	uint8_t addw, addr, lsb;

	addw = add<<1;
	addr = addw | 0x01;

	lsb = write_data;
	//msb = write_data>>8;
	
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	//i2c_write(I2C_1, msb);
	i2c_write(I2C_1, lsb);
	i2c_stop(I2C_1);	
}


uint16_t i2c_read_16(uint8_t port, uint8_t add, uint8_t reg){
	//I2C INA226 reading routine
	//int8(i2c address), int8(i2c register), -> int16
	//reads the bytes present in the i2c register of the addressed device and saves it on read_data 
	//example: i2c_write_16(ADDRESS, REG, setting)
	
	uint8_t addw, addr;
	uint16_t read_data;

	addw = add<<1;
	addr = addw | 0x01;
		
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	i2c_start(I2C_1);
	i2c_write(I2C_1, addr);

	read_data = i2c_read(I2C_1);
	read_data = (read_data<<8);
	read_data = read_data | i2c_read(I2C_1, FALSE);
	i2c_stop(I2C_1);

	return read_data;		
}

void i2c_write_16(uint8_t port, uint8_t add, uint8_t reg, uint16_t write_data) {
	//I2C INA226 writing routine
	//int8(i2c address), int8(i2c register), char(2 bytes write register pointer)  -> bool
	//Writes the bytes present in write_data into the i2c register of the addressed device 
	//example: i2c_write_16(ADDRESS, REG, setting)
	// unsigned char ina226_wdata[2]; // 2-byte ina226 writing

	uint8_t addw, addr, msb, lsb;

	addw = add<<1;
	addr = addw | 0x01;

	lsb = write_data;
	msb = write_data>>8;
	
	i2c_start(I2C_1);
	i2c_write(I2C_1, addw);
	i2c_write(I2C_1, reg);
	i2c_write(I2C_1, msb);
	i2c_write(I2C_1, lsb);
	i2c_stop(I2C_1);	
}

void i2c_start_transaction(uint8_t port) {
    switch (port) {
        case I2C_1: i2c_start(I2C_1); break;
        case I2C_2: i2c_start(I2C_2); break;
        case I2C_3: i2c_start(I2C_3); break;
        default:    return;
    }
}

uint8_t i2c_byte_avail(uint8_t port) {
    switch (port) {
        case I2C_1: return i2c_isr_state(I2C_1);
        case I2C_2: return i2c_isr_state(I2C_2);
        case I2C_3: return i2c_isr_state(I2C_3);
        default:    return 0;
    }
}

uint8_t i2c_read_byte(uint8_t port) {
    switch (port) {
        case I2C_1: return i2c_read(I2C_1);
        case I2C_2: return i2c_read(I2C_2);
        case I2C_3: return i2c_read(I2C_3);
        default:    return 0;
    }
}

void i2c_write_byte(uint8_t port, uint8_t b) {
    switch (port) {
        case I2C_1: i2c_write(I2C_1, b);
        case I2C_2: i2c_write(I2C_2, b);
        case I2C_3: i2c_write(I2C_3, b);
        default:    return;
    }
}

void i2c_stop_transaction(uint8_t port) {
    switch (port) {
        case I2C_1: i2c_stop(I2C_1); break;
        case I2C_2: i2c_stop(I2C_2); break;
        case I2C_3: i2c_stop(I2C_3); break;
        default:    return;
    }
}

//MASTER FUNCTION ONLY
void i2c_read_buf(uint8_t port, uint8_t add, uint8_t* buf, uint16_t len) {
    uint16_t i;
    
    uint8_t addw, addr;
	addw = add<<1;
	addr = addw | 0x01;
	
    // Start the Master Port Transfer
	i2c_start_transaction(port);
    // Send Address for Slave Device
    i2c_write_byte(port, addr);
    // Receive Data
    for (i = 0; i < len; i++) {
        buf[i] = i2c_read_byte(port);
    }
    // End the Master Port Transfer
    i2c_stop_transaction(port);
}

//MASTER FUNCTION ONLY
void i2c_write_buf(uint8_t port, uint8_t add, uint8_t* buf, uint16_t len) {
    uint16_t i;
    
    uint8_t addw;//, addr;

	addw = add<<1;
	//addr = addw | 0x01;
    
    // Start Master Port Transfer
	i2c_start_transaction(port);
    //Send Address for Slave Device
    i2c_write_byte(port, addw);
    
    // Send Data
    for (i = 0; i < len; i++) {
        i2c_write_byte(port, buf[i]);
    }
    
    //End Slave Port Transfer
    i2c_stop_transaction(I2C_1);
}
