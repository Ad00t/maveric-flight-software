/***************************************************************************

									adc.c

Company: USC/ISI
License: Proprietary
Author(s): Michael Aherne (mra)

Purpose: To provide Analog-To-Digital drivers

Changelog:

Date 	 |  Au.  |	Notes
07-19-10	mra		Created.

****************************************************************************
*/

#include <adc.h>

#module

float convertToCelcius(float voltage);
float convertToVoltage(int raw);

//==============================================
//  				Functions
//==============================================
int read_adc_channel(unsigned char channel)
{
	int retval;
	
	set_adc_channel(channel);
	delay_us(20);
	read_adc(ADC_START_ONLY);
	while(!adc_done()){}; // wait for ADC to be done
	retval = read_adc(ADC_READ_ONLY);
	return retval;
}

void collecTemperatures(float * temps)
{
	int channels[4] = {RTD1_CHAN,RTD2_CHAN,RTD3_CHAN,RTD4_CHAN};
	int i;

	for(i=0;i<4;i++)
	{
		temps[i] = convertToCelcius(convertToVoltage(read_adc_channel(channels[i])));
	} 
	
	return;
}

float convertToCelcius(float voltage)
{

	return (998.67*voltage - 827.54);	

//	return MINIMUM_TEMPERATURE +  * TEMPERATURE_RANGE;
}


// This conversion assumes that the upper 10 bits of the
// 16-bit integer contain the data.
// A value of 0x3FF0 -- 1111111111000000 -- is maximum
// 
float convertToVoltage(int raw)
{
	raw = raw>>6;

	return (float)raw/MAX_ADC_READING * VOLTAGE_RANGE + VOLTAGE_MINIMUM;
}


/*
void collectSenses(float * upper, float * lower)
{
	upper = convertToVoltave(read_adc_channel(SENSE_UPPER));
	lower = convertToVoltage(read_adc_channel(SENSE_LOWER));
	return;
}
*/
#ifndef FLIGHT
void ADCTest(int port)
{
	// Tests the ADC on pins: AN2, 8, 9, 10, 11
//	const unsigned char pins[5]={2,8,9,10,11};
	//const unsigned char pins[5]={8,8,8,8,8};
	int i;
	int adctest;
	float adcvolts;
	float adctemp;
	unsigned char temp;
	
	clrscrn(USER_PORT);
	locxy(USER_PORT,0,0);
	sendDBGALL(USER_PORT,"\r\n      ADC Test");	
	sendDBGALL(USER_PORT,"\r\n--------------------");
	disable_interrupts(INTR_GLOBAL);	
	for(i=8;i<12;i++)
	{
		//temp = pins[i];
		temp = i;
		set_adc_channel(temp);
		delay_us(20);
//		read_adc(ADC_START_ONLY);
//		while(!adc_done()){}; // wait for ADC to be done
		adctest = read_adc();
		while(!adc_done()){}; // wait for ADC to be done
		adcvolts = convertToVoltage(adctest);
		sprintf(dbgbuf,"\r\nADC%02i = %LX --- ",temp,adctest);
		sendDBGALL(USER_PORT,dbgbuf);
		byteMe(adctest);
		sprintf(dbgbuf," - %d/1023",(adctest>>6));
		sendDBGALL(USER_PORT,dbgbuf);
		sprintf(dbgbuf," - %6.4f V",adcvolts);		
		sendDBGALL(USER_PORT,dbgbuf);
		adctemp=convertToCelcius(adcvolts);
		sprintf(dbgbuf," - %6.4f deg C",adctemp);		
		sendDBGALL(USER_PORT,dbgbuf);
	}
	enable_interrupts(INTR_GLOBAL);
	//sendMSGALL(USER_PORT,"\r\n\n");	
}
#endif

