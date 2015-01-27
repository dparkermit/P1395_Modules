#include "ETM_CAN.h"
#include "p30fxxxx.h"

/* 
   EEPROM Utilization for P1395
   4K Words 0 -> FFF
   256 Pages 0 -> FF


   Calibration Data
   Page 0x00 -> 0x07 (Register 0x0000 -> 0x07)

   ECB Specific EEPROM
   Page 0x10 = Heater Complete Times
   Page 0x11 = System Time on 
   Page 0x12 = AFC & Heater/Magnet Settings
   Page 0x13 = HV Lambda Settings
   Page 0x14 = Gun Driver Settings
   Page 0x15 = Pulse Sync Personaility 1 Settings
   Page 0x16 = Pulse Sync Personaility 1 Setting
   Page 0x17 = Pulse Sync Personaility 1 Settings
   Page 0x18 = Pulse Sync Personaility 1 Settings
   Page 0x19 = Reserved for Future Use
   Page 0x1A = Reserved for Future Use
   Page 0x1B = Reserved for Future Use
   Page 0x1C = Reserved for Future Use
   Page 0x1D = Reserved for Future Use
   Page 0x1E = Reserved for Future Use
   Page 0x1F = Reserved for Future Use
   
   Pulse Monitor Board Specific EEPROM
   Page 0x20 = "Register A" for Pulse Total (4 words), Arc Total (2 words), CRC (1 word)
   Page 0x21 = "Register B" for Pulse Total (4 words), Arc Total (2 words), CRC (1 word)

   Page 0x7F = Event Log Pointer
   Page 0x80 -> 0xFF = Event Log Data 

*/


#ifdef __ETM_CAN_MASTER_MODULE
#include "A36507.h"
#endif

#ifdef __USE_EXTERNAL_EEPROM
ETMEEProm* ptr_external_eeprom;
#endif

void ETMCanLoadDefaultAnalogCalibration(void);

void ETMCanCheckForStatusChange(void);




// Public Buffers
ETMCanMessageBuffer etm_can_rx_message_buffer;
ETMCanMessageBuffer etm_can_tx_message_buffer;
#ifdef __ETM_CAN_MASTER_MODULE
ETMCanMessageBuffer etm_can_rx_data_log_buffer;
ETMCanRamMirrorEthernetBoard     etm_can_ethernet_board_data;

#endif

// Public Variables
unsigned int etm_can_next_pulse_level;
unsigned int etm_can_next_pulse_count;
#ifdef __ETM_CAN_MASTER_MODULE
unsigned int etm_can_pulse_sync_disable;
#else
unsigned int etm_can_high_speed_data_logging_enabled;
#endif

// Public Debug and Status registers
ETMCanSystemDebugData local_debug_data;
ETMCanStatusRegister  etm_can_status_register;
ETMCanAgileConfig     etm_can_my_configuration;
ETMCanCanStatus       local_can_errors;
ETMCanSyncMessage     etm_can_sync_message;



// Private Functions
void ETMCanProcessMessage(void);
void ETMCanSetValue(ETMCanMessage* message_ptr);
void ETMCanSetValueCalibration(ETMCanMessage* message_ptr);
void ETMCanCheckForTimeOut(void);


#ifdef __ETM_CAN_MASTER_MODULE
void ETMCanSetValueCalibrationUpload(ETMCanMessage* message_ptr);
void ETMCanProcessLogData(void);
void ETMCanMasterStandardCommunication(void);
void ETMCanSendSync();


typedef struct {
  unsigned int index;
  unsigned int scale;
  unsigned int offset;
  
} ETMCanCalibrationData;

ETMCanCalibrationData test_calibration_data;

void ETMCanMasterSet2FromSlave(ETMCanMessage* message_ptr);



#else

void ETMCanSendStatus(void);

void ETMCanExecuteCMD(ETMCanMessage* message_ptr);
void ETMCanExecuteCMDDefault(ETMCanMessage* message_ptr);

void ETMCanReturnValue(ETMCanMessage* message_ptr);
void ETMCanReturnEEPromRegister(ETMCanMessage* message_ptr);

void ETMCanDoSlaveSync(ETMCanMessage* message_ptr);
void ETMCanDoSlaveLog(void);

unsigned int previous_status;  // DPARKER - Need better name
  
#endif

//local variables
unsigned int etm_can_default_transmit_counter;

typedef struct {
  unsigned int reset_count;
  unsigned int can_timeout_count;
} PersistentData;


volatile PersistentData etm_can_persistent_data __attribute__ ((persistent));




#define DEFAULT_CALIBRATION_DATA_PAGE  {0, 0x8000, 0, 0x8000, 0, 0x8000, 0, 0x8000, 0, 0x8000, 0, 0x8000, 0, 0x8000, 0, 0x8000}
void ETMCanLoadDefaultAnalogCalibration(void) {

#ifdef __USE_EXTERNAL_EEPROM
  unsigned int default_calibration_data[16] = DEFAULT_CALIBRATION_DATA_PAGE;

  ETMEEPromWritePage(ptr_external_eeprom, 0, 16, default_calibration_data);
  ETMEEPromWritePage(ptr_external_eeprom, 1, 16, default_calibration_data);
  ETMEEPromWritePage(ptr_external_eeprom, 2, 16, default_calibration_data);
  ETMEEPromWritePage(ptr_external_eeprom, 3, 16, default_calibration_data);
  ETMEEPromWritePage(ptr_external_eeprom, 4, 16, default_calibration_data);
  ETMEEPromWritePage(ptr_external_eeprom, 5, 16, default_calibration_data);
  ETMEEPromWritePage(ptr_external_eeprom, 6, 16, default_calibration_data);
  ETMEEPromWritePage(ptr_external_eeprom, 7, 16, default_calibration_data);
#else
  // DPARKER configure the data in internal EEPROM
#endif
}



void ETMCanDoCan(void) {

  ETMCanProcessMessage();

#ifdef __ETM_CAN_MASTER_MODULE
  ETMCanProcessLogData();
  ETMCanMasterStandardCommunication();
#else
  ETMCanDoSlaveLog();
  ETMCanCheckForStatusChange();
#endif
  
  
  ETMCanCheckForTimeOut();

  local_debug_data.can_bus_error_count = local_can_errors.timeout;
  local_debug_data.can_bus_error_count += local_can_errors.message_tx_buffer_overflow;
  local_debug_data.can_bus_error_count += local_can_errors.message_rx_buffer_overflow;
  local_debug_data.can_bus_error_count += local_can_errors.data_log_rx_buffer_overflow;
  local_debug_data.can_bus_error_count += local_can_errors.address_error;
  local_debug_data.can_bus_error_count += local_can_errors.invalid_index;
  local_debug_data.can_bus_error_count += local_can_errors.unknown_message_identifier;
}

void ETMCanCheckForTimeOut(void) {
  if (_T3IF) {
    _T3IF = 0;
    local_can_errors.timeout++;
    etm_can_persistent_data.can_timeout_count = local_can_errors.timeout;
    _CONTROL_CAN_COM_LOSS = 1;
    
#ifdef __ETM_CAN_MASTER_MODULE
    // Save the status of recieved boards (only for display on GUI)
    etm_can_ethernet_board_data.status_connected_boards = etm_can_ethernet_board_data.status_received_register;

    // Indicate which board(s) are not connected
    _ION_PUMP_NOT_CONNECTED        = !ETMCanCheckBit(etm_can_ethernet_board_data.status_connected_boards, ETM_CAN_BIT_ION_PUMP_BOARD);
    _PULSE_CURRENT_NOT_CONNECTED   = !ETMCanCheckBit(etm_can_ethernet_board_data.status_connected_boards, ETM_CAN_BIT_MAGNETRON_CURRENT_BOARD);
    _PULSE_SYNC_NOT_CONNECTED      = !ETMCanCheckBit(etm_can_ethernet_board_data.status_connected_boards, ETM_CAN_BIT_PULSE_SYNC_BOARD);
    _HV_LAMBDA_NOT_CONNECTED       = !ETMCanCheckBit(etm_can_ethernet_board_data.status_connected_boards, ETM_CAN_BIT_HV_LAMBDA_BOARD);
    _AFC_NOT_CONNECTED             = !ETMCanCheckBit(etm_can_ethernet_board_data.status_connected_boards, ETM_CAN_BIT_AFC_CONTROL_BOARD);
    _COOLING_NOT_CONNECTED         = !ETMCanCheckBit(etm_can_ethernet_board_data.status_connected_boards, ETM_CAN_BIT_COOLING_INTERFACE_BOARD);
    _HEATER_MAGNET_NOT_CONNECTED   = !ETMCanCheckBit(etm_can_ethernet_board_data.status_connected_boards, ETM_CAN_BIT_HEATER_MAGNET_BOARD);
    _GUN_DRIVER_NOT_CONNECTED      = !ETMCanCheckBit(etm_can_ethernet_board_data.status_connected_boards, ETM_CAN_BIT_GUN_DRIVER_BOARD);

    etm_can_ethernet_board_data.status_received_register = 0x0000;
#endif
    
  }
}



