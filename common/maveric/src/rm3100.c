#include "rm3100.h"
#include "i2c.h"
#include <stdint.h>

#module					// !!! important: This command makes everything below scoped only to this file.

// HELPERS

uint8_t rm3100_read_8(rm3100_s* m, uint8_t reg) {
	return i2c_read_8(m->add, reg & ~0x80);
}

uint16_t rm3100_read_16(rm3100_s* m, uint8_t reg) {
	return i2c_read_16(m->add, reg & ~0x80);	
}

void rm3100_read_word(rm3100_s* m, uint8_t reg, uint8_t *output, uint8_t num_bytes) {
	i2c_read_word(m->add, reg & ~0x80, *output, num_bytes);	
}

void rm3100_write_8(rm3100_s* m, uint8_t reg, uint8_t write_data) {
	i2c_write_8(m->add, reg, write_data);	
}

void rm3100_write_16(rm3100_s* m, uint8_t reg, uint16_t write_data) {
	i2c_write_16(m->add, reg, write_data);	
}

// HIGH LEVEL API

void rm3100_init(rm3100_s* m, uint8_t add) {
	//int8(i2c address) -> bool
	//Check if the rm3100 device is connected in the indicated address (add)
	//example: rm3100_init(RM3100_ADDRESS_20) returns a no-zerobyte if there is a RM3100 connected in that direction, otherwise return zero byte
    m->add = add;
    rm3100_clear(m);
    fprintf(COM_D, "%s[%s] rm3100_init: addr=0x%02X\n", KGRN, NODE_LBL, m->add);
}

void rm3100_clear(rm3100_s* m) {
    m->mag_x = 0;
    m->mag_y = 0;
    m->mag_z = 0;
}

int1 rm3100_heartbeat(rm3100_s* m) {
	uint8_t heartbeat = rm3100_read_8(m, RM3100_REGISTER_REVID);
    fprintf(COM_D, "%s[%s] rm3100_heartbeat: %u\n", KGRN, NODE_LBL, heartbeat);
    return TRUE;
}

void rm3100_set_max_rate(rm3100_s* m, uint8_t rate = RM3100_037HZ) {
	rm3100_write_8(m, RM3100_REGISTER_WTMRC, rate);
    fprintf(COM_D, "%s[%s] rm3100_set_max_rate: %u\n", KGRN, NODE_LBL, rate);
}

void rm3100_set_cycle(rm3100_s* m, uint16_t count_x = RM3100_COUNT, uint16_t count_y = RM3100_COUNT, uint16_t count_z = RM3100_COUNT) {
	rm3100_write_16(m, RM3100_REGISTER_CMX, count_x);
	rm3100_write_16(m, RM3100_REGISTER_CMY, count_y);
	rm3100_write_16(m, RM3100_REGISTER_CMZ, count_z);
    fprintf(COM_D, "%s[%s] rm3100_set_cycle: %u %u %u\n", KGRN, NODE_LBL, count_x, count_y, count_z);
}

void rm3100_set_mode(rm3100_s* m, uint8_t mode = RM3100_MODE_CMM) {
	rm3100_write_8(m, RM3100_REGISTER_CMM, mode);
    fprintf(COM_D, "%s[%s] rm3100_set_mode: %u\n", KGRN, NODE_LBL, mode);
}

void rm3100_read_data(rm3100_s* m) {	
	uint8_t read_data[9];
    // memset(read_data, 0, sizeof(read_data));
	
 	i2c_read_buf(m->add, RM3100_REGISTER_MX2, read_data, 9);
	if (read_data[0] > 0x80) {
		m->mag_x = 0xFF << 24;
	} else {
		m->mag_x = 0x00 << 24;
	}
	m->mag_x += (int32_t)read_data[0] << 16;
	m->mag_x += (int32_t)read_data[1] << 8;
	m->mag_x += (int32_t)read_data[2];

	if (read_data[3] > 0x80) {
		m->mag_y = 0xFF << 24;
	} else {
		m->mag_y = 0x00 << 24;
	}
	m->mag_y += (int32_t)read_data[4] << 16;
	m->mag_y += (int32_t)read_data[5] << 8;
	m->mag_y += (int32_t)read_data[6];

	if (read_data[7] > 0x80) {
		m->mag_z = 0xFF << 24;
	} else {
		m->mag_z = 0x00 << 24;
	}
	m->mag_z += (int32_t)read_data[7] << 16;
	m->mag_z += (int32_t)read_data[8] << 8;
	m->mag_z += (int32_t)read_data[9];

    fprintf(COM_D, "%s[%s] rm3100_read_data: %u %u %u\n", KGRN, NODE_LBL, m->mag_x, m->mag_y, m->mag_z);
}
