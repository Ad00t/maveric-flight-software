#include "register_reads.h"

int8 calc_checksum(int8* message, int16 data_len) {
	// fprintf(COM_B, "data_len: %u\n", data_len);
	int8 checksum = 0x00;
	int16 i;
	for (i = 4 + data_len + 1 - 2; i > 0; i--)
		checksum += message[i];
	checksum += message[0];
	//fprintf(COM_B, "HELP MEEEEE");
	return 0xFF - checksum + 1;
}


/*
 * Sends a message through COM_A to the MTQ.  This message should be a write, meaning
 * it should have a body which contains data to be written.  If the message is sent and 
 * there is no response, or there is a checksum mismatch, or the response waiting times out, 
 * it prints a message to COM_B, and sets the return value to NULL.  On a successful write, 
 * also prints the return value to COM_B.  
 *
 * @param msg: the message being written to one of the MTQ's registers
 * @param len: the length of the message being written to one of the MTQ's registers
 * @return the response given by the MTQ, generally a 5-character acknowledgement.
 */
int8* send_write_and_read(int8* msg, int16 len) {
	int16 bytes = 4 + 1;
	int8* rcv_buf = calloc(bytes, 1);
	int16 i = 0;
	for (i = 0; i < len; i++) {
		fputc(msg[i], COM_A);
	}
	
	long timeout = 0;
	if (!kbhit(COM_A) && (++timeout < 5000))
		delay_us(10);
	if (timeout < 5000) {
		for (i = 0; i < 5; i++)
			rcv_buf[i] = fgetc(COM_A);
		fprintf(COM_B, "0x");
		for (i = 0; i < bytes; i++) {
			if (i == 4 || i == bytes - 1)
				fprintf(COM_B, " ");
			fprintf(COM_B, "%x", rcv_buf[i]);
		}
		fprintf(COM_B, "\n");
		if (rcv_buf[bytes - 1] != calc_checksum(rcv_buf, 0)) {
			fprintf(COM_B, "ERROR: Checksum mismatch.  Dumping received data [send_and_read()]\n");
			fprintf(COM_B, "checksum bytes: %x\n", rcv_buf[bytes - 1]);
			fprintf(COM_B, "calculated checksum: %x\n", calc_checksum(rcv_buf, 0));
			free(rcv_buf);
			rcv_buf = NULL;
		}
	} else {
		fprintf(COM_B, "ERROR: Timeout or no incoming data [send_write_and_read()]");
	}
	return rcv_buf;
}

/*
 * Sends a message through COM_A to the MTQ.  If the message is sent and there
 * is no response, or the message times out, or there is a checkful mismatch, 
 * returns NULL instead.  It also prints the response to COM_B, as well as returning,
 * and if an error is found, tries to report the reason it occurred, also into COM_B.  
 * 
 * NOTE: This function is generally called with the assumption that this is a READ, meaning
 * the length of len is generally 5.  Attempting to use this for a WRITE will likely cause 
 * undefined behavior.  For WRITEs, refer to and use send_write_and_read().
 * 
 * @param msg: the byte data of the message being sent.
 * @param len: the length of the msg data, in bytes
 * @return a pointer to heap memory where the response message is written.
 */
int8* send_and_read(int8* msg, int16 len) {
	//fprintf(COM_B, "Start of send_and_read()\n");
	//fprintf(COM_B, "len: %d\n", len);
	int8 data_count = msg[2];
	int16 bytes = 4 + (4 * data_count) + 1;
	int8* rcv_buf = calloc(bytes, 1);
	//fprintf(COM_B, "sizeof(int8): %u\n", sizeof(int8));
	//fprintf(COM_B, "data count is: %d, and expected byte buffer is: %d\n", data_count, bytes);
	int1 is_past_timeout = 0;
	int16 i = 0;
	for (i = 0; i < len; i++) {
		fputc(msg[i], COM_A);
	}
	
	long timeout = 0;
	if (!kbhit(COM_A) && (++timeout < 5000))
		delay_us(10);
	is_past_timeout = 1;
	
	
	if (timeout < 5000) {
		for (i = 0; i < bytes; i++) {
			rcv_buf[i] = fgetc(COM_A);
		}
		
		// print out
		fprintf(COM_B, "0x");
		for (i = 0; i < bytes; i++) {
			if (i == 4 || i == bytes - 1)
				fprintf(COM_B, " ");
			fprintf(COM_B, "%x", rcv_buf[i]);
		}
		fprintf(COM_B, "\n");
		

		if (rcv_buf[bytes - 1] != calc_checksum(rcv_buf, 4*rcv_buf[2])) {
			fprintf(COM_B, "ERROR: Checksum mismatch.  Dumping received data [send_and_read()]\n");
			fprintf(COM_B, "checksum bytes: %x\n", rcv_buf[bytes - 1]);
			fprintf(COM_B, "calculated checksum: %x\n", calc_checksum(rcv_buf, 4*rcv_buf[2]));
			free(rcv_buf);
			rcv_buf = NULL;
		}
		
		
		// free(rcv_buf);

	} else {
		// something is wrong
		fprintf(COM_B, "ERROR: Timeout or no incoming data [send_and_read()]");
	}
	//fprintf(COM_B, "Past kbhit(), timeout: %Ld\n", timeout);
	/*if (is_past_timeout)
		fprintf(COM_B, "Past timeout\n");*/
	return rcv_buf;
}