void ETMCanProcessMessage(void) {
  ETMCanMessage next_message;
  while (ETMCanBufferNotEmpty(&etm_can_rx_message_buffer)) {
    ETMCanReadMessageFromBuffer(&etm_can_rx_message_buffer, &next_message);
    
#ifdef __ETM_CAN_MASTER_MODULE
    if ((next_message.identifier & ETM_CAN_MSG_MASTER_ADDR_MASK) == ETM_CAN_MSG_SET_2_RX) {
      ETMCanMasterSet2FromSlave(&next_message);
      //ETMCanSetValue(&next_message);      
    } else if ((next_message.identifier & ETM_CAN_MSG_MASTER_ADDR_MASK) == ETM_CAN_MSG_STATUS_RX) {
      ETMCanUpdateStatusBoardSpecific(&next_message);
    } else {
      local_can_errors.unknown_message_identifier++;
    } 
#else
    if ((next_message.identifier & 0b0000000001111000) != (ETM_CAN_MY_ADDRESS << 3)) {
      // It was not addressed to this board
      local_can_errors.address_error++;
    } else if (next_message.identifier == (ETM_CAN_MSG_CMD_RX | (ETM_CAN_MY_ADDRESS << 3))) {
      ETMCanExecuteCMD(&next_message);      
    } else if (next_message.identifier == (ETM_CAN_MSG_SET_1_RX | (ETM_CAN_MY_ADDRESS << 3))) {
      ETMCanSetValue(&next_message);      
    } else if (next_message.identifier == (ETM_CAN_MSG_REQUEST_RX | (ETM_CAN_MY_ADDRESS << 3))) {
      ETMCanReturnValue(&next_message);
    } else if ((next_message.identifier & ETM_CAN_MSG_SLAVE_ADDR_MASK) == (ETM_CAN_MSG_SET_3_RX | (ETM_CAN_MY_ADDRESS << 3))) {
      ETMCanSetValue(&next_message);
      local_can_errors.unknown_message_identifier++;
    } 
#endif
  }

  local_can_errors.message_tx_buffer_overflow = etm_can_tx_message_buffer.message_overwrite_count;
  local_can_errors.message_rx_buffer_overflow = etm_can_rx_message_buffer.message_overwrite_count;
#ifdef __ETM_CAN_MASTER_MODULE
  local_can_errors.data_log_rx_buffer_overflow = etm_can_rx_data_log_buffer.message_overwrite_count;
#endif  

}

void ETMCanSetBit(unsigned int* int_ptr, unsigned int bit_mask) {
  *int_ptr = *int_ptr | bit_mask;
}

void ETMCanClearBit(unsigned int* int_ptr, unsigned int bit_mask) {
  *int_ptr = *int_ptr & (~bit_mask);
}

unsigned int ETMCanCheckBit(unsigned int data, unsigned int bit_mask) {
  return (data & bit_mask);
}


#ifdef __ETM_CAN_MASTER_MODULE
void ETMCanMasterSet2FromSlave(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  index_word = message_ptr->word3 & 0x0FFF;
  
  if ((index_word >= 0x0F00) & (index_word < 0x0F80)) {
    // This is Calibration data that was read from the slave EEPROM
    test_calibration_data.index = message_ptr->word3;
    test_calibration_data.scale = message_ptr->word1;
    test_calibration_data.offset = message_ptr->word0;
  } else {
    // It was not a set value index 
    local_can_errors.invalid_index++;
  }
}
#endif


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


void ETMCanCheckForStatusChange(void) {
  if ((previous_status == 0) && (_CONTROL_NOT_READY)) {
    // There is new condition that is causing this board to inhibit operation.
    // Send a status update upstream to Master
    ETMCanSendStatus();
  }
  
  previous_status = _CONTROL_NOT_READY;
  
}

void ETMCanExecuteCMD(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  index_word = message_ptr->word3;
  
  if ((index_word & 0xF000) != (ETM_CAN_MY_ADDRESS << 12)) {
    // The index is not addressed to this board
    local_can_errors.invalid_index++;
  } else if ((index_word & 0x0FFF) <= 0x007F) {
    // It is a default command
    ETMCanExecuteCMDDefault(message_ptr);
  } else if ((index_word & 0x0FFF) <= 0x0FF) {
    // It is a board specific command
    ETMCanExecuteCMDBoardSpecific(message_ptr);
  } else {
    // It was not a command ID
    local_can_errors.invalid_index++;
  }
}


void ETMCanExecuteCMDDefault(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  index_word = message_ptr->word3;
  index_word &= 0x0FFF;
  
  switch (index_word) {
    
  case ETM_CAN_REGISTER_DEFAULT_CMD_RESET_MCU:
    __asm__ ("Reset");
    break;

  case ETM_CAN_REGISTER_DEFAULT_CMD_RESET_ANALOG_CALIBRATION:
    ETMCanLoadDefaultAnalogCalibration();
    break;
 
  default:
    // The default command was not recognized 
    local_can_errors.invalid_index++;
    break;
  }
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

void ETMCanSendStatus(void) {
  ETMCanMessage message;
  message.identifier = ETM_CAN_MSG_STATUS_TX | (ETM_CAN_MY_ADDRESS << 3);

  message.word0 = _CONTROL_REGISTER;
  message.word1 = _FAULT_REGISTER;
  message.word2 = 0xA0A0;
  message.word3 = 0xB0B0;
  
  ETMCanTXMessage(&message, &CXTX1CON);
  local_can_errors.tx_1++;
}


void ETMCanDoSlaveSync(ETMCanMessage* message_ptr) {
  // Sync data is available in CXRX0B1->CXRX0B4
  // At this time all that happens is that the chip watchdog is reset
  // DPARKER move to assembly and issure W0-W3, SR usage

  // It should be noted that if any of these registers are written ANYWHERE else, then they can be bashed

  _SYNC_CONTROL_WORD = message_ptr->word0;
  etm_can_sync_message.sync_1 = message_ptr->word1;
  etm_can_sync_message.sync_2 = message_ptr->word2;
  etm_can_sync_message.sync_3 = 0xFFFF;  // Use this to indicate that a sync message has been recieved
  //_CONTROL_CAN_SYNC_REC = 1;
  
  ClrWdt();
  _CONTROL_CAN_COM_LOSS = 0;
  
  TMR3 = 0;

#ifdef __A36487
  // The Pulse Sync Board needs to see if it needs to inhibit X_RAYs
  // This can be based by an update to read/modify update to this PORT that is currently in process
  // It will get fixed the next time through the control loop, but there is nothing else we can do
  // Hopefully we are using good coding principles and not bashing a PORT register.
  if (_SYNC_CONTROL_PULSE_SYNC_DISABLE_HV || _SYNC_CONTROL_PULSE_SYNC_DISABLE_XRAY) {
    PIN_CPU_XRAY_ENABLE_OUT = !OLL_CPU_XRAY_ENABLE;
  }
#endif

}



void ETMCanLogData(unsigned int packet_id, unsigned int word3, unsigned int word2, unsigned int word1, unsigned int word0) {

  ETMCanMessage log_message;
  
  packet_id &= 0x000F;
  packet_id |= (ETM_CAN_MY_ADDRESS << 4);
  packet_id <<= 1;
  packet_id |= 0b0000011000000000;
  packet_id <<= 2;
  
  log_message.identifier = packet_id;
  log_message.identifier &= 0xFF00;
  log_message.identifier <<= 3;
  log_message.identifier |= (packet_id & 0x00FF);
  
  log_message.word0 = word0;
  log_message.word1 = word1;
  log_message.word2 = word2;
  log_message.word3 = word3;
  
  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &log_message);
  MacroETMCanCheckTXBuffer();
}



