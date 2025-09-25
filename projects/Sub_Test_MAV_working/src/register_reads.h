

int8 calc_checksum(int8* message, int16 data_len);

// for COM_A; len is length of msg array
// returns pointer to heap memory where received message is written, 
// or NULL if the message timed out, did not receive a return, or there was a 
// checksum mismatch.  tries to report the reason it occurred (into COM_B).
int8* send_and_read(int8* msg, int16 len);

// read = 1 then read, read = 0 then write.
// return pointer is stored on heap, needs to be freed.  please free quickly.
int8* build_header(int1 read, int8 ind, int8 data_count, int8 map_ind);
void test_read();
int8 calc_msg_size(size_t dtype_size, int16 reg_arr_len);
// DANGER.  This function assumes that rec_buf is freed and destroyed, then
// pointed to NULL. DO NOT attempt to access rec_buf after this function is called.
// This is used to send data to the ground station, and should not be used to send
// in other instances.
void send_cmd(int8* msg_header, void* rec_buf);
// assumes a message of length 5.  not suitable to other use
int8* create_send_packet(int8* msg_header);
// reads the entire user register

int8* send_write_and_read(int8* msg, int16 len);


/*
 * This function creates a send packet and asks to read from one of the magnetorquer's registers.
 * Its purpose is to minimize the amount of lines of code it takes to read from a register.
 * The user is to supply the necessary parameters, and the function will handle getting the result.
 * 
 * @param map_ind: the index of the register map the register being read from belongs to
 * @param ind: the index of the register being read to, on the indicated register map
 * @param dtype_size: the size of each of element being read of the register
 * @param reg_arr_len: the number of elements being read from the register
 * @return a buffer on the heap that contains the information on the register
 */
void* read_register(int8 map_ind, int8 ind, size_t dtype_size, int16 reg_arr_len);

/* 
 * This function creates a send packet and writes it to the magnetorquer, given the parameters.  
 * It returns the value returned by the the magnetorquer following the write.  This is 
 * usually a 5 byte acknowledgement, as is assumed by send_write_and_read().  Any issues with
 * that are the problem of that function.  
 * 
 * This function can be called with the requisite information to facilitate very short
 * register writes.  All we need to pass in the required information, and it should handle
 * everything else.  
 * 
 * @param map_ind: index of the register map the register being written to belongs to
 * @param reg_arr_len: the number of elements being written
 * @param ind: index of the register being written to
 * @param dtype_size: the size of each of the elements being written
 * @param data: the data being written into the register
 * @return the return value of the MTQ after the write
 */
void* write_register(int8 map_ind, int8 ind, size_t dtype_size, int16 reg_arr_len, int8* data);

// Register reads for MTQ Table 1
void* read_FACT();
void* read_SNID();
void* read_CONF();
void* read_TIME();
void* read_DATE();
void* read_LLA();
void* read_LLA_REF();
void* read_Q_REF();
void* read_POINTING_AXIS();
void* read_TLE();
void* read_GGA();
void* read_ZDA();
void* read_SV_USER();
void* read_MTQ_USER();
void* read_CMG0_G_RATE_USER();
void* read_CMG0_W_TOR_USER();
void* read_CMG1_G_RATE_USER();
void* read_CMG1_W_TOR_USER();
void* read_CMG2_G_RATE_USER();
void* read_CMG2_W_TOR_USER();
void* read_CMG3_G_RATE_USER();
void* read_CMG3_W_TOR_USER();
void* read_STAT();
void* read_ACT_ERR();
void* read_SEN_ERR();
void* read_CSS_ERR();
void* read_Q();
void* read_RATE();
void* read_ATT_ERROR();
void* read_ATT_ERROR_RATE();
void* read_ADCS_TMP();
void* read_CMG0_TMP();
void* read_CMG1_TMP();
void* read_CMG2_TMP();
void* read_CMG3_TMP();
void* read_FSS_TMP1();
void* read_FSS_TMP2();
void* read_FSS_TMP3();
void* read_SV();
void* read_MAG();

