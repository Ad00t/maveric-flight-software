//////// Standard Header file for the PIC24FJ256GA110 device ////////////////
#device PIC24FJ256GA110
#nolist
//////// Program memory: 87548x24  Data RAM: 16384  Stack: 31
//////// I/O: 85   Analog Pins: 16
//////// Fuses: NOJTAG,PROTECT,WRT,DEBUG,NODEBUG,NOWDT,NOWINDIS,WDT32
//////// Fuses: WPOSTS1,WPOSTS2,WPOSTS3,WPOSTS4,WPOSTS5,WPOSTS6,WPOSTS7
//////// Fuses: WPOSTS8,WPOSTS9,WPOSTS10,WPOSTS11,WPOSTS12,WPOSTS13,WPOSTS14
//////// Fuses: WPOSTS15,WPOSTS16,NOIESO,FRC,CKSFSM,OSCIO,NOPR,HS,XT,EC
//////// Fuses: NOIOL1WAY,WPEND_HIGH,WPCFG,WPDIS,NOWPFP,ICSP1,WPFP,NOWPCFG
//////// Fuses: IOL1WAY,NOCKSFSM,FRC_PS,SOSC,PR,IESO,WINDIS,ICSP3,NOWRT,JTAG
//////// Fuses: WPEND_LOW,CKSNOFSM,PR_PLL,WDT128,ICSP2,NOOSCIO,FRC_PLL
//////// Fuses: NOPROTECT,LPRC,WDT,NOWPDIS
//////// 
////////////////////////////////////////////////////////////////// I/O
// Discrete I/O Functions: SET_TRIS_x(), OUTPUT_x(), INPUT_x(),
//                         SET_PULLUP(), INPUT(),
//                         OUTPUT_LOW(), OUTPUT_HIGH(),
//                         OUTPUT_FLOAT(), OUTPUT_BIT()
// Constants used to identify pins in the above are:

#define PIN_A0  5648
#define PIN_A1  5649
#define PIN_A2  5650
#define PIN_A3  5651
#define PIN_A4  5652
#define PIN_A5  5653
#define PIN_A6  5654
#define PIN_A7  5655
#define PIN_A9  5657
#define PIN_A10  5658
#define PIN_A14  5662
#define PIN_A15  5663

#define PIN_B0  5712
#define PIN_B1  5713
#define PIN_B2  5714
#define PIN_B3  5715
#define PIN_B4  5716
#define PIN_B5  5717
#define PIN_B6  5718
#define PIN_B7  5719
#define PIN_B8  5720
#define PIN_B9  5721
#define PIN_B10  5722
#define PIN_B11  5723
#define PIN_B12  5724
#define PIN_B13  5725
#define PIN_B14  5726
#define PIN_B15  5727

#define PIN_C1  5777
#define PIN_C2  5778
#define PIN_C3  5779
#define PIN_C4  5780
#define PIN_C12  5788
#define PIN_C13  5789
#define PIN_C14  5790
#define PIN_C15  5791

#define PIN_D0  5840
#define PIN_D1  5841
#define PIN_D2  5842
#define PIN_D3  5843
#define PIN_D4  5844
#define PIN_D5  5845
#define PIN_D6  5846
#define PIN_D7  5847
#define PIN_D8  5848
#define PIN_D9  5849
#define PIN_D10  5850
#define PIN_D11  5851
#define PIN_D12  5852
#define PIN_D13  5853
#define PIN_D14  5854
#define PIN_D15  5855

#define PIN_E0  5904
#define PIN_E1  5905
#define PIN_E2  5906
#define PIN_E3  5907
#define PIN_E4  5908
#define PIN_E5  5909
#define PIN_E6  5910
#define PIN_E7  5911
#define PIN_E8  5912
#define PIN_E9  5913

#define PIN_F0  5968
#define PIN_F1  5969
#define PIN_F2  5970
#define PIN_F3  5971
#define PIN_F4  5972
#define PIN_F5  5973
#define PIN_F6  5974
#define PIN_F7  5975
#define PIN_F8  5976
#define PIN_F12  5980
#define PIN_F13  5981

#define PIN_G0  6032
#define PIN_G1  6033
#define PIN_G2  6034
#define PIN_G3  6035
#define PIN_G6  6038
#define PIN_G7  6039
#define PIN_G8  6040
#define PIN_G9  6041
#define PIN_G12  6044
#define PIN_G13  6045
#define PIN_G14  6046
#define PIN_G15  6047

