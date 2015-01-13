#ifndef __ETM_CAN_PUBLIC_H
#define __ETM_CAN_PUBLIC_H

#include "ETM_CAN_USER_CONFIG.h"




typedef struct {
  unsigned control_0_not_ready:1;
  unsigned control_1_can_status:1;
  unsigned control_2_not_configured:1;
  unsigned control_3_self_check_error:1;
  unsigned control_4_unused:1;
  unsigned control_5_unused:1;
  unsigned control_6_unused:1;
  unsigned control_7_ecb_can_not_active:1;
  
  unsigned status_0:1;
  unsigned status_1:1;
  unsigned status_2:1;
  unsigned status_3:1;
  unsigned status_4:1;
  unsigned status_5:1;
  unsigned status_6:1;
  unsigned status_7:1;
} ETMCanStatusRegisterStatusBits;



typedef struct {
  unsigned fault_0:1;
  unsigned fault_1:1;
  unsigned fault_2:1;
  unsigned fault_3:1;
  unsigned fault_4:1;
  unsigned fault_5:1;
  unsigned fault_6:1;
  unsigned fault_7:1;
  unsigned fault_8:1;
  unsigned fault_9:1;
  unsigned fault_A:1;
  unsigned fault_B:1;
  unsigned fault_C:1;
  unsigned fault_D:1;
  unsigned fault_E:1;
  unsigned fault_F:1;
} ETMCanStatusRegisterFaultBits;



typedef struct {
  ETMCanStatusRegisterStatusBits status_bits;  // 16 bits
  ETMCanStatusRegisterFaultBits  fault_bits;   // 16 bits
  unsigned int data_word_A;
  unsigned int data_word_B;

  unsigned int unused_A;                       // DPARKER Remove this when you figure out how
  unsigned int unused_B;                       // DPARKER Remove this when you figure out how
} ETMCanStatusRegister;

extern ETMCanStatusRegister  etm_can_status_register;


#define _CONTROL_NOT_READY            etm_can_status_register.status_bits.control_0_not_ready
#define _CONTROL_CAN_COM_LOSS         etm_can_status_register.status_bits.control_1_can_status
#define _CONTROL_NOT_CONFIGURED       etm_can_status_register.status_bits.control_2_not_configured
#define _CONTROL_SELF_CHECK_ERROR     etm_can_status_register.status_bits.control_3_self_check_error

#define _STATUS_0                     etm_can_status_register.status_bits.status_0
#define _STATUS_1                     etm_can_status_register.status_bits.status_1
#define _STATUS_2                     etm_can_status_register.status_bits.status_2
#define _STATUS_3                     etm_can_status_register.status_bits.status_3
#define _STATUS_4                     etm_can_status_register.status_bits.status_4
#define _STATUS_5                     etm_can_status_register.status_bits.status_5
#define _STATUS_6                     etm_can_status_register.status_bits.status_6
#define _STATUS_7                     etm_can_status_register.status_bits.status_7

#define _FAULT_0                      etm_can_status_register.fault_bits.fault_0
#define _FAULT_1                      etm_can_status_register.fault_bits.fault_1
#define _FAULT_2                      etm_can_status_register.fault_bits.fault_2
#define _FAULT_3                      etm_can_status_register.fault_bits.fault_3
#define _FAULT_4                      etm_can_status_register.fault_bits.fault_4
#define _FAULT_5                      etm_can_status_register.fault_bits.fault_5
#define _FAULT_6                      etm_can_status_register.fault_bits.fault_6
#define _FAULT_7                      etm_can_status_register.fault_bits.fault_7
#define _FAULT_8                      etm_can_status_register.fault_bits.fault_8
#define _FAULT_9                      etm_can_status_register.fault_bits.fault_9
#define _FAULT_A                      etm_can_status_register.fault_bits.fault_A
#define _FAULT_B                      etm_can_status_register.fault_bits.fault_B
#define _FAULT_C                      etm_can_status_register.fault_bits.fault_C
#define _FAULT_D                      etm_can_status_register.fault_bits.fault_D
#define _FAULT_E                      etm_can_status_register.fault_bits.fault_E
#define _FAULT_F                      etm_can_status_register.fault_bits.fault_F


#define _FAULT_REGISTER               *(unsigned int*)&etm_can_status_register.fault_bits
#define _CONTROL_REGISTER             *(unsigned int*)&etm_can_status_register.status_bits



