#ifndef __P1395_CAN_CORE_PRIVATE_H
#define __P1393_CAN_CORE_PRIVATE_H

#include "P1395_MODULE_CONFIG.H"
/*
  This requries FCY_CLK to be defined in order to figure out the CAN clock multiplier
*/

typedef struct {
  unsigned int identifier;
  unsigned int word0;
  unsigned int word1;
  unsigned int word2;
  unsigned int word3;
} ETMCanMessage;


typedef struct {
  unsigned int message_write_index;
  unsigned int message_read_index;
  unsigned int message_write_count;
  unsigned int message_overwrite_count;
  ETMCanMessage message_data[16];
} ETMCanMessageBuffer;


// Can Module Buffers
extern ETMCanMessageBuffer etm_can_rx_message_buffer;
extern ETMCanMessageBuffer etm_can_tx_message_buffer;




void ETMCanRXMessage(ETMCanMessage* message_ptr, volatile unsigned int* rx_register_address);
/*
  This stores the data selected by rx_register_address (C1RX0CON,C1RX1CON,C2RX0CON,C2RX1CON) into the message
  If there is no data RX Buffer then error information is placed into the message
  This clears the RXFUL bit so that the buffer is available to receive another message
  see ETM_CAN_UTILITY.s
*/


void ETMCanRXMessageBuffer(ETMCanMessageBuffer* buffer_ptr, volatile unsigned int* rx_data_address);
/*
  This stores the data selected by rx_data_address (C1RX0CON,C1RX1CON,C2RX0CON,C2RX1CON) into the next available slot in the selected buffer.
  If the message buffer is full the data in the RX buffer is discarded.
  If the RX buffer is empty, nothing is added to the message buffer
  This clears the RXFUL bit so that the buffer is available to receive another message
  see ETM_CAN_UTILITY.s
*/


void ETMCanTXMessage(ETMCanMessage* message_ptr, volatile unsigned int* tx_register_address);
/*
  This moves the message data to the TX register indicated by tx_register_address (C1TX0CON, C1TX1CON, C1TX2CON)
  If the TX register is not empty, the data will be overwritten.
  Also sets the transmit bit to queue transmission
  see ETM_CAN_UTILITY.s
*/


void ETMCanTXMessageBuffer(ETMCanMessageBuffer* buffer_ptr, volatile unsigned int* tx_register_address);
/*
  This moves the oldest message in the buffer to to the TX register indicated by tx_register_address (C1TX0CON, C1TX1CON, C1TX2CON)
  If the TX register is not empty, no data will be transfered and the message buffer state will remain unchanged
  If the message buffer is empty, no data will be transmited and no error will be generated
  Also sets the transmit bit to queue transmission
  see ETM_CAN_UTILITY.s
*/


void ETMCanAddMessageToBuffer(ETMCanMessageBuffer* buffer_ptr, ETMCanMessage* message_ptr);
/*
  This adds a message to the buffer
  If the buffer is full the data is discarded.
  see ETM_CAN_UTILITY.s
*/


void ETMCanReadMessageFromBuffer(ETMCanMessageBuffer* buffer_ptr, ETMCanMessage* message_ptr);
/*
  This moves the oldest message in the buffer to the message_ptr
  If the buffer is empty it returns the error identifier (0b0000111000000000) and fills the data with Zeros.
  see ETM_CAN_UTILITY.s
*/


void ETMCanBufferInitialize(ETMCanMessageBuffer* buffer_ptr);
/*
  This initializes a can message buffer.
  see ETM_CAN_UTILITY.s
*/


unsigned int ETMCanBufferRowsAvailable(ETMCanMessageBuffer* buffer_ptr);
/*
  This returns 0 if the buffer is full, otherwise returns the number of available rows
  see ETM_CAN_UTILITY.s
*/


unsigned int ETMCanBufferNotEmpty(ETMCanMessageBuffer* buffer_ptr);
/*
  Returns 0 if the buffer is Empty, otherwise returns the number messages in the buffer
  see ETM_CAN_UTILITY.s
*/



// Define RX SID Masks
// RECEIVE MODE                                  0bXXXCCCCCCAAAAMX0
#define ETM_CAN_MASTER_RX0_MASK                  0b0001100000000001
#define ETM_CAN_MASTER_RX1_MASK                  0b0001100000000101