////////////////////////////////////////////////////////////////// Useful defines
#define FALSE 0
#define TRUE 1

#define BYTE unsigned int8
#define BOOLEAN int1

#define getc getch
#define fgetc getch
#define getchar getch
#define putc putchar
#define fputc putchar
#define fgets gets
#define fputs puts

////////////////////////////////////////////////////////////////// UART
// Constants used in setup_uart() are:
// FALSE - Turn UART off
// TRUE  - Turn UART on
#define UART_ADDRESS           2
#define UART_DATA              4
#define UART_AUTODETECT        8
#define UART_AUTODETECT_NOWAIT 9
#define UART_WAKEUP_ON_RDA     10
#define UART_SEND_BREAK        13


////////////////////////////////////////////////////////////////// WDT
// Watch Dog Timer Functions: SETUP_WDT() and RESTART_WDT()
//
// Constants used for SETUP_WDT() are:
#define WDT_ON      1
#define WDT_OFF     0

////////////////////////////////////////////////////////////////// Control
// Control Functions:  RESET_CPU(), SLEEP(), RESTART_CAUSE()
// Constants passed into RESTART_CAUSE() are:
#define RESTART_POWER_UP      0
#define RESTART_BROWNOUT      1
#define RESTART_WATCHDOG      4
#define RESTART_SOFTWARE      6
#define RESTART_MCLR          7
#define RESTART_ILLEGAL_OP    14
#define RESTART_TRAP_CONFLICT 15
//
// Constants passed into SLEEP() are:
#define SLEEP_FULL      0  // Default
#define SLEEP_IDLE      1  // Clock and peripherals don't stop

////////////////////////////////////////////////////////////////// INTERNAL RC
// Constants used in setup_oscillator() are:
#define OSC_INTERNAL     32
#define OSC_CRYSTAL       1
#define OSC_CLOCK         2
#define OSC_RC            3
#define OSC_SECONDARY    16

////////////////////////////////////////////////////////////////// Timer
// Timer Functions: SETUP_TIMERx, GET_TIMERx, GET_TIMERxy,
// SET_TIMERx, SET_TIMERxy
// Constants used for SETUP_TIMERx() are:
//      (or (via |) together constants from each group)
#define TMR_DISABLED 0x0000
#define TMR_INTERNAL 0xA000
#define TMR_EXTERNAL 0xA002
#define TMR_GATE     0x0040 
#define T1_EXTERNAL_SYNC 0xA006 //This only applies to Timer1
#define T1_EXTERNAL_RTC  0xC002 //This only applies to Timer1

#define TMR_DIV_BY_1   0x0000
#define TMR_DIV_BY_8   0x0010
#define TMR_DIV_BY_64  0x0020
#define TMR_DIV_BY_256 0x0030
#define TMR_32_BIT     0x0008 // Only for even numbered timers

/////////////////////////////////////////////////////////// INPUT CAPTURE
// Functions: SETUP_CAPTURE, GET_CAPTURE,
//
// Constants used for SETUP_CAPTURE() are:
#define CAPTURE_OFF               0x0000  // Capture OFF
#define CAPTURE_EE                0x0001  // Capture Every Edge
#define CAPTURE_FE                0x0002  // Capture Falling Edge
#define CAPTURE_RE                0x0003  // Capture Rising Edge
#define CAPTURE_DIV_4             0x0004  // Capture Every 4th Rising Edge
#define CAPTURE_DIV_16            0x0005  // Capture Every 16th Rising Edge
#define CAPTURE_INTERRUPT_ONLY    0x0007  // Interrupt on Rising Edge when in Sleep or Idle

// The following defines can be ORed | with above to configure interrupts
#define INTERRUPT_EVERY_CAPTURE   0x0000  // Interrupt on every capture event
#define INTERRUPT_SECOND_CAPTURE  0x0020  // Interrupt on every second capture event
#define INTERRUPT_THIRD_CAPTURE   0x0040  // Interrupt on every third capture event
#define INTERRUPT_FOURTH_CAPTURE  0x0060  // Interrupt on every fourth capture event

// The following defines can be ORed | with above to select timer
#define CAPTURE_TIMER2            0x0080  // On capture event Timer 2 is captured
#define CAPTURE_TIMER3            0x0000  // On capture event Timer 3 is captured

