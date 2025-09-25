//=============================================================================================================
// main.c
// Testing routine template for components connected into the PPMs
//=============================================================================================================
#include <24FJ256GA110.h>			// Device Header File.  Switched to brackets to use version in PICC Library
//#include "pinsupper.h"              // Add pins Upper PPM Definition
#include "pinslower.h"              // Add pins Upper PPM Definition
#device PASS_STRINGS=IN_RAM			// Should allow for things like sendMSGALL(port,"Message") <--- const weren't allowed there before
#device ADC=16						// ??
#build(stack=0x300)					// Use a larger stack size.
#fuses NOPROTECT					// Code not protected from reading
#fuses NOWDT						// No automatic WDT -- it must be enabled by software.
//#fuses XT							// Primary Clock Select
//#fuses FRC_PLL				    // Internal Fast RC Oscillator with Phase Lock Loop gives 32 MHz
#fuses HS
#fuses PR_PLL
#fuses WPOSTS13						// Watchdog Postscaler.
									// At current processor settings: WPOSTS12 = ~12 seconds
									//								  WPOSTS13 = ~20 seconds
									//								  default  = ~120 seconds
#fuses IESO							// Internal-External Switchover
//#fuses IOL1WAY					// Locks the I/O Lock after setting it once.
#fuses NOIOL1WAY					// Allows multiple changes to PIC register settings.
#fuses WRT
#fuses CKSFSM						// Clock fail-safe monitor
#pragma case						// Makes all code case-sensitive

#use delay(clock=32MHZ,internal=8M)  // Tells compiler what the clock speed is
//#use delay(clock=8MHZ,internal=8M)  // Tells compiler what the clock speed is
#include <string.h>

//========================================
//  		PIC Registers
//========================================
#word RCON = 0x0740
#word OSCCON = getenv("SFR:OSCCON")
#bit IOLOCK = OSCCON.6
#word RPINR20 = getenv("SFR:RPINR20")
#word RPOR1 = getenv("SFR:RPOR1")
/* // DEBUG DISPLAY
#warning OSCCON is located at getenv("SFR:OSCCON")
#warning IOLOCK is located at getenv("BIT:IOLOCK")
#warning RPINR20 is located at getenv("SFR:RPINR20")
#warning RPOR1 is located at getenv("SFR:RPOR1")
*/

// Port 0
#pin_select U1TX = U1TX_PIN
#pin_select U1RX = U1RX_PIN

// Port 1
#pin_select U2TX = U2TX_PIN
#pin_select U2RX = U2RX_PIN

// Port 2
#pin_select U3TX = U3TX_PIN
#pin_select U3RX = U3RX_PIN

// Port 3
#pin_select U4TX = U4TX_PIN
#pin_select U4RX = U4RX_PIN

#define COM_A_BAUD		 115200  // Beacon
#define COM_B_BAUD       115200 // Transceiver
#define COM_C_BAUD       115200 // Other PPM
#define COM_D_BAUD       115200  // Payload

#define COM_A 1
#define COM_B 2
#define COM_C 3
#define COM_D 4

// #define TRANSMIT_BUFFER 1

//==================================================================
//  		Serial Port Initialization
//===================================================================
#use rs232(baud = COM_A_BAUD, UART1, bits = 8, STREAM = COM_A, ERRORS)  // try using TIMEOUT=5000ms or something
#use rs232(baud = COM_B_BAUD, UART2, bits = 8, STREAM = COM_B, ERRORS)
#use rs232(baud = COM_C_BAUD, UART3, bits = 8, STREAM = COM_C, ERRORS) // To/From other PIC
#use rs232(baud = COM_D_BAUD, UART4, bits = 8, STREAM = COM_D, ERRORS)

#include "uart_interface.c"
#include <stdlibm.h>
#include "register_reads.h"

