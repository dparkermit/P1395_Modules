#ifndef __P1395_CAN_MASTER_H
#define __P1395_CAN_MASTER_H

#include "P1395_CAN_CORE.h"
#include "P1395_CAN_CORE_PRIVATE.h"


typedef struct {
  unsigned high_energy_pulse:1;
  unsigned arc_this_pulse:1;
  unsigned tbd_2:1;
  unsigned tbd_3:1;

  unsigned tbd_4:1;
  unsigned tbd_5:1;
  unsigned tbd_6:1;
  unsigned tbd_7:1;

  unsigned tbd_8:1;
  unsigned tbd_9:1;
  unsigned tbd_A:1;
  unsigned tbd_B:1;

  unsigned tbd_C:1;
  unsigned tbd_D:1;
  unsigned tbd_E:1;
  unsigned tbd_F:1;
} HighSpeedLogStatusBits;

typedef struct {
  unsigned int pulse_count;
  HighSpeedLogStatusBits status_bits; //This will contain high_low_energy?, arc_this_pulse?, what else???
  
  unsigned int x_ray_on_seconds_lsw;  // This is the lsw of x_ray_on_seconds, when the ECB recieved the "next pulse level" command
  unsigned int x_ray_on_milliseconds; // This is a representation of the milliseconds, when the ECB recieved the "next pulse level" command

  unsigned int hvlambda_readback_high_energy_lambda_program_voltage;
  unsigned int hvlambda_readback_low_energy_lambda_program_voltage;
  unsigned int hvlambda_readback_peak_lambda_voltage;

  unsigned int afc_readback_current_position;
  unsigned int afc_readback_target_position;
  unsigned int afc_readback_a_input;
  unsigned int afc_readback_b_input;
  unsigned int afc_readback_filtered_error_reading;

  unsigned int ionpump_readback_high_energy_target_current_reading;
  unsigned int ionpump_readback_low_energy_target_current_reading;

  unsigned int magmon_readback_magnetron_high_energy_current;
  unsigned int magmon_readback_magnetron_low_energy_current;

  unsigned int psync_readback_trigger_width_and_filtered_trigger_width;
  unsigned int psync_readback_high_energy_grid_width_and_delay;
  unsigned int psync_readback_low_energy_grid_width_and_delay;
} ETMCanHighSpeedData;
// 19 words



typedef struct {
  // ------------------- ETHERNET CONTROL BOARD --------------------- //
  // Standard Registers for all Boards
  ETMCanStatusRegister   status_data; 
  ETMCanSystemDebugData  debug_data;
  ETMCanCanStatus        can_status;
  ETMCanAgileConfig     configuration;

  unsigned int           status_received_register;  // When a status message is recieved, the corresponding bit is set in this register
  unsigned int           mirror_control_state;
  unsigned long          mirror_system_powered_seconds;
  unsigned long          mirror_system_hv_on_seconds;
  unsigned long          mirror_system_xray_on_seconds;
  unsigned long          mirror_time_seconds_now;
  unsigned int           mirror_average_output_power_watts;
  unsigned int           mirror_thyratron_warmup_counter_seconds;
  unsigned int           mirror_magnetron_heater_warmup_counter_seconds;
  unsigned int           mirror_gun_driver_heater_warmup_counter_seconds;

  unsigned int           mirror_sync_0_control_word; //pulse_inhibit_status_bits;
  unsigned int           unused_2; //software_pulse_enable;
  unsigned int           unused_3; //pulse_sync_disable_requested; // This is used by the CAN interrupt to signal that we need to send a pulse_sync_disable message
  unsigned int           unused_4; //status_connected_boards;   // This register indicates which boards are connected.
  unsigned int           unused_array[14];
} ETMCanRamMirrorEthernetBoard;

extern ETMCanRamMirrorEthernetBoard     etm_can_ethernet_board_data;


