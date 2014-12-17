#include "ETM_CAN.h"

#ifdef __A36224_500
#include "A36224_500.h"
#endif


#ifdef __A36444
#include "A36444.h"
#endif

#ifdef __A36487
#include "A36487.h"
#endif

unsigned int global_reset_faults;

void ETMCanSetValueBoardSpecific(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  index_word = message_ptr->word3;
  switch (index_word) {
    /*
      Place all board specific set values here
    */

#ifdef __A36224_500
  case ETM_CAN_REGISTER_HEATER_MAGNET_SET_1_CURRENT_SET_POINT:
    ETMAnalogSetOutput(&global_data_A36224_500.analog_output_heater_current, message_ptr->word1);
    ETMAnalogSetOutput(&global_data_A36224_500.analog_output_electromagnet_current, message_ptr->word0);
    ETMCanClearBit(&etm_can_status_register.status_word_0, STATUS_BIT_BOARD_WAITING_INITIAL_CONFIG);
    break;

#endif


#ifdef __A36444
  case ETM_CAN_REGISTER_HV_LAMBDA_SET_1_LAMBDA_SET_POINT:
    ETMAnalogSetOutput(&global_data_A36444.analog_output_high_energy_vprog, message_ptr->word1); 
    ETMAnalogSetOutput(&global_data_A36444.analog_output_low_energy_vprog,message_ptr->word2);
    ETMCanClearBit(&etm_can_status_register.status_word_0, STATUS_BIT_BOARD_WAITING_INITIAL_CONFIG);
    break;
#endif


#ifdef __A36487   //A36487 PULSE SYNC BOARD
    unsigned int temp;
  case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_HIGH_ENERGY_TIMING_REG_0:
      temp = (message_ptr->word3 & 0xFF00) >> 8;
      psb_params.grid_delay_high3 = temp;
      temp = message_ptr->word3 & 0x00FF;
      psb_params.grid_delay_high2 = temp;
      temp = (message_ptr->word2 & 0xFF00) >> 8;
      psb_params.grid_delay_high1 = temp;
      temp = message_ptr->word2 & 0x00FF;
      psb_params.grid_delay_high0 = temp;
      temp = (message_ptr->word1 & 0xFF00) >> 8;
      psb_params.pfn_delay_high = temp;
      temp = message_ptr->word1 & 0x00FF;
      psb_params.rf_delay_high = temp;
      psb_data.counter_config_received |= 0b0001;
      break;

  case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_HIGH_ENERGY_TIMING_REG_1:
      temp = (message_ptr->word3 & 0xFF00) >> 8;
      psb_params.grid_width_high3 = temp;
      temp = message_ptr->word3 & 0x00FF;
      psb_params.grid_width_high2 = temp;
      temp = (message_ptr->word2 & 0xFF00) >> 8;
      psb_params.grid_width_high1 = temp;
      temp = message_ptr->word2 & 0x00FF;
      psb_params.grid_width_high0 = temp;
      temp = (message_ptr->word1 & 0xFF00) >> 8;
      psb_params.afc_delay_high = temp;
      temp = message_ptr->word1 & 0x00FF;
      psb_params.spare_delay_high = temp;
      psb_data.counter_config_received |=0b0010;
      break;

  case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_LOW_ENERGY_TIMING_REG_0:
      temp = (message_ptr->word3 & 0xFF00) >> 8;
      psb_params.grid_delay_low3 = temp;
      temp = message_ptr->word3 & 0x00FF;
      psb_params.grid_delay_low2 = temp;
      temp = (message_ptr->word2 & 0xFF00) >> 8;
      psb_params.grid_delay_low1 = temp;
      temp = message_ptr->word2 & 0x00FF;
      psb_params.grid_delay_low0 = temp;
      temp = (message_ptr->word1 & 0xFF00) >> 8;
      psb_params.pfn_delay_low = temp;
      temp = message_ptr->word1 & 0x00FF;
      psb_params.rf_delay_low = temp;
      psb_data.counter_config_received |= 0b0100;
      break;

  case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_LOW_ENERGY_TIMING_REG_1:
      temp = (message_ptr->word3 & 0xFF00) >> 8;
      psb_params.grid_width_low3 = temp;
      temp = message_ptr->word3 & 0x00FF;
      psb_params.grid_width_low2 = temp;
      temp = (message_ptr->word2 & 0xFF00) >> 8;
      psb_params.grid_width_low1 = temp;
      temp = message_ptr->word2 & 0x00FF;
      psb_params.grid_width_low0 = temp;
      temp = (message_ptr->word1 & 0xFF00) >> 8;
      psb_params.afc_delay_low = temp;
      temp = message_ptr->word1 & 0x00FF;
      psb_params.spare_delay_low = temp;
      psb_data.counter_config_received |= 0b1000;
      break;

  case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_CUSTOMER_LED_OUTPUT:
      psb_data.system_state = message_ptr->word0;
      break;
#endif


  default:
    etm_can_can_status.can_status_invalid_index++;
    break;
  }
}




#ifndef __ETM_CAN_MASTER_MODULE

