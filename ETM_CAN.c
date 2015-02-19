#include "ETM_CAN.h"
#include "p30fxxxx.h"


#ifdef __ETM_CAN_MASTER_MODULE
#include "A36507.h"
#endif

#ifdef __USE_EXTERNAL_EEPROM
ETMEEProm* ptr_external_eeprom;
#endif

void ETMCanLoadDefaultAnalogCalibration(void);




/*
  EEPROM ALLOCATION
  Each Page is 16 words (16 registers)
  External EEPROM is Pages 0->255 (Registers 0->8191)
  Internal EEPROM is Pages 0->127 (Registers 0->4095)
  
  Page 0x000 -> Page 0x006   - System Configuration Value, ECB ONLY
  Page 0x007                 - System On Counters, ECB ONLY
  Page 0x008                 - System Heater last complete times, ECB ONLY
  Page 0x009 -> Page 0x0F    - Reserved for ECB

  Page 0x010 -> Page 0x017   - Calibration Data - ALL BOARDS (may not be used of course)
  Page 0x018 -> Page 0x01F   - Reserved for ALL Board calibration data

  Page 0x020                 - Pulse/Arc Counters, Pulse Current Monitor Borad Only                
  Page 0x021                 - SF6 Pulses Remaining, Cooling Interface Board
  Page 0x022 -> Page 0x02F   - Reserved for board specific saving

  Page 0x030 -> Page 0x126   - Reserved for ???????

  Page 0x127                 - Event Log Point, ECB ONLY
  Page 0x128 -> Page 0x255   - Event Log Data, ECB ONLY  
  

  Reading and writing Calibration Data
  There are 2 words of data for each calibration (gain, offset)
  These are always transmitted as a pair by the CAN bus and the GUI should do the same.
  These are addressed as EVEN registers.
  A command should be formatted
  Word4 = Register Address (must be even) = (high nibble = board address) (3 low nibbles = calibration register)
  For example the GUI would write the "High Voltage lambda DAC2 Ext Cal Data" with EEPROM Address 0x414A
  It would read the calibration data with the same address
  

 

*/

// Public Buffers
ETMCanMessageBuffer etm_can_rx_message_buffer;
ETMCanMessageBuffer etm_can_tx_message_buffer;

// Public Variables
unsigned int etm_can_next_pulse_level;
unsigned int etm_can_next_pulse_count;


// Public Debug and Status registers
ETMCanSystemDebugData local_debug_data;
ETMCanStatusRegister  etm_can_status_register;
ETMCanAgileConfig     etm_can_my_configuration;
ETMCanCanStatus       local_can_errors;
ETMCanSyncMessage     etm_can_sync_message;



// DPARKER this is not global data so it can be pushed to to master/slave modules
typedef struct {
  unsigned int reset_count;
  unsigned int can_timeout_count;
} PersistentData;
volatile PersistentData etm_can_persistent_data __attribute__ ((persistent));



// Private Functions

void ETMCanSetValue(ETMCanMessage* message_ptr);
void ETMCanSetValueCalibration(ETMCanMessage* message_ptr);
void ETMCanCheckForTimeOut(void);











void ETMCanSetBit(unsigned int* int_ptr, unsigned int bit_mask) {
  *int_ptr = *int_ptr | bit_mask;
}

void ETMCanClearBit(unsigned int* int_ptr, unsigned int bit_mask) {
  *int_ptr = *int_ptr & (~bit_mask);
}

unsigned int ETMCanCheckBit(unsigned int data, unsigned int bit_mask) {
  return (data & bit_mask);
}



void ETMCanSetValue(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  index_word = message_ptr->word3;
  
#ifdef __ETM_CAN_MASTER_MODULE
  if ((index_word & 0x0FFF) <= 0x00FF) {
    // It is not a valid set Value ID
    local_can_errors.invalid_index++;
  } else if ((index_word & 0x0FFF) <= 0x2FF) {
    // It is a board specific set value
    ETMCanSetValueBoardSpecific(message_ptr);
  } else if ((index_word & 0x0FFF) <= 0x3FF) {
    // Default Register index
    // This is not valid for the master module
    local_can_errors.invalid_index++;
  } else if ( ((index_word & 0x0FFF) >= 0xF00) && ((index_word & 0x0FFF) < 0xF80)  ) {
    ETMCanSetValueCalibrationUpload(message_ptr);
  } else {
    // It was not a set value index 
    local_can_errors.invalid_index++;
  }
#else
  if ((index_word & 0xF000) != (ETM_CAN_MY_ADDRESS << 12)) {
    // The index is not addressed to this board
    local_can_errors.invalid_index++;
  } else if ((index_word & 0x0FFF) <= 0x00FF) {
    // It is not a valid set Value ID
    local_can_errors.invalid_index++;
  } else if ((index_word & 0x0FFF) <= 0x2FF) {
    // It is a board specific set value
    ETMCanSetValueBoardSpecific(message_ptr);
  } else if ((index_word & 0x0FFF) <= 0x3FF) {
    // It is a board specific defailt registers
    // These are not implimented at this time because there are no default set values
    local_can_errors.invalid_index++;
  } else if ( ((index_word & 0x0FFF) >= 0xF00) && ((index_word & 0x0FFF) < 0xF80)  ) {
    ETMCanSetValueCalibration(message_ptr);
  } else {
    // It was not a set value index 
    local_can_errors.invalid_index++;
  }    
#endif
}