typedef struct {
  // ------------------- HV LAMBDA BOARD --------------------- //
  // Standard Registers for all Boards
  ETMCanStatusRegister  status_data; 
  ETMCanSystemDebugData debug_data;
  ETMCanCanStatus       can_status;
  ETMCanAgileConfig     configuration;
  
  // Values that the Ethernet control board sets on HV Lambda
  unsigned int ecb_high_set_point;
  unsigned int ecb_low_set_point;

  // "SLOW" Data that the Ethernet control board reads back from HV Lambda
  unsigned int eoc_not_reached_count;
  unsigned int readback_vmon;
  unsigned int readback_imon;
  unsigned int readback_base_plate_temp;

  // DPARKER still need to add this data to Ethernet Interface
  unsigned int readback_high_vprog;
  unsigned int readback_low_vprog;
  unsigned int readback_peak_lambda_voltage;

  //unsigned int high_vprog;
  //unsigned int low_vprog;
  unsigned int           unused_array[23];

} ETMCanRamMirrorHVLambda;

extern ETMCanRamMirrorHVLambda          etm_can_hv_lambda_mirror;

#define _HV_LAMBDA_NOT_CONFIGURED          etm_can_hv_lambda_mirror.status_data.status_bits.control_2_not_configured
#define _HV_LAMBDA_NOT_CONNECTED           etm_can_hv_lambda_mirror.status_data.status_bits.control_7_ecb_can_not_active
#define _HV_LAMBDA_NOT_READY               etm_can_hv_lambda_mirror.status_data.status_bits.control_0_not_ready


typedef struct {
  // ------------------- ION PUMP BOARD --------------------- //
  // Standard Registers for all Boards
  ETMCanStatusRegister  status_data; 
  ETMCanSystemDebugData debug_data;
  ETMCanCanStatus       can_status;
  ETMCanAgileConfig     configuration;
  
  // Values that the Ethernet control board sets 
  // NONE!!!!
  
  // "SLOW" Data that the Ethernet control board reads back
  unsigned int ionpump_readback_ion_pump_volage_monitor;
  unsigned int ionpump_readback_ion_pump_current_monitor;
  unsigned int ionpump_readback_filtered_high_energy_target_current;
  unsigned int ionpump_readback_filtered_low_energy_target_current;

  unsigned int           unused_array[28];

} ETMCanRamMirrorIonPump;

extern ETMCanRamMirrorIonPump           etm_can_ion_pump_mirror;

#define _ION_PUMP_NOT_READY                   etm_can_ion_pump_mirror.status_data.status_bits.control_0_not_ready
#define _ION_PUMP_NOT_CONFIGURED              etm_can_ion_pump_mirror.status_data.status_bits.control_2_not_configured
#define _ION_PUMP_NOT_CONNECTED               etm_can_ion_pump_mirror.status_data.status_bits.control_7_ecb_can_not_active

typedef struct {
  // -------------------- AFC CONTROL BOARD ---------------//
  // Standard Registers for all Boards
  ETMCanStatusRegister  status_data; 
  ETMCanSystemDebugData debug_data;
  ETMCanCanStatus       can_status;
  ETMCanAgileConfig     configuration;
  
  // Values that the Ethernet control board sets 
  unsigned int afc_home_position;
  int          afc_offset;
    
  // "SLOW" Data that the Ethernet control board reads back
  unsigned int afc_readback_home_position;
  unsigned int afc_readback_offset;
  unsigned int afc_readback_current_position;

  // DPARKER - Need to add this to ethernet interface
  unsigned int afc_readback_afc_a_input_reading;
  unsigned int afc_readback_afc_b_input_reading;
  unsigned int afc_readback_filtered_error_reading;
  unsigned int afc_readback_target_position;
  
  unsigned int aft_control_voltage;
  unsigned int readback_aft_control_voltage;
  unsigned int           unused_array[21];  

} ETMCanRamMirrorAFC;

extern ETMCanRamMirrorAFC               etm_can_afc_mirror;

#define _AFC_NOT_READY                   etm_can_afc_mirror.status_data.status_bits.control_0_not_ready
#define _AFC_NOT_CONFIGURED              etm_can_afc_mirror.status_data.status_bits.control_2_not_configured
#define _AFC_NOT_CONNECTED               etm_can_afc_mirror.status_data.status_bits.control_7_ecb_can_not_active