void ETMCanDoSlaveLog(void) {
  // Sends the debug information up as log data  
  if (_T2IF) {
    // should be true once every 100mS
    _T2IF = 0;
    
    etm_can_default_transmit_counter++;
    etm_can_default_transmit_counter &= 0xF;
    
    ETMCanSendStatus(); // Send out the status every 100mS
    
    switch (etm_can_default_transmit_counter) 
      {
      case 0x0:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_ERROR_0, local_debug_data.i2c_bus_error_count, local_debug_data.spi_bus_error_count, local_debug_data.can_bus_error_count, local_debug_data.scale_error_count);      
	break;
	
      case 0x1:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_ERROR_1, local_debug_data.reset_count, local_debug_data.self_test_result_register, 0, 0);
	break;
	
      case 0x2:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_0, local_debug_data.debug_0, local_debug_data.debug_1, local_debug_data.debug_2, local_debug_data.debug_3);  
	break;
	
      case 0x3:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_1, local_debug_data.debug_4, local_debug_data.debug_5, local_debug_data.debug_6, local_debug_data.debug_7);
	break;
	
      case 0x4:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_2, local_debug_data.debug_8, local_debug_data.debug_9, local_debug_data.debug_A, local_debug_data.debug_B);
	break;
	
      case 0x5:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_3, local_debug_data.debug_C, local_debug_data.debug_D, local_debug_data.debug_E, local_debug_data.debug_F);
	break;
	
      case 0x6:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_0, local_can_errors.CXEC_reg, local_can_errors.error_flag, local_can_errors.tx_1, local_can_errors.tx_2);
	break;
	
      case 0x7:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_1, local_can_errors.rx_0_filt_0, local_can_errors.rx_0_filt_1, local_can_errors.rx_1_filt_2, local_can_errors.isr_entered);
	break;
	
      case 0x8:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_2, local_can_errors.unknown_message_identifier, local_can_errors.invalid_index, local_can_errors.address_error, local_can_errors.tx_0);
	break;

      case 0x9:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_3, local_can_errors.message_tx_buffer_overflow, local_can_errors.message_rx_buffer_overflow, local_can_errors.data_log_rx_buffer_overflow, local_can_errors.timeout);
	break;
	
      case 0xA:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CONFIG_0, etm_can_my_configuration.agile_number_high_word, etm_can_my_configuration.agile_number_low_word, etm_can_my_configuration.agile_dash, etm_can_my_configuration.agile_rev_ascii);
	break;

      case 0xB:
	ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CONFIG_1, etm_can_my_configuration.serial_number, etm_can_my_configuration.firmware_branch, etm_can_my_configuration.firmware_major_rev, etm_can_my_configuration.firmware_minor_rev);
	break;
      
      case 0xC:
	ETMCanLogCustomPacketC();
	break;
	
      case 0xD:
	ETMCanLogCustomPacketD();
	break;

      case 0xE:
	ETMCanLogCustomPacketE();
	break;

      case 0xF:
	ETMCanLogCustomPacketF();
	break;
      }
  }
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









/*

  unsigned int* dan_test = ((unsigned int*)&test_status_register) + 1;

  test_status_register.status_0_not_ready = *dan_test & 0b1000000000000000;
  
*/

#ifdef __USE_EXTERNAL_EEPROM
void ETMCanSelectExternalEEprom(ETMEEProm* ptr_eeprom) {
  ptr_external_eeprom = ptr_eeprom;
}
#endif


void ETMCanInitialize(void) {

  if (_POR || _BOR) {
    // This was a power cycle;
    etm_can_persistent_data.reset_count = 0;
    etm_can_persistent_data.can_timeout_count = 0;
  } else {
    etm_can_persistent_data.reset_count++;
  }

  _SYNC_CONTROL_WORD = 0;
  etm_can_sync_message.sync_1 = 1;
  etm_can_sync_message.sync_2 = 2;
  etm_can_sync_message.sync_3 = 3;
  

  _POR = 0;
  _BOR = 0;
  _SWR = 0;
  _EXTR = 0;
  _TRAPR = 0;
  _WDTO = 0;
  _IOPUWR = 0;


  local_debug_data.reset_count = etm_can_persistent_data.reset_count;
  local_can_errors.timeout = etm_can_persistent_data.can_timeout_count;

  _CXIE = 0;
  _CXIF = 0;
  _CXIP = ETM_CAN_INTERRUPT_PRIORITY;
  
  CXINTF = 0;
  
  CXINTEbits.RX0IE = 1; // Enable RXB0 interrupt
  CXINTEbits.RX1IE = 1; // Enable RXB1 interrupt
  CXINTEbits.TX0IE = 1; // Enable TXB0 interrupt
  CXINTEbits.ERRIE = 1; // Enable Error interrupt

  // DPARKER - Zero all the counters in the error structure.
  
  ETMCanBufferInitialize(&etm_can_rx_message_buffer);
  ETMCanBufferInitialize(&etm_can_tx_message_buffer);

#ifdef __ETM_CAN_MASTER_MODULE
  ETMCanBufferInitialize(&etm_can_rx_data_log_buffer);
#endif
  
  // ---------------- Set up CAN Control Registers ---------------- //
  
  // Set Baud Rate
  CXCTRL = CXCTRL_CONFIG_MODE_VALUE;
  while(CXCTRLbits.OPMODE != 4);
  
  CXCFG1 = ETM_CAN_CXCFG1_VALUE;
  CXCFG2 = CXCFG2_VALUE;
  
  
  // Load Mask registers for RX0 and RX1
#ifdef __ETM_CAN_MASTER_MODULE
  CXRXM0SID = ETM_CAN_MASTER_RX0_MASK;
  CXRXM1SID = ETM_CAN_MASTER_RX1_MASK;
#else
  CXRXM0SID = ETM_CAN_SLAVE_RX0_MASK;
  CXRXM1SID = ETM_CAN_SLAVE_RX1_MASK;
#endif

  // Load Filter registers
#ifdef __ETM_CAN_MASTER_MODULE
  CXRXF0SID = ETM_CAN_MSG_LVL_FILTER;
  CXRXF1SID = ETM_CAN_MSG_DATA_LOG_FILTER;
  CXRXF2SID = ETM_CAN_MSG_MASTER_FILTER;
  CXRXF3SID = ETM_CAN_MSG_FILTER_OFF;
  CXRXF4SID = ETM_CAN_MSG_FILTER_OFF;
  CXRXF5SID = ETM_CAN_MSG_FILTER_OFF;
#else
  CXRXF0SID = ETM_CAN_MSG_LVL_FILTER;
  CXRXF1SID = ETM_CAN_MSG_SYNC_FILTER;
  CXRXF2SID = (ETM_CAN_MSG_SLAVE_FILTER | (ETM_CAN_MY_ADDRESS << 3));
  CXRXF3SID = ETM_CAN_MSG_FILTER_OFF;
  CXRXF4SID = ETM_CAN_MSG_FILTER_OFF;
  CXRXF5SID = ETM_CAN_MSG_FILTER_OFF;
#endif

  // Set Transmitter Mode
  CXTX0CON = CXTXXCON_VALUE_LOW_PRIORITY;
  CXTX1CON = CXTXXCON_VALUE_MEDIUM_PRIORITY;
  CXTX2CON = CXTXXCON_VALUE_HIGH_PRIORITY;

  CXTX0DLC = CXTXXDLC_VALUE;
  CXTX1DLC = CXTXXDLC_VALUE;
  CXTX2DLC = CXTXXDLC_VALUE;

  
  // Set Receiver Mode
  CXRX0CON = CXRXXCON_VALUE;
  CXRX1CON = CXRXXCON_VALUE;
  
  // Switch to normal operation
  CXCTRL = CXCTRL_OPERATE_MODE_VALUE;
  while(CXCTRLbits.OPMODE != 0);
  
#ifdef __ETM_CAN_MASTER_MODULE
  // Set up data structure for ethernet board
  etm_can_ethernet_board_data.can_status    = &local_can_errors;
  etm_can_ethernet_board_data.debug_data    = &local_debug_data;
  etm_can_ethernet_board_data.configuration = &etm_can_my_configuration;
  etm_can_ethernet_board_data.status_data   = &etm_can_status_register;

  etm_can_ethernet_board_data.status_received_register = 0x0000;
#endif


  
  // Enable Can interrupt
  _CXIE = 1;



  // Configure T2
  T2CON = T2CON_VALUE;
  PR2 = PR2_VALUE;  
  TMR2 = 0;
  _T2IF = 0;
  _T2IE = 0;
  T2CONbits.TON = 1;

  // Configure T3
  T3CON = T3CON_VALUE;
  PR3 = PR3_VALUE;
  _T3IF = 0;
  _T3IE = 0;
  T3CONbits.TON = 1;


  etm_can_my_configuration.agile_number_high_word = ETM_CAN_AGILE_ID_HIGH;
  etm_can_my_configuration.agile_number_low_word  = ETM_CAN_AGILE_ID_LOW;
  etm_can_my_configuration.agile_dash             = ETM_CAN_AGILE_DASH;
  etm_can_my_configuration.agile_rev_ascii        = ETM_CAN_AGILE_REV;
  etm_can_my_configuration.serial_number          = ETM_CAN_SERIAL_NUMBER;

  /*
  ptr_test_sync_message = (ETMCanSyncMessageNew*)&dan_test;
  //if (ptr_test_sync_message.sync_0_logging_enable) {
  //  test_sync_message.sync_0_logging_enable = 1;
  //}
  //test_sync_message = *ptr_test_sync_message;
  test_sync_message = *(ETMCanSyncMessageNew*)&dan_test;


  ptr_fault_register = (unsigned int*)&test_status_register.fault_bits;
  *ptr_fault_register = 0b0101000000000001;
  
  //test_status_register.fault_bits.fault_0 = 1;
  //test_status_register.fault_bits.fault_7 = 1;
  //test_status_register.fault_bits.fault_E = 1;


  //fault_register = *(unsigned int*)&test_status_register.fault_bits;
  fault_register = *ptr_fault_register;


  Nop();
  Nop();

  */
  
}


