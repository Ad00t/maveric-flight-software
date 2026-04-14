//=============================================================================================================
// Lower EPS main.c
//=============================================================================================================

//#define DEVELOP
#include <24FJ256GA110.h>			// Device Header File.  Switched to brackets to use version in PICC Library
#include <string.h>
#include "include/pineps.h"

#build(stack=0x300)					// Use a larger stack size.
#fuses NOPROTECT					// Code not protected from reading
// #fuses NOWDT						// No automatic WDT -- it must be enabled by software.
#fuses WDT
//#fuses XT							// Primary Clock Select
//#fuses FRC_PLL				    // Internal Fast RC Oscillator with Phase Lock Loop gives 32 MHz
#fuses HS
#fuses PR_PLL
#fuses WPOSTS13						// Watchdog Postscaler.  
									// At current processor settings: WPOSTS12 = ~10 seconds
									//								  WPOSTS13 = ~17 seconds
									//								  default  = ~120 seconds
#fuses IESO							// Internal-External Switchover
//#fuses IOL1WAY					// Locks the I/O Lock after setting it once.
#fuses NOIOL1WAY					// Allows multiple changes to PIC register settings.
#fuses WRT
#fuses CKSFSM						// Clock fail-safe monitor	
#pragma case						// Makes all code case-sensitive

//#use delay(clock=4000000)
#use delay(clock=32MHZ, internal=8M)  // Tells compiler what the clock speed is

// PIC registers

#word RCON = 0x0740
#word OSCCON = getenv("SFR:OSCCON") 
#bit IOLOCK = OSCCON.6
#word RPINR20 = getenv("SFR:RPINR20")
#word RPOR1 = getenv("SFR:RPOR1")

#define I2C_1       1 // Stream I2C3
#define I2C_2       2 // Stream I2C2
#define I2C_3       3 // Stream I2C3

#define LPPM_PORT I2C_2
#define UPPM_PORT I2C_3
//==================================================================
//  		I2C Port Initialization
//===================================================================
#use i2c(master, sda=Device_SDA, scl=Device_SCL, STREAM=I2C_1)
// Slave addresses are shifted by one bit: ex b11111111->b01111111
#use i2c(slave, sda=PIN_A3, scl=PIN_A2, address=0x33, RESTART_WDT, FORCE_HW, STREAM=I2C_2)
#use i2c(slave, sda=PIN_E7, scl=PIN_E6, address=0x31, RESTART_WDT, FORCE_HW, SMBUS, STREAM=I2C_3)
//#use i2c(slave, sda=PIN_E7, scl=PIN_E6, address=0x31, SMBUS, STREAM=I2C_3)

// Port 0
#pin_select U1TX = U1TX_PIN
#pin_select U1RX = U1RX_PIN

// Port 1
//#pin_select U2TX = U2TX_PIN
//#pin_select U2RX = U2RX_PIN

// Port 2
//#pin_select U3TX = U3TX_PIN
//#pin_select U3RX = U3RX_PIN

// Port 3
//#pin_select U4TX = U4TX_PIN
//#pin_select U4RX = U4RX_PIN


#define COM_A_BAUD  115200  // UART Testing Port
//#define COM_B_BAUD  115200  // Transceiver (Test purposes)
//#define COM_C_BAUD  115200 // Other PPM
//#define COM_D_BAUD  115200 // Payload

#define COM_A		1 // Stream Port 1
//#define COM_B       2 // Stream Port 2
//#define COM_C       3 // Stream Port 3
//#define COM_D       4 // Stream Port 4

#define EPS_PORT COM_A
//==================================================================
//  		Serial Port Initialization
//===================================================================
#use rs232(baud = COM_A_BAUD, UART1, bits = 8, STREAM = COM_A, ERRORS, PARITY = N, TIMEOUT=1000)
//#use rs232(baud = COM_B_BAUD, UART2, bits = 8, STREAM = COM_B, ERRORS, PARITY = N, TIMEOUT=1000)
//#use rs232(baud = COM_C_BAUD, UART3, bits = 8, STREAM = COM_C, ERRORS, PARITY = N, TIMEOUT=1000) // To/From other PIC
//#use rs232(baud = COM_D_BAUD, UART4, bits = 8, STREAM = COM_D, ERRORS, PARITY = N, TIMEOUT=1000)