// commenting out all of the functions
/*

//#define HEADER_SIZE 4
//#define CHECKSUM_SIZE 1





// 0 = COM_A, 1 = COM_B, 2 = COM_C, 3 = COM_D
// returns 1 when it works, 0 when it doesn't
int1 send_instruction(int8* instruction, size_t len, int8 stream_id);
int1 send_instruction(int8* instruction, size_t len, int8 stream_id) {
	size_t i;
	fprintf(COM_B, "Sending instruction: ");
	switch (stream_id) {
		case 0:
			for (i = 0; i < len; i++) {
				fprintf(COM_B, "%x, ", instruction[i]);
				fputc(instruction[i], COM_A);
			}
			break;
		case 1:
			for (i = 0; i < len; i++)
				fputc(instruction[i], COM_B);
			break;
		case 2:
			for (i = 0; i < len; i++)
				fputc(instruction[i], COM_C);
			break;
		case 3:
			for (i = 0; i < len; i++)
				fputc(instruction[i], COM_D);
			break;
		default:
			return 0;
	}
	fprintf(COM_B, "\n");
	return 1;
}

// if this function doesn't read anything, result may return garbage.  
// TODO -- need to ensure this doesn't happen

//int8* read_message(int8 stream_id, size_t result_len) {
//	int8* result = malloc(sizeof(int8) * result_len);
//	long timeout = 0;
//	while (timeout++ < 5000) {
//		switch (stream_id) {
//			//case 0:
//			//	fgetc(COM_A)
//			default:
//				// here be bad things.
//				free(result);
//				return NULL;
//		}
//	}
//}


// THERE IS A MEMORY ERROR OCCURRING IN THIS CODE.
// A PRINT STATEMENT BEFORE AND AFTER YIELDS CORRECT RESULTS, BUT WITHOUT, IT IS 
// INDETERMINATE.  

int8* read_message_COM_A();
int8* read_message_COM_A() {
	long timeout = 0;
	int8 count = 0;
	int8* message = malloc(4 * sizeof(int8));	// header size is 4
	//memset(message, 0, 4 * sizeof(int8)); //-- FOR SOME REASON, THIS LINE RESULTS IN INDETERMINATE BEHAVIOR.
	int8 data_count = 0;	// replacement for n in flowchart
	int1 is_data_count_assigned = 0;
	//fprintf(COM_B, "sizeof %u\n", sizeof(int8));
	// loops until timeout, or until we have read all the data (count <= n + 2), which we cannot determine until count is at least 3
	while (timeout < 5000 && (count < 2 || (is_data_count_assigned == 1 && count <= (data_count * 4) + 2))) {
		//fprintf(COM_B, "LOOPING");
		if (kbhit(COM_A) == 1) {
			timeout = 0;
			message[count] = fgetc(COM_A);
			fprintf(COM_B, "kbhit() triggered, char read: %d\n", message[count]);
			count++;
			if (count < 2) {
				delay_us(10);
				timeout++;
				continue;
			}
			// ERROR - for some reason, it's not going down into this area.  kbhit() is only triggered once.
			// THIS is why using a memset(0) or calloc() above is throwing an error... because it's just
			// reading the garbage data that the system has given it, instead of what we want to see.
			else if (count == 2 && is_data_count_assigned == 0) {	// data count is unassigned.  what happens when it's equal to 0, in truth?
				data_count = message[2];		// this assignment might assign data_count = 0
				is_data_count_assigned = 1;
				// next few lines are copying the header over to a larger memory block which is depending on the size, 
				// and freeing the smaller one
				size_t len = (4 * sizeof(int8)) + (4 * data_count * sizeof(int8)) + (1 * sizeof(int8)) + 1;
				fprintf(COM_B, "realloc len: %u\n", len);
				message = realloc(message, len);
			}
		}
		delay_us(10);
		timeout++;
	}
	if (timeout >= 5000) { //might still want to receive the 2 or less bytes of data passing through
		if (message != NULL) {
			free(message);
			message = NULL;
		}
		return NULL;
	}
	return message;
}


void sanity_check();
void sanity_check() {
	fprintf(COM_B, "hello world\n");
}

void checksum_calc_test_empty_body();
void checksum_calc_test_empty_body() {
	int8 index2     = 0x01;
	int8 header1    = (0x64<<1)|index2;
	
	//fprintf(COM_B, "help me");

	int8 count3     = 0x03;
	int8 map_error4 = (0x00<<4)|0x00;
	
	int8 sum = index2 + header1 + count3 + map_error4;
	sum = 0xff - sum + 1;
	int8 message [5];
	message[0] = header1;
	message[1] = index2;
	message[2] = count3;
	message[3] = map_error4;
	message[4] = sum;
	//fprintf(COM_B, "help me2\n");
	int8 returned_sum = calc_checksum(&message, 0);
	//fprintf(COM_B, "help me3\n");
	if (sum == returned_sum)
		fprintf(COM_B, "PASSED - Checksum Test 1 %d\n", count3);
	else
		fprintf(COM_B, "FAILED - Checksum 1: %d, Checksum 2: %d\n", sum, returned_sum);
}

void checksum_calc_test_4_byte_body();
void checksum_calc_test_4_byte_body() {
	int8 a = 0xC8;
	int8 b = 0x00;
	int8 c = 0x01;
	int8 d = 0x20;

	int8 e = 0x00;
	int8 f = 0x00;
	int8 g = 0x70;
	int8 h = 0x41;
	int8 i = 0x66;

	int8 message2 [9];
	message2[0] = a;
	message2[1] = b;
	message2[2] = c;
	message2[3] = d;
	message2[4] = e;
	message2[5] = f;
	message2[6] = g;
	message2[7] = h;
	message2[8] = i;

	int8 returned_sum2 = calc_checksum(&message2, 4);
	if (i == returned_sum2)
		fprintf(COM_B, "PASSED - Checksum Body Test\n");
	else
		fprintf(COM_B, "FAILED - Checksum 1: %d, Checksum 2: %d\n", i, returned_sum2);
}

// int1 send_instruction(int8* instruction, size_t len, int8 stream_id)
void send_instruction_test();
void send_instruction_test() {
	int8 index2     = 0x01;
	int8 header1    = (0x64<<1)|index2;
	int8 count3     = 0x03;
	int8 map_error4 = (0x00<<4)|0x00;
	int8 instruction [5];
	instruction[0] = header1;
	instruction[1] = index2;
	instruction[2] = count3;
	instruction[3] = map_error4;
	instruction[4] = calc_checksum(&instruction, 0);

	fprintf(COM_B, "instruction: ");
	int8 i;
	for (i = 0; i < sizeof(instruction); i++) {
		fprintf(COM_B, "%x, ", instruction[i]);
	}
	fprintf(COM_B, "\n");

	int1 ret = send_instruction(&instruction, (size_t) 5, 0);
	if (ret == 1)
		fprintf(COM_B, "PASSED - Instruction Size 0 Sent.\n");
	else
		fprintf(COM_B, "FAILED - Instruction Size 0 Not Sent.\n");
}

// ONLY RUN THIS AFTER SEND_INSTRUCTION_TEST, or in other words, when there's another
// function that has sent an instruction there, and it will send a response
void read_instruction_response_test() {
	//fprintf(COM_B, "HELP MEEEEE\n");
	int8* message = read_message_COM_A();
	//fprintf(COM_B, "HELP MEEEE 2\n");
	size_t data_len = 4 * message[2];	// data_count is in the 3rd element of the header
	fprintf(COM_B, "data_len: %u\n", data_len);
	int8* data = malloc(data_len * sizeof(int8));
	memcpy(data, (message + sizeof(int8) * 4), data_len);
	fprintf(COM_B, "SNID DATA - %d %d %d %d\n", data[0], data[1], data[2], data[3]);
	delay_ms(1000);
	free(data);
	data = NULL;
	free(message);
	message = NULL;
}

// reads from COM_A
int8 timed_getc();
int8 timed_getc() {
	long timeout = 0;
	while (!kbhit() && (++timeout < 5000)) 	// 1/2 second
		delay_us(10);
	if (kbhit())
		return getc();
	else
		return 0;
}

void read_snid_response();
void read_snid_response() {
	int8 buf [17];
	memset(buf, 0, sizeof(buf));
	int8 i;
	long timeout = 0;
	//fprintf(COM_B, "buf size: %d\n", sizeof(buf));
	//fprintf(COM_B, "Errors (COM_A, Start): %d\n", RS232_ERRORS);
	fprintf(COM_B, "snid response: ");
	for (i = 0; i < sizeof(buf); i++) {
		while(timeout < 5000) {
			if (kbhit(COM_A)) {
				//buf[i] = getc();
				buf[i] = fgetc(COM_A);    // appears equivalent to above line
				fprintf(COM_B, "%x, \n", buf[i]);
				fprintf(COM_B, "rs232_errors: %x\n", rs232_errors);
				timeout = 0;
			} else {
				timeout++;
				delay_us(10);
			}
		}
	}
	if (timeout < 5000) {
		fprintf(COM_B, "Read SNID response TIMEOUT: %Ld\n", timeout);
		//fprintf(COM_B, "Errors (COM_A, End): %d\n", ERRORS);
	}

	//while (!kbhit(COM_A) && (++timeout < 5000))
	//	delay_us(10);
	//delay_us(10);
	//if (kbhit(COM_A)) {
	//for (i = 0; i < sizeof(buf); i++) {
	//	buf[i] = fgetc(COM_A);
		//fprintf(COM_B, "%x, ", buf[i]);
	//}
	//} else {
	//	fprintf(COM_B, "Read SNID response TIMEOUT: %Ld\n", timeout);
	//}
	
	for (i = 0; i < sizeof(buf); i++) {
		buf[i] = timed_getc();
		fprintf(COM_B, "%x, ", buf[i]);
	}
	
	fprintf(COM_B, "\ndone reading.\n");
}


*/