typedef struct {
  unsigned int i2c_bus_error_count;
  unsigned int spi_bus_error_count;
  unsigned int can_bus_error_count;
  unsigned int scale_error_count;

  unsigned int reset_count;
  unsigned int self_test_result_register;
  unsigned int reserved_0;
  unsigned int reserved_1;           // This is reserved by the ECB to Indicate if the board is connected so it can be displayed on the GUI.  If the board is NOT CONNECTED this will be 1 (Zero if connected)

  unsigned int debug_0;
  unsigned int debug_1;
  unsigned int debug_2;
  unsigned int debug_3;

  unsigned int debug_4;
  unsigned int debug_5;
  unsigned int debug_6;
  unsigned int debug_7;

  unsigned int debug_8;
  unsigned int debug_9;
  unsigned int debug_A;
  unsigned int debug_B;

  unsigned int debug_C;
  unsigned int debug_D;
  unsigned int debug_E;
  unsigned int debug_F;

} ETMCanSystemDebugData;

// Self test register BIT Deffinitions  
#define SELF_TEST_5V_OV             0x0001
#define SELF_TEST_5V_UV             0x0002
#define SELF_TEST_15V_OV            0x0004
#define SELF_TEST_15V_UV            0x0008
#define SELF_TEST_N15V_OV           0x0010
#define SELF_TEST_N15V_UV           0x0020
#define SELF_TEST_ADC_OV            0x0040
#define SELF_TEST_ADC_UV            0x0080
#define SELF_TEST_ADC_EXT           0x0100
#define SELF_TEST_EEPROM            0x0200
#define SELF_TEST_DAC               0x0400



typedef struct {
  // Configuration 0
  unsigned int  agile_number_high_word;
  unsigned int  agile_number_low_word;
  unsigned int  agile_dash;
  unsigned int  agile_rev_ascii;

  // Configuarion 1
  unsigned int  serial_number;
  unsigned int  firmware_branch;
  unsigned int  firmware_major_rev;
  unsigned int  firmware_minor_rev;

} ETMCanAgileConfig;


typedef struct {
  unsigned sync_0_reset_enable:1;
  unsigned sync_1_high_speed_logging_enabled:1;
  unsigned sync_2_pulse_sync_disable_hv:1;
  unsigned sync_3_pulse_sync_disable_xray:1;
  unsigned sync_4_cooling_fault:1;
  unsigned sync_5_unused:1;
  unsigned sync_6_unused:1;
  unsigned sync_7_unused:1;

  unsigned sync_8_unused:1;
  unsigned sync_9_unused:1;
  unsigned sync_A_unused:1;
  unsigned sync_B_unused:1;
  unsigned sync_C_unused:1;
  unsigned sync_D_unused:1;
  unsigned sync_E_unused:1;
  unsigned sync_F_unused:1;
} ETMCanSyncControlWord;


typedef struct {
  ETMCanSyncControlWord sync_0_control_word;
  unsigned int sync_1;
  unsigned int sync_2;
  unsigned int sync_3;

} ETMCanSyncMessage;

extern ETMCanSyncMessage     etm_can_sync_message;


#define _SYNC_CONTROL_RESET_ENABLE            etm_can_sync_message.sync_0_control_word.sync_0_reset_enable
#define _SYNC_CONTROL_HIGH_SPEED_LOGGING      etm_can_sync_message.sync_0_control_word.sync_1_high_speed_logging_enabled
#define _SYNC_CONTROL_PULSE_SYNC_DISABLE_HV   etm_can_sync_message.sync_0_control_word.sync_2_pulse_sync_disable_hv
#define _SYNC_CONTROL_PULSE_SYNC_DISABLE_XRAY etm_can_sync_message.sync_0_control_word.sync_3_pulse_sync_disable_xray
#define _SYNC_CONTROL_COOLING_FAULT           etm_can_sync_message.sync_0_control_word.sync_4_cooling_fault

#define _SYNC_CONTROL_WORD                 *(unsigned int*)&etm_can_sync_message.sync_0_control_word



typedef struct {
  unsigned sync_0_logging_enable:1;
  unsigned sync_1_reset_faults:1;
  unsigned sync_2_pulse_sync_disable_HV:1;
  unsigned sync_3_pulse_sync_disable_XRAY:1;
  unsigned sync_4_cooling_fault:1;
  unsigned sync_5_can_timeout_sum:1;
  unsigned sync_6_operate_sum:1;
  unsigned sync_7_gun_HV_disable:1;
  unsigned sync_8_lambda_disable:1;
  unsigned sync_9_unused:1;
  unsigned sync_A_unused:1;
  unsigned sync_B_unused:1;
  unsigned sync_C_unused:1;
  unsigned sync_D_unused:1;
  unsigned sync_E_unused:1;
  unsigned sync_F_unused:1;
} ETMCanSyncMessageNew;



#define ETM_CAN_HIGH_ENERGY           1
#define ETM_CAN_LOW_ENERGY            0

// Public Variables
extern unsigned int etm_can_next_pulse_level;  // This value will get updated in RAM as when a next pulse level command is received
extern unsigned int etm_can_next_pulse_count;  // This value will get updated in RAM as when a next pulse level command is received