// Global defines

//==================================================================
//  		Timer Port Initialization
//===================================================================
#USE TIMER(TIMER=1,TICK=500ms,BITS=16,NOISR)

#include "nodes.h"                                                               // C standard time library header

#define EPS
#define NODE                NODE_EPS 
#define LOG_LEVEL           LL_DEBUG 

#include <time.h>                                                               // C standard time library header
#include <time.c>                                                               // C standard time
#include "systime.c"                                                            //Time System Library

#include "common.c"                                                             // DEC-HEX conversion
#include "crcnew.c"                                                             //CRC library
#include "hashtable.c"                                                          //Hash Table Library
#include "ringbuf.c"                                                            //Ring Buffer Library

#include "uart.c"                                                               // UART DRIVER
#include "i2c.c"                                                                // I2C DRIVER

#include "interrupt.c"                                                          //EPS Interrup library

#include "framer.c"                                                             //Framer Library (KISS & CSP)

#include "cmdpkt.c"                                                             //Command Packaging Library
#include "logger.c"                                                             //Log check library (to NODE_FTDI)
#include "cmdmgr.c"                                                             //Command Manager Library
#include "cmdimpl.c"                                                            //Command List Library               

//Sub-Systems Include
#include "BQ25672.c"                                                            //EPS Controller DRIVER
#include "ina226.c"                                                             //Current Sensor Controller DRIVER


//Command handling

//System Functionalities Include OLD CODE USE AS REFERENCE FOR FUNTION IMPLEMENTATION
//#include "eps.c"                                                                //OLD EPS COMMAND HANDLING
//#include "cmd.c"                                                                //OLD COMMAND HANDLING

//System Functions
void eps_init(void);
void eps_superloop(void);

//void eps_cleanup(void);

//The Functions here shall be part of the commands
unsigned int16 tick_difference(unsigned int16 current, unsigned int16 previous);
void power_set_test(int pin, unsigned int8 add);
//void power_io(unsigned int8 eps_output, unsigned int8 eps_state);
void cut_wire(unsigned int8 cut_output, unsigned int8 cut_state, unsigned int8 cut_time);
void eps_housekeeping(void);
//void eps_housekeeping(unsigned char *hk_bbq,unsigned char *hk_ina);
int8 pass_command(char* cmd);

int1 g_superloop_running = TRUE;
uint8_t g_rbt_cause = 0;

irqmgr_s g_irqmgr = {0};            // Interrupts manager
i2cmgr_s g_i2cmgr = {0};            // Interrupts manager
cmdmgr_s g_cmdmgr = {0};            // Commands manager
uint8_t i2c_frame[FRAME_MAX_SIZE] = {0};

uint8_t eps_state = 0;
uint8_t eps_mode = 0;

unsigned int16 current_tick, previous_tick, counter_reset;

int16 I_BUS;
int16 I_BAT;
int16 V_BUS;
int16 V_AC1;
int16 V_AC2;
int16 V_BAT;
int16 V_SYS;
int16 TS_ADC;
int16 T_DIE;
//int16 D_P;
//int16 D_M;
int16 V3V3;
int16 I3V3;
int16 P3V3;
int16 V5V0;
int16 I5V0;
int16 P5V0;
int16 VOUT1;
int16 IOUT1;
int16 POUT1;
int16 VOUT2;
int16 IOUT2;
int16 POUT2;
int16 VOUT3;
int16 IOUT3;
int16 POUT3;
int16 VOUT4;
int16 IOUT4;
int16 POUT4;
int16 VOUT5;
int16 IOUT5;
int16 POUT5;
int16 VOUT6;
int16 IOUT6;
int16 POUT6;
int16 VBRN1;
int16 IBRN1;
int16 PBRN1;
int16 VBRN2;
int16 IBRN2;
int16 PBRN2;
int16 VSIN1;
int16 ISIN1;
int16 PSIN1;
int16 VSIN2;
int16 ISIN2;
int16 PSIN2;
int16 VSIN3;
int16 ISIN3;
int16 PSIN3;