typedef struct {
  // -------------------- COOLING INTERFACE BOARD ---------------//
  // Standard Registers for all Boards
  ETMCanStatusRegister  status_data; 
  ETMCanSystemDebugData debug_data;
  ETMCanCanStatus       can_status;
  ETMCanAgileConfig     configuration;
  
  // Values that the Ethernet control board sets 
  // NONE!!!!
    
  // "SLOW" Data that the Ethernet control board reads back
  unsigned int cool_readback_hvps_coolant_flow;
  unsigned int cool_readback_magnetron_coolant_flow;
  unsigned int cool_readback_linac_coolant_flow;
  unsigned int cool_readback_circulator_coolant_flow;
  unsigned int cool_readback_hx_coolant_flow;
  unsigned int cool_readback_spare_coolant_flow;
  unsigned int cool_readback_coolant_temperature;
  unsigned int cool_readback_sf6_pressure;
  unsigned int cool_readback_cabinet_temperature;
  unsigned int cool_readback_linac_temperature;
  unsigned int cool_readback_spare_word_0;
  unsigned int cool_readback_spare_word_1;
  unsigned int           unused_array[20];  

} ETMCanRamMirrorCooling;

extern ETMCanRamMirrorCooling           etm_can_cooling_mirror;

#define _COOLING_NOT_READY                   etm_can_cooling_mirror.status_data.status_bits.control_0_not_ready
#define _COOLING_NOT_CONFIGURED              etm_can_cooling_mirror.status_data.status_bits.control_2_not_configured
#define _COOLING_NOT_CONNECTED               etm_can_cooling_mirror.status_data.status_bits.control_7_ecb_can_not_active



typedef struct {
  // -------------------- HEATER/MAGNET INTERFACE BOARD ---------------//
  // Standard Registers for all Boards
  ETMCanStatusRegister  status_data; 
  ETMCanSystemDebugData debug_data;
  ETMCanCanStatus       can_status;
  ETMCanAgileConfig     configuration;
  
  // Values that the Ethernet control board sets 
  unsigned int htrmag_magnet_current_set_point;
  unsigned int htrmag_heater_current_set_point;
      
  // "SLOW" Data that the Ethernet control board reads back
  unsigned int htrmag_readback_heater_current;
  unsigned int htrmag_readback_heater_voltage;
  unsigned int htrmag_readback_magnet_current;
  unsigned int htrmag_readback_magnet_voltage;
  unsigned int htrmag_readback_heater_current_set_point;
  unsigned int htrmag_readback_heater_voltage_set_point;
  unsigned int htrmag_readback_magnet_current_set_point;
  unsigned int htrmag_readback_magnet_voltage_set_point;

  // Additional Control/Interface data
  unsigned int htrmag_heater_current_set_point_scaled;//htrmag_heater_enable;
  unsigned int unused_0;  //htrmag_magnet_enable;
  unsigned int           unused_array[20];  

} ETMCanRamMirrorHeaterMagnet;

extern ETMCanRamMirrorHeaterMagnet      etm_can_heater_magnet_mirror;


#define _HEATER_MAGNET_OFF                  etm_can_heater_magnet_mirror.status_data.status_bits.control_0_not_ready
#define _HEATER_MAGNET_NOT_CONFIGURED       etm_can_heater_magnet_mirror.status_data.status_bits.control_2_not_configured
#define _HEATER_MAGNET_NOT_CONNECTED        etm_can_heater_magnet_mirror.status_data.status_bits.control_7_ecb_can_not_active


typedef struct {
  // -------------------- GUN DRIVER INTERFACE BOARD ---------------//
  // Standard Registers for all Boards
  ETMCanStatusRegister  status_data; 
  ETMCanSystemDebugData debug_data;
  ETMCanCanStatus       can_status;
  ETMCanAgileConfig     configuration;
  
  // Values that the Ethernet control board sets 
  unsigned int gun_high_energy_pulse_top_voltage_set_point;
  unsigned int gun_low_energy_pulse_top_voltage_set_point;
  unsigned int gun_heater_voltage_set_point;
  unsigned int gun_cathode_voltage_set_point;
  
  // "SLOW" Data that the Ethernet control board reads back
  unsigned int gun_readback_high_energy_pulse_top_voltage_monitor;
  unsigned int gun_readback_low_energy_pulse_top_voltage_monitor;
  unsigned int gun_readback_cathode_voltage_monitor;
  unsigned int gun_readback_peak_beam_current;
  unsigned int gun_readback_heater_voltage_monitor;
  unsigned int gun_readback_heater_current_monitor;
  unsigned int gun_readback_heater_time_delay_remaining;
  unsigned int gun_readback_driver_temperature;
  unsigned int gun_readback_high_energy_pulse_top_set_point;
  unsigned int gun_readback_low_energy_pulse_top_set_point;
  unsigned int gun_readback_heater_voltage_set_point;
  unsigned int gun_readback_cathode_voltage_set_point;

  // DPARKER NEED TO ADD TO INTERFACE
  unsigned int gun_readback_fpga_asdr_register;
  unsigned int gun_readback_analog_fault_status;
  unsigned int gun_readback_system_logic_state;
  unsigned int gun_readback_bias_voltage_mon;
  unsigned int           unused_array[12];  

} ETMCanRamMirrorGunDriver;

