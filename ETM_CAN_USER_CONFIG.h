#ifndef __ETM_CAN_USER_CONFIG_H
#define __ETM_CAN_USER_CONFIG_H

#include "ETM_CAN_BOARD_SELECT.h"

#ifdef __A36444
#define __USE_CAN_1
#define __USE_EXTERNAL_EEPROM
#define FCY_CLK                             10000000      // 10 MHz
#define FCY_CLK_MHZ                         10.000        // 10 MHz
#define ETM_CAN_MY_ADDRESS                  ETM_CAN_ADDR_HV_LAMBDA_BOARD
#define PIN_CAN_OPERATION_LED               _LATG13
#define ETM_CAN_INTERRUPT_PRIORITY          4
#define ETM_CAN_AGILE_ID_HIGH               0
#define ETM_CAN_AGILE_ID_LOW                36444
#define ETM_CAN_AGILE_DASH                  0
#define ETM_CAN_AGILE_REV                   'A' 
#define ETM_CAN_SERIAL_NUMBER               100 // DPARKER need to figure out how to set this in FLASH when programming
#endif

#ifdef __A36417
#define __USE_CAN_1
#define FCY_CLK                             10000000      // 10 MHz
#define FCY_CLK_MHZ                         10.000        // 10 MHz
#define ETM_CAN_MY_ADDRESS                  ETM_CAN_ADDR_ION_PUMP_BOARD
#define PIN_CAN_OPERATION_LED               _LATG13
#define ETM_CAN_INTERRUPT_PRIORITY          4
#define ETM_CAN_AGILE_ID_HIGH               0
#define ETM_CAN_AGILE_ID_LOW                36417
#define ETM_CAN_AGILE_DASH                  0
#define ETM_CAN_AGILE_REV                   'A'
#define ETM_CAN_SERIAL_NUMBER               100 // DPARKER need to figure out how to set this in FLASH when programming
#endif

#ifdef __A36224_500
#define __USE_CAN_2
#define FCY_CLK                             10000000      // 10 MHz
#define FCY_CLK_MHZ                         10.000        // 10 MHz
#define ETM_CAN_MY_ADDRESS                  ETM_CAN_ADDR_HEATER_MAGNET_BOARD
#define PIN_CAN_OPERATION_LED               _LATC4
#define ETM_CAN_INTERRUPT_PRIORITY          4
#define ETM_CAN_AGILE_ID_HIGH               0
#define ETM_CAN_AGILE_ID_LOW                36224
#define ETM_CAN_AGILE_DASH                  500
#define ETM_CAN_AGILE_REV                   'A'                   
#define ETM_CAN_SERIAL_NUMBER               100 // DPARKER need to figure out how to set this in FLASH when programming
#endif

#ifdef __A36224
#define __USE_CAN_2
#define FCY_CLK                             10000000      // 10 MHz
#define FCY_CLK_MHZ                         10.000        // 10 MHz
#define ETM_CAN_MY_ADDRESS                  ETM_CAN_ADDR_COOLING_INTERFACE_BOARD
#define PIN_CAN_OPERATION_LED               _LATC4
#define ETM_CAN_INTERRUPT_PRIORITY          4
#define ETM_CAN_AGILE_ID_HIGH               0
#define ETM_CAN_AGILE_ID_LOW                36224
#define ETM_CAN_AGILE_DASH                  000
#define ETM_CAN_AGILE_REV                   'C'
#define ETM_CAN_SERIAL_NUMBER               100 // DPARKER need to figure out how to set this in FLASH when programming
#endif


#ifdef __A36507
#define __USE_CAN_1
#define __ETM_CAN_MASTER_MODULE
#define __USE_EXTERNAL_EEPROM
#define FCY_CLK                             20000000      // 10 MHz
#define FCY_CLK_MHZ                         20.000        // 10 MHz
#define ETM_CAN_MY_ADDRESS                  ETM_CAN_ADDR_ETHERNET_BOARD
#define PIN_CAN_OPERATION_LED               _LATG13
#define ETM_CAN_INTERRUPT_PRIORITY          4
#define ETM_CAN_AGILE_ID_HIGH               0
#define ETM_CAN_AGILE_ID_LOW                36507
#define ETM_CAN_AGILE_DASH                  0
#define ETM_CAN_AGILE_REV                   'A'                   
#define ETM_CAN_SERIAL_NUMBER               100 // DPARKER need to figure out how to set this in FLASH when programming
#define __IGNORE_HV_LAMBDA_MODULE
#define __IGNORE_ION_PUMP_MODULE
#define __IGNORE_AFC_MODULE
#define __IGNORE_COOLING_INTERFACE_MODULE
#define __IGNORE_HEATER_MAGNET_MODULE
#define __IGNORE_GUN_DRIVER_MODULE
#define __IGNORE_PULSE_CURRENT_MODULE
//#define __IGNORE_PULSE_SYNC_MODULE
#endif


#ifdef __A36487
#define __USE_CAN_1
#define FCY_CLK                             10000000      // 10 MHz
#define FCY_CLK_MHZ                         10.000        // 10 MHz
#define ETM_CAN_MY_ADDRESS                  ETM_CAN_ADDR_PULSE_SYNC_BOARD
#define PIN_CAN_OPERATION_LED               _LATG14
#define ETM_CAN_INTERRUPT_PRIORITY          4
#define ETM_CAN_AGILE_ID_HIGH               0
#define ETM_CAN_AGILE_ID_LOW                36487
#define ETM_CAN_AGILE_DASH                  0
#define ETM_CAN_AGILE_REV                   'A'
#define ETM_CAN_SERIAL_NUMBER               100 // DPARKER need to figure out how to set this in FLASH when programming
#include "A36487.h"
#endif


#ifdef __A36582
#define __USE_CAN_1
#define FCY_CLK                             10000000      // 10 MHz
#define FCY_CLK_MHZ                         10.000        // 10 MHz
#define ETM_CAN_MY_ADDRESS                  ETM_CAN_ADDR_MAGNETRON_CURRENT_BOARD
#define PIN_CAN_OPERATION_LED               _LATG13
#define ETM_CAN_INTERRUPT_PRIORITY          4
#define ETM_CAN_AGILE_ID_HIGH               0
#define ETM_CAN_AGILE_ID_LOW                36582
#define ETM_CAN_AGILE_DASH                  0
#define ETM_CAN_AGILE_REV                   'A'
#define ETM_CAN_SERIAL_NUMBER               100 // DPARKER need to figure out how to set this in FLASH when programming
#endif



#endif //#ifndef __ETM_CAN_USER_CONFIG_H