void main(void)
{	
	//enable_interrupts(INT_SI2C3);
	//enable_interrupts(GLOBAL);
	
	//unsigned char eps_hk[10]="hk";
	//// ex: hk
	////hk buffers
	//unsigned char hk_bbq[128];
	//unsigned char hk_ina[128];

	//unsigned char eps_io[10]="io";
	//// ex: io 1 1 / io 1 0
	//unsigned char eps_ct[10]="ct";
	//// ex: ct 1 1 5 / ct 1 0 3
	////unsigned int1 bq_bit;
	
	//start_flag = FALSE;
	//cmd_flag = FALSE;
	//delay_ms(1000);
    //isr_enable_all();
	//start_flag = TRUE;
	//delay_ms(1000);
    
    //fprintf(COM_A,"EPS_USC_SERC\n\r");	
	//delay_ms(1000);
    
    eps_init();
    while (g_superloop_running) {
        eps_superloop(); 
    }
    
	/*
    bq_bit = bq25672_init();
	bq25672_update();
    
	i2c_write_16(INA226_ADDRESS_8, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_7, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_6, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_5, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_4, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_3, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_2, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_1, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_12, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_13, INA226_REG_CALIBRATION, 0x1400);
	//3.3V Bus
	//N/A
	//5V Bus
	//N/A
	//5V Input 6
	output_low(PIN_D6);
	//5V Input 5
	output_low(PIN_D5);
	//5V Input 4
	output_low(PIN_D4);
	//3.3V Input 3
	output_low(PIN_D3);
	//3.3V Input 2
	output_low(PIN_D2);
	//3.3V Input 1
	output_low(PIN_D1);
	// 5V Burn circuit 1
	output_low(PIN_E8);
	// 5V Burn circuit 2
	output_low(PIN_E9);

	fprintf(COM_A,"EPS_USC_SERC\n\r");	
	delay_ms(250);
     */
    
    
	//do
	//{	
	//	unsigned char command[256];
	//	unsigned char params[256];
	//
	//	unsigned char *cmd=command;
	//	unsigned char *prms=params;
	//	
	//	unsigned int8 eps_output, eps_state;
	//	unsigned int8 cut_output, cut_state, cut_time;

	//	//buffer_cmd="";
	//	//buffer2_cmd="";

	//#ifdef DEVELOP
	//	fprintf(COM_A,"Enter a string\n\r");
	//#else
	//	printf("cmd\n");
	//#endif
		
        /*
        if (cmd_flag)
		{	
			fprintf(COM_A,"Interrupt_activated\n\r");
			fprintf(COM_A,"Received: %s\n\r", rcv_cmd);
            isr_disable_all();

			fprintf(COM_A,"\033[31m[SYS] Solving cmd: %s; len: %u; start: %u; check: %s\n\r",rcv_cmd, len, rcv_cmd[0], &rcv_cmd[len-1]);
			//Get the cmd components
			cmd_get_command(rcv_cmd,&orgn,&dest,&ech, cmd, prms);			
			fprintf(COM_A,"[SYS] origin: %u; destination: %u; echo: %u\n\r",orgn,dest,ech);
			
			delay_ms(10);
			//eps_get_command(rcv_cmd, cmd, prms);
		
			#ifdef DEVELOP
				fprintf(COM_A,"%s\n\r",rcv_cmd);
				fprintf(COM_A,"String:%s, Command:%s, Parameters:%s\n\r", str1, command, params);
			#endif
		
				if (strcmp(command, eps_io)==0)
				{
					//Switch On/Off Function
					eps_get_prmts(params, &eps_output, &eps_state);
				#ifdef DEVELOP
					fprintf(COM_A,"output command\n");
					fprintf(COM_A,"eps output:%u eps state:%u \n\r", eps_output, eps_state);
				#endif
					power_io(eps_output, eps_state);
					
				}
				else if (strcmp(command, eps_ct)==0)
				{
					//Switch On/Off Function
					
					eps_get_prmts3(params, &cut_output, &cut_state, &cut_time);
				#ifdef DEVELOP
					fprintf(COM_A,"Wire Cut command\n\r");
					fprintf(COM_A,"cut output:%u cut state:%u cut state:%u\n\r", cut_output, cut_state, cut_time);
				#endif
					cut_wire(cut_output, cut_state, cut_time);
				}
				else if (strcmp(command,eps_hk)==0)
				{
				#ifdef DEVELOP
					fprintf(COM_A,"hk command\n\r");
				#endif
					//eps_housekeeping(hk_bbq,hk_ina);
					
					strcpy(buffer_cmd,hk_bbq);
					strcpy(buffer2_cmd,hk_ina);
					//counter = 0;
					//counter2 = 0;
					fprintf(COM_A,buffer_cmd);
					fprintf(COM_A,"\n");
					fprintf(COM_A,"Counter 1: %u , Counter 2: %u\n", counter, counter2);
					fprintf(COM_A,buffer2_cmd);
					fprintf(COM_A,"\n");
					eps_housekeeping(hk_bbq,hk_ina);
				}
				
				else
				{
				#ifdef DEVELOP
					fprintf(COM_A,"No command\n\r");
				#endif
				}
				//printf(command);
				//printf("Output = %d , State = %d", eps_output, eps_state);
				cmd_flag = FALSE;
                isr_enable_all();
		}*/
	//} while(TRUE);
}