extern ETMCanRamMirrorGunDriver         etm_can_gun_driver_mirror;


#define _GUN_HEATER_ON                  etm_can_gun_driver_mirror.status_data.status_bits.status_7 // DPARKER Need to put the real status number
#define _GUN_DRIVER_NOT_READY           etm_can_gun_driver_mirror.status_data.status_bits.control_0_not_ready
#define _GUN_DRIVER_NOT_CONFIGURED      etm_can_gun_driver_mirror.status_data.status_bits.control_2_not_configured
#define _GUN_DRIVER_NOT_CONNECTED       etm_can_gun_driver_mirror.status_data.status_bits.control_7_ecb_can_not_active




typedef struct {
  // -------------------- MAGNETRON CURRENT MONITOR BOARD ---------------//
  // Standard Registers for all Boards
  ETMCanStatusRegister  status_data; 
  ETMCanSystemDebugData debug_data;
  ETMCanCanStatus       can_status;
  ETMCanAgileConfig     configuration;
  
  // Values that the Ethernet control board sets 
  // NONE!!!!
  
  // "SLOW" Data that the Ethernet control board reads back
  unsigned int magmon_readback_spare;
  unsigned int magmon_readback_arcs_this_hv_on;
  unsigned int magmon_filtered_high_energy_pulse_current;
  unsigned int magmon_filtered_low_energy_pulse_current;
  unsigned long magmon_arcs_lifetime;
  unsigned long magmon_pulses_this_hv_on;
  unsigned long long magmon_pulses_lifetime;
  unsigned int           unused_array[20];  

} ETMCanRamMirrorMagnetronCurrent;

extern ETMCanRamMirrorMagnetronCurrent  etm_can_magnetron_current_mirror;

#define _PULSE_CURRENT_NOT_READY               etm_can_magnetron_current_mirror.status_data.status_bits.control_0_not_ready
#define _PULSE_CURRENT_NOT_CONFIGURED          etm_can_magnetron_current_mirror.status_data.status_bits.control_2_not_configured
#define _PULSE_CURRENT_NOT_CONNECTED           etm_can_magnetron_current_mirror.status_data.status_bits.control_7_ecb_can_not_active


typedef struct {
  // -------------------- PULSE SYNC BOARD ---------------//
  // Standard Registers for all Boards
  ETMCanStatusRegister  status_data; 
  ETMCanSystemDebugData debug_data;
  ETMCanCanStatus       can_status;
  ETMCanAgileConfig     configuration;
  
  // Values that the Ethernet control board sets 
  unsigned char psync_grid_delay_high_intensity_3;
  unsigned char psync_grid_delay_high_intensity_2;
  unsigned char psync_grid_delay_high_intensity_1;
  unsigned char psync_grid_delay_high_intensity_0;
  unsigned char psync_pfn_delay_high;
  unsigned char psync_dose_sample_delay_high;

  unsigned char psync_grid_width_high_intensity_3;
  unsigned char psync_grid_width_high_intensity_2;
  unsigned char psync_grid_width_high_intensity_1;
  unsigned char psync_grid_width_high_intensity_0;
  unsigned char psync_afc_delay_high;
  unsigned char psync_magnetron_current_sample_delay_high;

  unsigned char psync_grid_delay_low_intensity_3;
  unsigned char psync_grid_delay_low_intensity_2;
  unsigned char psync_grid_delay_low_intensity_1;
  unsigned char psync_grid_delay_low_intensity_0;
  unsigned char psync_pfn_delay_low;
  unsigned char psync_dose_sample_delay_low;
 
  unsigned char psync_grid_width_low_intensity_3;
  unsigned char psync_grid_width_low_intensity_2;
  unsigned char psync_grid_width_low_intensity_1;
  unsigned char psync_grid_width_low_intensity_0;
  unsigned char psync_afc_delay_low;
  unsigned char psync_magnetron_current_sample_delay_low;

  unsigned int  psync_customer_led;

  // "SLOW" Data that the Ethernet control board reads back
  // NONE!!!!!!
  unsigned int           unused_array[19];  

} ETMCanRamMirrorPulseSync;