#ifdef __ETM_CAN_MASTER_MODULE
void __attribute__((interrupt(__save__(CORCON,SR)), no_auto_psv)) _CXInterrupt(void) {
  ETMCanMessage can_message;
  ETMCanMessage sync_message;
  unsigned int msg_address;

  _CXIF = 0;
  local_can_errors.isr_entered++;
  
  if(CXRX0CONbits.RXFUL) {
    /*
      A message has been received in Buffer Zero
    */
    if (!CXRX0CONbits.FILHIT0) {
      // The command was received by Filter 0
      local_can_errors.rx_0_filt_0++;
      // It is a Next Pulse Level Command
      ETMCanRXMessage(&can_message, &CXRX0CON);
      etm_can_next_pulse_level = can_message.word2;
      etm_can_next_pulse_count = can_message.word3;
    } else {
      // The commmand was received by Filter 1
      local_can_errors.rx_0_filt_1++;
      // The command is a data log.  Add it to the data log buffer
      ETMCanRXMessageBuffer(&etm_can_rx_data_log_buffer, &CXRX0CON);
    }
    CXINTFbits.RX0IF = 0; // Clear the Interuppt Status bit
  }
  
  if(CXRX1CONbits.RXFUL) {
    /* 
       A message has been recieved in Buffer 1
       This command gets pushed onto the command message buffer
    */
    local_can_errors.rx_1_filt_2++;
    if ((CXRX1SID & ETM_CAN_MSG_MASTER_ADDR_MASK) == ETM_CAN_MSG_STATUS_RX)  {
      // The master is receiving a status update
      // We need to immediately update the fault and pulse inhibit information
      msg_address = CXRX1SID;
      msg_address >>= 3;
      msg_address &= 0x000F;
      msg_address = 1 << msg_address;
      if (CXRX1B1 & 0x0001)  {
	// This board is faulted set the fault bit for this board address
	if ((etm_can_ethernet_board_data.not_operate_bits & msg_address) == 0) {
	  // This is a NEW not operate bit
	  // There could already be an "enable" command in process that would overwrite this disable when the interrupt exits (possibly even before the disable message is sent)
	  etm_can_ethernet_board_data.not_operate_bits |= msg_address;
	  etm_can_ethernet_board_data.pulse_sync_disable_requested = 1;
	}
      } else {
	// Clear the fault bit for this board address
	etm_can_ethernet_board_data.not_operate_bits &= ~msg_address;
      }
    }
    ETMCanRXMessageBuffer(&etm_can_rx_message_buffer, &CXRX1CON);
    CXINTFbits.RX1IF = 0; // Clear the Interuppt Status bit
  }
  
  if ((!CXTX0CONbits.TXREQ) && (ETMCanBufferNotEmpty(&etm_can_tx_message_buffer))) {
    /*
      TX0 is empty and there is a message waiting in the transmit message buffer
      Load the next message into TX0
    */
    ETMCanTXMessageBuffer(&etm_can_tx_message_buffer, &CXTX0CON);
    CXINTFbits.TX0IF = 0;
    local_can_errors.tx_0++;
  }
  
  if (CXINTFbits.ERRIF) {
    // There was some sort of CAN Error
    // DPARKER - figure out which error and fix/reset
    local_can_errors.error_flag++;
    CXINTFbits.ERRIF = 0;
  } else {
    // FLASH THE CAN LED
    if (PIN_CAN_OPERATION_LED) {
      PIN_CAN_OPERATION_LED = 0;
    } else {
      PIN_CAN_OPERATION_LED = 1;
    }
  }
  local_can_errors.CXEC_reg = CXEC;
}

#else //#ifdef __ETM_CAN_MASTER_MODULE

void __attribute__((interrupt(__save__(CORCON,SR)), no_auto_psv)) _CXInterrupt(void) {
  ETMCanMessage can_message;
    
  _CXIF = 0;
  local_can_errors.isr_entered++;
  
  if(CXRX0CONbits.RXFUL) {
    /*
      A message has been received in Buffer Zero
    */
    if (!CXRX0CONbits.FILHIT0) {
      // The command was received by Filter 0
      local_can_errors.rx_0_filt_0++;
      // It is a Next Pulse Level Command
      ETMCanRXMessage(&can_message, &CXRX0CON);
      etm_can_next_pulse_level = can_message.word2;
      etm_can_next_pulse_count = can_message.word3;
    } else {
      // The commmand was received by Filter 1
      local_can_errors.rx_0_filt_1++;
      // The command is a sync command.
      ETMCanRXMessage(&can_message, &CXRX0CON);
      ETMCanDoSlaveSync(&can_message);
    }
    CXINTFbits.RX0IF = 0; // Clear the Interuppt Status bit
  }
    
  if(CXRX1CONbits.RXFUL) {
    /* 
       A message has been recieved in Buffer 1
       This command gets pushed onto the command message buffer
    */
    local_can_errors.rx_1_filt_2++;
    ETMCanRXMessageBuffer(&etm_can_rx_message_buffer, &CXRX1CON);
    CXINTFbits.RX1IF = 0; // Clear the Interuppt Status bit
  }

  if ((!CXTX0CONbits.TXREQ) && (ETMCanBufferNotEmpty(&etm_can_tx_message_buffer))) {
    /*
      TX0 is empty and there is a message waiting in the transmit message buffer
      Load the next message into TX0
    */
    ETMCanTXMessageBuffer(&etm_can_tx_message_buffer, &CXTX0CON);
    CXINTFbits.TX0IF = 0;
    local_can_errors.tx_0++;
  }
  
  if (CXINTFbits.ERRIF) {
    // There was some sort of CAN Error
    // DPARKER - figure out which error and fix/reset
    local_can_errors.error_flag++;
    CXINTFbits.ERRIF = 0;
  } else {
    // FLASH THE CAN LED
    if (PIN_CAN_OPERATION_LED) {
      PIN_CAN_OPERATION_LED = 0;
    } else {
      PIN_CAN_OPERATION_LED = 1;
    }
  }
  local_can_errors.CXEC_reg = CXEC;
}

#endif // #ifdef __ETM_CAN_MASTER_MODULE










#ifdef __ETM_CAN_MASTER_MODULE