//calculates the checksum of the message.  
// assumes 4 bytes for the header, and 1 byte for the checksum
// data_len is in bytes, so make sure you multiply the data count by 4
// if there is no data body, I believe 0 sending 0 should work.
// TODO - NEEDS TO BE TESTED!

void test_read() {
	int16 reg_arr_len = 4;
	int8 ind = 130;
	int8 msg_size = (int8) (sizeof(unsigned int8) * reg_arr_len / 4);
	if ((sizeof(unsigned int8) * reg_arr_len) % 4 != 0)	// if the message only needs 2.4 data counts, ask for 3
		msg_size++;
	fprintf(COM_B, "msg size: %d\n", msg_size);
	int8* msg_header = build_header(1, ind, msg_size, 0);
	
	int8 i = 0;
	int8 send_buf [5];
	for (i = 0; i < 4; i++)
		send_buf[i] = msg_header[i];
	send_buf[4] = calc_checksum(send_buf, 0);
	free(msg_header);

	// print the instruction to be sent.
	fprintf(COM_B, "Sending instruction: 0x");
	for (i = 0; i < 5; i++) {
		if (i == 4 || i == 5 - 1)
			fprintf(COM_B, " ");
		fprintf(COM_B, "%x", send_buf[i]);
	}
	fprintf(COM_B, "\n");

	
	fprintf(COM_B, "Index being read: %u\n", (unsigned int8) ind);

	void* rcv_buf = send_and_read(send_buf, 5);
	//fprintf(COM_B, "sizeof float: %u\n", sizeof(float));
	//float* num1 = rcv_buf + (4 * sizeof(int8));
	//float* num2 = rcv_buf + (4 * sizeof(int8)) + sizeof(float);
	//float* num3 = rcv_buf + (4 * sizeof(int8)) + (2 * sizeof(float));
	//float* num4 = rcv_buf + (4 * sizeof(int8)) + (3 * sizeof(float));
	//fprintf(COM_B, "%f\n", *num1);
	//fprintf(COM_B, "float1: %f\n", rcv_buf[4]);
	free(rcv_buf);
}