void eps_init(void) {
    // Watchdog, millisecond timer, logger, rbt_cause init
    setup_wdt(WDT_ON);
	setup_timer1(TMR_INTERNAL | TMR_DIV_BY_64, 0x00FA); 
    //logger_init();
    g_rbt_cause = restart_cause();
    	
    // Interrupts init 
    irqmgr_init(&g_irqmgr);
    i2cmgr_init(&g_i2cmgr);	
    isr_enable_all();	
    g_irqmgr.started = TRUE;
    g_i2cmgr.started = TRUE;
    cmdmgr_init(&g_cmdmgr);	
    cmdimpl_init();	
    
    //Start EPS Manager
    //sprintf(LOGBUF, "system initialized"); log_info();
    delay_ms(1000);
	
    int1 bq_bit = bq25672_init();
	bq25672_update();
    //Calibrate INA226 sensors
    i2c_write_16(INA226_ADDRESS_8, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_7, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_6, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_5, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_4, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_3, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_2, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_1, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_12, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_13, INA226_REG_CALIBRATION, 0x1400);
    i2c_write_16(INA226_ADDRESS_9, INA226_REG_CALIBRATION, 0x1400);
	i2c_write_16(INA226_ADDRESS_10, INA226_REG_CALIBRATION, 0x1400);
    i2c_write_16(INA226_ADDRESS_11, INA226_REG_CALIBRATION, 0x1400);
	
	
    //Turning off the switchable Power supply
    //3.3V Bus
	//N/A
	//5V Bus
	//N/A
	//5V Input 6
	//output_low(PIN_D6);
    output_low(EPS_SW6);
	//5V Input 5
	output_low(EPS_SW5);
	//5V Input 4
	output_low(EPS_SW4);
	//3.3V Input 3
	output_low(EPS_SW3);
	//3.3V Input 2
	output_low(EPS_SW2);
	//3.3V Input 1
	output_low(EPS_SW1);
	// 5V Burn circuit 1
	output_low(EPS_BR1);
	// 5V Burn circuit 2
	output_low(EPS_BR2);
    
    
    //Sending Starting Message to UART port
    fprintf(COM_A,"EPS_USC_SERC %X\n\r", bq_bit);	
	delay_ms(1000);
    
    //time conditions for the eps.
    counter_reset = 0;
    current_tick = previous_tick = get_ticks();
}