// The following defines can be ORed | with above to select idle operation mode
#define CAPTURE_HALT_IDLE         0x2000  // Capture module halts during idle mode
#define CAPTURE_CONTINUE_IDLE     0x0000  // Capture module continues during idle mode

/////////////////////////////////////////////////////////// OUTPUT COMPARE
// Functions: SETUP_COMPARE, SET_PWM_DUTY, SET_COMPARE_TIME
//
// Constants used for SETUP_COMPARE() are:
#define COMPARE_OFF               0x0000  // Compare OFF
#define COMPARE_SET_ON_MATCH      0x0001  // Pin from low to high on match
#define COMPARE_CLR_ON_MATCH      0x0002  // Pin from high to low on match
#define COMPARE_TOGGEL            0x0003  // Pin will toggle on every match occurrence
#define COMPARE_SINGLE_PULSE      0x0004  // Pin will generate single pulse on first match
#define COMPARE_CONT_PULSE        0x0005  // Pin will pulse for every match
#define COMPARE_PWM_EDGE          0x0006
#define COMPARE_PWM_CENTER        0x0007

// The following defines can be ORed | with above
#define COMPARE_TIMER2	      	  0x0000
#define COMPARE_TIMER3		  0x0400
#define COMPARE_TIMER4		  0x0800
#define COMPARE_TIMER5		  0x0C00
#define COMPARE_TIMER1		  0x1000
#define COMPARE_SYSTEM_CLOCK	  0x1C00

#define COMPARE_FAULT0		0x0080
#define COMPARE_NO_FAULT0		0x0000

#define COMPARE_FAULT_CLEAR		0x80000000
#define COMPARE_FAULT_NEW		0x00000000
#define COMPARE_PWM_HIGH_ON_FAULT	0x40000000
#define COMPART_PWM_LOW_ON_FAULT	0x00000000
#define COMPARE_FAULT_OUTPUT		0x20000000
#define COMPARE_INVERT_OC_PIN		0x10000000
#define COMPARE_32_BIT			0x01000000
#define COMPARE_TRIGGER			0x00800000
#define COMPARE_SYNCHRONIZE		0x00000000
#define COMPARE_OC_PIN_TRISTATED	0x00200000
#define COMPARE_TRIG_SYNC_SELF		0x001F0000
#define COMPARE_TRIG_SYNC_CAP9		0x001E0000
#define COMPARE_TRIG_SYNC_CAP6		0x001D0000
#define COMPARE_TRIG_SYNC_CTMU		0x001C0000
#define COMPARE_TRIG_SYNC_ADC		0x001B0000
#define COMPARE_TRIG_SYNC_COMP3	        0x001A0000
#define COMPARE_TRIG_SYNC_COMP2	        0x00190000
#define COMPARE_TRIG_SYNC_COMP1	        0x00180000
#define COMPARE_TRIG_SYNC_CAP4		0x00170000
#define COMPARE_TRIG_SYNC_CAP3		0x00160000
#define COMPARE_TRIG_SYNC_CAP2		0x00150000
#define COMPARE_TRIG_SYNC_CAP1		0x00140000
#define COMPARE_TRIG_SYNC_CAP8		0x00130000
#define COMPARE_TRIG_SYNC_CAP7		0x00120000
#define COMPARE_TRIG_SYNC_TIMER5	0x000F0000
#define COMPARE_TRIG_SYNC_TIMER4	0x000E0000
#define COMPARE_TRIG_SYNC_TIMER3	0x000D0000
#define COMPARE_TRIG_SYNC_TIMER2	0x000C0000
#define COMPARE_TRIG_SYNC_TIMER1	0x000B0000
#define COMPARE_TRIG_SYNC_CAP5		0x000A0000
#define COMPARE_TRIG_SYNC_OC9		0x00090000
#define COMPARE_TRIG_SYNC_OC8		0x00080000
#define COMPARE_TRIG_SYNC_OC7		0x00070000
#define COMPARE_TRIG_SYNC_OC6		0x00060000
#define COMPARE_TRIG_SYNC_OC5		0x00050000
#define COMPARE_TRIG_SYNC_OC4		0x00040000
#define COMPARE_TRIG_SYNC_OC3		0x00030000
#define COMPARE_TRIG_SYNC_OC2		0x00020000
#define COMPARE_TRIG_SYNC_OC1		0x00010000
#define COMPARE_TRIG_SYNC_NONE		0x00000000