#define ETM_CAN_SLAVE_RX0_MASK                   0b0001100000000001
#define ETM_CAN_SLAVE_RX1_MASK                   0b0001100001111101

// Define RX SID Filters
#define ETM_CAN_MSG_LVL_FILTER                   0b0000000000000000
#define ETM_CAN_MSG_SYNC_FILTER                  0b0000100000000000
#define ETM_CAN_MSG_SLAVE_FILTER                 0b0001000000000000
#define ETM_CAN_MSG_MASTER_FILTER                0b0001000000000100
#define ETM_CAN_MSG_DATA_LOG_FILTER              0b0001100000000000
#define ETM_CAN_MSG_FILTER_OFF                   0b0001011111111100


// Define RX SID VALUES
#define ETM_CAN_MSG_MASTER_ADDR_MASK             0b0001111110000100
#define ETM_CAN_MSG_SLAVE_ADDR_MASK              0b0001110001111100

//#define ETM_CAN_MSG_SET_3_RX                     0b0001000000000000
#define ETM_CAN_MSG_SET_2_RX                     0b0001010000000100
#define ETM_CAN_MSG_CMD_RX                       0b0001010010000000
#define ETM_CAN_MSG_STATUS_RX                    0b0001010100000100
//#define ETM_CAN_MSG_SET_1_RX                     0b0001010110000000
#define ETM_CAN_MSG_REQUEST_RX                   0b0001011000000000


// Define TX SID VALUES
#define ETM_CAN_MSG_LVL_TX                       0b0000000000000000
#define ETM_CAN_MSG_SYNC_TX                      0b0100000000000000
//#define ETM_CAN_MSG_SET_3_TX                     0b1000000000000000
#define ETM_CAN_MSG_SET_2_TX                     0b1010000000000100
#define ETM_CAN_MSG_CMD_TX                       0b1010000010000000
#define ETM_CAN_MSG_STATUS_TX                    0b1010100000000100
//#define ETM_CAN_MSG_SET_1_TX                     0b1010100010000000
#define ETM_CAN_MSG_REQUEST_TX                   0b1011000000000000
#define ETM_CAN_MSG_DATA_LOG_TX                  0b1100000000000000


// Can configuration parameters
#define CXCTRL_CONFIG_MODE_VALUE                 0b0000010000000000      // This sets Fcan to 4xFcy
#define CXCTRL_OPERATE_MODE_VALUE                0b0000000000000000      // This sets Fcan to 4xFcy
#define CXCTRL_LOOPBACK_MODE_VALUE               0b0000001000000000      // This sets Fcan to 4xFcy

#define CXCFG1_10MHZ_FCY_VALUE                   0b0000000000000001      // This sets TQ to 4/Fcan
#define CXCFG1_20MHZ_FCY_VALUE                   0b0000000000000011      // This sets TQ to 8/Fcan
#define CXCFG1_25MHZ_FCY_VALUE                   0b0000000000000100      // This sets TQ to 10/Fcan

#if FCY_CLK == 25000000
#define ETM_CAN_CXCFG1_VALUE                CXCFG1_25MHZ_FCY_VALUE
#elif FCY_CLK == 20000000
#define ETM_CAN_CXCFG1_VALUE                CXCFG1_20MHZ_FCY_VALUE
#elif FCY_CLK == 10000000
#define ETM_CAN_CXCFG1_VALUE                CXCFG1_10MHZ_FCY_VALUE
#else
#define ETM_CAN_CXCFG1_VALUE                COMPILE_ERROR_YOU_SHOULD_FIX_THIS
#endif

/*
  Can Bit Timing
  Syncchronization segment     - 1xTq
  Propagation Time Segments    - 2xTq
  Phase Buffer Segment 1       - 3xTq
  Sample Type                  - Single Sample
  Phase Buffer Segment 2       - 4xTq
  Maximum Jump Width(CXCFG1)   - 1xTq

*/
//#define CXCFG2_VALUE                             0b0000001110010001      // This will created a bit timing of 10x TQ