/*
 * Given the data necessary to construct a header, builds it and allocates it on the stack.
 * This return should be deallocated when copied over to a general message buffer.
 * 
 * @param read: 0/1, 0 if write, 1 if read (I BELIEVE)
 * @param ind: the index of the register the message has to do with
 * @param data_count: the size of the body or expected body in the return message, divided by 4 (see data sheet)
 * @param map_ind: 0/1/2, depending on which map of register is being read from (see data sheet)
 */
int8* build_header(int1 read, int8 ind, int8 data_count, int8 map_ind) {
	int8* header = calloc(4, 1);
	header[0] = (0x64 << 1) | read;
	header[1] = ind;
	header[2] = data_count;
	header[3] = (map_ind << 4);
	return header;
}

/*
 * This function calculates the size of the message in "data counts", or the schema that
 * the magnetorquer communications schema uses determine the message length in the 3rd byte.
 * It is the total number of bytes, divided by 4, rounding up.  
 *
 * @param dtype_size: the size of the data type being read from the register, usually derived from sizeof()
 * @param reg_arr_len: the number of elements read from the register
 */
int8 calc_msg_size(size_t dtype_size, int16 reg_arr_len) {
	int8 msg_size = (int8) (dtype_size * reg_arr_len / 4);
	if ((dtype_size * reg_arr_len) % 4 != 0)
		msg_size++;
	return msg_size;
}

/*
 * This function is a wrapper around send_and_read() that discards and frees the input and
 * output.  It is used in the code that reads all registers.
 *
 * @param msg_header: the header for the read request
 * @param rec_buf: the receive buffer, necessary for many read requests.  it points to null afterwards.
 */
void send_cmd(int8* msg_header, void* rec_buf) {
	rec_buf = send_and_read(msg_header, 5);
	free(msg_header);
	msg_header = NULL;
	free(rec_buf);
	rec_buf = NULL;
}

// this function is defunct for the regular reads and writes
int8* create_send_packet(int8* msg_header) {
	int8* send_packet = calloc(5, 1);
	int8 i;
	for (i = 0; i < 4; i++)
		send_packet[i] = msg_header[i];
	send_packet[4] = calc_checksum(send_packet, 0);
	free(msg_header);
	msg_header = NULL;
	return send_packet;
}


void* write_register(int8 map_ind, int8 ind, size_t dtype_size, int16 reg_arr_len, int8* data) {
	int8 msg_size = 0;
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	int16 packet_length;
	packet_length = (dtype_size * reg_arr_len) + 5;
	int8* send_packet = calloc(packet_length, 1);
	int8* header = NULL;
	header = build_header(0, ind, msg_size, map_ind);	// 0 means write, 1 means read
	int16 i;
	for (i = 0; i < packet_length; i++) {
		if (i < 4)
			send_packet[i] = header[i];
		else if (i == packet_length - 1)
			send_packet[i] = calc_checksum(send_packet, (int16) (packet_length - 1));
		else
			send_packet[i] = data[i - 4];
	}
	free(header);
	void* rec_buf = send_write_and_read(send_packet, packet_length);
	free(send_packet);
	send_packet = NULL;
	return rec_buf;
}


void* write_CONF(unsigned int8* conf) {
	return write_register(0, 4, sizeof(unsigned int8), 4, conf);
}

void* write_TIME(unsigned int8* time) {
	return write_register(0, 5, sizeof(unsigned int8), 4, time);
}


void* write_DATE(unsigned int8* date) {
	return write_register(0, 6, sizeof(unsigned int8), 4, date);
}


void* write_LLA_REF(float* lla_ref) {
	return write_register(0, 7, sizeof(float), 3, lla_ref);
}


void* write_Q_REF(float* q) {
	return write_register(0, 10, sizeof(float), 4, q);
}


void* write_POINTING_AXIS(float* pointing_axis) {
	return write_register(0, 14, sizeof(float), 3, pointing_axis);
}

void* write_TLE(char* tle) {
	return write_register(0, 17, sizeof(char), 140, tle);
}


void* write_GGA(char* gga) {
	return write_register(0, 52, sizeof(char), 128, gga);
}


void* write_ZDA(char* zda) {
	return write_register(0, 84, sizeof(char), 64, zda);
}


void* write_SV_USER(float* sv_user) {
	return write_register(0, 100, sizeof(float), 3, sv_user);
}


void* write_MTQ_USER(float* mtq_user) {
	return write_register(0, 103, sizeof(float), 3, mtq_user);
}

// start of writes for register map 3


void* write_MASS(float mass) {
	return write_register(2, 0, sizeof(float), 1, mass);
}


void* write_INE_TEN(float* ine_ten) {
	return write_register(2, 1, sizeof(float), 9, ine_ten);
}


void* write_POS_HB_B(float* pos_hb_b) {
	return write_register(2, 10, sizeof(float), 3, pos_hb_b);
}


void* write_ORIEN_HB(float* orien_hb) {
	return write_register(2, 13, sizeof(float), 4, orien_hb);
}


void* write_MAG_INFO(unsigned int8* mag_info) {
	return write_register(2, 17, sizeof(unsigned int8), 4, mag_info);
}


void* write_MAG0_ORIEN_BS(float* orien_bs) {
	return write_register(2, 18, sizeof(float), 4, orien_bs);
}


void* write_MAG1_ORIEN_BS(float* orien_bs) {
	return write_register(2, 22, sizeof(float), 4, orien_bs);
}


void* write_MAG2_ORIEN_BS(float* orien_bs) {
	return write_register(2, 26, sizeof(float), 4, orien_bs);
}


void* write_MAG3_ORIEN_BS(float* orien_bs) {
	return write_register(2, 30, sizeof(float), 4, orien_bs);
}


void* write_MAG4_ORIEN_BS(float* orien_bs) {
	return write_register(2, 34, sizeof(float), 4, orien_bs);
}


