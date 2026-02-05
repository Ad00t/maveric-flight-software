/////////////////////////////////////////////////////////////////////////
////                            EX_RTCC.C                            ////
////                                                                 ////
////  This program reads and writes to the Internal Real-Time Clock  ////
////  and Calendar (RTCC).  It communicates to the user over the     ////
////  RS-232 interface.                                              ////
////                                                                 ////
////  This example works with the PCD and PCH compilers on PICs that ////
////  have a built in RTCC. The following conditional compilation    //// 
////  lines are used to include a valid device for each compiler.    //// 
////  Change the device, clock and RS232 pins for your hardware if   //// 
////  needed.                                                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <24FJ256GA110.h>
//========================================
//  		       fuses
//========================================

// JTAG Choices: JTAG,NOJTAG
#fuses JTAG

// Code Protection Choices: PROTECT,NOPROTECT
#fuses NOPROTECT					// Code not protected from reading

// Power up timer choices?? WRT,NOWRT
#fuses WRT						

// Debug Choices: DEBUG,NODEBUG
#fuses NODEBUG

// In-Circuit Serial Programming Choices: ICSP1,ICSP2,ICSP3

// Watchdog Timer Choices: WDT,NOWDT
#fuses NOWDT							// Enable Watch Dog Timer

// Windowed Watchdog Timer Disable Big
//#fuses WINDIS						// Windowed Watchdog Timer DIsable Bit ON
//#fuses NOWINDIS					// Windowed Watchdog Timer DIsable Bit OFF

// Watchdog Prescaler Choices: WPRES128,WPRES32

// Watchdog Postscaler Choices: WPOSTS1,WPOSTS2,WPOSTS3,WPOSTS4,WPOSTS5,WPOSTS6,WPOSTS7
//								WPOSTS8,WPOSTS9,WPOSTS10,WPOSTS11,WPOSTS12,WPOSTS13,WPOSTS14
//								WPOSTS15,WPOSTS16
#fuses WPOSTS12						// Watchdog Postscaler.  
									// At current processor settings: WPOSTS12 = ~10 seconds
									//								  WPOSTS13 = ~20 seconds
									//								  default  = ~120 seconds

// Internal External Switchoff Bit Choices: IESO,NOIESO,
#fuses IESO						// Internal External SwitchOff Bit ON
//#fuses NOIESO						// Internal External SwitchOff Bit OFF

// FNOSC Oscillator Select Choices: FRC,FRC_PLL,PR,PR_PLL,SOSC,LPRC,FRC_PS
//#fuses FRC_PLL						// Internal Fast RC Oscillator with Phase Lock Loop gives 32 MHz
#fuses SOSC	//FRC and EC work

// Primary Oscialltor Choices: NOPR,HS,XT,EC
#fuses NOPR

// Clock Failsafe Monitor Choices: CKSFSM,CKSNOFSM
#fuses CKSNOFSM

// OSCO Pin Configuration Bit
//OSCIO,NOOSCIO
#fuses NOOSCIO

// Input/Out Lock Enable Bit Choices: IOL1WAY,NOIOL1WAY
// Allows the ioclock bit to be set once or multiple times
#fuses NOIOL1WAY

// OThers:
//WPEND_LOW,WPEND_HIGH,WPCFG
//////// Fuses: NOWPCFG,WPDIS,NOWPDIS,NOWPFP
#use delay(clock=8M)
#pin_select U1TX = PIN_F3
#pin_select U1RX = PIN_F2
#use rs232(baud=57600,UART1,STREAM=COM_A)

int8 get_number() 
{
  unsigned char first,second;

  do {
    first=getc();
  } while ((first<'0') || (first>'9'));
  putc(first);
  first-='0';

  do {
    second=getc();
  } while (((second<'0') || (second>'9')) && (second!='\r'));
  putc(second);

  if(second=='\r')
    return(first);
  else
    return((first*10)+(second-'0'));
}

void set_clock(rtc_time_t &date_time)
{
   printf("\r\nPress ENTER after 1 digit answers.");
   printf("\r\nYear 20: ");
   date_time.tm_year=get_number();
   printf("\r\nMonth: ");
   date_time.tm_mon=get_number();
   printf("\r\nDay: ");
   date_time.tm_mday=get_number();
   printf("\r\nWeekday 1-7: ");
   date_time.tm_wday=get_number();
   printf("\r\nHour: ");
   date_time.tm_hour=get_number();
   printf("\r\nMin: ");
   date_time.tm_min=get_number();
   date_time.tm_sec=0;

   printf("\r\n\n");
}


void main()
{
   rtc_time_t write_clock, read_clock;

   //setup_oscillator(OSC_CLOCK|DOZE_DIV_1,);
   
   setup_rtc(RTC_ENABLE,0);         //enables internal RTCC
   
   read_clock.tm_year=10;
   read_clock.tm_mon=4;
   read_clock.tm_mday=17;
   read_clock.tm_wday=3;
   read_clock.tm_hour=3;
   read_clock.tm_min=28;
   read_clock.tm_sec=0;

   //set_clock(write_clock);

   rtc_write(&read_clock);         //writes new clock setting to RTCC
   
   printf("\r\n\n\n\n");
   printf("\r\n================================================="); 
   printf("\r\n=              CLOCK   TESTING                  =");
   printf("\r\n================================================="); 

   while(1)
   {
      rtc_read(&read_clock);        //reads clock value from RTCC
      printf("\r\n%02u/%02u/20%02u %02u:%02u:%02u",read_clock.tm_mon,read_clock.tm_mday,read_clock.tm_year,read_clock.tm_hour,read_clock.tm_min,read_clock.tm_sec);
      delay_ms(1000);
	  //fputc('g',COM_A);
   }
}