void ETMCanExecuteCMDBoardSpecific(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  index_word = message_ptr->word3;
  switch (index_word) 
    {
      /*
	Place all board specific commands here
      */
#ifdef __A36224_500
    case ETM_CAN_REGISTER_HEATER_MAGNET_CMD_OUTPUT_ENABLE:
      ETMCanClearBit(&etm_can_status_register.status_word_0, STATUS_BIT_SOFTWARE_DISABLE); 
    break;
    
    case ETM_CAN_REGISTER_HEATER_MAGNET_CMD_OUTPUT_DISABLE:
      ETMCanSetBit(&etm_can_status_register.status_word_0, STATUS_BIT_SOFTWARE_DISABLE);
    break;
#endif
    
    
    
#ifdef __A36444
    case ETM_CAN_REGISTER_HV_LAMBDA_CMD_HV_ON:
      ETMCanClearBit(&etm_can_status_register.status_word_0, STATUS_BIT_SOFTWARE_DISABLE); 
      break;
      
    case ETM_CAN_REGISTER_HV_LAMBDA_CMD_HV_OFF:
      ETMCanSetBit(&etm_can_status_register.status_word_0, STATUS_BIT_SOFTWARE_DISABLE);
      break;
#endif
      
      
#ifdef __A36487     //A36487 PULSE SYNC BOARD
      case ETM_CAN_REGISTER_PULSE_SYNC_CMD_ENABLE_PULSES:
          psb_data.enable_pulses = 1;
          psb_data.can_comm_ok = 1;         //For 200ms Timeout
          break;
      case ETM_CAN_REGISTER_PULSE_SYNC_CMD_DISABLE_PULSES:
          psb_data.enable_pulses = 0;
          psb_data.can_comm_ok = 1;         //For 200ms Timeout
          break;
#endif
      
      
      
      
    default:
      etm_can_can_status.can_status_invalid_index++;
      break;
    }
}


void ETMCanReturnValueBoardSpecific(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  index_word = message_ptr->word3;
  index_word &= 0x0FFF;
  switch (index_word) {
    
    /*
      Place all board specific return value commands here
    */

#ifdef __A36487     //A36487 PULSE SYNC BOARD
      case ETM_CAN_REGISTER_PULSE_SYNC_REQUEST_PERSONALITY_MODULE:
          etm_can_status_register.status_word_0 = psb_data.personality;         //Where do you want this stored????
          break;
#endif


  default:
    etm_can_can_status.can_status_invalid_index++;
    break;
  }
}


void ETMCanResetFaults(void) {
  // Reset faults associated with this board
  global_reset_faults = 1;

#ifdef __A36487
    psb_faults.reset_faults = 1;
#endif

}


void ETMCanLogCustomPacketC(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */
#ifdef __A36224_500
  ETMCanLogData(
		ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_READINGS, 
		global_data_A36224_500.analog_input_heater_current.reading_scaled_and_calibrated,
		global_data_A36224_500.analog_input_heater_voltage.reading_scaled_and_calibrated,
		global_data_A36224_500.analog_input_electromagnet_current.reading_scaled_and_calibrated,
		global_data_A36224_500.analog_input_electromagnet_voltage.reading_scaled_and_calibrated
		);
#endif


#ifdef __A36444
  ETMCanLogData(
		ETM_CAN_DATA_LOG_REGISTER_HV_LAMBDA_FAST_PROGRAM_VOLTAGE,
		etm_can_next_pulse_count,
		global_data_A36444.analog_output_high_energy_vprog.set_point,
		global_data_A36444.analog_output_low_energy_vprog.set_point,
		global_data_A36444.analog_input_lambda_vpeak.reading_scaled_and_calibrated
		);
#endif

#ifdef __A36487
    ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_FAST_TRIGGER_DATA,
            psb_data.pulses_on,
            (psb_data.trigger_input << 8) & psb_data.trigger_filtered,
            (psb_data.grid_width << 8) & psb_data.grid_delay,
            0);
#endif

}

void ETMCanLogCustomPacketD(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */
#ifdef __A36224_500
  ETMCanLogData(
		ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_SET_POINTS, 
		global_data_A36224_500.analog_output_heater_current.set_point,
		0,
		global_data_A36224_500.analog_output_electromagnet_current.set_point,
		0
		);
#endif


#ifdef __A36444
  ETMCanLogData(
		ETM_CAN_DATA_LOG_REGISTER_HV_LAMBDA_SLOW_SET_POINT,
		global_data_A36444.eoc_not_reached_count,
		global_data_A36444.analog_input_lambda_vmon.reading_scaled_and_calibrated,
		global_data_A36444.analog_input_lambda_imon.reading_scaled_and_calibrated,
		global_data_A36444.analog_input_lambda_heat_sink_temp.reading_scaled_and_calibrated
		);
#endif


#ifdef __A36487
    ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_0,
            (psb_params.grid_delay_high3 << 8) & psb_params.grid_delay_high2,
            (psb_params.grid_delay_high1 << 8) &  psb_params.grid_delay_high0,
            (psb_params.pfn_delay_high << 8) & psb_params.rf_delay_high,
            (psb_params.grid_width_high3 << 8) & psb_params.grid_width_high2);
#endif

}

void ETMCanLogCustomPacketE(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */

  // There is no E packet for HV Lamdba, leave blank

#ifdef __A36487
    ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_1,
            (psb_params.grid_width_high1 << 8) & psb_params.grid_width_high0,
            (psb_params.afc_delay_high << 8) & psb_params.spare_delay_high,
            (psb_params.grid_delay_low3 << 8) & psb_params.grid_delay_low2,
            (psb_params.grid_delay_low1 << 8) & psb_params.grid_delay_low0);
#endif

}

void ETMCanLogCustomPacketF(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */

  // There is no F packet for HV Lamdba, leave blank

#ifdef __A36487
    ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_1,
            (psb_params.pfn_delay_low << 8) & psb_params.rf_delay_low,
            (psb_params.grid_width_low3 << 8) & psb_params.grid_width_low2,
            (psb_params.grid_width_low1 << 8) & psb_params.grid_width_low0,
            (psb_params.afc_delay_low << 8) & psb_params.spare_delay_low);
#endif

}


  
#endif  //#ifndef __ETM_CAN_MASTER_MODULE




