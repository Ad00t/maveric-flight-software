#define DEVELOP
#include <24FJ256GA110.h>
#device PASS_STRINGS=IN_RAM
#include <string.h>


#fuses NOPROTECT
#fuses NOWDT

//#use delay(clock=4000000)
#use delay(clock=32MHZ, internal = 8M)
//#use rs232(baud=9600, xmit=PIN_F2, rcv=PIN_F3)
#use rs232(baud=115200, xmit=PIN_F2, rcv=PIN_F4)
#use i2c(master, sda=PIN_E7, scl=PIN_E6, STREAM=I2C_1)

//-----------------------------------------------------------
//                       PIC Registers
//-----------------------------------------------------------
#word RCON = 0x0740
#word OSCON = getenv("SFR:OSCON")
#bit IOBLOCK = OSCON.6
#word RPINR20 = getenv("SFR:RPINR20")
#word RPOR1 = getenv("SFR:RPOR1")

//#include "ina226.h"
#include "i2c.c"
#include "rm3100.c"


/*
void power_set_test(int pin, unsigned int8 add);
void power_io(unsigned int8 eps_output, unsigned int8 eps_state);
void cut_wire(unsigned int8 cut_output, unsigned int8 cut_state, unsigned int8 cut_time);
void eps_housekeeping(void);
*/
void main()
{
	/*
	unsigned char eps_hk[10]="hk";
	// ex: hk
	unsigned char eps_io[10]="io";
	// ex: io 1 1 / io 1 0
	unsigned char eps_ct[10]="ct";
	// ex: ct 1 1 5 / ct 1 0 3
	unsigned int1 bq_bit;
	*/
#ifdef DEVELOP

	do
	{	
		printf("Lower PPM USB test\n\r");
	}
	while(TRUE);

	
	/*
	unsigned int i;
	*/
#endif
/*
	bq_bit = bq25672_init();
	bq25672_update();

#ifdef DEVELOP
	printf("BQ25672 check %d \n\r",bq_bit);

	for (i=0;i<=0x48;i++)
	{
		bq25672_state(BQ_ADDR, i);
	}
	
	delay_ms(100);

	//3.3V Bus
	power_set_test(0, INA226_ADDRESS_8);
	//5V Bus
	power_set_test(0, INA226_ADDRESS_7);
	//5V Input 6
	power_set_test(PIN_D6, INA226_ADDRESS_6);
	//5V Input 5
	power_set_test(PIN_D5, INA226_ADDRESS_5);
	//5V Input 4
	power_set_test(PIN_D4, INA226_ADDRESS_4);
	//3.3V Input 3
	power_set_test(PIN_D3, INA226_ADDRESS_3);
	//3.3V Input 2
	power_set_test(PIN_D2, INA226_ADDRESS_2);
	//3.3V Input 1
	power_set_test(PIN_D1, INA226_ADDRESS_1);
	// 5V Burn circuit 1
	power_set_test(PIN_E8, INA226_ADDRESS_12);
	// 5V Burn circuit 2
	power_set_test(PIN_E9, INA226_ADDRESS_13);
#else
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

#endif

	do
	{	
		unsigned char str1[256];
		unsigned char *ptr1=str1;

		unsigned char command[256];
		unsigned char params[256];
	
		unsigned char *cmd=command;
		unsigned char *prms=params;
		
		unsigned int8 eps_output, eps_state;
		unsigned int8 cut_output, cut_state, cut_time;

		
	#ifdef DEVELOP
		printf("Enter a string\n\r");
	//#else
	//	printf("cmd\n");
	#endif
		eps_read_command(ptr1);

		delay_ms(1000);
		
		eps_get_command(str1, cmd, prms);
	#ifdef DEVELOP
		printf("%s\n\r",str1);
		printf("String:%s, Command:%s, Parameters:%s\n\r", str1, command, params);
	#endif

		if (strcmp(command, eps_io)==0)
		{
			//Switch On/Off Function
			eps_get_prmts(params, &eps_output, &eps_state);
		#ifdef DEVELOP
			printf("output command\n");
			printf("eps output:%u eps state:%u \n\r", eps_output, eps_state);
		#endif
			power_io(eps_output, eps_state);
			
		}
		else if (strcmp(command, eps_ct)==0)
		{
			//Switch On/Off Function
			
			eps_get_prmts3(params, &cut_output, &cut_state, &cut_time);
		#ifdef DEVELOP
			printf("Wire Cut command\n\r");
			printf("cut output:%u cut state:%u cut state:%u\n\r", cut_output, cut_state, cut_time);
		#endif
			cut_wire(cut_output, cut_state, cut_time);
		}
		else if (strcmp(command,eps_hk)==0)
		{
		#ifdef DEVELOP
			printf("hk command\n\r");
		#endif
			eps_housekeeping();
		}
		
		else
		{
		#ifdef DEVELOP
			printf("No command\n\r");
		#endif
		}
		//printf(command);
		//printf("Output = %d , State = %d", eps_output, eps_state);
		
	} while(TRUE);

*/
}

/*
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

			printf("CUT Output Error\n\r");
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
				printf("CUT State Error\n\r");
#else
				printf("0\n");
#endif				
				break;
		}

		
		if (dp_flag)
		{
			output_high(pin);
			delay_ms(100);
			sw_state = input(sw_pin);
#ifdef DEVELOP
			printf("Deployer Selected: %2X %2X, Switch State: %d\n\r",pin, dp_pin, sw_state);		
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
			printf("Shunt voltage: %Ld, Bus voltage: %Ld, Power: %Ld, Current: %Ld\n\r", shunt_voltage, bus_voltage, power, current);
			printf("Deployer Selected: %2X %2X, Switch State: %d\n\r",pin, dp_pin, sw_state);		
#else
			//printf(",%2X,%2X,%d\n\r",pin, dp_pin, sw_state);	
			printf("ct,%2X,%2X,%Ld,%Ld,%d\n", pin, dp_pin, bus_voltage, current, sw_state);			
#endif
		}
		
	}
}

void eps_housekeeping(void)
{
	int16 shunt_voltage, bus_voltage, power, current;
	unsigned int8 add;

	int j;

	bq25672_update();
	printf("hk,");
	for (j=0x31;j<=0x45;j+=2)
	{
		bq25672_state(BQ_ADDR, j,1);
		printf(",");
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
		printf("%Ld,%Ld,%Ld,", bus_voltage, power, current);
	}
	printf("\n");
	
}
*/