//end commenting out all the functions

// MEMORY LEAK!  MALLOC MAY NOT BE FREED, 
// WE NEED TO DETERMINE HOW THIS FUNCTION ENDS,
// IN ORDER TO FREE IT PROPERLY
void main(void)
{
	//fprintf(COM_B, "\n\n\n\n\n\n");
	//sanity_check();
	int1 even = 0;
	do {
		
		//fprintf(COM_B, "-----------------------------------------------------------\n");
		delay_ms(3000);
		
		//test_read();
		//read_all_registers();
		
		//char fill_char;
		//if (even)
		//	fill_char = 0xFF;
		//else
		//	fill_char = 0xAA;
		/*char tle [140];
		memset(&tle, 0xFF, 140);
		void* prev_tle = read_TLE();
		void* write_ret = write_TLE(tle);
		void* post_tle = read_TLE();
		free(prev_tle);
		free(write_ret);
		free(post_tle);*/
		
		/*void* rcv_buf = read_LLA();
		free(rcv_buf);
		rcv_buf = NULL;*/
		read_table1();
		read_table2();
		read_table3();

		// construct snid request message
		/*
		int8 index2     = 0x01;
		int8 header1    = (0x64<<1)|index2;
	
		int8 count3     = 0x03;
		int8 map_error4 = (0x00<<4)|0x00;
		
		int8 buf [5];
		buf[0] = header1;
		buf[1] = index2;
		buf[2] = count3;
		buf[3] = map_error4;
		*/

		// calc checksum
		/*
		int8 len = 4;
		int8 sum = 0;
		while (--len) {
			sum += buf[len];
		}
		sum += buf[len];
		buf[4] = 0xff - sum + 1;
		*/
		/*
		buf[4] = calc_checksum(buf, 0);

		int8* rcv = send_and_read(buf, 5);
		free(rcv);
		*/
		
		/*
		// send instruction
		int8 i = 0;
		for (i = 0; i < 5; i++) {
			fputc(buf[i], COM_A);
		}
		
		
		int8 rec_buf [17];
		i = 0;
		long timeout = 0;
		while (!kbhit(COM_A) && (++timeout < 5000))
			delay_us(10);
		for (i = 0; i < 17; i++) {
			rec_buf[i] = fgetc(COM_A);
		}
		fprintf(COM_B, "done reading \n");
		i = 0;
		for (i = 0; i < 17; i++)
			fprintf(COM_B, "%x, ", rec_buf[i]);
		fprintf(COM_B, "\n");
		*/
		
		/*int8 buf [17];
		memset(buf, 0, sizeof(buf));
		fprintf(COM_B, "size of buffer: %d\n", sizeof(buf));
		int8 i;
		long timeout = 0;
		while (!kbhit(COM_A) && (++timeout < 5000))
			delay_us(10);
		delay_us(10);
		//if (kbhit(COM_A)) {
		fprintf(COM_B, "timeout: %Ld\n", timeout);
		for (i = 0; i < sizeof(buf); i++) {
			fprintf(COM_B, "i: %d, ", i);
			buf[i] = fgetc(COM_A);
			//fprintf(COM_B, "%x, ", buf[i]);
		}
		fprintf(COM_B, "\n");
		//fprintf(COM_B, "read done\n");*/
		//delay_ms(5000);
		//delay_ms(100);
		//read_snid_response();
		//fprintf(COM_B, "final rs232_errors: %x\n", rs232_errors);
		//fprintf(COM_B, "rs232_buffer_errors: %x\n", rs232_buffer_errors);
		//read_instruction_response_test();
		even = !even;
	} while (TRUE);

	/*
	//Header Generator
	int8 index2     = 0x01;
	int8 header1    = (0x64<<1)|index2;
	
	int8 count3     = 0x03;
	int8 map_error4 = (0x00<<4)|0x00;
	
	//Definitions
	int1 hit;
	int8 chk_sum;
	int8 result_chk_sum = 0;
	int8 instruction[5];
	//int8 result[4* count3 + 4* sizeof(header1) + sizeof(chk_sum)];
	
	
	int8 read_valid;
	read_valid = 0;

	int8 buf[4];
	int8 i = 0;
	int8 len = 4;
	
	int8 sum = 0;

	long timeout;
	timeout = 0;
	
	//Checksum generator
	buf[0] = header1;
	buf[1] = index2;
	buf[2] = count3;
	buf[3] = map_error4;
	
	//while(--len)
	//{
	//	sum+=buf[len];
	//}
	//sum+=buf[len];
	sum = calc_checksum(&buf, 0);	// is &buf the correct way to create the necessary layer of indirection?
	chk_sum = 0xff-sum+1;	// this looks like it's getting the 2's complement... why?
	
	//Header array
	len = 5;

	instruction[0]=header1;
	instruction[1]=index2;
	instruction[2]=count3;
	instruction[3]=map_error4;
	instruction[4]=chk_sum;

	//Main stuff
	do
	{
		// create variable buffer size for result, depending on the size of data sent
		int8 data_count = count3;
		size_t result_len;
		result_len = (4 * data_count) * sizeof(int8) + 5 * sizeof(int8);
		int8* result = malloc(result_len);
		

		unsigned char str1[256];
		str1 = "Hello World ";
		unsigned char *ptr1=str1;

		unsigned char command[256];
		unsigned char params[256];

		//unsigned char *cmd=command;
		//unsigned char *prms=params;
		//read_valid = uart_read_command(ptr1, COM_C);
		delay_ms(1000);
		//fprintf(COM_C, "I'm alive and COM_C!\n");

		//fprintf(COM_A, "I'm alive and COM_A!... %d%s\n", read_valid, &str1);

		//Send instruction
		//for (i=0;i<len;++i)
		//{			
		//	fputc(instruction[i], COM_A);
		//}

		// 0 = COM_A, 1 = COM_B, 2 = COM_C, 3 = COM_D
		send_instruction(&instruction, len, 0);
		
		//Read answer
		hit = 0;
		while(!kbhit(COM_A)&&(++timeout < 5000))
			delay_us(10);

		//if(kbhit(COM_A))
		if (timeout < 5000)
		{
		//	delay_us(10);
			// this for loop assumes all the data is populated.. it's possible
			// kbhit() returned true for a single bit being loaded, so the next
			// fgetc() call will immediately block continuation
			for (i=0;i<result_len;++i)
			{		
				result[i]=fgetc(COM_A);
			}

			//Answer checksum checking
			result_chk_sum = calc_checksum(result, (size_t) data_count * 4);
			// QUESTION!  the way this checksum has been done is different from how
			// it was done before.  Why is that?  
			while(--i)
			{	
				result_chk_sum+=result[i];
			}
			result_chk_sum+=result[i];
		}
		
		
		//fprintf(COM_B, "I'm alive and COM_B!... %d%s\n", read_valid, &str1);
		fprintf(COM_B, "instruction 1: %x. instruction 2: %x. instruction 3: %x. instruction 4: %x. instruction 5: %x\n",
				instruction[0], instruction[1], instruction[2], instruction[3], instruction[4]);
		fprintf(COM_B, "r1: %x. r2: %x. r3: %x. r4: %x. r5: %x. r6: %x. r7: %x. r8: %x. r9: %x. r10: %x. r11: %x. r12: %x. r13: %x. r14: %x. r15: %x. r16: %x. r17: %x. chk: %x\n",
				result[0], result[1], result[2], result[3], result[4],result[5], result[6], result[7], result[8], result[9],result[10], 
				result[11], result[12], result[13], result[14],result[15], result[16], result_chk_sum);
		
		fprintf(COM_C, "I'm alive and COM_C!... %d%s\n", read_valid, ptr1);
		fprintf(COM_D, "I'm alive and COM_D!... %d%s\n", read_valid, &str1);

		free(result);	// CHANGE THE NAME OF result?  could be NULL free.  be sure.
		result = NULL;
	} while(TRUE);
	*/
	
}