void* write_MAG5_ORIEN_BS(float* orien_bs) {
	return write_register(2, 38, sizeof(float), 4, orien_bs);
}


void* write_FSS_INFO(unsigned int8* fss_info) {
	return write_register(2, 42, sizeof(unsigned int8), 4, fss_info);
}


void* write_FSS0_ORIEN_BS(float* orien_bs) {
	return write_register(2, 43, sizeof(float), 4, orien_bs);
}


void* write_FSS1_ORIEN_BS(float* orien_bs) {
	return write_register(2, 47, sizeof(float), 4, orien_bs);
}


void* write_FSS2_ORIEN_BS(float* orien_bs) {
	return write_register(2, 51, sizeof(float), 4, orien_bs);
}


void* write_FSS3_ORIEN_BS(float* orien_bs) {
	return write_register(2, 55, sizeof(float), 4, orien_bs);
}


void* write_FSS4_ORIEN_BS(float* orien_bs) {
	return write_register(2, 59, sizeof(float), 4, orien_bs);
}


void* write_FSS5_ORIEN_BS(float* orien_bs) {
	return write_register(2, 63, sizeof(float), 4, orien_bs);
}


void* write_IMU_INFO(unsigned int8* imu_info) {
	return write_register(2, 67, sizeof(unsigned int8), 4, imu_info);
}


void* write_IMU0_ORIEN_BS(float* orien_bs) {
	return write_register(2, 68, sizeof(float), 4, orien_bs);
}


void* write_IMU1_ORIEN_BS(float* orien_bs) {
	return write_register(2, 72, sizeof(float), 4, orien_bs);
}


void* write_IMU2_ORIEN_BS(float* orien_bs) {
	return write_register(2, 76, sizeof(float), 4, orien_bs);
}


void* write_IMU3_ORIEN_BS(float* orien_bs) {
	return write_register(2, 80, sizeof(float), 4, orien_bs);
}

// this function MAY not have the write (potentially also a read), check the comment on the data sheet.

void* write_STR_INFO(unsigned int8* str_info) {
	return write_register(2, 84, sizeof(unsigned int8), 4, str_info);
}


void* write_STR0_ORIEN_BS(float* orien_bs) {
	return write_register(2, 85, sizeof(float), 4, orien_bs);
}


void* write_STR1_ORIEN_BS(float* orien_bs) {
	return write_register(2, 89, sizeof(float), 4, orien_bs);
}


void* write_NVM(unsigned int8* nvm) {
	return write_register(2, 255, sizeof(unsigned int8), 4, nvm);
}


void* read_register(int8 map_ind, int8 ind, size_t dtype_size, int16 reg_arr_len) {
	int8 msg_size;
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	int8* send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));	// 1 = read
	void* rec_buf = send_and_read(send_packet, 5);
	return rec_buf;
}



void* read_LLA() {
	return read_register(0, 139, sizeof(float), 3);
}

void* read_TLE() {
	return read_register(0, 17, sizeof(char), 140);
}

void* read_TIME() {
	return read_register(0, 5, sizeof(unsigned int8), 4);
}


void* read_FACT() {
	return read_register(0, 0, sizeof(unsigned int16), 2);
}


void* read_SNID() {
	return read_register(0, 1, sizeof(char), 12);
}


void* read_CONF() {
	return read_register(0, 4, sizeof(unsigned int8), 4);
}


void* read_DATE() {
	return read_register(0, 6, sizeof(unsigned int8), 4);
}


void* read_LLA_REF() {
	return read_register(0, 7, sizeof(float), 3);
}


void* read_Q_REF() {
	return read_register(0, 10, sizeof(float), 4);
}


void* read_POINTING_AXIS() {
	return read_register(0, 14, sizeof(float), 3);
}


void* read_GGA() {
	return read_register(0, 52, sizeof(char), 128);
}


void* read_ZDA() {
	return read_register(0, 84, sizeof(char), 64);
}


void* read_SV_USER() {
	return read_register(0, 100, sizeof(float), 3);
}


void* read_MTQ_USER() {
	return read_register(0, 103, sizeof(float), 3);
}


void* read_CMG0_G_RATE_USER() {
	return read_register(0, 106, sizeof(float), 1);
}


void* read_CMG0_W_TOR_USER() {
	return read_register(0, 107, sizeof(float), 1);
}


void* read_CMG1_G_RATE_USER() {
	return read_register(0, 108, sizeof(float), 1);
} 


void* read_CMG1_W_TOR_USER() {
	return read_register(0, 109, sizeof(float), 1);
}


void* read_CMG2_G_RATE_USER() {
	return read_register(0, 110, sizeof(float), 1);
} 


void* read_CMG2_W_TOR_USER() {
	return read_register(0, 111, sizeof(float), 1);
}


void* read_CMG3_G_RATE_USER() {
	return read_register(0, 112, sizeof(float), 1);
}


void* read_CMG3_W_TOR_USER() {
	return read_register(0, 113, sizeof(float), 1);
}


void* read_STAT() {
	return read_register(0, 128, sizeof(unsigned int8), 4);
}


void* read_ACT_ERR() {
	return read_register(0, 129, sizeof(unsigned int8), 4);
}


void* read_SEN_ERR() {
	return read_register(0, 130, sizeof(unsigned int8), 4);
}


void* read_CSS_ERR() {
	return read_register(0, 131, sizeof(unsigned int8), 4);
}


void* read_Q() {
	return read_register(0, 132, sizeof(float), 4);
}


void* read_RATE() {
	return read_register(0, 136, sizeof(float), 3);
}