// The following defines can be ORed | with above to select idle operation mode
#define COMPARE_HALT_IDLE         0x2000  // Compare module halts during idle mode
#define COMPARE_CONTINUE_IDLE     0x0000  // Compare module continues during idle mode


////////////////////////////////////////////////////////////////// SPI
// SPI Functions: SETUP_SPI, SPI_WRITE, SPI_READ, SPI_DATA_IN
//
// Constants used in SETUP_SPI() are:
//      (or (via |) together constants from each group)
#define SPI_MASTER                 0x0020
#define SPI_SLAVE                  0x0000

#define SPI_SCK_IDLE_HIGH          0x0040
#define SPI_SCK_IDLE_LOW           0x0000

#define SPI_XMIT_IDLE_TO_ACTIVE    0x0100
#define SPI_XMIT_ACTIVE_TO_IDLE    0x0000

#define SPI_MODE_16B               0x0400
#define SPI_MODE_8B                0x0000

#define SPI_SAMPLE_AT_END          0x0200
#define SPI_SAMPLE_AT_MIDDLE       0x0000

//or (via |) one of the following when operating as MASTER 
#define SPI_CLK_DIV_1              0x001F
#define SPI_CLK_DIV_2              0x001B
#define SPI_CLK_DIV_3              0x0017
#define SPI_CLK_DIV_4              0x001E
#define SPI_CLK_DIV_5              0x000F
#define SPI_CLK_DIV_6              0x000B
#define SPI_CLK_DIV_7              0x0007
#define SPI_CLK_DIV_8              0x0003
#define SPI_CLK_DIV_12             0x0016
#define SPI_CLK_DIV_16             0x001D
#define SPI_CLK_DIV_20             0x000E
#define SPI_CLK_DIV_24             0x000A
#define SPI_CLK_DIV_28             0x0006
#define SPI_CLK_DIV_32             0x0002
#define SPI_CLK_DIV_48             0x0015
#define SPI_CLK_DIV_64             0x001C
#define SPI_CLK_DIV_80             0x000D
#define SPI_CLK_DIV_96             0x0009
#define SPI_CLK_DIV_112            0x0005
#define SPI_CLK_DIV_128            0x0001
#define SPI_CLK_DIV_192            0x0014
#define SPI_CLK_DIV_256            0x0010
#define SPI_CLK_DIV_320            0x000C
#define SPI_CLK_DIV_384            0x0008
#define SPI_CLK_DIV_448            0x0004
#define SPI_CLK_DIV_512            0x0000

//or (via |) one of the following when operating as SLAVE 
#define SPI_SS_ENABLED             0x0080
#define SPI_SS_DISABLED            0x0000

//The following defines are provided for compatibility
#define SPI_L_TO_H         SPI_SCK_IDLE_LOW
#define SPI_H_TO_L         SPI_SCK_IDLE_HIGH
#define SPI_XMIT_L_TO_H    SPI_XMIT_IDLE_TO_ACTIVE
#define SPI_XMIT_H_TO_L    SPI_XMIT_ACTIVE_TO_IDLE


////////////////////////////////////////////////////////////////// ADC
// ADC Functions: SETUP_ADC(), SETUP_ADC_PORTS()
//                SET_ADC_CHANNEL(), READ_ADC()
//
// Constants used for SETUP_ADC() are:
// Clock is at ADCS<5:0> of ADCON3 Reg. (0x02A4)
// Tad = (Tcy/2)*(ADCS<5:0>+1)
#define ADC_OFF           0x10000
#define ADC_CLOCK          0x0000     // External
#define ADC_CLOCK_DIV_2    0x0001
#define ADC_CLOCK_DIV_4    0x0003
#define ADC_CLOCK_DIV_8    0x0007
#define ADC_CLOCK_DIV_16   0x000F
#define ADC_CLOCK_DIV_32   0x001F
#define ADC_CLOCK_DIV_64   0x007F   
#define ADC_CLOCK_DIV_128  0x00FF   
#define ADC_CLOCK_INTERNAL 0x8000     // Internal

// The following may be OR'ed in with the above using |
// Auto-Sample Time bits
#define ADC_TAD_MUL_0      0x0000
#define ADC_TAD_MUL_2      0x0200
#define ADC_TAD_MUL_4      0x0400
#define ADC_TAD_MUL_8      0x0800
#define ADC_TAD_MUL_16     0x1000
#define ADC_TAD_MUL_31     0x1F00