// Register reads for MTQ table 2
void* read_MAG_MAT();
void* read_MAG_VEC();
void* read_MAG_STAT();
void* read_MAG0_S();
void* read_MAG1_S();
void* read_MAG2_S();
void* read_MAG3_S();
void* read_MAG4_S();
void* read_MAG5_S();
void* read_FSS_STAT();
void* read_FSS0_SV();
void* read_FSS0_PDSUM();
void* read_FSS1_SV();
void* read_FSS1_PDSUM();
void* read_FSS2_SV();
void* read_FSS2_PDSUM();
void* read_FSS3_SV();
void* read_FSS3_PDSUM();
void* read_FSS4_SV();
void* read_FSS4_PDSUM();
void* read_FSS5_SV();
void* read_FSS5_PDSUM();
void* read_IMU_STAT();
void* read_IMU0_S();
void* read_IMU1_S();
void* read_IMU2_S();
void* read_IMU3_S();
void* read_STR_STAT();
void* read_STR0_S();
void* read_STR1_S();
void* read_CSS();
void* read_CMG_STAT();
void* read_CMG0_G_ANGLE();
void* read_CMG1_G_ANGLE();
void* read_CMG2_G_ANGLE();
void* read_CMG3_G_ANGLE();
void* read_CMG0_W_RATE();
void* read_CMG1_W_RATE();
void* read_CMG2_W_RATE();
void* read_CMG3_W_RATE();
void* read_MTQ();
void* read_CMG0_G_RATE();
void* read_CMG1_G_RATE();
void* read_CMG2_G_RATE();
void* read_CMG3_G_RATE();
void* read_CMG0_W_ACC();
void* read_CMG1_W_ACC();
void* read_CMG2_W_ACC();
void* read_CMG3_W_ACC();

// Register reads for MTQ label 3
void* read_MASS();
void* read_INE_TEN();
void* read_POS_HB_B();
void* read_ORIEN_HB();
void* read_MAG_INFO();
void* read_MAG0_ORIEN_BS();
void* read_MAG1_ORIEN_BS();
void* read_MAG2_ORIEN_BS();
void* read_MAG3_ORIEN_BS();
void* read_MAG4_ORIEN_BS();
void* read_MAG5_ORIEN_BS();
void* read_FSS_INFO();
void* read_FSS0_ORIEN_BS();
void* read_FSS1_ORIEN_BS();
void* read_FSS2_ORIEN_BS();
void* read_FSS3_ORIEN_BS();
void* read_FSS4_ORIEN_BS();
void* read_FSS5_ORIEN_BS();
void* read_IMU_INFO();
void* read_IMU0_ORIEN_BS();
void* read_IMU1_ORIEN_BS();
void* read_IMU2_ORIEN_BS();
void* read_IMU3_ORIEN_BS();
void* read_STR_INFO();
void* read_STR0_ORIEN_BS();
void* read_STR1_ORIEN_BS();
void* read_NVM();

// Write registers for Table 1
void* write_CONF(unsigned int8* conf);
void* write_TLE(char* tle);
void* write_TIME(unsigned int8* time);
void* write_DATE(unsigned int8* date);
void* write_LLA_REF(float* lla_ref);
void* write_Q_REF(float* q);
void* write_POINTING_AXIS(float* pointing_axis);
void* write_GGA(char* gga);
void* write_ZDA(char* zda);
void* write_SV_USER(float* sv_user);
void* write_MTQ_USER(float* mtq_user);

// Write registers for Table 3 (none for Table 2)
void* write_MASS(float mass);
void* write_INE_TEN(float* ine_ten);
void* write_POS_HB_B(float* pos_hb_b);
void* write_ORIEN_HB(float* orien_hb);
void* write_MAG_INFO(unsigned int8* mag_info);
void* write_MAG0_ORIEN_BS(float* orien_bs);
void* write_MAG1_ORIEN_BS(float* orien_bs);
void* write_MAG2_ORIEN_BS(float* orien_bs);
void* write_MAG3_ORIEN_BS(float* orien_bs);
void* write_MAG4_ORIEN_BS(float* orien_bs);
void* write_MAG5_ORIEN_BS(float* orien_bs);
void* write_FSS_INFO(unsigned int8* fss_info);
void* write_FSS0_ORIEN_BS(float* orien_bs);
void* write_FSS1_ORIEN_BS(float* orien_bs);
void* write_FSS2_ORIEN_BS(float* orien_bs);
void* write_FSS3_ORIEN_BS(float* orien_bs);
void* write_FSS4_ORIEN_BS(float* orien_bs);
void* write_FSS5_ORIEN_BS(float* orien_bs);
void* write_IMU_INFO(unsigned int8* imu_info);
void* write_IMU0_ORIEN_BS(float* orien_bs);
void* write_IMU1_ORIEN_BS(float* orien_bs);
void* write_IMU2_ORIEN_BS(float* orien_bs);
void* write_IMU3_ORIEN_BS(float* orien_bs);
void* write_STR_INFO(unsigned int8* str_info);
void* write_STR0_ORIEN_BS(float* orien_bs);
void* write_STR1_ORIEN_BS(float* orien_bs);
void* write_NVM(unsigned int8* nvm);



// old method of reading from all registers
void read_all_registers();
void read_user_register();
void read_sensor_actuator_register();
void read_parameter_register();

// new method of reading from all registers, calls the getter functions above
void read_table1();
void read_table2();
void read_table3();