void* read_ATT_ERROR() {
	return read_register(0, 142, sizeof(float), 3);
}


void* read_ATT_ERROR_RATE() {
	return read_register(0, 145, sizeof(float), 3);
}


void* read_ADCS_TMP() {
	return read_register(0, 148, sizeof(int16), 2);
}


void* read_CMG0_TMP() {
	return read_register(0, 149, sizeof(int16), 2);
}


void* read_CMG1_TMP() {
	return read_register(0, 150, sizeof(int16), 2);
}


void* read_CMG2_TMP() {
	return read_register(0, 151, sizeof(int16), 2);
}


void* read_CMG3_TMP() {
	return read_register(0, 152, sizeof(int16), 2);
}


void* read_FSS_TMP1() {
	return read_register(0, 153, sizeof(int16), 2);
}


void* read_FSS_TMP2() {
	return read_register(0, 154, sizeof(int16), 2);
}


void* read_FSS_TMP3() {
	return read_register(0, 155, sizeof(int16), 2);
}


void* read_SV() {
	return read_register(0, 156, sizeof(float), 3);
}


void* read_MAG() {
	return read_register(0, 159, sizeof(float), 3);
}

// start of table 2 read defs

void* read_MAG_MAT() {
	return read_register(1, 0, sizeof(float), 9);
}


void* read_MAG_VEC() {
	return read_register(1, 9, sizeof(float), 3);
}


void* read_MAG_STAT() {
	return read_register(1, 12, sizeof(unsigned int8), 4);
}


void* read_MAG0_S() {
	return read_register(1, 13, sizeof(float), 3);
}


void* read_MAG1_S() {
	return read_register(1, 16, sizeof(float), 3);
}


void* read_MAG2_S() {
	return read_register(1, 19, sizeof(float), 3);
}


void* read_MAG3_S() {
	return read_register(1, 22, sizeof(float), 3);
}


void* read_MAG4_S() {
	return read_register(1, 25, sizeof(float), 3);
}


void* read_MAG5_S() {
	return read_register(1, 28, sizeof(float), 3);
}


void* read_FSS_STAT() {
	return read_register(1, 31, sizeof(unsigned int8), 4);
}


void* read_FSS0_SV() {
	return read_register(1, 32, sizeof(unsigned int16), 2);
}


void* read_FSS0_PDSUM() {
	return read_register(1, 33, sizeof(unsigned int16), 2);
}


void* read_FSS1_SV() {
	return read_register(1, 34, sizeof(unsigned int16), 2);
}


void* read_FSS1_PDSUM() {
	return read_register(1, 35, sizeof(unsigned int16), 2);
}


void* read_FSS2_SV() {
	return read_register(1, 36, sizeof(unsigned int16), 2);
}


void* read_FSS2_PDSUM() {
	return read_register(1, 37, sizeof(unsigned int16), 2);
}


void* read_FSS3_SV() {
	return read_register(1, 38, sizeof(unsigned int16), 2);
}


void* read_FSS3_PDSUM() {
	return read_register(1, 39, sizeof(unsigned int16), 2);
}


void* read_FSS4_SV() {
	return read_register(1, 40, sizeof(unsigned int16), 2);
}


void* read_FSS4_PDSUM() {
	return read_register(1, 41, sizeof(unsigned int16), 2);
}


void* read_FSS5_SV() {
	return read_register(1, 42, sizeof(unsigned int16), 2);
}


void* read_FSS5_PDSUM() {
	return read_register(1, 43, sizeof(unsigned int16), 2);
}


void* read_IMU_STAT() {
	return read_register(1, 44, sizeof(unsigned int8), 4);
}


void* read_IMU0_S() {
	return read_register(1, 45, sizeof(float), 3);
}


void* read_IMU1_S() {
	return read_register(1, 48, sizeof(float), 3);
}


void* read_IMU2_S() {
	return read_register(1, 51, sizeof(float), 3);
}


void* read_IMU3_S() {
	return read_register(1, 54, sizeof(float), 3);
}


void* read_STR_STAT() {
	return read_register(1, 57, sizeof(unsigned int8), 4);
}


void* read_STR0_S() {
	return read_register(1, 58, sizeof(float), 4);
}


void* read_STR1_S() {
	return read_register(1, 62, sizeof(float), 4);
}


void* read_CSS() {
	return read_register(1, 66, sizeof(float), 12);
}


void* read_CMG_STAT() {
	return read_register(1, 78, sizeof(unsigned int8), 4);
}


void* read_CMG0_G_ANGLE() {
	return read_register(1, 79, sizeof(float), 1);
}


void* read_CMG1_G_ANGLE() {
	return read_register(1, 81, sizeof(float), 1);
}


void* read_CMG2_G_ANGLE() {
	return read_register(1, 83, sizeof(float), 1);
}


void* read_CMG3_G_ANGLE() {
	return read_register(1, 85, sizeof(float), 1);
}


void* read_CMG0_W_RATE() {
	return read_register(1, 80, sizeof(float), 1);
}


void* read_CMG1_W_RATE() {
	return read_register(1, 82, sizeof(float), 1);
}


void* read_CMG2_W_RATE() {
	return read_register(1, 84, sizeof(float), 1);
}


void* read_CMG3_W_RATE() {
	return read_register(1, 86, sizeof(float), 1);
}


void* read_MTQ() {
	return read_register(1, 87, sizeof(float), 3);
}


void* read_CMG0_G_RATE() {
	return read_register(1, 90, sizeof(float), 1);
}


void* read_CMG1_G_RATE() {
	return read_register(1, 92, sizeof(float), 1);
}