// Public Debug and Status registers
extern ETMCanSystemDebugData local_debug_data;  
extern ETMCanAgileConfig     etm_can_my_configuration;
/*
  DPARKER provide more description here.  How is it used.  What bits to set and what affect will setting them have
  This is the status register for this board.  Word0 bits (0,1) and Word1 bits (0) are mangaged by the Can module
*/

// Public Functions
void ETMCanDoCan(void);
/*
  This function should be called every time through the processor execution loop (which should be on the order of 10-100s of uS)
  If will do the following
  1) Look for an execute can commands
  2) Look for changes in status bits, update the Fault/Inhibit bits and send out a new status command if nessesary
  3) Send out regularly schedule communication (On slaves this is status update and logging data)
*/

void ETMCanInitialize(void);
/*
  This is called once when the processor starts up to initialize the can bus and all of the can variables
*/


void ETMCanSetBit(unsigned int* int_ptr, unsigned int bit_mask);
void ETMCanClearBit(unsigned int* int_ptr, unsigned int bit_mask);
unsigned int ETMCanCheckBit(unsigned int data, unsigned int bit_mask);


#ifndef __ETM_CAN_MASTER_MODULE
void ETMCanLogCustomPacketC(void);
void ETMCanLogCustomPacketD(void);
void ETMCanLogCustomPacketE(void);
void ETMCanLogCustomPacketF(void);
#endif


#ifdef __A35487
// Only for Pulse Sync Board
void ETMCanPulseSyncSendNextPulseLevel(unsigned int next_pulse_level, unsigned int next_pulse_count);
#endif


#ifdef __A63417
// Only for Ion Pump Board
void ETMCanIonPumpSendTargetCurrentReading(unsigned int target_current_reading, unsigned int energy_level, unsigned int pulse_count);
#endif



/*
// ------------------- STATUS/INHBIT REGISTER --------------------------//

#define STATUS_BIT_SUM_FAULT                       0b0000000000000001
#define STATUS_BIT_PULSE_INHIBITED                 0b0000000000000010
#define STATUS_BIT_BOARD_WAITING_INITIAL_CONFIG    0b0000000000000100
#define STATUS_BIT_BOARD_SELF_CHECK_FAILED         0b0000000000001000
#define STATUS_BIT_HIGH_SPEED_LOGGING_ENABLED      0b0000000000010000
#define STATUS_BIT_STATUS_UNUSED_1                 0b0000000000100000
#define STATUS_BIT_STATUS_UNUSED_2                 0b0000000001000000
#define STATUS_BIT_STATUS_UNUSED_3                 0b0000000010000000

#define STATUS_BIT_USER_DEFINED_8                  0b0000000100000000
#define STATUS_BIT_USER_DEFINED_9                  0b0000001000000000
#define STATUS_BIT_USER_DEFINED_10                 0b0000010000000000
#define STATUS_BIT_USER_DEFINED_11                 0b0000100000000000
#define STATUS_BIT_USER_DEFINED_12                 0b0001000000000000
#define STATUS_BIT_USER_DEFINED_13                 0b0010000000000000
#define STATUS_BIT_USER_DEFINED_14                 0b0100000000000000
#define STATUS_BIT_USER_DEFINED_15                 0b1000000000000000



// ----------------- FAULT/WARNING Register --------------------------//

#define FAULT_BIT_CAN_BUS_TIMEOUT                  0b0000000000000001
#define FAULT_BIT_USER_DEFINED_1                   0b0000000000000010
#define FAULT_BIT_USER_DEFINED_2                   0b0000000000000100
#define FAULT_BIT_USER_DEFINED_3                   0b0000000000001000
#define FAULT_BIT_USER_DEFINED_4                   0b0000000000010000
#define FAULT_BIT_USER_DEFINED_5                   0b0000000000100000
#define FAULT_BIT_USER_DEFINED_6                   0b0000000001000000
#define FAULT_BIT_USER_DEFINED_7                   0b0000000010000000

#define FAULT_BIT_USER_DEFINED_8                   0b0000000100000000
#define FAULT_BIT_USER_DEFINED_9                   0b0000001000000000
#define FAULT_BIT_USER_DEFINED_10                  0b0000010000000000
#define FAULT_BIT_USER_DEFINED_11                  0b0000100000000000
#define FAULT_BIT_USER_DEFINED_12                  0b0001000000000000
#define FAULT_BIT_USER_DEFINED_13                  0b0010000000000000
#define FAULT_BIT_USER_DEFINED_14                  0b0100000000000000
#define FAULT_BIT_USER_DEFINED_15                  0b1000000000000000

*/

extern unsigned int global_reset_faults;

#endif