// Master code of what runs every superloop iteration
void eps_superloop(void) {
    
    //Kick the dog
    restart_wdt();
    //fprintf(COM_A,".\r");	
    
    //delay_ms(1000);
    // Handle received byte interrupts
    // Do driver handling before commands so data is up to date
    // HERE IT SHOULD BE THE HOUSE KEEPING VARIABLES BEFORE 
    // CHECKING THE COMMANDS
    // nvg_parse_stream(&g_nvg, &g_irqmgr.irqbufs[NVG_PORT-1]); // Handle naviguider data
    cmdmgr_parse_stream(&g_cmdmgr, &g_irqmgr.irqbufs[EPS_PORT-1], &g_cmdmgr.rcvpkts[0], FALSE); // Handle UART commands 
    //cmdmgr_parse_stream(&g_cmdmgr, &g_i2cmgr.i2cbufs[LPPM_PORT-2], &g_cmdmgr.rcvpkts[1], FALSE); // Handle LPPM commands 
    cmdmgr_parse_stream(&g_cmdmgr, &g_i2cmgr.i2cbufs[UPPM_PORT-2], &g_cmdmgr.rcvpkts[2], FALSE); // Handle UPPM commands 
    
    current_tick = get_ticks();
    if(previous_tick > current_tick){
        previous_tick = current_tick;
    }
    if(tick_difference(current_tick, previous_tick) > 1500) {
        previous_tick = current_tick;
        eps_housekeeping();
        counter_reset += 1;
        fprintf(COM_A,"Current tick %Lu\n\r", counter_reset);
        //the number is to match the tick counter to around 2 hours
        if (counter_reset == 10986){
            //placeholder for a reset
            fprintf(COM_A,"EPS_USC_SERC Reset\n\r");	
            counter_reset = 0;
        }
        
    }
}

unsigned int16 tick_difference(unsigned int16 current, unsigned int16 previous) {
    return (current - previous);
}

void power_set_test(int pin, unsigned int8 add)
{
	unsigned int16 ina226_rdata, ina226_wdata;
	int16 shunt_voltage, bus_voltage, power, current;
	unsigned int8 addw, addr;
	unsigned int1 init_bit;
	//Turn off regulator
	printf("Turning off regulator %d \n\r", pin);
	if (pin > 0)
		output_low(pin);
	
	delay_ms(100);
	
	// Address Test
	addw = add <<1;
	addr = addw | 0x01;

	// Init routine
	printf("Starting ina226  in addresss %2X ...\n\r", add);
	init_bit = ina226_init(add);
	printf("Starting check %d \r\n",init_bit);

	//Configure routine
	ina226_rdata = i2c_read_16(add, INA226_REG_CONFIG);
	printf("Configuration Register: %LX \n\r", ina226_rdata);
	printf("Changing Configuration Register \n\r");
	ina226_configure(add); 
	ina226_rdata = i2c_read_16(add, INA226_REG_CONFIG);
	printf("Configuration Register: %LX\n\r", ina226_rdata);
	//Calibration routine
	ina226_rdata = i2c_read_16(add, INA226_REG_CALIBRATION);
	printf("Writing: %LX, Calibration Register: %LX\n\r", ina226_wdata, ina226_rdata);
	ina226_wdata = 0x1400;
	i2c_write_16(add, INA226_REG_CALIBRATION, ina226_wdata);
	ina226_rdata = i2c_read_16(add, INA226_REG_CALIBRATION);
	printf("Writing: %LX, Calibration Register: %LX\n\r", ina226_wdata, ina226_rdata);

	delay_ms(100);
	// Mesurement routine
	printf("Taking measurements reg off... \n\r");
	ina226_read_data(add, &shunt_voltage, &bus_voltage, &power, &current);
	printf("Shunt voltage: %Ld, Bus voltage: %Ld, Power: %Ld, Current: %Ld\n\r", shunt_voltage, bus_voltage, power, current);
	
	if (pin > 0)
		output_high(pin);
	
	delay_ms(1000);
	// Mesurement routine
	printf("Taking measurements reg on... \n\r");
	ina226_read_data(add, &shunt_voltage, &bus_voltage, &power, &current);
	printf("Shunt voltage: %Ld, Bus voltage: %Ld, Power: %Ld, Current: %Ld\n\r", shunt_voltage, bus_voltage, power, current);
	ina226_rdata = i2c_read_16(add, INA226_REG_ID);
	printf("ina226 addresss: %2X ina226 write: %2X ina226 read: %2X ina226 register: %2X \n\r",add, addw, addr, INA226_REG_ID);
	printf("Reg bytes: %LX\n\r", ina226_rdata);
	
	if (pin > 0)
		output_low(pin);

	delay_ms(100);
	printf("Done!\n\r");
}
/*
void power_io(unsigned int8 eps_output, unsigned int8 eps_state)
{

#ifdef DEVELOP
	int16 shunt_voltage, bus_voltage, power, current;
#endif

	unsigned int8 add;
	int pin;

	switch(eps_output)
	{
		case 1:
			pin = PIN_D1;
			add =  INA226_ADDRESS_1;
			break;
		case 2:
			pin = PIN_D2;
			add =  INA226_ADDRESS_2;
			break;
		case 3:
			pin = PIN_D3;
			add =  INA226_ADDRESS_3;
			break;
		case 4:
			pin = PIN_D4;
			add =  INA226_ADDRESS_4;
			break;
		case 5:
			pin = PIN_D5;
			add =  INA226_ADDRESS_5;
			break;
		case 6:
			pin = PIN_D6;
			add =  INA226_ADDRESS_6;
			break;
		default:
#ifdef DEVELOP
				printf("EPS Output Error\n\r");
#else
				printf("0\n");
#endif
			pin = 0;
			break;
	}

	if (pin)
	{
		output_high(pin);
		switch(eps_state)
		{
			case 0:
				output_low(pin);
				break;
			case 1:
				output_high(pin);
				break;
			default:
#ifdef DEVELOP
				printf("EPS State Error\n\r");
#else
				printf("0\n");
#endif
				break;
		}
#ifdef DEVELOP
		ina226_read_data(add, &shunt_voltage, &bus_voltage, &power, &current);
		printf("Shunt voltage: %Ld, Bus voltage: %Ld, Power: %Ld, Current: %Ld\n\r", shunt_voltage, bus_voltage, power, current);
#else
		printf("1\n");
#endif
	}
}
*/

