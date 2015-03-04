#ifndef __P1395_CAN_SLAVE_PUBLIC_H
#define __P1395_CAN_SLAVE_PUBLIC_H

#include "P1395_CAN_CORE.h"



//------------ SLAVE PUBLIC FUNCTIONS AND VARIABLES ------------------- //


// Public Functions
void ETMCanSlaveDoCan(void);
/*
  This function should be called every time through the processor execution loop (which should be on the order of 10-100s of uS)
  If will do the following
  1) Look for an execute can commands
  2) Look for changes in status bits, update the Fault/Inhibit bits and send out a new status command if nessesary
  3) Send out regularly schedule communication (On slaves this is status update and logging data)
*/

void ETMCanSlaveInitialize(void);
/*
  This is called once when the processor starts up to initialize the can bus and all of the can variables
*/


void ETMCanSlaveLogCustomPacketC(void);
void ETMCanSlaveLogCustomPacketD(void);
void ETMCanSlaveLogCustomPacketE(void);
void ETMCanSlaveLogCustomPacketF(void);
/*
  These are used to send out the logging data packets for each board.
  These are sent by the can module once every 1.6 seconds.
  These are made visable so that they can be sent at a higher speed as needed (for pulse by pulse data logging)
*/





#ifdef __A36487
// Only for Pulse Sync Board
void ETMCanSlavePulseSyncSendNextPulseLevel(unsigned int next_pulse_level, unsigned int next_pulse_count);
#endif


#ifdef __A63417
// Only for Ion Pump Board
void ETMCanIonPumpSendTargetCurrentReading(unsigned int target_current_reading, unsigned int energy_level, unsigned int pulse_count);
#endif



#endif
