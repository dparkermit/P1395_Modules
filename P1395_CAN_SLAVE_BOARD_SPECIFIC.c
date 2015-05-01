#include "P1395_CAN_SLAVE.h"
#include "P1395_CAN_CORE_PRIVATE.h"

#ifdef __A36746
#include "A36746.h"
#endif

#ifdef __A36224_500
#include "A36224_500.h"
#endif

#ifdef __A36444_500
#include "A36444_500.h"
#endif

#ifdef __A36444
#include "A36444.h"
#endif

#ifdef __A36224
#include "A36224_000.h"
#endif

#ifdef __A36487
#include "A36487.h"
#endif

#ifdef __A36582
#include "A36582.h"
#endif

#ifdef __A36465
#include "A36465.h"
#endif

#ifdef __A35975
#include "A35975.h"
#endif


void ETMCanSlaveExecuteCMDBoardSpecific(ETMCanMessage* message_ptr);
void ETMCanSlaveLogData(unsigned int packet_id, unsigned int word3, unsigned int word2, unsigned int word1, unsigned int word0);

void ETMCanSlaveExecuteCMDBoardSpecific(ETMCanMessage* message_ptr) {
  unsigned int index_word;
  #ifdef __A35975
  unsigned int value;
  #endif
  index_word = message_ptr->word3;
  switch (index_word) 
    {
      /*
	Place all board specific commands here
      */
      
#ifdef __A36746
    case ETM_CAN_REGISTER_COOLING_CMD_SF6_PULSE_LIMIT_OVERRIDE:
      global_data_A36746.SF6_pulses_available = 25;
      ETMEEPromWritePage(ETM_EEPROM_PAGE_COOLING_INTERFACE, 2, &global_data_A36746.SF6_pulses_available);
      break;
      
    case ETM_CAN_REGISTER_COOLING_CMD_RESET_BOTTLE_COUNT:
      global_data_A36746.SF6_bottle_pulses_remaining = message_ptr->word0;
      ETMEEPromWritePage(ETM_EEPROM_PAGE_COOLING_INTERFACE, 2, &global_data_A36746.SF6_pulses_available);
      break;
      
    case ETM_CAN_REGISTER_COOLING_CMD_SF6_LEAK_LIMIT_OVERRIDE:
      global_data_A36746.SF6_low_pressure_override_counter = message_ptr->word0;
      break;
#endif

#ifdef __A36224
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
      

      
#ifdef __A36224_500
    case ETM_CAN_REGISTER_HEATER_MAGNET_SET_1_CURRENT_SET_POINT:
      ETMAnalogSetOutput(&global_data_A36224_500.analog_output_heater_current, message_ptr->word1);
      ETMAnalogSetOutput(&global_data_A36224_500.analog_output_electromagnet_current, message_ptr->word0);
      _CONTROL_NOT_CONFIGURED = 0;
      break;
#endif

#ifdef __A36444_500
    case ETM_CAN_REGISTER_HEATER_MAGNET_SET_1_CURRENT_SET_POINT:
      ETMAnalogSetOutput(&global_data_A36444_500.analog_output_heater_current, message_ptr->word1);
      ETMAnalogSetOutput(&global_data_A36444_500.analog_output_electromagnet_current, message_ptr->word0);
      _CONTROL_NOT_CONFIGURED = 0;
      break;
#endif

      
#ifdef __A36444
    case ETM_CAN_REGISTER_HV_LAMBDA_SET_1_LAMBDA_SET_POINT:
      ETMAnalogSetOutput(&global_data_A36444.analog_output_high_energy_vprog, message_ptr->word1); 
      ETMAnalogSetOutput(&global_data_A36444.analog_output_low_energy_vprog,message_ptr->word2);
      _CONTROL_NOT_CONFIGURED = 0;
      break;
#endif
      
      
#ifdef __A36487     //A36487 PULSE SYNC BOARD
      unsigned int temp;
    case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_HIGH_ENERGY_TIMING_REG_0:
      temp = (message_ptr->word2 & 0xFF00) >> 8;
      psb_params.grid_delay_high3 = temp;
      temp = message_ptr->word2 & 0x00FF;
      psb_params.grid_delay_high2 = temp;
      temp = (message_ptr->word1 & 0xFF00) >> 8;
      psb_params.grid_delay_high1 = temp;
      temp = message_ptr->word1 & 0x00FF;
      psb_params.grid_delay_high0 = temp;
      temp = (message_ptr->word0 & 0xFF00) >> 8;
      psb_params.pfn_delay_high = temp;
      temp = message_ptr->word0 & 0x00FF;
      psb_params.dose_sample_delay_high = temp;
      psb_data.counter_config_received |= 0b0001;
      break;

    case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_HIGH_ENERGY_TIMING_REG_1:
      temp = (message_ptr->word2 & 0xFF00) >> 8;
      psb_params.grid_width_high3 = temp;
      temp = message_ptr->word2 & 0x00FF;
      psb_params.grid_width_high2 = temp;
      temp = (message_ptr->word1 & 0xFF00) >> 8;
      psb_params.grid_width_high1 = temp;
      temp = message_ptr->word1 & 0x00FF;
      psb_params.grid_width_high0 = temp;
      temp = (message_ptr->word0 & 0xFF00) >> 8;
      psb_params.afc_delay_high = temp;
      temp = message_ptr->word0 & 0x00FF;
      psb_params.magnetron_current_sample_delay_high = temp;
      psb_data.counter_config_received |=0b0010;
      break;

    case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_LOW_ENERGY_TIMING_REG_0:
      temp = (message_ptr->word2 & 0xFF00) >> 8;
      psb_params.grid_delay_low3 = temp;
      temp = message_ptr->word2 & 0x00FF;
      psb_params.grid_delay_low2 = temp;
      temp = (message_ptr->word1 & 0xFF00) >> 8;
      psb_params.grid_delay_low1 = temp;
      temp = message_ptr->word1 & 0x00FF;
      psb_params.grid_delay_low0 = temp;
      temp = (message_ptr->word0 & 0xFF00) >> 8;
      psb_params.pfn_delay_low = temp;
      temp = message_ptr->word0 & 0x00FF;
      psb_params.dose_sample_delay_low = temp;
      psb_data.counter_config_received |= 0b0100;
      break;

    case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_LOW_ENERGY_TIMING_REG_1:
      temp = (message_ptr->word2 & 0xFF00) >> 8;
      psb_params.grid_width_low3 = temp;
      temp = message_ptr->word2 & 0x00FF;
      psb_params.grid_width_low2 = temp;
      temp = (message_ptr->word1 & 0xFF00) >> 8;
      psb_params.grid_width_low1 = temp;
      temp = message_ptr->word1 & 0x00FF;
      psb_params.grid_width_low0 = temp;
      temp = (message_ptr->word0 & 0xFF00) >> 8;
      psb_params.afc_delay_low = temp;
      temp = message_ptr->word0 & 0x00FF;
      psb_params.magnetron_current_sample_delay_low = temp;
      psb_data.counter_config_received |= 0b1000;
      break;

    case ETM_CAN_REGISTER_PULSE_SYNC_SET_1_CUSTOMER_LED_OUTPUT:
      psb_data.led_state = message_ptr->word0;
      break;
#endif
      
      
#ifdef __A36465
    case ETM_CAN_REGISTER_AFC_SET_1_HOME_POSITION_AND_OFFSET:
      afc_motor.home_position = message_ptr->word0;
      // unused offset
      ETMAnalogSetOutput(&global_data_A36465.aft_control_voltage, message_ptr->word2);
      _CONTROL_NOT_CONFIGURED = 0;
      break;
      
    case ETM_CAN_REGISTER_AFC_CMD_SELECT_AFC_MODE:
      _STATUS_AFC_MODE_MANUAL_MODE = 0;
      break;
      
    case ETM_CAN_REGISTER_AFC_CMD_SELECT_MANUAL_MODE:
      _STATUS_AFC_MODE_MANUAL_MODE = 1;
      break;
      
    case ETM_CAN_REGISTER_AFC_CMD_SET_MANUAL_TARGET_POSITION:
      global_data_A36465.manual_target_position = message_ptr->word0;
      break;

    case ETM_CAN_REGISTER_AFC_CMD_RELATIVE_MOVE_MANUAL_TARGET:
      if (message_ptr->word1) {
	// decrease the target position;
	if (global_data_A36465.manual_target_position > message_ptr->word0) {
	  global_data_A36465.manual_target_position -= message_ptr->word0;
	} else {
	  global_data_A36465.manual_target_position = 0;
	}
      } else {
	// increase the target position;
	if ((0xFFFF - message_ptr->word0) > global_data_A36465.manual_target_position) {
	  global_data_A36465.manual_target_position += message_ptr->word0;
	} else {
	  global_data_A36465.manual_target_position = 0xFFFF;
	}
      }
      break;

#endif      

#ifdef __A35975	  // Gun Driver
  case ETM_CAN_REGISTER_GUN_DRIVER_SET_1_GRID_TOP_SET_POINT:
    value = ETMScaleFactor16(message_ptr->word1, CAN_scale_table[CAN_SET_EGSET].fixed_scale, 
  				0);            
    SetEg(value);    
    // word0 for low Eg, not used 
    _CONTROL_NOT_CONFIGURED = AreAnyReferenceNotConfigured();
    break;

  case ETM_CAN_REGISTER_GUN_DRIVER_SET_1_HEATER_CATHODE_SET_POINT:
    value = ETMScaleFactor16(message_ptr->word1, CAN_scale_table[CAN_SET_EKSET].fixed_scale, 
  				0);            
    SetEk(value);    
    value = ETMScaleFactor16(message_ptr->word0, CAN_scale_table[CAN_SET_EFSET].fixed_scale, 
  				0);            
    SetEf(value);    
    _CONTROL_NOT_CONFIGURED = AreAnyReferenceNotConfigured();    
    break;
#endif

      
    default:
      local_can_errors.invalid_index++;
      break;
    }
}