/*
  Can Bit Timing
  Syncchronization segment     - 1xTq
  Propagation Time Segments    - 3xTq
  Phase Buffer Segment 1       - 4xTq
  Sample Type                  - Single Sample
  Phase Buffer Segment 2       - 4xTq
  Maximum Jump Width(CXCFG1)   - 1xTq

*/
#define CXCFG2_VALUE                             0b0000001110011010      // This will created a bit timing of 12x TQ




#define CXTXXCON_VALUE_HIGH_PRIORITY             0b0000000000000011
#define CXTXXCON_VALUE_MEDIUM_PRIORITY           0b0000000000000010
#define CXTXXCON_VALUE_LOW_PRIORITY              0b0000000000000001
#define CXRXXCON_VALUE                           0b0000000000000000

#define CXTXXDLC_VALUE                           0b0000000011000000

typedef struct {
  // Can data log 0
  unsigned int CXEC_reg;   // THIS is now the MAX instead of instantaneous value of CXEC
  unsigned int error_flag;
  unsigned int tx_1;
  unsigned int tx_2;
  
  // Can data log 1
  unsigned int rx_0_filt_0; 
  unsigned int rx_0_filt_1;
  unsigned int rx_1_filt_2;  
  unsigned int isr_entered;  // THIS IS Now a logical or of the CXINTF register every time the can ISR is entered

  // Can data log 2
  unsigned int unknown_message_identifier;
  unsigned int invalid_index; 
  unsigned int address_error;
  unsigned int tx_0;
  
  // Can data log 3
  unsigned int message_tx_buffer_overflow;
  unsigned int message_rx_buffer_overflow;
  unsigned int data_log_rx_buffer_overflow;
  unsigned int timeout;

} ETMCanCanStatus;

// Can Module Debug and Status registers
extern ETMCanCanStatus       local_can_errors;


//------------------------------- Specific Board and Command Defines -------------------------- // 

#define ETM_CAN_ADDR_ETHERNET_BOARD                                     14
#define ETM_CAN_ADDR_ION_PUMP_BOARD                                     1
#define ETM_CAN_ADDR_MAGNETRON_CURRENT_BOARD                            2
#define ETM_CAN_ADDR_PULSE_SYNC_BOARD                                   3
#define ETM_CAN_ADDR_HV_LAMBDA_BOARD                                    4
#define ETM_CAN_ADDR_AFC_CONTROL_BOARD                                  5
#define ETM_CAN_ADDR_COOLING_INTERFACE_BOARD                            6
#define ETM_CAN_ADDR_HEATER_MAGNET_BOARD                                7
#define ETM_CAN_ADDR_GUN_DRIVER_BOARD                                   8

typedef struct {
  unsigned unused_0:1;
  unsigned ion_pump_board:1;
  unsigned magnetron_current_board:1;
  unsigned pulse_sync_board:1;
  unsigned hv_lambda_board:1;
  unsigned afc_board:1;
  unsigned cooling_interface_board:1;
  unsigned heater_magnet_board:1;
  unsigned gun_driver_board:1;
  unsigned unused_9:1;
  unsigned unused_10:1;
  unsigned unused_11:1;
  unsigned unused_12:1;
  unsigned unused_13:1;
  unsigned ethernet_board:1;
  unsigned unused_15:1;
} P1395BoardBits;

/*
#define ETM_CAN_BIT_ETHERNET_BOARD                                      0b0100000000000000
#define ETM_CAN_BIT_ION_PUMP_BOARD                                      0b0000000000000010
#define ETM_CAN_BIT_MAGNETRON_CURRENT_BOARD                             0b0000000000000100
#define ETM_CAN_BIT_PULSE_SYNC_BOARD                                    0b0000000000001000
#define ETM_CAN_BIT_HV_LAMBDA_BOARD                                     0b0000000000010000
#define ETM_CAN_BIT_AFC_CONTROL_BOARD                                   0b0000000000100000
#define ETM_CAN_BIT_COOLING_INTERFACE_BOARD                             0b0000000001000000
#define ETM_CAN_BIT_HEATER_MAGNET_BOARD                                 0b0000000010000000
#define ETM_CAN_BIT_GUN_DRIVER_BOARD                                    0b0000000100000000
*/

// Default Register Locations
#define ETM_CAN_REGISTER_DEFAULT_CMD_RESET_MCU                          0x001
#define ETM_CAN_REGISTER_DEFAULT_CMD_RESET_ANALOG_CALIBRATION           0x003