// Constants used in READ_ADC() are:
#define ADC_START_AND_READ 0x07
#define ADC_START_ONLY     0x01
#define ADC_READ_ONLY      0x06



// Constants used in SETUP_ADC_PORTS() are:
// First argument:
// OR together desired pins
#define NO_ANALOGS      0           // None
#define ALL_ANALOG      0xFFFFFFFF  // All
#define sAN0      0x00000001 //| B0
#define sAN1      0x00000002 //| B1
#define sAN2      0x00000004 //| B2
#define sAN3      0x00000008 //| B3
#define sAN4      0x00000010 //| B4
#define sAN5      0x00000020 //| B5
#define sAN6      0x00000040 //| B6
#define sAN7      0x00000080 //| B7
#define sAN8      0x00000100 //| B8
#define sAN9      0x00000200 //| B9
#define sAN10      0x00000400 //| B10
#define sAN11      0x00000800 //| B11
#define sAN12      0x00001000 //| B12
#define sAN13      0x00002000 //| B13
#define sAN14      0x00004000 //| B14
#define sAN15      0x00008000 //| 15
#define sAN16      0x00010000 //| C1
#define sAN17      0x00020000 //| C2
#define sAN18      0x00040000 //| C3
#define sAN19      0x00080000 //| 4
#define sAN20      0x00100000 //| A12
#define sAN21      0x00200000 //| A13
#define sAN22      0x00400000 //| A6
#define sAN23      0x00800000 //| A7
#define sAN24      0x01000000 //| E0
#define sAN25      0x02000000 //| E1
#define sAN26      0x04000000 //| E2
#define sAN27      0x08000000 //| E3
#define sAN28      0x10000000 //| E4
#define sAN29      0x20000000 //| E5
#define sAN30      0x40000000 //| E6
#define sAN31      0x80000000 //| 7

// Optional Second argument:
#define VSS_VDD            0x0000 // Range 0-Vdd
#define VREF_VREF          0x6000 // Range VrefL-VrefH
#define VREF_VDD           0x4000 // Range VrefL-Vdd
#define VSS_VREF           0x2000 // Range 0-VrefH

////////////////////////////////////////////////////////////////// COMP
// setup_comparator() first param is the comparator number.
// Constants used in second setup_comparator() param are:
#define NC_NC            0x00
#define CXINB_CXINA      0x8000
#define CXINC_CXINA      0x8001
#define CXIND_CXINA      0x8002
#define VBG2_CXINA       0x8003
#define CXINB_VREF       0x8010
#define CXINC_VREF       0x8011
#define CXIND_VREF       0x8012
#define VBG2_VREF        0x8013
      // OR in any of the following
#define COMP_INVERT      0x2000   // Invert output
#define COMP_OUTPUT      0x4000   // Output on pin
#define COMP_INTR        0x0040   // Generate interrupt on high
#define COMP_INTR_INV    0x0080   // Generate interrupt on low
#define COMP_INTR_CHANGE 0x00c0   // Generate interrupt on change

#bit C1OUT = 0x634.8
#bit C1ENV = 0x634.9  // User must clear
#bit C2OUT = 0x636.8
#bit C2ENV = 0x636.9  // User must clear
#bit C3OUT = 0x638.8
#bit C3ENV = 0x638.9  // User must clear

////////////////////////////////////////////////////////////////// VREF
// Constants used in setup_vref() are:
//
#define VREF_DISABLED  0x0000
#define VREF_ANALOG    0x0010
#define VREF_HIGH      0x1000
#define VREF_LOW       0x1020
// Or (with |) the above with a number 0-15