void ETMCanSlaveLogCustomPacketC(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */


#ifdef __A36746
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_0,
		     global_data_A36746.flow_hvps,
		     global_data_A36746.flow_magnetron,
		     global_data_A36746.flow_linac,
		     global_data_A36746.flow_circulator
		     );
#endif

#ifdef __A36224
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_0,
		     global_data_A36224_000.analog_input_flow_1.reading_scaled_and_calibrated,     //HVPS flow
		     global_data_A36224_000.analog_input_flow_0.reading_scaled_and_calibrated,     //Magnetron flow
		     0,                                              //Linac flow
		     global_data_A36224_000.analog_input_flow_2.reading_scaled_and_calibrated     //Circulator
		     );
#endif

#ifdef __A36224_500
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_READINGS, 
		     global_data_A36224_500.analog_input_heater_current.reading_scaled_and_calibrated,
		     global_data_A36224_500.analog_input_heater_voltage.reading_scaled_and_calibrated,
		     global_data_A36224_500.analog_input_electromagnet_current.reading_scaled_and_calibrated,
		     global_data_A36224_500.analog_input_electromagnet_voltage.reading_scaled_and_calibrated
		     );
#endif

#ifdef __A36444_500
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_READINGS, 
		     global_data_A36444_500.analog_input_heater_current.reading_scaled_and_calibrated,
		     global_data_A36444_500.analog_input_heater_voltage.reading_scaled_and_calibrated,
		     global_data_A36444_500.analog_input_electromagnet_current.reading_scaled_and_calibrated,
		     global_data_A36444_500.analog_input_electromagnet_voltage.reading_scaled_and_calibrated
		     );
