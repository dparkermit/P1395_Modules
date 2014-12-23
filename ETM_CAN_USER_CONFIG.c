#include "ETM_CAN.h"

#ifdef __A36224_500
#include "A36224_500.h"
#endif


#ifdef __A36444
#include "A36444.h"
#endif

#ifdef __A36224
#include "A36224_000.h"
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

#ifdef __A36224
      case ETM_CAN_REGISTER_COOLING_CMD_OPEN_SF6_SOLENOID_RELAY:
          ETMCanClearBit(&etm_can_status_register.status_word_0,STATUS_BIT_SF6_SOLENOID_RELAY_STATE);
    break;

      case ETM_CAN_REGISTER_COOLING_CMD_CLOSE_SF6_SOLENOID_RELAY:
          ETMCanSetBit(&etm_can_status_register.status_word_0,STATUS_BIT_SF6_SOLENOID_RELAY_STATE);
    break;
      case ETM_CAN_REGISTER_COOLING_CMD_SF6_PULSE_LIMIT_OVERRIDE:
          OverrideSF6PulseLimit();
          break;
      case ETM_CAN_REGISTER_COOLING_CMD_RESET_BOTTLE_COUNT:
          global_data_A36224_000.SF6_bottle_counter=700;
        break;
      case ETM_CAN_REGISTER_COOLING_CMD_SF6_LEAK_LIMIT_OVERRIDE:
          OverrideSF6LowPressure();
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
  default:
    etm_can_can_status.can_status_invalid_index++;
    break;
  }
}


void ETMCanResetFaults(void) {
  // Reset faults associated with this board
  global_reset_faults = 1;
}


void ETMCanLogCustomPacketC(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */

#ifdef __A36224
    ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_0,
            global_data_A36224_000.analog_input_flow_1.reading_scaled_and_calibrated,     //HVPS flow
            global_data_A36224_000.analog_input_flow_0.reading_scaled_and_calibrated,     //Magnetron flow
            0,                                              //Linac flow
            global_data_A36224_000.analog_input_flow_2.reading_scaled_and_calibrated     //Circulator
            );
#endif
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


}

void ETMCanLogCustomPacketD(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */

#ifdef __A36224
    ETMCanLogData(ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_1,
            0,   //Coolant spare word 1
            0,      //Coolant spare word 0
            0,  //HX coolant flow
            0  //coolant Flow (unused)
            );
#endif

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
  
}

void ETMCanLogCustomPacketE(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */
#ifdef __A36224
    ETMCanLogData(
    ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_ANALOG_READINGS,
            global_data_A36224_000.analog_input_coolant_temp.reading_scaled_and_calibrated,
            global_data_A36224_000.analog_input_SF6_pressure.reading_scaled_and_calibrated,
            global_data_A36224_000.analog_input_cabinet_temp.reading_scaled_and_calibrated,
            0       //linac temp
    );
#endif
  // There is no E packet for HV Lamdba, leave blank

}

void ETMCanLogCustomPacketF(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */

  // There is no F packet for HV Lamdba, leave blank

}


  
#endif  //#ifndef __ETM_CAN_MASTER_MODULE