////////////////////////////////////////////////////////////////// PMP
// SPI Functions: setup_pmp(), pmp_address(), psp_read(), pmp_read(),
// psp_write(), pmp_write(), psp_output_full(),psp_input_full(),
// psp_overflow(), pmp_output_full(), pmp_input_full(),
// pmp_overflow()
// Constants used in SETUP_PMP() and SETUP_PSP() are:
#define PAR_ENABLE                0x8000
#define PAR_DISABLE               0x0000    // Module enable/disable options
#define PAR_STOP_IN_IDLE          0x2000
#define PAR_CONTINUE_IN_IDLE      0x0000
#define PAR_ADDR_NOT_MULTIPLEXED  0x0000    // Address multiplexing options
#define PAR_LOW_ADDR_MULTIPLEXED  0x0800    // Address multiplexing options
#define PAR_FULL_ADDR_MULTIPLEXED 0x1000    // Address multiplexing options
#define PAR_PTBEEN_ENABLE         0x0400  // Byte Enable Port Enable bit
#define PAR_PTWREN_ENABLE         0x0200  // Write Enable Strobe bit
#define PAR_PTRDEN_ENABLE         0x0100  // Read/Write Strobe Port bit
#define PAR_ALP_ACTIVE_HIGH       0x0020 // Address latch polarity high
#define PAR_BEP_ACTIVE_HIGH       0x0004 // Byte Enable Polarity
#define PAR_WRSP_ACTIVE_HIGH      0x0002 // Write strobe Polarity bit
#define PAR_RDSP_ACTIVE_HIGH      0x0001 // Read strobe Polarity bit
#define PAR_CS_XX                 0x0000   // Chip select pins used for address
#define PAR_CS_X1                 0x0008   // Chip select pin 1 used, active high
#define PAR_CS_X0                 0x0000   // Chip select pin 1 used, active low
#define PAR_CS_1X                 0x0050   // Chip select pin 2 used, active high
#define PAR_CS_0X                 0x0040   // Chip select pin 2 used, active low
#define PAR_CS_00                 0x0080   // Chip select pins 1,2 used, active low
#define PAR_CS_11                 0x0098   // Chip select pins 1,2 used, active high
#define PAR_CS_10                 0x0090   // Chip select pins 1,2 used, 1 is active low, 2 is high
#define PAR_CS_01                 0x0088   // Chip select pins 1,2 used, 1 is active high, 2 is low
#define PAR_INTR_ON_RW                0x20000000  // Interrupt on read write
#define PAR_NO_INTR_STALL_ACTIVATED   0x40000000  // No interrupt, processor stall activated
#define PAR_INTR_ON_3_RW_BUF          0x60000000 // Interrupt on write to Buffer 3 or read from Buffer 3
#define PAR_PSP_AUTO_INC              0x18000000 // Read write buffers auto increment
#define PAR_DEC_ADDR                  0x10000000 // Increment the address
#define PAR_INC_ADDR                  0x08000000 // Decrement the address
#define PAR_MASTER_MODE_1             0x03000000  // Master mode 1
#define PAR_MASTER_MODE_2             0x02000000  // Master mode 2
#define PAR_WAITB1                    0x00000000 // 1 Tcy Wait state for data setup R/W
#define PAR_WAITB2                    0x00400000 // 2 Tcy Wait state for data setup R/W
#define PAR_WAITB3                    0x00800000 // 3 Tcy Wait state for data setup R/W
#define PAR_WAITB4                    0x00C00000 // 4 Tcy Wait state for data setup R/W
#define PAR_WAITM0                    0x00000000 // 0 wait state for Read to byte
#define PAR_WAITM1                    0x00040000 // 1 Tcy wait state for Read to byte
#define PAR_WAITM2                    0x00080000 // 2 Tcy wait state for Read to byte
#define PAR_WAITM3                    0x000C0000 // 3 Tcy wait state for Read to byte
#define PAR_WAITM15                   0x003C0000 // 15 wait states
#define PAR_WAITE1                    0x00000000  // 1 Tcy Wait for data hold after strobe
#define PAR_WAITE2                    0x00010000 // 2 Tcy Wait for data hold after strobe
#define PAR_WAITE3                    0x00020000 // 3 Tcy Wait for data hold after strobe
#define PAR_WAITE4                    0x00030000 // 4 Tcy Wait for data hold after strobe

////////////////////////////////////////////////////////////////// CRC
// CRC Functions: setup_crc(), crc_init(), crc_calc(), crc_calc8()
//