#endif

#ifdef __A36444
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_HV_LAMBDA_FAST_PROGRAM_VOLTAGE,
		     etm_can_next_pulse_count,
		     global_data_A36444.analog_output_high_energy_vprog.set_point,
		     global_data_A36444.analog_output_low_energy_vprog.set_point,
		     global_data_A36444.analog_input_lambda_vpeak.reading_scaled_and_calibrated
		     );
#endif

#ifdef __A36487
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_FAST_TRIGGER_DATA,
		     psb_data.pulses_on,
		     7,//(psb_data.trigger_input << 8) & psb_data.trigger_filtered,
		     8,//(psb_data.grid_width << 8) & psb_data.grid_delay,
		     9);
#endif


#ifdef __A36582
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_FAST_PREVIOUS_PULSE,
		     global_data_A36582.sample_index,
		     global_data_A36582.imag_internal_adc.reading_scaled_and_calibrated,
		     global_data_A36582.imag_external_adc.reading_scaled_and_calibrated,
		     _STATUS_ARC_DETECTED
		     );
#endif

#ifdef __A36465
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_AFC_FAST_POSITION,
		     etm_can_next_pulse_count,
		     afc_motor.current_position,
		     afc_motor.target_position,
		     0
		     );

#endif


#ifdef __A35975
  unsigned int word3 = ETMScaleFactor2(analog_reads[ANA_RD_EG].read_cur, CAN_scale_table[CAN_RD_EG].fixed_scale, 
  				CAN_scale_table[CAN_RD_EG].fixed_offset);
  unsigned int word2 = 0; // low energy Eg((faults_from_ADC << 8) | control_state);        
  unsigned int word1 = ETMScaleFactor16(analog_reads[ANA_RD_EK].read_cur, CAN_scale_table[CAN_RD_EK].fixed_scale, 
  				CAN_scale_table[CAN_RD_EK].fixed_offset);
  unsigned int word0 = ETMScaleFactor16(analog_reads[ANA_RD_IKP].read_cur, CAN_scale_table[CAN_RD_IKP].fixed_scale, 
  				CAN_scale_table[CAN_RD_IKP].fixed_offset);

  ETMCanSlaveLogData(
		ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_PULSE_TOP_MON, word3, word2, word1, word0);