extern ETMCanRamMirrorPulseSync         etm_can_pulse_sync_mirror;
#define _PULSE_SYNC_NOT_READY              etm_can_pulse_sync_mirror.status_data.status_bits.control_0_not_ready
#define _PULSE_SYNC_NOT_CONFIGURED         etm_can_pulse_sync_mirror.status_data.status_bits.control_2_not_configured
#define _PULSE_SYNC_NOT_CONNECTED          etm_can_pulse_sync_mirror.status_data.status_bits.control_7_ecb_can_not_active
#define _PULSE_SYNC_CUSTOMER_HV_OFF        etm_can_pulse_sync_mirror.status_data.status_bits.status_0
#define _PULSE_SYNC_CUSTOMER_XRAY_OFF      etm_can_pulse_sync_mirror.status_data.status_bits.status_1

// PUBLIC Variables
#define HIGH_SPEED_DATA_BUFFER_SIZE   16
extern ETMCanHighSpeedData high_speed_data_buffer_a[HIGH_SPEED_DATA_BUFFER_SIZE];
extern ETMCanHighSpeedData high_speed_data_buffer_b[HIGH_SPEED_DATA_BUFFER_SIZE];


extern ETMCanHighSpeedData              etm_can_high_speed_data_test;


void ETMCanMasterDoCan(void);
void ETMCanMasterInitialize(void);



// Can Module Buffers
extern ETMCanMessageBuffer etm_can_rx_data_log_buffer;

// Can Module Funtions

//void ETMCanMasterPulseSyncDisable(void);
//unsigned int ETMCanMasterReadyToPulse(void);



void SendCalibrationSetPointToSlave(unsigned int index, unsigned int data_1, unsigned int data_0);

void ReadCalibrationSetPointFromSlave(unsigned int index);

void SendSlaveLoadDefaultEEpromData(unsigned int board_id);

void SendSlaveReset(unsigned int board_id);

void SendToEventLog(unsigned int log_id, unsigned int data);



#define LOG_ID_ENTERED_STATE_STARTUP                                          0x0010
#define LOG_ID_ENTERED_STATE_WAIT_FOR_PERSONALITY_FROM_PULSE_SYNC             0x0011
#define LOG_ID_PERSONALITY_RECEIVED                                           0x0012
#define LOG_ID_PERSONALITY_ERROR_6_4                                          0x0013
#define LOG_ID_PERSONALITY_ERROR_2_5                                          0x0014
#define LOG_ID_ENTERED_STATE_WAITING_FOR_INITIALIZATION                       0x0015
#define LOG_ID_ALL_MODULES_CONFIGURED                                         0x0016
#define LOG_ID_ENTERED_STATE_WARMUP                                           0x0017
#define LOG_ID_WARMUP_DONE                                                    0x0018
#define LOG_ID_ENTERED_STATE_STANDBY                                          0x0019
#define LOG_ID_CUSTOMER_HV_ON                                                 0x001A
#define LOG_ID_ENTERED_STATE_DRIVE_UP                                         0x001B
#define LOG_ID_DRIVEUP_COMPLETE                                               0x001C
#define LOG_ID_CUSTOMER_HV_OFF                                                0x001D
#define LOG_ID_DRIVE_UP_TIMEOUT                                               0x001E
#define LOG_ID_ENTERED_STATE_READY                                            0x001F
#define LOG_ID_CUSTOMER_XRAY_ON                                               0x0020

#define LOG_ID_ENTERED_STATE_XRAY_ON                                          0x0022
#define LOG_ID_CUSTOMER_XRAY_OFF                                              0x0023

#define LOG_ID_ENTERED_STATE_FAULT_HOLD                                       0x0025