void* read_CMG2_G_RATE() {
	return read_register(1, 94, sizeof(float), 1);
}


void* read_CMG3_G_RATE() {
	return read_register(1, 96, sizeof(float), 1);
}


void* read_CMG0_W_ACC() {
	return read_register(1, 91, sizeof(float), 1);
}


void* read_CMG1_W_ACC() {
	return read_register(1, 93, sizeof(float), 1);
}


void* read_CMG2_W_ACC() {
	return read_register(1, 95, sizeof(float), 1);
}


void* read_CMG3_W_ACC() {
	return read_register(1, 97, sizeof(float), 1);
}

// start of register map 3 reads


void* read_MASS() {
	return read_register(2, 0, sizeof(float), 1);
}


void* read_INE_TEN() {
	return read_register(2, 1, sizeof(float), 9);
}


void* read_POS_HB_B() {
	return read_register(2, 10, sizeof(float), 3);
}


void* read_ORIEN_HB() {
	return read_register(2, 13, sizeof(float), 4);
}


void* read_MAG_INFO() {
	return read_register(2, 17, sizeof(unsigned int8), 4);
}


void* read_MAG0_ORIEN_BS() {
	return read_register(2, 18, sizeof(float), 4);
}


void* read_MAG1_ORIEN_BS() {
	return read_register(2, 22, sizeof(float), 4);
}


void* read_MAG2_ORIEN_BS() {
	return read_register(2, 26, sizeof(float), 4);
}


void* read_MAG3_ORIEN_BS() {
	return read_register(2, 30, sizeof(float), 4);
}


void* read_MAG4_ORIEN_BS() {
	return read_register(2, 34, sizeof(float), 4);
}


void* read_MAG5_ORIEN_BS() {
	return read_register(2, 38, sizeof(float), 4);
}


void* read_FSS_INFO() {
	return read_register(2, 42, sizeof(unsigned int8), 4);
}


void* read_FSS0_ORIEN_BS() {
	return read_register(2, 43, sizeof(float), 4);
}


void* read_FSS1_ORIEN_BS() {
	return read_register(2, 47, sizeof(float), 4);
}


void* read_FSS2_ORIEN_BS() {
	return read_register(2, 51, sizeof(float), 4);
}



void* read_FSS3_ORIEN_BS() {
	return read_register(2, 55, sizeof(float), 4);
}


void* read_FSS4_ORIEN_BS() {
	return read_register(2, 59, sizeof(float), 4);
}


void* read_FSS5_ORIEN_BS() {
	return read_register(2, 63, sizeof(float), 4);
}


void* read_IMU_INFO() {
	return read_register(2, 67, sizeof(unsigned int8), 4);
}


void* read_IMU0_ORIEN_BS() {
	return read_register(2, 68, sizeof(float), 4);
}


void* read_IMU1_ORIEN_BS() {
	return read_register(2, 72, sizeof(float), 4);
}


void* read_IMU2_ORIEN_BS() {
	return read_register(2, 76, sizeof(float), 4);
}


void* read_IMU3_ORIEN_BS() {
	return read_register(2, 80, sizeof(float), 4);
}


void* read_STR_INFO() {
	return read_register(2, 84, sizeof(unsigned int8), 4);
}


void* read_STR0_ORIEN_BS() {
	return read_register(2, 85, sizeof(float), 4);
}


void* read_STR1_ORIEN_BS() {
	return read_register(2, 89, sizeof(float), 4);
}


void* read_NVM() {
	return read_register(2, 255, sizeof(unsigned int8), 4);
}