#ifdef __ETM_CAN_MASTER_MODULE

#else
// ------------------------- These are all slave board functions -------------------------- //






void ETMCanLoadDefaultAnalogCalibration(void) {
  //DPARKER WRITE THIS
}

void ETMCanReturnValue(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  unsigned int message_index;
  index_word = message_ptr->word3;
  message_ptr->word0 = 0;
  message_ptr->word1 = 0;
  message_ptr->word2 = 0;

  if ((index_word & 0xF000) != (ETM_CAN_MY_ADDRESS << 12)) {
    // The index is not addressed to this board
    local_can_errors.invalid_index++;
    return;
  }
  
  message_index = (message_ptr->word3 & 0x0FFF);
  if ((message_index >= 0x200) && (message_index < 0x300)) {
    // It is a board specific return value
    ETMCanReturnValueBoardSpecific(message_ptr);
  } else if ((message_index >= 0xF00) && (message_index < 0xF80)) {
    ETMCanReturnEEPromRegister(message_ptr);
  } else {
    // It was not a set value index 
    local_can_errors.invalid_index++;
    return;
  }
  
  // Send Message Back to ECB with data
  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, message_ptr);
  MacroETMCanCheckTXBuffer();  // DPARKER - Figure out how to build this into ETMCanAddMessageToBuffer()

}


void ETMCanSetValueCalibration(ETMCanMessage* message_ptr) {
  unsigned int register_address;
  unsigned int scale_value;
  unsigned int offset_value;
  register_address = (message_ptr->word3 & 0x00FF);
  
  offset_value = message_ptr->word0;
  scale_value = message_ptr->word1;

#ifdef __USE_EXTERNAL_EEPROM
  ETMEEPromWriteWord(ptr_external_eeprom, register_address, offset_value);
  ETMEEPromWriteWord(ptr_external_eeprom, (register_address + 1), scale_value);
#else
  // Add functions for internal EEPROM
#endif

  // Send Message Back to ECB with data
  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, message_ptr);
  MacroETMCanCheckTXBuffer();  // DPARKER - Figure out how to build this into ETMCanAddMessageToBuffer()
}

void ETMCanReturnEEPromRegister(ETMCanMessage* message_ptr) {
  unsigned int register_address;
  unsigned int scale_value;
  unsigned int offset_value;
  register_address = (message_ptr->word3 & 0x00FF);
  
#ifdef __USE_EXTERNAL_EEPROM
  offset_value= ETMEEPromReadWord(ptr_external_eeprom, register_address);
  scale_value = ETMEEPromReadWord(ptr_external_eeprom, (register_address + 1));
#else
  offset_value = 0;
  scale_value  = 0;
  // Add functions for internal EEPROM
#endif
    
  message_ptr->word0 = offset_value;
  message_ptr->word1 = scale_value;
}


#endif





//------------------------- For use with pulse sync board only ----------------------- //

void ETMCanPulseSyncSendNextPulseLevel(unsigned int next_pulse_level, unsigned int next_pulse_count) {
  ETMCanMessage lvl_msg;

  lvl_msg.identifier = ETM_CAN_MSG_LVL_TX;
  lvl_msg.word1 = next_pulse_count;
  lvl_msg.word0 = next_pulse_level;
  ETMCanTXMessage(&lvl_msg, &CXTX2CON);
  local_can_errors.tx_2++;
}


//---------------------- For use with Ion Pump board only -------------------------- //
void ETMCanIonPumpSendTargetCurrentReading(unsigned int target_current_reading, unsigned int energy_level, unsigned int pulse_count) {
  ETMCanMessage can_msg;
  
  can_msg.identifier = ETM_CAN_MSG_SET_2_TX | (ETM_CAN_MY_ADDRESS << 3);
  can_msg.word0 = pulse_count;
  can_msg.word1 = target_current_reading;
  if (energy_level == ETM_CAN_HIGH_ENERGY) {
    can_msg.word3 = ETM_CAN_REGISTER_ECB_SET_2_HIGH_ENERGY_TARGET_CURRENT_MON;
  } else {
    can_msg.word3 = ETM_CAN_REGISTER_ECB_SET_2_LOW_ENERGY_TARGET_CURRENT_MON;
  }
  ETMCanTXMessage(&can_msg, &CXTX2CON);
  local_can_errors.tx_2++;
  
}









#ifdef __USE_EXTERNAL_EEPROM
void ETMCanSelectExternalEEprom(ETMEEProm* ptr_eeprom) {
  ptr_external_eeprom = ptr_eeprom;
}
#endif