#endif
}

void ETMCanSlaveLogCustomPacketD(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */

#ifdef __A36746
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_1,
		     global_data_A36746.SF6_bottle_pulses_remaining,
		     global_data_A36746.SF6_pulses_available,
		     global_data_A36746.flow_hv_tank,
		     global_data_A36746.flow_spare
		     );
#endif

#ifdef __A36224
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_1,
		     0,
		     0,   //Coolant spare word 0
		     0,   //HVPS coolant flow
		     0    //coolant Flow (unused)
		     );
#endif

#ifdef __A36224_500
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_SET_POINTS, 
		     global_data_A36224_500.analog_output_heater_current.set_point,
		     0,
		     global_data_A36224_500.analog_output_electromagnet_current.set_point,
		     0
		     );
#endif

#ifdef __A36444_500
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_SET_POINTS, 
		     global_data_A36444_500.analog_output_heater_current.set_point,
		     0,
		     global_data_A36444_500.analog_output_electromagnet_current.set_point,
		     0
		     );
#endif

#ifdef __A36444
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_HV_LAMBDA_SLOW_SET_POINT,
		     global_data_A36444.eoc_not_reached_count,
		     global_data_A36444.analog_input_lambda_vmon.reading_scaled_and_calibrated,
		     global_data_A36444.analog_input_lambda_imon.reading_scaled_and_calibrated,
		     global_data_A36444.analog_input_lambda_heat_sink_temp.reading_scaled_and_calibrated
		     );
#endif


#ifdef __A36487
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_0,
		     (psb_params.grid_delay_high3 << 8) & psb_params.grid_delay_high2,
		     (psb_params.grid_delay_high1 << 8) &  psb_params.grid_delay_high0,
		     (psb_params.pfn_delay_high << 8) & psb_params.dose_sample_delay_high,
		     (psb_params.grid_width_high3 << 8) & psb_params.grid_width_high2);
#endif


#ifdef __A36582
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_FILTERED_PULSE,
		     0,
		     global_data_A36582.arc_this_hv_on,
		     global_data_A36582.filt_ext_adc_low,
		     global_data_A36582.filt_ext_adc_high
		     );
#endif

#ifdef __A35975	  // Gun Driver
  unsigned int word3 = ETMScaleFactor16(analog_reads[ANA_RD_EF].read_cur, CAN_scale_table[CAN_RD_EF].fixed_scale, 
  				CAN_scale_table[CAN_RD_EF].fixed_offset);
  unsigned int word2 = ETMScaleFactor2(analog_reads[ANA_RD_IF].read_cur, CAN_scale_table[CAN_RD_IF].fixed_scale, 
  				CAN_scale_table[CAN_RD_IF].fixed_offset);
  unsigned int word1 = 0; // htd remaining 
  unsigned int word0 = ETMScaleFactor2(analog_reads[ANA_RD_TEMP].read_cur, CAN_scale_table[CAN_RD_TEMP].fixed_scale, 
  				CAN_scale_table[CAN_RD_TEMP].fixed_offset);

  ETMCanSlaveLogData(
		ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_HEATER_MON, word3, word2, word1, word0);

#endif

#ifdef __A36465
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_AFC_FAST_READINGS,
		     etm_can_next_pulse_count,
		     1,//global_data_A36465.aft_A_sample.reading_scaled_and_calibrated,
		     35,//global_data_A36465.aft_B_sample.reading_scaled_and_calibrated,
		     17//global_data_A36465.aft_filtered_error_for_client
		     );

#endif


}

void ETMCanSlaveLogCustomPacketE(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */

#ifdef __A36746
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_ANALOG_READINGS,
		     global_data_A36746.coolant_temperature_kelvin,
		     global_data_A36746.analog_input_SF6_pressure.reading_scaled_and_calibrated,
		     global_data_A36746.cabinet_temperature_kelvin,
		     0
		     );