void read_parameter_register() {
	const int8 map_ind = 2;
	int16 reg_arr_len = 1;
	int8 ind = 0;
	size_t dtype_size = sizeof(float);
	int8 msg_size = 0;	
	void* rec_buf = NULL;
	int8* send_packet = NULL;

	// MASS
	fprintf(COM_B, "MASS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// INE_TEN
	ind = 1;
	dtype_size = sizeof(float);
	reg_arr_len = 9;
	fprintf(COM_B, "INE_TEN:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// POS_HB_B
	ind = 10;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "POS_HB_B:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// ORIEN_HB
	ind = 13;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "ORIEN_HB:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG_INFO
	ind = 17;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "MAG_INFO:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG0_ORIEN_BS
	ind = 18;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "MAG0_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG1_ORIEN_BS
	ind = 22;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "MAG1_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG2_ORIEN_BS
	ind = 26;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "MAG2_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG3_ORIEN_BS
	ind = 30;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "MAG3_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG4_ORIEN_BS
	ind = 34;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "MAG4_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG5_ORIEN_BS
	ind = 38;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "MAG5_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS_INFO
	ind = 42;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "FSS_INFO:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS0_ORIEN_BS
	ind = 43;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "FSS0_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS1_ORIEN_BS
	ind = 47;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "FSS1_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS2_ORIEN_BS
	ind = 51;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "FSS2_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS3_ORIEN_BS
	ind = 55;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "FSS3_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);
	
	// FSS4_ORIEN_BS
	ind = 59;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "FSS4_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS5_ORIEN_BS
	ind = 63;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "FSS5_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU_INFO
	ind = 67;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "IMU_INFO:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU0_ORIEN_BS
	ind = 68;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "IMU0_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU1_ORIEN_BS
	ind = 72;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "IMU1_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU2_ORIEN_BS
	ind = 76;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "IMU2_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU3_ORIEN_BS
	ind = 80;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "IMU3_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// STR_INFO
	ind = 84;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "STR_INFO:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// STR0_ORIEN_BS
	ind = 85;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "STR0_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// STR1_ORIEN_BS
	ind = 89;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "STR1_ORIEN_BS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// NVM
	ind = 255;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "NVM:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);
}


void read_all_registers() {
	fprintf(COM_B, "START_REGISTERS\n");
	read_user_register();	// need to recheck this
	read_sensor_actuator_register();
	read_parameter_register();
	fprintf(COM_B, "END_REGISTERS\n");
}

void read_sensor_actuator_register() {
	const int8 map_ind = 1;
	int16 reg_arr_len = 9;
	int8 ind = 0;
	size_t dtype_size = sizeof(float);
	int8 msg_size = 0;	
	void* rec_buf = NULL;
	int8* send_packet = NULL;

	// MAG_MAT
	fprintf(COM_B, "MAG_MAT:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG_VEC
	ind = 9;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MAG_VEC:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG_STAT
	ind = 12;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "MAG_STAT:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);
	

	// MAG0_S
	ind = 13;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MAG0_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG1_S
	ind = 16;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MAG1_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG2_S
	ind = 19;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MAG2_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG3_S
	ind = 22;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MAG3_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);
	
	// MAG4_S
	ind = 25;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MAG4_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG5_S
	ind = 28;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MAG5_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS_STAT
	ind = 31;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "FSS_STAT:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS0_SV
	ind = 32;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS0_SV:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS0_PDSUM
	ind = 33;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS0_PDSUM:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS1_SV
	ind = 34;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS1_SV:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);
	
	// FSS1_PDSUM
	ind = 35;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS1_PDSUM:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS2_SV
	ind = 36;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS2_SV:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS2_PDSUM
	ind = 37;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS2_PDSUM:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS3_SV
	ind = 38;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS3_SV:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS3_PDSUM
	ind = 39;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS3_PDSUM:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS4_SV
	ind = 40;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS4_SV:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS4_PDSUM
	ind = 41;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS4_PDSUM:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS5_SV
	ind = 42;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS5_SV:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS5_PDSUM
	ind = 43;
	dtype_size = sizeof(unsigned int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS5_PDSUM:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU_STAT
	ind = 44;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "IMU_STAT:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU0_S
	ind = 45;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "IMU0_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU1_S
	ind = 48;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "IMU1_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU2_S
	ind = 51;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "IMU2_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// IMU3_S
	ind = 54;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "IMU3_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// STR_STAT
	ind = 57;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "STR_STAT:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// STR0_S
	ind = 58;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "STR0_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// STR1_S
	ind = 62;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "STR1_S:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CSS
	ind = 66;
	dtype_size = sizeof(float);
	reg_arr_len = 12;
	fprintf(COM_B, "CSS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG_STAT
	ind = 78;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "CMG_STAT:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG0_G_ANGLE
	ind = 79;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG0_G_ANGLE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);
	
	// CMG0_W_RATE
	ind = 80;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG0_W_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG1_G_ANGLE
	ind = 81;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG1_G_ANGLE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG1_W_RATE
	ind = 82;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG1_W_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG2_G_ANGLE
	ind = 83;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG2_G_ANGLE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG2_W_RATE
	ind = 84;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG2_W_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG3_G_ANGLE
	ind = 85;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG3_G_ANGLE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG3_W_RATE
	ind = 86;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG3_W_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MTQ
	ind = 87;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MTQ:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG0_G_RATE
	ind = 90;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG0_G_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG0_W_ACC
	ind = 91;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG0_W_ACC:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG1_G_RATE
	ind = 92;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG1_G_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG1_W_ACC
	ind = 93;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG1_W_ACC:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG2_G_RATE
	ind = 94;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG2_G_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG2_W_ACC
	ind = 95;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG2_W_ACC:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG3_G_RATE
	ind = 96;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG3_G_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG3_W_ACC
	ind = 97;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG3_W_ACC:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);
}



void read_user_register() {
	const int8 map_ind = 0;
	int16 reg_arr_len = 2;
	int8 ind = 0;
	size_t dtype_size = sizeof(unsigned int16);
	int8 msg_size = 0;	
	void* rec_buf = NULL;
	int8* send_packet = NULL;

	// FACT
	fprintf(COM_B, "FACT:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// SNID
	ind = 1;
	dtype_size = sizeof(char);
	reg_arr_len = 12;
	fprintf(COM_B, "SNID:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CONF
	ind = 4;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "CONF:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// TIME
	ind = 5;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "TIME:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// DATE
	ind = 6;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "DATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// LLA_REF
	ind = 7;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "LLA_REF:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// Q_REF
	ind = 10;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "Q_REF:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// POINTING_AXIS
	ind = 14;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "POINTING_AXIS:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// TLE
	ind = 17;
	dtype_size = sizeof(char);
	reg_arr_len = 140;
	fprintf(COM_B, "TLE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// GGA
	ind = 52;
	dtype_size = sizeof(char);
	reg_arr_len = 128;
	fprintf(COM_B, "GGA:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// ZDA
	ind = 84;
	dtype_size = sizeof(char);
	reg_arr_len = 64;
	fprintf(COM_B, "ZDA:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// SV_USER
	ind = 100;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "SV_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	
	// MTQ_USER
	ind = 103;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MTQ_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG0_G_RATE_USER
	ind = 106;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG0_G_RATE_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG0_W_TOR_USER
	ind = 107;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG0_W_TOR_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG1_G_RATE_USER
	ind = 108;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG1_G_RATE_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG1_W_TOR_USER
	ind = 109;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG1_W_TOR_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG2_G_RATE_USER
	ind = 110;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG2_G_RATE_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG2_W_TOR_USER
	ind = 111;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG2_W_TOR_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG3_G_RATE_USER
	ind = 112;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG3_G_RATE_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG3_W_TOR_USER
	ind = 113;
	dtype_size = sizeof(float);
	reg_arr_len = 1;
	fprintf(COM_B, "CMG3_W_TOR_USER:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// STAT
	ind = 128;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "STAT:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// ACT_ERR
	ind = 129;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "ACT_ERR:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// SEN_ERR
	ind = 130;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "SEN_ERR:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CSS_ERR
	ind = 131;
	dtype_size = sizeof(unsigned int8);
	reg_arr_len = 4;
	fprintf(COM_B, "CSS_ERR:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// Q
	ind = 132;
	dtype_size = sizeof(float);
	reg_arr_len = 4;
	fprintf(COM_B, "Q:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// RATE
	ind = 136;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// LLA
	ind = 139;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "LLA:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// ATT_ERROR
	ind = 142;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "ATT_ERROR:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// ATT_ERROR_RATE
	ind = 145;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "ATT_ERROR_RATE:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// ADCS_TMP
	ind = 148;
	dtype_size = sizeof(int16);
	reg_arr_len = 2;
	fprintf(COM_B, "ADCS_TMP:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG0_TMP
	ind = 149;
	dtype_size = sizeof(int16);
	reg_arr_len = 2;
	fprintf(COM_B, "CMG0_TMP:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG1_TMP
	ind = 150;
	dtype_size = sizeof(int16);
	reg_arr_len = 2;
	fprintf(COM_B, "CMG1_TMP:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG2_TMP
	ind = 151;
	dtype_size = sizeof(int16);
	reg_arr_len = 2;
	fprintf(COM_B, "CMG2_TMP:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// CMG3_TMP
	ind = 152;
	dtype_size = sizeof(int16);
	reg_arr_len = 2;
	fprintf(COM_B, "CMG3_TMP:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS_TMP1
	ind = 153;
	dtype_size = sizeof(int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS_TMP1:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS_TMP2
	ind = 154;
	dtype_size = sizeof(int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS_TMP2:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// FSS_TMP3
	ind = 155;
	dtype_size = sizeof(int16);
	reg_arr_len = 2;
	fprintf(COM_B, "FSS_TMP3:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// SV
	ind = 156;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "SV:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);

	// MAG
	ind = 159;
	dtype_size = sizeof(float);
	reg_arr_len = 3;
	fprintf(COM_B, "MAG:");
	msg_size = calc_msg_size(dtype_size, reg_arr_len);
	send_packet = create_send_packet(build_header(1, ind, msg_size, map_ind));
	send_cmd(send_packet, rec_buf);
}



void read_table1() {
	void* rcv = NULL;
	
	fprintf(COM_B, "FACT:");
	rcv = read_FACT();
	free(rcv);
	rcv = NULL;
	
	fprintf(COM_B, "SNID:");
	rcv = read_SNID();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CONF:");
	rcv = read_CONF();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "TIME:");
	rcv = read_TIME();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "DATE:");
	rcv = read_DATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "LLA_REF:");
	rcv = read_LLA_REF();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "Q_REF:");
	rcv = read_Q_REF();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "POINTING_AXIS:");
	rcv = read_POINTING_AXIS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "TLE:");
	rcv = read_TLE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "GGA:");
	rcv = read_GGA();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "ZDA:");
	rcv = read_ZDA();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "SV_USER:");
	rcv = read_SV_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MTQ_USER:");
	rcv = read_MTQ_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG0_G_RATE_USER:");
	rcv = read_CMG0_G_RATE_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG0_W_TOR_USER:");
	rcv = read_CMG0_W_TOR_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG1_G_RATE_USER:");
	rcv = read_CMG1_G_RATE_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG1_W_TOR_USER:");
	rcv = read_CMG1_W_TOR_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG2_G_RATE_USER:");
	rcv = read_CMG2_G_RATE_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG2_W_TOR_USER:");
	rcv = read_CMG2_W_TOR_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG3_G_RATE_USER:");
	rcv = read_CMG3_G_RATE_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG3_W_TOR_USER:");
	rcv = read_CMG3_W_TOR_USER();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "STAT:");
	rcv = read_STAT();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "ACT_ERR:");
	rcv = read_ACT_ERR();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "SEN_ERR:");
	rcv = read_SEN_ERR();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CSS_ERR:");
	rcv = read_CSS_ERR();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "Q:");
	rcv = read_Q();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "RATE:");
	rcv = read_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "LLA:");
	rcv = read_LLA();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "ATT_ERROR:");
	rcv = read_ATT_ERROR();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "ATT_ERROR_RATE:");
	rcv = read_ATT_ERROR_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "ADCS_TMP:");
	rcv = read_ADCS_TMP();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG0_TMP:");
	rcv = read_CMG0_TMP();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG1_TMP:");
	rcv = read_CMG1_TMP();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG2_TMP:");
	rcv = read_CMG2_TMP();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG3_TMP:");
	rcv = read_CMG3_TMP();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS_TMP1:");
	rcv = read_FSS_TMP1();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS_TMP2:");
	rcv = read_FSS_TMP2();
	free(rcv);
	rcv = NULL;
	
	fprintf(COM_B, "FSS_TMP3:");
	rcv = read_FSS_TMP3();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "SV:");
	rcv = read_SV();
	free(rcv);
	rcv = NULL;
	
	fprintf(COM_B, "MAG:");
	rcv = read_MAG();
	free(rcv);
	rcv = NULL;
}