void cut_wire(unsigned int8 cut_output, unsigned int8 cut_state, unsigned int8 cut_time)
{
	int16 shunt_voltage, bus_voltage, power, current;
	unsigned int8 add, sw_state;
	int pin, sw_pin, dp_pin, dp_flag;

	switch(cut_output)
	{
		case 1:
			pin = PIN_E8;
			sw_pin = PIN_B9;
			dp_pin = PIN_B13;
			add =  INA226_ADDRESS_12;
			break;
		case 2:
			pin = PIN_E9;
			sw_pin = PIN_B10;
			dp_pin = PIN_B12;
			add =  INA226_ADDRESS_13;
			break;
		default:

			fprintf(COM_A,"CUT Output Error\n\r");
			pin = 0;
			break;
	}

	if (pin)
	{	
		switch(cut_state)
		{
			case 0:
				output_low(dp_pin);
				dp_flag = 1;
				break;
			case 1:
				output_high(dp_pin);
				dp_flag = 1;
				break;
			default:
				dp_flag = 0;
#ifdef DEVELOP
				fprintf(COM_A,"CUT State Error\n\r");
#else
				fprintf(COM_A,"0\n");
#endif				
				break;
		}

		
		if (dp_flag)
		{
			output_high(pin);
			delay_ms(100);
			sw_state = input(sw_pin);
#ifdef DEVELOP
			fprintf(COM_A,"Deployer Selected: %2X %2X, Switch State: %d\n\r",pin, dp_pin, sw_state);		
//#else
			//printf("Shunt voltage: %Ld, Bus voltage: %Ld, Power: %Ld, Current: %Ld\n\r", shunt_voltage, bus_voltage, power, current);
			//printf("ct,%Ld,%Ld,%d",bus_voltage, current, sw_state);		
#endif

			delay_ms(1000*cut_time);
		
			sw_state = input(sw_pin);
			ina226_read_data(add, &shunt_voltage, &bus_voltage, &power, &current);
		
			output_low(pin);
		
			delay_ms(100);
#ifdef DEVELOP
			fprintf(COM_A,"Shunt voltage: %Ld, Bus voltage: %Ld, Power: %Ld, Current: %Ld\n\r", shunt_voltage, bus_voltage, power, current);
			fprintf(COM_A,"Deployer Selected: %2X %2X, Switch State: %d\n\r",pin, dp_pin, sw_state);		
#else
			//printf(",%2X,%2X,%d\n\r",pin, dp_pin, sw_state);	
			fprintf(COM_A,"ct,%2X,%2X,%Ld,%Ld,%d\n", pin, dp_pin, bus_voltage, current, sw_state);			
#endif
		}
		
	}
}