#define LOG_ID_ENTERED_STATE_FAULT_RESET                                      0x0027
#define LOG_ID_HV_OFF_FAULTS_CLEAR                                            0x0028
#define LOG_ID_ENTERED_STATE_FAULT_SYSTEM                                     0x0029
#define LOG_ID_FAULT_HTR_MAG_BOARD                                            0x002A
#define LOG_ID_FAULT_HTR_MAG_HEATER_OVER_CURRENT_ABSOLUTE                     0x002B
#define LOG_ID_FAULT_HTR_MAG_HEATER_UNDER_CURRENT_ABSOLUTE                    0x002C
#define LOG_ID_FAULT_HTR_MAG_HEATER_OVER_CURRENT_RELATIVE                     0x002D
#define LOG_ID_FAULT_HTR_MAG_HEATER_UNDER_CURRENT_RELATIVE                    0x002E
#define LOG_ID_FAULT_HTR_MAG_HEATER_OVER_VOLTAGE_ABSOLUTE                     0x002F
#define LOG_ID_FAULT_HTR_MAG_HEATER_UNDER_VOTLAGE_RELATIVE                    0x0030
#define LOG_ID_FAULT_HTR_MAG_MAGNET_OVER_CURRENT_ABSOLUTE                     0x0031
#define LOG_ID_FAULT_HTR_MAG_MAGNET_UNDER_CURRENT_ABSOLUTE                    0x0032
#define LOG_ID_FAULT_HTR_MAG_MAGNET_OVER_CURRENT_RELATIVE                     0x0033
#define LOG_ID_FAULT_HTR_MAG_MAGNET_UNDER_CURRENT_RELATIVE                    0x0034
#define LOG_ID_FAULT_HTR_MAG_MAGNET_OVER_VOLTAGE_ABSOLUTE                     0x0035
#define LOG_ID_FAULT_HTR_MAG_MAGNET_UNDER_VOTLAGE_RELATIVE                    0x0036
#define LOG_ID_FAULT_HTR_MAG_HW_HEATER_OVER_VOLTAGE                           0x0037
#define LOG_ID_FAULT_HTR_MAG_HW_TEMPERATURE_SWITCH                            0x0038
#define LOG_ID_FAULT_HTR_MAG_COOLANT_FAULT                                    0x0039
#define LOG_ID_FAULT_HTR_MAG_CAN_COMMUNICATION_LATCHED                        0x003A
#define LOG_ID_FAULT_GUN_DRIVER_BOARD_HV_OFF                                  0x003B
#define LOG_ID_FAULT_HV_LAMBDA_BOARD                                          0x003C
#define LOG_ID_FAULT_ION_PUMP_BOARD                                           0x003D
#define LOG_ID_FAULT_AFC_BOARD                                                0x003E
#define LOG_ID_FAULT_COOING_INTERFACE_BOARD                                   0x003F
#define LOG_ID_FAULT_GUN_DRIVER_BOARD_GENERAL                                 0x0040
#define LOG_ID_FAULT_PULSE_MONITOR_BOARD                                      0x0041
#define LOG_ID_FAULT_PULSE_SYNC_BOARD                                         0x0042

#define LOG_ID_NOT_CONNECTED_ION_PUMP_BOARD                                   0x0043
#define LOG_ID_NOT_CONNECTED_MAGNETRON_CURRENT_BOARD                          0x0044
#define LOG_ID_NOT_CONNECTED_PULSE_SYNC_BOARD                                 0x0045
#define LOG_ID_NOT_CONNECTED_HV_LAMBDA_BOARD                                  0x0046
#define LOG_ID_NOT_CONNECTED_AFC_BOARD                                        0x0047
#define LOG_ID_NOT_CONNECTED_COOLING_INTERFACE_BOARD                          0x0048
#define LOG_ID_NOT_CONNECTED_GUN_DRIVER                                       0x0049
#define LOG_ID_FAULT_MODULE_NOT_CONFIGURED                                    0x004A
#define LOG_ID_NOT_CONNECTED_HEATER_MAGNET_BOARD                              0x004B




typedef struct {
  unsigned int  event_number; // this resets to zero at power up
  unsigned long event_time;   // this is the custom time format
  unsigned int  event_id;     // This tells what the event was

  // In the future we may add more data to the event;
} TYPE_EVENT;

#define EVENT_ENTRIES    128

extern TYPE_EVENT event_array[EVENT_ENTRIES];
extern unsigned int event_array_write_pointer;
extern unsigned int event_array_gui_pointer;
extern unsigned int event_array_eeprom_pointer;

#endif