void read_table2() {
	void* rcv = NULL;

	fprintf(COM_B, "MAG_MAT:");
	rcv = read_MAG_MAT();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG_VEC:");
	rcv = read_MAG_VEC();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG_STAT:");
	rcv = read_MAG_STAT();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG0_S:");
	rcv = read_MAG0_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG1_S:");
	rcv = read_MAG1_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG2_S:");
	rcv = read_MAG2_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG3_S:");
	rcv = read_MAG3_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG4_S:");
	rcv = read_MAG4_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG5_S:");
	rcv = read_MAG5_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS_STAT:");
	rcv = read_FSS_STAT();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS0_SV:");
	rcv = read_FSS0_SV();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS0_PDSUM:");
	rcv = read_FSS0_PDSUM();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS1_SV:");
	rcv = read_FSS1_SV();
	free(rcv);
	rcv = NULL;
	
	fprintf(COM_B, "FSS1_PDSUM:");
	rcv = read_FSS1_PDSUM();
	free(rcv);
	rcv = NULL;
	
	fprintf(COM_B, "FSS2_SV:");
	rcv = read_FSS2_SV();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS2_PDSUM:");
	rcv = read_FSS2_PDSUM();
	free(rcv);
	rcv = NULL;
	
	fprintf(COM_B, "FSS3_SV:");
	rcv = read_FSS3_SV();
	free(rcv);
	rcv = NULL;
	
	fprintf(COM_B, "FSS3_PDSUM:");
	rcv = read_FSS3_PDSUM();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS4_SV:");
	rcv = read_FSS4_SV();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS4_PDSUM:");
	rcv = read_FSS4_PDSUM();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS5_SV:");
	rcv = read_FSS5_SV();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS5_PDSUM:");
	rcv = read_FSS5_PDSUM();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU_STAT:");
	rcv = read_IMU_STAT();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU0_S:");
	rcv = read_IMU0_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU1_S:");
	rcv = read_IMU1_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU2_S:");
	rcv = read_IMU2_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU3_S:");
	rcv = read_IMU3_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "STR_STAT:");
	rcv = read_STR_STAT();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "STR0_S:");
	rcv = read_STR0_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "STR1_S:");
	rcv = read_STR1_S();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CSS:");
	rcv = read_CSS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG_STAT:");
	rcv = read_CMG_STAT();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG0_G_ANGLE:");
	rcv = read_CMG0_G_ANGLE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG0_W_RATE:");
	rcv = read_CMG0_W_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG1_G_ANGLE:");
	rcv = read_CMG1_G_ANGLE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG1_W_RATE:");
	rcv = read_CMG1_W_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG2_G_ANGLE:");
	rcv = read_CMG2_G_ANGLE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG2_W_RATE:");
	rcv = read_CMG2_W_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG3_G_ANGLE:");
	rcv = read_CMG3_G_ANGLE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG3_W_RATE:");
	rcv = read_CMG3_W_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MTQ:");
	rcv = read_MTQ();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG0_G_RATE:");
	rcv = read_CMG0_G_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG0_W_ACC:");
	rcv = read_CMG0_W_ACC();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG1_G_RATE:");
	rcv = read_CMG1_G_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG1_W_ACC:");
	rcv = read_CMG1_W_ACC();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG2_G_RATE:");
	rcv = read_CMG2_G_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG2_W_ACC:");
	rcv = read_CMG2_W_ACC();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG3_G_RATE:");
	rcv = read_CMG3_G_RATE();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "CMG3_W_ACC:");
	rcv = read_CMG3_W_ACC();
	free(rcv);
	rcv = NULL;
}