ETMCanRamMirrorHVLambda          etm_can_hv_lamdba_mirror;
ETMCanRamMirrorIonPump           etm_can_ion_pump_mirror;
ETMCanRamMirrorAFC               etm_can_afc_mirror;
ETMCanRamMirrorCooling           etm_can_cooling_mirror;
ETMCanRamMirrorHeaterMagnet      etm_can_heater_magnet_mirror;
ETMCanRamMirrorGunDriver         etm_can_gun_driver_mirror;
ETMCanRamMirrorMagnetronCurrent  etm_can_magnetron_current_mirror;
ETMCanRamMirrorPulseSync         etm_can_pulse_sync_mirror;

ETMCanHighSpeedData              etm_can_high_speed_data_test;







// DPARKER move these commands to ETM_CAN_MASTER
// DPARKER create a master C and H file and move configuration there


void ETMCanSetValueCalibrationUpload(ETMCanMessage* message_ptr) {
  // Dparker impliment this
}

void ETMCanSendSync(void) {
  ETMCanMessage sync_message;
  sync_message.identifier = ETM_CAN_MSG_SYNC_TX;
  sync_message.word0 = _SYNC_CONTROL_WORD;
  sync_message.word1 = etm_can_sync_message.sync_1;
  sync_message.word2 = etm_can_sync_message.sync_2;
  sync_message.word3 = etm_can_sync_message.sync_3;
  
  ETMCanTXMessage(&sync_message, &CXTX1CON);
  local_can_errors.tx_1++;
}


void ETMCanMasterStandardCommunication(void) {
  /*
    One command is schedule to be sent every 25ms
    This loops through 8 times so each command is sent once every 200mS (5Hz)
    The sync command and Pulse Sync enable command are each sent twice for an effecive rate of 100ms (10Hz)
  */

  ETMCanMessage master_message;
  
  if (etm_can_ethernet_board_data.pulse_sync_disable_requested) {
    etm_can_ethernet_board_data.pulse_sync_disable_requested = 0;
    _SYNC_CONTROL_PULSE_SYNC_DISABLE_XRAY = 1;
    ETMCanSendSync();
  }

  if (_T2IF) {
    // should be true once every 25mS
    // each of the 8 cases will be true once every 200mS
    _T2IF = 0;
    
    etm_can_default_transmit_counter++;
    etm_can_default_transmit_counter &= 0x7;

    
    switch (etm_can_default_transmit_counter) 
      {
      case 0x0:
	// Send Sync Command (this is on TX1) - This also includes Pulse Sync Enable/Disable
	ETMCanSendSync();
	if (global_data_A36507.send_pulse_sync_config) {
	  global_data_A36507.send_pulse_sync_config = 0;
	  // DPARKER SEND OUT PULSE SYNC CONFIGURATION DATA
	  // DPARKER MUST FIRST CONFIRM THAT WE HAVE A VALID MODE SELECTED
	  // DPARKER, FOR NOW SEND OUT TEMP CONFIGURATION

	  master_message.identifier = (ETM_CAN_MSG_SET_1_TX | (ETM_CAN_ADDR_PULSE_SYNC_BOARD << 3));
	  master_message.word3      = ETM_CAN_REGISTER_PULSE_SYNC_SET_1_HIGH_ENERGY_TIMING_REG_0;
	  master_message.word2      = 0;
	  master_message.word1      = 0;
	  master_message.word0      = 0;
	  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &master_message);

	  master_message.identifier = (ETM_CAN_MSG_SET_1_TX | (ETM_CAN_ADDR_PULSE_SYNC_BOARD << 3));
	  master_message.word3      = ETM_CAN_REGISTER_PULSE_SYNC_SET_1_HIGH_ENERGY_TIMING_REG_1;
	  master_message.word2      = 0;
	  master_message.word1      = 0;
	  master_message.word0      = 0;
	  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &master_message);

	  master_message.identifier = (ETM_CAN_MSG_SET_1_TX | (ETM_CAN_ADDR_PULSE_SYNC_BOARD << 3));
	  master_message.word3      = ETM_CAN_REGISTER_PULSE_SYNC_SET_1_LOW_ENERGY_TIMING_REG_0;
	  master_message.word2      = 0;
	  master_message.word1      = 0;
	  master_message.word0      = 0;
	  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &master_message);

	  master_message.identifier = (ETM_CAN_MSG_SET_1_TX | (ETM_CAN_ADDR_PULSE_SYNC_BOARD << 3));
	  master_message.word3      = ETM_CAN_REGISTER_PULSE_SYNC_SET_1_LOW_ENERGY_TIMING_REG_1;
	  master_message.word2      = 0;
	  master_message.word1      = 0;
	  master_message.word0      = 0;
	  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &master_message);
	  MacroETMCanCheckTXBuffer();  // DPARKER - Figure out how to build this into ETMCanAddMessageToBuffer()

	}
	break;

      case 0x1:
	// Send High/Low Energy Program voltage to Lambda Board
	ETMCanMasterHVLambdaUpdateOutput();
	break;
	
      case 0x2:
	// Send Sync Command (this is on TX1) - This also includes Pulse Sync Enable/Disable
	ETMCanSendSync();
	break;
	
      case 0x3:
	// Send Heater/Magnet Current to Heater Magnet Board
	master_message.identifier = (ETM_CAN_MSG_SET_1_TX | (ETM_CAN_ADDR_HEATER_MAGNET_BOARD << 3));
	master_message.word3 = ETM_CAN_REGISTER_HEATER_MAGNET_SET_1_CURRENT_SET_POINT;
	master_message.word2 = 0;
	master_message.word1 = etm_can_heater_magnet_mirror.htrmag_heater_current_set_point;
	master_message.word0 = etm_can_heater_magnet_mirror.htrmag_magnet_current_set_point;
	ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &master_message);
	MacroETMCanCheckTXBuffer();  // DPARKER - Figure out how to build this into ETMCanAddMessageToBuffer()
	break;
	
      case 0x4:
	// Send Sync Command (this is on TX1) - This also includes Pulse Sync Enable/Disable
	ETMCanSendSync();
	break;

      case 0x5:
	// Send High/Low Energy Pulse top voltage to Gun Driver
	ETMCanMasterGunDriverUpdatePulseTop();
	break;
	
      case 0x6:
	// Send Sync Command (this is on TX1) - This also includes Pulse Sync Enable/Disable
	ETMCanSendSync();
	break;
	
      case 0x7:
	// Send Heater/Cathode set points to Gun Driver
	master_message.identifier = (ETM_CAN_MSG_SET_1_TX | (ETM_CAN_ADDR_GUN_DRIVER_BOARD << 3));
	master_message.word3 = ETM_CAN_REGISTER_GUN_DRIVER_SET_1_HEATER_CATHODE_SET_POINT;
	master_message.word2 = 0;
	master_message.word1 = etm_can_gun_driver_mirror.gun_cathode_voltage_set_point;
	master_message.word0 = etm_can_gun_driver_mirror.gun_heater_voltage_set_point;
	ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &master_message);
	MacroETMCanCheckTXBuffer();  // DPARKER - Figure out how to build this into ETMCanAddMessageToBuffer()
	break;
      }
  }
}

unsigned int ETMCanMasterReadyToPulse(void) {
  unsigned int ready_to_pulse;
  
  ready_to_pulse = etm_can_ethernet_board_data.software_pulse_enable;
  
  // DPARKER - ADD CHECK FOR ENABLED BOARDS
  if (etm_can_ethernet_board_data.not_operate_bits) {
    // One of the boards is not ready to pulse
    ready_to_pulse = 0;
  } 
  return ready_to_pulse;
}

void ETMCanMasterPulseSyncDisable(void) {
  ETMCanMessage can_message;
  
  can_message.identifier = (ETM_CAN_MSG_CMD_TX | (ETM_CAN_ADDR_PULSE_SYNC_BOARD << 3));
  can_message.word3 = ETM_CAN_REGISTER_PULSE_SYNC_CMD_DISABLE_PULSES;
  can_message.word2 = 0;
  can_message.word1 = 0;
  can_message.word0 = 0;
  ETMCanTXMessage(&can_message, &CXTX2CON);
}

void ETMCanMasterHVLambdaUpdateOutput(void) {
  ETMCanMessage can_message;
  
  can_message.identifier = (ETM_CAN_MSG_SET_1_TX | (ETM_CAN_ADDR_HV_LAMBDA_BOARD << 3));
  can_message.word3 = ETM_CAN_REGISTER_HV_LAMBDA_SET_1_LAMBDA_SET_POINT;
  can_message.word2 = etm_can_hv_lamdba_mirror.hvlambda_low_energy_set_point;
  can_message.word1 = etm_can_hv_lamdba_mirror.hvlambda_high_energy_set_point;
  can_message.word0 = 0;
  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &can_message);
  MacroETMCanCheckTXBuffer();  // DPARKER - Figure out how to build this into ETMCanAddMessageToBuffer()
}

