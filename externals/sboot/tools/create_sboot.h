// create_sboot.h  21-Oct-2011 Craig Milo Rogers <rogers@isi.edu>


#ifndef __CREATE_SBOOT_H
#define __CREATE_SBOOT_H

#define CHECK_UART_DIVISOR_LS_OPTION "--check-uart-divisor-low-speed"
#define CHECK_UART_DIVISOR_HS_OPTION "--check-uart-divisor-high-speed"
#define ENABLE_MAESTRO_OPTION "--enable-maestro"
#define GET_MESSAGE_TIMEOUT_OPTION "--get-message-timeout"
#define HIGH_SPEED_UART_OPTION "--high-speed"
#define NO_HIGH_SPEED_UART_OPTION "--no-high-speed"
#define LEVEL1_FILE_OPTION "--l1"
#define LEVEL2_FILE_OPTION "--l2"
#define MAESTRO_POST_RESET_DELAY_OPTION "--maestro-post-reset-delay"
#define MAESTRO_POWER_ON_DELAY_OPTION "--maestro-power-on-delay"
#define MAESTRO_UART_CONFIGURATION_DELAY_OPTION "--maestro-uart-configuration-delay"
#define NO_CHECK_UART_DIVISOR_HS_OPTION "--no-check-uart-divisor-low-speed"
#define NO_CHECK_UART_DIVISOR_LS_OPTION "--no-check-uart-divisor-high-speed"
#define NO_ENABLE_MAESTRO_OPTION "--no-enable-maestro"
#define NO_LEVEL1_FILE_OPTION "--no-l1-file"
#define NO_PRINT_PASS_OPTION "--no-print-pass"
#define NO_PRINT_RESULTS_OPTION "--no-print-results"
#define NO_RESET_BEFORE_DIVISOR_CHECK_OPTION "--no-reset-before-divisor-check"
#define NO_SKIP_PROGRAM_TIMEOUT_OPTION "--no-skip-program-timeout"
#define NO_SKIP_READ_TIMEOUT_OPTION "--no-skip-read-timeout"
#define NO_VERBOSE_OPTION "--no-verbose"
#define PRINT_PASS_OPTION "--print-pass"
#define PRINT_RESULTS_OPTION "--print-results"
#define PROGRAM_TIMEOUT_OPTION "--program-timeout"
#define READ_TIMEOUT_OPTION "--read-timeout"
#define RESET_BEFORE_DIVISOR_CHECK_OPTION "--reset-before-divisor-check"
#define RESET_SECONDS_OPTION "--reset-seconds"
#define SBOOT_FILE_OPTION "--sboot"
#define SKIP_PROGRAM_TIMEOUT_OPTION "--skip-program-timeout"
#define SKIP_READ_TIMEOUT_OPTION "--skip-read-timeout"
#define TEST_NAME_OPTION "--test-name"
#define TEST_TYPE_OPTION "--test-type"
#define VERBOSE_OPTION "--verbose"

#define CHECK_UART_DIVISOR_LS_DEFAULT      TRUE
// ***#define CHECK_UART_DIVISOR_HS_DEFAULT      FALSE
#define CHECK_UART_DIVISOR_HS_DEFAULT      TRUE
#define ENABLE_MAESTRO_DEFAULT             TRUE
#define GET_MESSAGE_TIMEOUT_DEFAULT        20 // was 180 in python code
#define HIGH_SPEED_UART_DEFAULT            TRUE // If false, set CHECK_UART_DIVISOR_HS_DEFAULT to FALSE
#define MAESTRO_POST_RESET_DELAY_DEFAULT   2
#define MAESTRO_POWER_ON_DELAY_DEFAULT     2
#define MAESTRO_UART_CONFIGURATION_DELAY_DEFAULT 2
#define PRINT_PASS_DEFAULT                 FALSE
#define PRINT_RESULTS_DEFAULT              TRUE
#define PROGRAM_TIMEOUT_DEFAULT            SBOOT_PROGRAM_TIMEOUT_DEFAULT
#define READ_TIMEOUT_DEFAULT               SBOOT_READ_TIMEOUT_DEFAULT // Was 60 in earlier code
#define RESET_BEFORE_DIVISOR_CHECK_DEFAULT TRUE
#define RESET_BEFORE_TEST_DEFAULT          FALSE
#define RESET_SECONDS_DEFAULT              1
#define SKIP_PROGRAM_TIMEOUT_DEFAULT       FALSE
#define SKIP_READ_TIMEOUT_DEFAULT          FALSE
#define VERBOSE_DEFAULT                    FALSE

#define TEST_TYPE_CRC		  "crc"
#define TEST_TYPE_L1              "l1"
#define TEST_TYPE_L1_MEM          "l1_mem"
#define TEST_TYPE_L2_1TILE        "l2_1tile"
#define TEST_TYPE_L2_ALL          "l2_all"
#define TEST_TYPE_ITC_L2_1TILE    "itc_l2_1tile"
#define TEST_TYPE_ITC_L2_TILE11   "itc_l2_tile11"
#define TEST_TYPE_ITC_L2_ALL_TILE "itc_l2_all_tile"
#define TEST_TYPE_ITC_GBE         "itc_gbe"

#endif // __CREATE_SBOOT_H                                                      