////////////////////////////////////////////////////////////////// RTC
// RTC Functions: setup_rtc(), setup_rtc_alarm(), rtc_read(),
//                rtc_write(), rtc_alarm_read(), rtc_alarm_write()
// Structure used in read and write functions (pass pointer):
typedef struct {
   int8 tm_year;
   int8 tm_yday;  // Not used by built in functions
   int8 tm_mday;
   int8 tm_mon;
   int8 tm_hour;
   int8 tm_wday;
   int8 tm_sec;
   int8 tm_min;
   int8 tm_isdst; // Not used by built in functions
} rtc_time_t;
//
// Constants used in setup_rtc() are:  // Second param is calibration
#define RTC_ENABLE           0x8000
#define RTC_DISABLE          0
#define RTC_OUTPUT_SECONDS   0x20400
#define RTC_OUTPUT_ALARM     0x00400
//
// Constants used in setup_rtc_alarm() first param are:  
#define RTC_ALARM_ENABLE     0x8000
#define RTC_ALARM_DISABLE    0
#define RTC_CHIME_ENABLE     0x4000
#define RTC_CHIME_DISABLE    0
// Constants used in setup_rtc_alarm() second param are:  // Third param is repeat#
#define RTC_ALARM_HALFSECOND 0x0000
#define RTC_ALARM_SECOND     0x0400
#define RTC_ALARM_10_SECONDS 0x0800
#define RTC_ALARM_MINUTE     0x0C00
#define RTC_ALARM_10_MINUTES 0x1000
#define RTC_ALARM_HOUR       0x1400
#define RTC_ALARM_DAY        0x1800
#define RTC_ALARM_WEEK       0x1C00
#define RTC_ALARM_MONTH      0x2000
#define RTC_ALARM_YEAR       0x2400

////////////////////////////////////////////////////////////////// INT
// Interrupt Functions: ENABLE_INTERRUPTS(), DISABLE_INTERRUPTS(),
//                      CLEAR_INTERRUPT(), INTERRUPT_ACTIVE(),
//                      EXT_INT_EDGE()
//
// Constants used in EXT_INT_EDGE() are:
#define L_TO_H              0x40
#define H_TO_L                 0
//
// Constants used in other interrupt functions are:
#define INTR_GLOBAL               0x400
#define INTR_NORMAL               0x100
#define INTR_ALTERNATE            0x200
#define INTR_LEVEL0               0x500
#define INTR_LEVEL1               0x501
#define INTR_LEVEL2               0x502
#define INTR_LEVEL3               0x503
#define INTR_LEVEL4               0x504
#define INTR_LEVEL5               0x505
#define INTR_LEVEL6               0x506
#define INTR_LEVEL7               0x507

#define INTR_CN_PIN              0x8000  // or in a PIN_xx constant

#define INT_OSCFAIL               1
#define INT_ADDRERR               2
#define INT_STACKERR              3
#define INT_MATHERR               4
#define INT_EXT0                  6
#define INT_IC1                   7
#define INT_OC1                   8
#define INT_TIMER1                9
#define INT_IC2                   70
#define INT_OC2                   71
#define INT_TIMER2                72
#define INT_TIMER3                73
#define INT_SPI1E                 74
#define INT_SPI1                  75
#define INT_RDA                   76
#define INT_TBE                   77
#define INT_ADC1                  78
#define INT_SI2C                  81
#define INT_MI2C                  82
#define INT_CNI                   84
#define INT_EXT1                  85
#define INT_IC7                   87
#define INT_IC8                   88
#define INT_OC3                   90
#define INT_OC4                   91
#define INT_TIMER4                92
#define INT_TIMER5                93
#define INT_EXT2                  94
#define INT_RDA2                  95
#define INT_TBE2                  96
#define INT_SPI2E                 97
#define INT_SPI2                  98
#define INT_IC3                   102
#define INT_IC4                   103
#define INT_IC5                   104
#define INT_IC6                   105
#define INT_OC5                   106
#define INT_OC6                   107
#define INT_OC7                   108
#define INT_OC8                   109
#define INT_SI2C2                 114
#define INT_MI2C2                 115
#define INT_EXT3                  118
#define INT_EXT4                  119
#define INT_UART1E                130
#define INT_UART2E                131
#define INT_SPI3                  137
#define INT_UART3E                141
#define INT_RDA3                  142
#define INT_TBE3                  143
#define INT_UART4E                144
#define INT_RDA4                  145
#define INT_TBE4                  146
#define INT_SI2C3                 148
#define INT_MI2C3                 149
#define INT_COMP                  152
#define INT_OC9                   162
#define INT_IC9                   163
#define INT_PMP                   164
#define INT_SPI3E                 172
#define INT_RTC                   173
#define INT_CRC                   174
#define INT_CTMU                  176

#list