void ETMCanMasterGunDriverUpdatePulseTop(void) {
  ETMCanMessage can_message;
  
  can_message.identifier = (ETM_CAN_MSG_SET_1_TX | (ETM_CAN_ADDR_GUN_DRIVER_BOARD << 3));
  can_message.word3 = ETM_CAN_REGISTER_GUN_DRIVER_SET_1_GRID_TOP_SET_POINT;
  can_message.word2 = 0;
  can_message.word1 = etm_can_gun_driver_mirror.gun_high_energy_pulse_top_voltage_set_point;
  can_message.word0 = etm_can_gun_driver_mirror.gun_low_energy_pulse_top_voltage_set_point;
  ETMCanAddMessageToBuffer(&etm_can_tx_message_buffer, &can_message);
  MacroETMCanCheckTXBuffer();  // DPARKER - Figure out how to build this into ETMCanAddMessageToBuffer()
}


void ETMCanProcessLogData(void) {
  ETMCanMessage          next_message;
  unsigned int           data_log_index;
  ETMCanSystemDebugData* debug_data_ptr;
  ETMCanAgileConfig*     config_data_ptr;
  ETMCanCanStatus*       can_status_ptr;

  while (ETMCanBufferNotEmpty(&etm_can_rx_data_log_buffer)) {
    ETMCanReadMessageFromBuffer(&etm_can_rx_data_log_buffer, &next_message);
    data_log_index = next_message.identifier;
    data_log_index >>= 3;
    data_log_index &= 0x00FF;
    
    if ((data_log_index & 0x000F) <= 0x000B) {
      // It is a common data logging buffer, load into the appropraite ram location
      

      // First figure out which board the data is from
      switch (data_log_index >> 4) 
	{
	case ETM_CAN_ADDR_ION_PUMP_BOARD:
	  debug_data_ptr  = &etm_can_ion_pump_mirror.debug_data;
	  config_data_ptr = &etm_can_ion_pump_mirror.configuration;
	  can_status_ptr  = &etm_can_ion_pump_mirror.can_status;
	  break;
	  
	case ETM_CAN_ADDR_MAGNETRON_CURRENT_BOARD:
	  debug_data_ptr  = &etm_can_magnetron_current_mirror.debug_data;
	  config_data_ptr = &etm_can_magnetron_current_mirror.configuration;
	  can_status_ptr  = &etm_can_magnetron_current_mirror.can_status;
	  break;

	case ETM_CAN_ADDR_PULSE_SYNC_BOARD:
	  debug_data_ptr  = &etm_can_pulse_sync_mirror.debug_data;
	  config_data_ptr = &etm_can_pulse_sync_mirror.configuration;
	  can_status_ptr  = &etm_can_pulse_sync_mirror.can_status;
	  break;
	  
	case ETM_CAN_ADDR_HV_LAMBDA_BOARD:
	  debug_data_ptr  = &etm_can_hv_lamdba_mirror.debug_data;
	  config_data_ptr = &etm_can_hv_lamdba_mirror.configuration;
	  can_status_ptr  = &etm_can_hv_lamdba_mirror.can_status;
	  break;
	  
	case ETM_CAN_ADDR_AFC_CONTROL_BOARD:
	  debug_data_ptr  = &etm_can_afc_mirror.debug_data;
	  config_data_ptr = &etm_can_afc_mirror.configuration;
	  can_status_ptr  = &etm_can_afc_mirror.can_status;
	  break;

	case ETM_CAN_ADDR_COOLING_INTERFACE_BOARD:
	  debug_data_ptr  = &etm_can_cooling_mirror.debug_data;
	  config_data_ptr = &etm_can_cooling_mirror.configuration;
	  can_status_ptr  = &etm_can_cooling_mirror.can_status;
	  break;
	  
	case ETM_CAN_ADDR_HEATER_MAGNET_BOARD:
	  debug_data_ptr  = &etm_can_heater_magnet_mirror.debug_data;
	  config_data_ptr = &etm_can_heater_magnet_mirror.configuration;
	  can_status_ptr  = &etm_can_heater_magnet_mirror.can_status;
	  break;

	case ETM_CAN_ADDR_GUN_DRIVER_BOARD:
	  debug_data_ptr  = &etm_can_gun_driver_mirror.debug_data;
	  config_data_ptr = &etm_can_gun_driver_mirror.configuration;
	  can_status_ptr  = &etm_can_gun_driver_mirror.can_status;
	  break;

	default:
	  //DPARKER add in some error counter
	  break;
	  
	}

      // Now figure out which data log it is
      switch (data_log_index & 0x000F)
	{
	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_ERROR_0:
	  debug_data_ptr->i2c_bus_error_count        = next_message.word3;
	  debug_data_ptr->spi_bus_error_count        = next_message.word2;
	  debug_data_ptr->can_bus_error_count        = next_message.word1;
	  debug_data_ptr->scale_error_count          = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_ERROR_1:
	  debug_data_ptr->reset_count                = next_message.word3;
	  debug_data_ptr->self_test_result_register  = next_message.word2;
	  debug_data_ptr->reserved_0                 = next_message.word1;
	  //debug_data_ptr->reserved_1                 = next_message.word0;  // This is now overwritten by the ECB to indicate if there is a connection
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_0:
	  debug_data_ptr->debug_0                    = next_message.word3;
	  debug_data_ptr->debug_1                    = next_message.word2;
	  debug_data_ptr->debug_2                    = next_message.word1;
	  debug_data_ptr->debug_3                    = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_1:
	  debug_data_ptr->debug_4                    = next_message.word3;
	  debug_data_ptr->debug_5                    = next_message.word2;
	  debug_data_ptr->debug_6                    = next_message.word1;
	  debug_data_ptr->debug_7                    = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_2:
	  debug_data_ptr->debug_8                    = next_message.word3;
	  debug_data_ptr->debug_9                    = next_message.word2;
	  debug_data_ptr->debug_A                    = next_message.word1;
	  debug_data_ptr->debug_B                    = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_3:
	  debug_data_ptr->debug_C                    = next_message.word3;
	  debug_data_ptr->debug_D                    = next_message.word2;
	  debug_data_ptr->debug_E                    = next_message.word1;
	  debug_data_ptr->debug_F                    = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CONFIG_0:
	  config_data_ptr->agile_number_high_word     = next_message.word3;
	  config_data_ptr->agile_number_low_word      = next_message.word2;
	  config_data_ptr->agile_dash                 = next_message.word1;
	  config_data_ptr->agile_rev_ascii            = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CONFIG_1:
	  config_data_ptr->serial_number              = next_message.word3;
	  config_data_ptr->firmware_branch            = next_message.word2;
	  config_data_ptr->firmware_major_rev         = next_message.word1;
	  config_data_ptr->firmware_minor_rev         = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_0:
	  can_status_ptr->CXEC_reg        = next_message.word3;
	  can_status_ptr->error_flag      = next_message.word2;
	  can_status_ptr->tx_1            = next_message.word1;
	  can_status_ptr->tx_2            = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_1:
	  can_status_ptr->rx_0_filt_0     = next_message.word3;
	  can_status_ptr->rx_0_filt_1     = next_message.word2;
	  can_status_ptr->rx_1_filt_2     = next_message.word1;
	  can_status_ptr->isr_entered     = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_2:
	  can_status_ptr->unknown_message_identifier     = next_message.word3;
	  can_status_ptr->invalid_index                  = next_message.word2;
	  can_status_ptr->address_error                  = next_message.word1;
	  can_status_ptr->tx_0                           = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_3:
	  can_status_ptr->message_tx_buffer_overflow     = next_message.word3;
	  can_status_ptr->message_rx_buffer_overflow     = next_message.word2;
	  can_status_ptr->data_log_rx_buffer_overflow    = next_message.word1;
	  can_status_ptr->timeout                        = next_message.word0;
	  break;
	  
	} 

    } else {
      // It is board specific logging data
      switch (data_log_index) 
	{
	case ETM_CAN_DATA_LOG_REGISTER_HV_LAMBDA_FAST_PROGRAM_VOLTAGE:
	  etm_can_high_speed_data_test.pulse_count = next_message.word3;
	  etm_can_high_speed_data_test.hvlambda_readback_high_energy_lambda_program_voltage = next_message.word2;
	  etm_can_high_speed_data_test.hvlambda_readback_low_energy_lambda_program_voltage = next_message.word1;
	  etm_can_high_speed_data_test.hvlambda_readback_peak_lambda_voltage = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_HV_LAMBDA_SLOW_SET_POINT:
	  etm_can_hv_lamdba_mirror.hvlambda_eoc_not_reached_count = next_message.word3;
	  etm_can_hv_lamdba_mirror.hvlambda_readback_vmon = next_message.word2;
	  etm_can_hv_lamdba_mirror.hvlambda_readback_imon = next_message.word1;
	  etm_can_hv_lamdba_mirror.hvlambda_readback_base_plate_temp = next_message.word0;
	  break;

	  
	case ETM_CAN_DATA_LOG_REGISTER_ION_PUMP_SLOW_MONITORS:
	  etm_can_ion_pump_mirror.ionpump_readback_ion_pump_volage_monitor = next_message.word3;
	  etm_can_ion_pump_mirror.ionpump_readback_ion_pump_current_monitor = next_message.word2;
	  etm_can_ion_pump_mirror.ionpump_readback_filtered_high_energy_target_current = next_message.word1;
	  etm_can_ion_pump_mirror.ionpump_readback_filtered_low_energy_target_current = next_message.word0;
	  break;


	case ETM_CAN_DATA_LOG_REGISTER_AFC_FAST_POSITION:
	  etm_can_high_speed_data_test.pulse_count = next_message.word3;
	  etm_can_high_speed_data_test.afc_readback_current_position = next_message.word2;
	  etm_can_high_speed_data_test.afc_readback_target_position = next_message.word1;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_AFC_FAST_READINGS:
	  etm_can_high_speed_data_test.pulse_count = next_message.word3;
	  etm_can_high_speed_data_test.afc_readback_a_input = next_message.word2;
	  etm_can_high_speed_data_test.afc_readback_b_input = next_message.word1;
	  etm_can_high_speed_data_test.afc_readback_filtered_error_reading = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_AFC_SLOW_SETTINGS:
	  etm_can_afc_mirror.afc_readback_home_position = next_message.word3;
	  etm_can_afc_mirror.afc_readback_offset = next_message.word2;
	  etm_can_afc_mirror.afc_readback_current_position = next_message.word1;
	  break;


	case ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_FAST_PREVIOUS_PULSE:
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_FILTERED_PULSE:
	  etm_can_magnetron_current_mirror.magmon_readback_spare = next_message.word3;
	  etm_can_magnetron_current_mirror.magmon_readback_arcs_this_hv_on = next_message.word2;
	  etm_can_magnetron_current_mirror.magmon_filtered_low_energy_pulse_current = next_message.word1;
	  etm_can_magnetron_current_mirror.magmon_filtered_high_energy_pulse_current = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_ARCS:
	  etm_can_magnetron_current_mirror.magmon_arcs_lifetime = next_message.word3;
	  etm_can_magnetron_current_mirror.magmon_arcs_lifetime <<= 16;
	  etm_can_magnetron_current_mirror.magmon_arcs_lifetime += next_message.word2;
	  etm_can_magnetron_current_mirror.magmon_pulses_this_hv_on = next_message.word1;
	  etm_can_magnetron_current_mirror.magmon_pulses_this_hv_on <<= 16;
	  etm_can_magnetron_current_mirror.magmon_pulses_this_hv_on += next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_PULSE_COUNT:
	  etm_can_magnetron_current_mirror.magmon_pulses_lifetime = next_message.word3;
	  etm_can_magnetron_current_mirror.magmon_pulses_lifetime <<= 16;
	  etm_can_magnetron_current_mirror.magmon_pulses_lifetime += next_message.word2;
	  etm_can_magnetron_current_mirror.magmon_pulses_lifetime <<= 16;
	  etm_can_magnetron_current_mirror.magmon_pulses_lifetime += next_message.word1;
	  etm_can_magnetron_current_mirror.magmon_pulses_lifetime <<= 16;
	  etm_can_magnetron_current_mirror.magmon_pulses_lifetime += next_message.word0;
	  break;


	case ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_0:
	  etm_can_cooling_mirror.cool_readback_hvps_coolant_flow = next_message.word3;
	  etm_can_cooling_mirror.cool_readback_magnetron_coolant_flow = next_message.word2;
	  etm_can_cooling_mirror.cool_readback_linac_coolant_flow = next_message.word1;
	  etm_can_cooling_mirror.cool_readback_circulator_coolant_flow = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_1:
	  etm_can_cooling_mirror.cool_readback_spare_word_1 = next_message.word3;
	  etm_can_cooling_mirror.cool_readback_spare_word_0 = next_message.word2;
	  etm_can_cooling_mirror.cool_readback_hx_coolant_flow = next_message.word1;
	  etm_can_cooling_mirror.cool_readback_spare_coolant_flow = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_ANALOG_READINGS:
	  etm_can_cooling_mirror.cool_readback_coolant_temperature = next_message.word3;
	  etm_can_cooling_mirror.cool_readback_sf6_pressure = next_message.word2;
	  etm_can_cooling_mirror.cool_readback_cabinet_temperature = next_message.word1;
	  etm_can_cooling_mirror.cool_readback_linac_temperature = next_message.word0;
	  break;


	case ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_READINGS:
	  etm_can_heater_magnet_mirror.htrmag_readback_heater_current = next_message.word3;
	  etm_can_heater_magnet_mirror.htrmag_readback_heater_voltage = next_message.word2;
	  etm_can_heater_magnet_mirror.htrmag_readback_magnet_current = next_message.word1;
	  etm_can_heater_magnet_mirror.htrmag_readback_magnet_voltage = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_SET_POINTS:
	  etm_can_heater_magnet_mirror.htrmag_readback_heater_current_set_point = next_message.word3;
	  etm_can_heater_magnet_mirror.htrmag_readback_heater_voltage_set_point = next_message.word2;
	  etm_can_heater_magnet_mirror.htrmag_readback_magnet_current_set_point = next_message.word1;
	  etm_can_heater_magnet_mirror.htrmag_readback_magnet_voltage_set_point = next_message.word0;
	  break;


	case ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_PULSE_TOP_MON:
	  etm_can_gun_driver_mirror.gun_readback_high_energy_pulse_top_voltage_monitor = next_message.word3;
	  etm_can_gun_driver_mirror.gun_readback_low_energy_pulse_top_voltage_monitor = next_message.word2;
	  etm_can_gun_driver_mirror.gun_readback_cathode_voltage_monitor = next_message.word1;
	  etm_can_gun_driver_mirror.gun_readback_peak_beam_current = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_HEATER_MON:
	  etm_can_gun_driver_mirror.gun_readback_heater_voltage_monitor = next_message.word3;
	  etm_can_gun_driver_mirror.gun_readback_heater_current_monitor = next_message.word2;
	  etm_can_gun_driver_mirror.gun_readback_heater_time_delay_remaining = next_message.word1;
	  etm_can_gun_driver_mirror.gun_readback_driver_temperature = next_message.word0;
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_SET_POINTS:
	  etm_can_gun_driver_mirror.gun_readback_high_energy_pulse_top_set_point = next_message.word3;
	  etm_can_gun_driver_mirror.gun_readback_low_energy_pulse_top_set_point = next_message.word2;
	  etm_can_gun_driver_mirror.gun_readback_heater_voltage_set_point = next_message.word1;
	  etm_can_gun_driver_mirror.gun_readback_cathode_voltage_set_point = next_message.word0;
	  break;


	case ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_FAST_TRIGGER_DATA:
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_0:
	  // DPARKER these are special because they need to be broken down into byte
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_1:
	  // DPARKER these are special because they need to be broken down into byte
	  break;

	case ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_2:
	  // DPARKER these are special because they need to be broken down into byte
	  break;

	default:
	  //DPARKER add some error
	  break;
	}
    }
  }
}