#endif


#ifdef __A36224
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_ANALOG_READINGS,
		     global_data_A36224_000.analog_input_coolant_temp.reading_scaled_and_calibrated,
		     global_data_A36224_000.analog_input_SF6_pressure.reading_scaled_and_calibrated,
		     global_data_A36224_000.analog_input_cabinet_temp.reading_scaled_and_calibrated,
		     0       //linac temp
		     );
#endif
  // There is no E packet for HV Lamdba, leave blank

#ifdef __A36487
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_1,
		     (psb_params.grid_width_high1 << 8) & psb_params.grid_width_high0,
		     (psb_params.afc_delay_high << 8) & psb_params.magnetron_current_sample_delay_high,
		     (psb_params.grid_delay_low3 << 8) & psb_params.grid_delay_low2,
		     (psb_params.grid_delay_low1 << 8) & psb_params.grid_delay_low0);
#endif

#ifdef __A36582
  ETMCanSlaveLogData(
		     // DPARKER need to confirm this pointer math
		     ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_ARCS,
		     *((unsigned int*)&global_data_A36582.arc_total + 1),          // This is the high word
		     *((unsigned int*)&global_data_A36582.arc_total),              // This is the low word
		     *((unsigned int*)&global_data_A36582.pulse_this_hv_on + 1),   // This is the high word
		     *((unsigned int*)&global_data_A36582.pulse_this_hv_on)        // This is the high word
		     );
#endif

#ifdef __A36465
  ETMCanSlaveLogData(
		     ETM_CAN_DATA_LOG_REGISTER_AFC_SLOW_SETTINGS,
		     afc_motor.home_position,
		     0, // Dparker return the programed offset here
		     afc_motor.current_position, 
		     global_data_A36465.aft_control_voltage.set_point
		     );

#endif


#ifdef __A35975
  unsigned int word3 = ETMScaleFactor2(analog_sets[ANA_SET_EG].ip_set, CAN_scale_table[CAN_RD_EGSET].fixed_scale, 
  				CAN_scale_table[CAN_RD_EGSET].fixed_offset);
  unsigned int word2 = 0; // low energy Eg set
  unsigned int word1 = ETMScaleFactor2(analog_sets[ANA_SET_EF].ip_set, CAN_scale_table[CAN_RD_EFSET].fixed_scale, 
  				CAN_scale_table[CAN_RD_EFSET].fixed_offset);
  unsigned int word0 = ETMScaleFactor2(analog_sets[ANA_SET_EK].ip_set, CAN_scale_table[CAN_RD_EKSET].fixed_scale, 
  				CAN_scale_table[CAN_RD_EKSET].fixed_offset);

  ETMCanSlaveLogData(
		ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_SET_POINTS, word3, word2, word1, word0);

#endif				 

}

void ETMCanSlaveLogCustomPacketF(void) {
  /* 
     Use this to log Board specific data packet
     This will get executed once per update cycle (1.6 seconds) and will be spaced out in time from the other log data
  */
  
  // There is no F packet for HV Lamdba, leave blank
  
#ifdef __A36487
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_2,
		     (psb_params.pfn_delay_low << 8) & psb_params.dose_sample_delay_low,
		     (psb_params.grid_width_low3 << 8) & psb_params.grid_width_low2,
		     (psb_params.grid_width_low1 << 8) & psb_params.grid_width_low0,
		     (psb_params.afc_delay_low << 8) & psb_params.magnetron_current_sample_delay_low);
#endif
  
  
  
#ifdef __A36582
  ETMCanSlaveLogData(ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_PULSE_COUNT,
		     *((unsigned int*)&global_data_A36582.pulse_total + 3),        // This is the most significant word
		     *((unsigned int*)&global_data_A36582.pulse_total + 2),        
		     *((unsigned int*)&global_data_A36582.pulse_total + 1),        
		     *((unsigned int*)&global_data_A36582.pulse_total)             // This is the least significant word
		     );
#endif

#ifdef __A35975	 // Gun Driver
  ETMCanSlaveLogData(
		ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_FPGA_DATA,
		fpga_ASDR,
		faults_from_ADC,
        control_state,
		ETMScaleFactor2(analog_reads[ANA_RD_EC].read_cur, CAN_scale_table[CAN_RD_EC].fixed_scale, 0));

#endif

}


  