void read_table3() {
	void* rcv = NULL;

	fprintf(COM_B, "MASS:");
	rcv = read_MASS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "INE_TEN:");
	rcv = read_INE_TEN();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "POS_HB_B:");
	rcv = read_POS_HB_B();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "ORIEN_HB:");
	rcv = read_ORIEN_HB();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG_INFO:");
	rcv = read_MAG_INFO();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG0_ORIEN_BS:");
	rcv = read_MAG0_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG1_ORIEN_BS:");
	rcv = read_MAG1_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG2_ORIEN_BS:");
	rcv = read_MAG2_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG3_ORIEN_BS:");
	rcv = read_MAG3_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG4_ORIEN_BS:");
	rcv = read_MAG4_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "MAG5_ORIEN_BS:");
	rcv = read_MAG5_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS_INFO:");
	rcv = read_FSS_INFO();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS0_ORIEN_BS:");
	rcv = read_FSS0_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS1_ORIEN_BS:");
	rcv = read_FSS1_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS2_ORIEN_BS:");
	rcv = read_FSS2_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS3_ORIEN_BS:");
	rcv = read_FSS3_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS4_ORIEN_BS:");
	rcv = read_FSS4_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "FSS5_ORIEN_BS:");
	rcv = read_FSS5_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU_INFO:");
	rcv = read_IMU_INFO();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU0_ORIEN_BS:");
	rcv = read_IMU0_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU1_ORIEN_BS:");
	rcv = read_IMU1_ORIEN_BS();
	free(rcv);
	rcv = NULL;
	
	fprintf(COM_B, "IMU2_ORIEN_BS:");
	rcv = read_IMU2_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "IMU3_ORIEN_BS:");
	rcv = read_IMU3_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "STR_INFO:");
	rcv = read_STR_INFO();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "STR0_ORIEN_BS:");
	rcv = read_STR0_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "STR1_ORIEN_BS:");
	rcv = read_STR1_ORIEN_BS();
	free(rcv);
	rcv = NULL;

	fprintf(COM_B, "NVM:");
	rcv = read_NVM();
	free(rcv);
	rcv = NULL;
}