void ETMCanUpdateStatusBoardSpecific(ETMCanMessage* message_ptr) {
  unsigned int source_board;
  unsigned int all_boards_connected;
  source_board = (message_ptr->identifier >> 3);
  source_board &= 0x000F;

  
  switch (source_board) {
    /*
      Place all board specific status updates here
    */

  case ETM_CAN_ADDR_ION_PUMP_BOARD:
    etm_can_ion_pump_mirror.status_data.status_bits = *(ETMCanStatusRegisterStatusBits*)&message_ptr->word0;
    etm_can_ion_pump_mirror.status_data.fault_bits = *(ETMCanStatusRegisterFaultBits*)&message_ptr->word1;
    etm_can_ion_pump_mirror.status_data.data_word_A   = message_ptr->word2;
    etm_can_ion_pump_mirror.status_data.data_word_B   = message_ptr->word3;
    ETMCanSetBit(&etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_ION_PUMP_BOARD);
    ClrWdt();
    break;

  case ETM_CAN_ADDR_MAGNETRON_CURRENT_BOARD:
    etm_can_magnetron_current_mirror.status_data.status_bits = *(ETMCanStatusRegisterStatusBits*)&message_ptr->word0;
    etm_can_magnetron_current_mirror.status_data.fault_bits = *(ETMCanStatusRegisterFaultBits*)&message_ptr->word1;
    etm_can_magnetron_current_mirror.status_data.data_word_A   = message_ptr->word2;
    etm_can_magnetron_current_mirror.status_data.data_word_B   = message_ptr->word3;
    ETMCanSetBit(&etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_MAGNETRON_CURRENT_BOARD);
    ClrWdt();
    break;

  case ETM_CAN_ADDR_PULSE_SYNC_BOARD:
    etm_can_pulse_sync_mirror.status_data.status_bits = *(ETMCanStatusRegisterStatusBits*)&message_ptr->word0;
    etm_can_pulse_sync_mirror.status_data.fault_bits = *(ETMCanStatusRegisterFaultBits*)&message_ptr->word1;
    etm_can_pulse_sync_mirror.status_data.data_word_A   = message_ptr->word2;
    etm_can_pulse_sync_mirror.status_data.data_word_B   = message_ptr->word3;
    ETMCanSetBit(&etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_PULSE_SYNC_BOARD);
    ClrWdt();
    break;

  case ETM_CAN_ADDR_HV_LAMBDA_BOARD:
    etm_can_hv_lamdba_mirror.status_data.status_bits = *(ETMCanStatusRegisterStatusBits*)&message_ptr->word0;
    etm_can_hv_lamdba_mirror.status_data.fault_bits = *(ETMCanStatusRegisterFaultBits*)&message_ptr->word1;
    etm_can_hv_lamdba_mirror.status_data.data_word_A   = message_ptr->word2;
    etm_can_hv_lamdba_mirror.status_data.data_word_B   = message_ptr->word3;
    ETMCanSetBit(&etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_HV_LAMBDA_BOARD);
    ClrWdt();
    break;

  case ETM_CAN_ADDR_AFC_CONTROL_BOARD:
    etm_can_afc_mirror.status_data.status_bits = *(ETMCanStatusRegisterStatusBits*)&message_ptr->word0;
    etm_can_afc_mirror.status_data.fault_bits = *(ETMCanStatusRegisterFaultBits*)&message_ptr->word1;
    etm_can_afc_mirror.status_data.data_word_A   = message_ptr->word2;
    etm_can_afc_mirror.status_data.data_word_B   = message_ptr->word3;
    ETMCanSetBit(&etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_AFC_CONTROL_BOARD);
    ClrWdt();
    break;
    
  case ETM_CAN_ADDR_COOLING_INTERFACE_BOARD:
    etm_can_cooling_mirror.status_data.status_bits = *(ETMCanStatusRegisterStatusBits*)&message_ptr->word0;
    etm_can_cooling_mirror.status_data.fault_bits = *(ETMCanStatusRegisterFaultBits*)&message_ptr->word1;
    etm_can_cooling_mirror.status_data.data_word_A   = message_ptr->word2;
    etm_can_cooling_mirror.status_data.data_word_B   = message_ptr->word3;
    ETMCanSetBit(&etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_COOLING_INTERFACE_BOARD);
    ClrWdt();
    break;

  case ETM_CAN_ADDR_HEATER_MAGNET_BOARD:
    etm_can_heater_magnet_mirror.status_data.status_bits = *(ETMCanStatusRegisterStatusBits*)&message_ptr->word0;
    etm_can_heater_magnet_mirror.status_data.fault_bits = *(ETMCanStatusRegisterFaultBits*)&message_ptr->word1;
    etm_can_heater_magnet_mirror.status_data.data_word_A   = message_ptr->word2;
    etm_can_heater_magnet_mirror.status_data.data_word_B   = message_ptr->word3;
    etm_can_heater_magnet_mirror.status_data.status_bits.control_7_ecb_can_not_active = 0;
    ETMCanSetBit(&etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_HEATER_MAGNET_BOARD);
    ClrWdt();
    break;

  case ETM_CAN_ADDR_GUN_DRIVER_BOARD:
    etm_can_gun_driver_mirror.status_data.status_bits = *(ETMCanStatusRegisterStatusBits*)&message_ptr->word0;
    etm_can_gun_driver_mirror.status_data.fault_bits = *(ETMCanStatusRegisterFaultBits*)&message_ptr->word1;
    etm_can_gun_driver_mirror.status_data.data_word_A   = message_ptr->word2;
    etm_can_gun_driver_mirror.status_data.data_word_B   = message_ptr->word3;
    ETMCanSetBit(&etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_GUN_DRIVER_BOARD);
    ClrWdt();
    break;


  default:
    local_can_errors.address_error++;
    break;
  }

  // Figure out if all the boards are connected
  all_boards_connected = 1;
  
#ifndef __IGNORE_HV_LAMBDA_MODULE
  if (ETMCanCheckBit(etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_HV_LAMBDA_BOARD)) {
    _HV_LAMBDA_NOT_CONNECTED = 0;
  } else {
    all_boards_connected = 0;
  }
#endif

#ifndef __IGNORE_ION_PUMP_MODULE
  if (ETMCanCheckBit(etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_ION_PUMP_BOARD)) {
    _ION_PUMP_NOT_CONNECTED = 0;
  } else  {
    all_boards_connected = 0;
  }
#endif

#ifndef __IGNORE_AFC_MODULE
  if (ETMCanCheckBit(etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_AFC_CONTROL_BOARD)) {
    _AFC_NOT_CONNECTED = 0;
  } else {
    all_boards_connected = 0;
  }
#endif

#ifndef __IGNORE_COOLING_INTERFACE_MODULE
  if (ETMCanCheckBit(etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_COOLING_INTERFACE_BOARD)) {
    _COOLING_NOT_CONNECTED = 0;
  } else {
    all_boards_connected = 0;
  }
#endif

#ifndef __IGNORE_HEATER_MAGNET_MODULE
  if (ETMCanCheckBit(etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_HEATER_MAGNET_BOARD)) {
    _HEATER_MAGNET_NOT_CONNECTED = 0;
  } else {
    all_boards_connected = 0;
  }
#endif

#ifndef __IGNORE_GUN_DRIVER_MODULE
  if (ETMCanCheckBit(etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_GUN_DRIVER_BOARD)) {
    _GUN_DRIVER_NOT_CONNECTED = 0;
  } else {
    all_boards_connected = 0;
  }
#endif

#ifndef __IGNORE_PULSE_CURRENT_MODULE
  if (ETMCanCheckBit(etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_MAGNETRON_CURRENT_BOARD)) {
    _PULSE_CURRENT_NOT_CONNECTED = 0;
  } else {
    all_boards_connected = 0;
  }
#endif

#ifndef __IGNORE_PULSE_SYNC_MODULE
  if (ETMCanCheckBit(etm_can_ethernet_board_data.status_received_register, ETM_CAN_BIT_PULSE_SYNC_BOARD)) {
    _PULSE_SYNC_NOT_CONNECTED = 0;
  } else {
    all_boards_connected = 0;
  }
#endif

  if (all_boards_connected) {
    // Save the state of recieved boards (only for display on GUI)
    etm_can_ethernet_board_data.status_connected_boards = etm_can_ethernet_board_data.status_received_register;

    // Clear the status received register
    etm_can_ethernet_board_data.status_received_register = 0x0000; 
    
    // Reset T3 to start the next timer cycle
    TMR3 = 0;
  }
}


#endif