// Board Specific Register Locations
#define ETM_CAN_REGISTER_HV_LAMBDA_SET_1_LAMBDA_SET_POINT               0x4200

#define ETM_CAN_REGISTER_AFC_SET_1_HOME_POSITION_AND_OFFSET             0x5200
#define ETM_CAN_REGISTER_AFC_CMD_DO_AUTO_ZERO                           0x5201
#define ETM_CAN_REGISTER_AFC_CMD_SELECT_AFC_MODE                        0x5202
#define ETM_CAN_REGISTER_AFC_CMD_SELECT_MANUAL_MODE                     0x5203
#define ETM_CAN_REGISTER_AFC_CMD_SET_MANUAL_TARGET_POSITION             0x5204
#define ETM_CAN_REGISTER_AFC_CMD_RELATIVE_MOVE_MANUAL_TARGET            0x5205

#define ETM_CAN_REGISTER_COOLING_CMD_SF6_PULSE_LIMIT_OVERRIDE           0x6200
#define ETM_CAN_REGISTER_COOLING_CMD_SF6_LEAK_LIMIT_OVERRIDE            0x6201
#define ETM_CAN_REGISTER_COOLING_CMD_RESET_BOTTLE_COUNT                 0x6202

#define ETM_CAN_REGISTER_HEATER_MAGNET_SET_1_CURRENT_SET_POINT          0x7200

#define ETM_CAN_REGISTER_GUN_DRIVER_SET_1_GRID_TOP_SET_POINT            0x8200
#define ETM_CAN_REGISTER_GUN_DRIVER_SET_1_HEATER_CATHODE_SET_POINT      0x8201

#define ETM_CAN_REGISTER_PULSE_SYNC_SET_1_HIGH_ENERGY_TIMING_REG_0      0x3200
#define ETM_CAN_REGISTER_PULSE_SYNC_SET_1_HIGH_ENERGY_TIMING_REG_1      0x3201
#define ETM_CAN_REGISTER_PULSE_SYNC_SET_1_LOW_ENERGY_TIMING_REG_0       0x3202
#define ETM_CAN_REGISTER_PULSE_SYNC_SET_1_LOW_ENERGY_TIMING_REG_1       0x3203
#define ETM_CAN_REGISTER_PULSE_SYNC_SET_1_CUSTOMER_LED_OUTPUT           0x3204

#define ETM_CAN_REGISTER_ECB_SET_2_TARGET_CURRENT_MON                   0xE200


//------------------ DATA LOGGING REGISTERS --------------------------//

// Default data logging registers
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_ERROR_0                       0x0
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_ERROR_1                       0x1
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_0                       0x2
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_1                       0x3
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_2                       0x4
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_DEBUG_3                       0x5
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CONFIG_0                      0x6
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CONFIG_1                      0x7
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_0                   0x8
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_1                   0x9
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_2                   0xA
#define ETM_CAN_DATA_LOG_REGISTER_DEFAULT_CAN_ERROR_3                   0xB


#define ETM_CAN_DATA_LOG_REGISTER_HV_LAMBDA_FAST_PROGRAM_VOLTAGE        0x4C
#define ETM_CAN_DATA_LOG_REGISTER_HV_LAMBDA_SLOW_SET_POINT              0x4D

#define ETM_CAN_DATA_LOG_REGISTER_ION_PUMP_SLOW_MONITORS                0x1C

#define ETM_CAN_DATA_LOG_REGISTER_AFC_FAST_POSITION                     0x5C
#define ETM_CAN_DATA_LOG_REGISTER_AFC_FAST_READINGS                     0x5D
#define ETM_CAN_DATA_LOG_REGISTER_AFC_SLOW_SETTINGS                     0x5E

#define ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_FAST_PREVIOUS_PULSE     0x2C
#define ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_FILTERED_PULSE     0x2D
#define ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_ARCS               0x2E
#define ETM_CAN_DATA_LOG_REGISTER_MAGNETRON_MON_SLOW_PULSE_COUNT        0x2F

#define ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_0                   0x6C
#define ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_FLOW_1                   0x6D
#define ETM_CAN_DATA_LOG_REGISTER_COOLING_SLOW_ANALOG_READINGS          0x6E