void eps_housekeeping(void)
{
    int16 shunt_voltage;
    extern int16 I_BUS;
    extern int16 I_BAT;
    extern int16 V_BUS;
    extern int16 V_AC1;
    extern int16 V_AC2;
    extern int16 V_BAT;
    extern int16 V_SYS;
    extern int16 TS_ADC;
    extern int16 T_DIE;
    //int16 D_P;
    //int16 D_M;
    extern int16 V3V3;
    extern int16 I3V3;
    extern int16 P3V3;
    extern int16 V5V0;
    extern int16 I5V0;
    extern int16 P5V0;
    extern int16 VOUT1;
    extern int16 IOUT1;
    extern int16 POUT1;
    extern int16 VOUT2;
    extern int16 IOUT2;
    extern int16 POUT2;
    extern int16 VOUT3;
    extern int16 IOUT3;
    extern int16 POUT3;
    extern int16 VOUT4;
    extern int16 IOUT4;
    extern int16 POUT4;
    extern int16 VOUT5;
    extern int16 IOUT5;
    extern int16 POUT5;
    extern int16 VOUT6;
    extern int16 IOUT6;
    extern int16 POUT6;
    extern int16 VBRN1;
    extern int16 IBRN1;
    extern int16 PBRN1;
    extern int16 VBRN2;
    extern int16 IBRN2;
    extern int16 PBRN2;
    extern int16 VSIN1;
    extern int16 ISIN1;
    extern int16 PSIN1;
    extern int16 VSIN2;
    extern int16 ISIN2;
    extern int16 PSIN2;
    extern int16 VSIN3;
    extern int16 ISIN3;
    extern int16 PSIN3;
    
    bq25672_get(BQ_ADDR,0x31,&I_BUS);
    bq25672_get(BQ_ADDR,0x33,&I_BAT);
    bq25672_get(BQ_ADDR,0x35,&V_BUS);
    bq25672_get(BQ_ADDR,0x37,&V_AC1);
    bq25672_get(BQ_ADDR,0x39,&V_AC2);
    bq25672_get(BQ_ADDR,0x3B,&V_BAT);
    bq25672_get(BQ_ADDR,0x3D,&V_SYS);
    bq25672_get(BQ_ADDR,0x3F,&TS_ADC); //Recall multiply by 0.0976563
    bq25672_get(BQ_ADDR,0x41,&T_DIE); //Recall multiply by 0.5
    
    ina226_read_data(INA226_ADDRESS_8, &shunt_voltage, &V3V3, &P3V3, &I3V3);
    ina226_read_data(INA226_ADDRESS_7, &shunt_voltage, &V5V0, &P5V0, &I5V0);
    
    ina226_read_data(INA226_ADDRESS_1, &shunt_voltage, &VOUT1, &POUT1, &IOUT1);
    ina226_read_data(INA226_ADDRESS_2, &shunt_voltage, &VOUT2, &POUT2, &IOUT2);
    ina226_read_data(INA226_ADDRESS_3, &shunt_voltage, &VOUT3, &POUT3, &IOUT3);
    ina226_read_data(INA226_ADDRESS_4, &shunt_voltage, &VOUT4, &POUT4, &IOUT4);
    ina226_read_data(INA226_ADDRESS_5, &shunt_voltage, &VOUT5, &POUT5, &IOUT5);
    ina226_read_data(INA226_ADDRESS_6, &shunt_voltage, &VOUT6, &POUT6, &IOUT6);
    
    ina226_read_data(INA226_ADDRESS_12, &shunt_voltage, &VBRN1, &PBRN1, &IBRN1);
    ina226_read_data(INA226_ADDRESS_13, &shunt_voltage, &VBRN2, &PBRN2, &IBRN2);
    
    ina226_read_data(INA226_ADDRESS_9, &shunt_voltage, &VSIN1, &PSIN1, &ISIN1);
    ina226_read_data(INA226_ADDRESS_10, &shunt_voltage, &VSIN2, &PSIN2, &ISIN2);
    ina226_read_data(INA226_ADDRESS_11, &shunt_voltage, &VSIN3, &PSIN3, &ISIN3);
}
/*
void eps_housekeeping(unsigned char *hk_bbq,unsigned char *hk_ina)
{
	unsigned char hk_bbq_in[128];
	unsigned char hk_ina_in[128];
	unsigned char buffer_bbq[32];
	unsigned char buffer_ina[32];
	unsigned char comma[4];
	int16 shunt_voltage, bus_voltage, power, current;
	unsigned int8 add;

	int j;

	bq25672_update();
	//fprintf(COM_A,"hk,");
	hk_bbq_in = "hk,";
	hk_ina_in = "";
	comma = ",";

	for (j=0x31;j<=0x45;j+=2)
	{
		bq25672_state(BQ_ADDR,buffer_bbq, j,1);
		strcat(hk_bbq_in,buffer_bbq);
		strcat(hk_bbq_in,comma);
	}
	for (j=1;j<=10;j++)
	{
		switch(j)
		{
			//3.3V Bus
			case 1:
				//pin = PIN_D1;
				add =  INA226_ADDRESS_8;
				break;
			case 2:
				//pin = PIN_D1;
				add =  INA226_ADDRESS_7;
				break;
			case 3:
				//pin = PIN_D1;
				add =  INA226_ADDRESS_1;
				break;
			case 4:
				//pin = PIN_D2;
				add =  INA226_ADDRESS_2;
				break;
			case 5:
				//pin = PIN_D3;
				add =  INA226_ADDRESS_3;
				break;
			case 6:
				//pin = PIN_D4;
				add =  INA226_ADDRESS_4;
				break;
			case 7:
				//pin = PIN_D5;
				add =  INA226_ADDRESS_5;
				break;
			case 8:
				//pin = PIN_D6;
				add =  INA226_ADDRESS_6;
				break;
			case 9:
				//pin = PIN_E8;
				add =  INA226_ADDRESS_12;
				break;
			case 10:
				//pin = PIN_E9;
				add =  INA226_ADDRESS_13;
				break;
			default:
				break;
		}
	
		ina226_read_data(add, &shunt_voltage, &bus_voltage, &power, &current);
		sprintf(buffer_ina,"%Ld,%Ld,%Ld,", bus_voltage, power, current);
		strcat(hk_ina_in,buffer_ina);
	}
	strcpy(hk_bbq,hk_bbq_in);
	strcpy(hk_ina,hk_ina_in);
	
}
*/
/*
 * Interface function, reads an incoming command.  If the command is for this
 * component (the EPS subsystem), it runs the command (somehow), after performing
 * a checksum review.  If it isn't, it passes it along to the next chain in the 
 * destination.  
 *
 * DANGER: this function is mostly deprecated.  The EPS cannot send information to
 * other components on its own.  It can only send information to the upper and
 * lower PPM, and that too, only on their terms, because both are respectively the
 * "parents" of the communication protocol, and the EPS is the "child".  
 *
 */
int8 pass_command(char* cmd) {
	if (cmd == NULL)
		return 0;
	char dest = cmd[3];
	char source = cmd[2];
	char curr = "6";    // currently in the EPS system = id 006
	if (dest == curr) {
		// what do I do if the command needs to go here?
		// do i pass it off to some other subroutine?
		// call another function?
	} else if (dest == source) {
		// this has recursively found its way back here, and this signal
		// likely needs to be squelched. 
		// OR.
		// the command originates here, and is passed through this function.
	} else if (dest == "7") {
		// send to the lower ppm (immediate EPS connection, i2c)
	} else if (dest == "3") {
		// send to the upper ppm (immediate EPS connection, i2c)
	} else if (dest == "1") {
		// send to upper ppm, which will send to AX100 and to the ground station
	} else if (dest == "2") {
		// send to upper ppm, which will send to AX100 (for some reason?)
	} else if (dest == "4") {
		// send to upper ppm, which will send to payload 1
	} else if (dest == "5") {
		// send to upper ppm, which will send to payload 2
	} else if (dest == "0") {
		// an error or contingency case, perhaps the debugging port for the
		// computer uart output
	} else {
		// utterly undefined behavior
	}
	
}