#define ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_READINGS           0x7C
#define ETM_CAN_DATA_LOG_REGISTER_HEATER_MAGNET_SLOW_SET_POINTS         0x7D

#define ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_PULSE_TOP_MON         0x8C
#define ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_HEATER_MON            0x8D
#define ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_SLOW_SET_POINTS            0x8E
#define ETM_CAN_DATA_LOG_REGISTER_GUN_DRIVER_FPGA_DATA                  0x8F          

#define ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_FAST_TRIGGER_DATA          0x3C
#define ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_0         0x3D
#define ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_1         0x3E
#define ETM_CAN_DATA_LOG_REGISTER_PULSE_SYNC_SLOW_TIMING_DATA_2         0x3F


// This allows the module to be configured to use CAN1 or CAN2
#ifdef __USE_CAN_1

#define _CXIE                       _C1IE
#define _CXIF                       _C1IF
#define _CXIP                       _C1IP
#define CXTX0CON                    C1TX0CON
#define CXTX1CON                    C1TX1CON
#define CXINTF                      C1INTF
#define CXCTRL                      C1CTRL
#define CXCFG1                      C1CFG1
#define CXCFG2                      C1CFG2
#define CXRXM0SID                   C1RXM0SID
#define CXRXM1SID                   C1RXM1SID
#define CXRXF0SID                   C1RXF0SID
#define CXRXF1SID                   C1RXF1SID
#define CXRXF2SID                   C1RXF2SID
#define CXRXF3SID                   C1RXF3SID
#define CXRXF4SID                   C1RXF4SID
#define CXRXF5SID                   C1RXF5SID
#define CXTX2CON                    C1TX2CON
#define CXTX0DLC                    C1TX0DLC
#define CXTX1DLC                    C1TX1DLC
#define CXTX2DLC                    C1TX2DLC
#define CXRX0CON                    C1RX0CON
#define CXRX1CON                    C1RX1CON
#define CXRX1SID                    C1RX1SID
#define CXRX1B1                     C1RX1B1
#define CXEC                        C1EC
#define CXRX1CONbits                C1RX1CONbits
#define CXCTRLbits                  C1CTRLbits
#define CXTX0CONbits                C1TX0CONbits
#define CXTX1CONbits                C1TX1CONbits
#define CXINTFbits                  C1INTFbits
#define CXRX0CONbits                C1RX0CONbits
#define CXINTEbits                  C1INTEbits
#define _CXInterrupt                _C1Interrupt

#else

#define _CXIE                       _C2IE
#define _CXIF                       _C2IF
#define _CXIP                       _C2IP
#define CXTX0CON                    C2TX0CON
#define CXTX1CON                    C2TX1CON
#define CXINTF                      C2INTF
#define CXCTRL                      C2CTRL
#define CXCFG1                      C2CFG1
#define CXCFG2                      C2CFG2
#define CXRXM0SID                   C2RXM0SID
#define CXRXM1SID                   C2RXM1SID
#define CXRXF0SID                   C2RXF0SID
#define CXRXF1SID                   C2RXF1SID
#define CXRXF2SID                   C2RXF2SID
#define CXRXF3SID                   C2RXF3SID
#define CXRXF4SID                   C2RXF4SID
#define CXRXF5SID                   C2RXF5SID
#define CXTX2CON                    C2TX2CON
#define CXTX0DLC                    C2TX0DLC
#define CXTX1DLC                    C2TX1DLC
#define CXTX2DLC                    C2TX2DLC
#define CXRX0CON                    C2RX0CON
#define CXRX1CON                    C2RX1CON
#define CXRX1SID                    C2RX1SID
#define CXRX1B1                     C2RX1B1
#define CXEC                        C2EC
#define CXRX1CONbits                C2RX1CONbits
#define CXCTRLbits                  C2CTRLbits
#define CXTX0CONbits                C2TX0CONbits
#define CXTX1CONbits                C2TX1CONbits
#define CXINTFbits                  C2INTFbits
#define CXRX0CONbits                C2RX0CONbits
#define CXINTEbits                  C2INTEbits
#define _CXInterrupt                _C2Interrupt

#endif


#define MacroETMCanCheckTXBuffer() if (!CXTX0CONbits.TXREQ) { _CXIF = 1; }


#endif
