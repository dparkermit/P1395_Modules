#include "LTC265X.h"

/*
  DPARKER - I know that there are problems with the multichannel DAC command from monitoring the LINAC System.  This needs to be debugged
  In lab it looked like full dac updates were taking over 1ms.
  On recent testing in lab it appears to work (300us write time or so, I don't know what the problem was before)
  The "new version" has been tested at 220uS for full update
  Consider trying to figure out how to use the LDAC pin.  Is it even possible with our methodology
*/


unsigned int LTC265X_single_channel_error_count = 0;
unsigned int LTC265X_all_channel_error_count = 0;


void SetupLTC265X(LTC265X* ptr_LTC265X, unsigned char spi_port, unsigned long fcy_clk, unsigned long spi_bit_rate, unsigned long pin_chip_select_not, unsigned long pin_load_dac_not) {
  
  ptr_LTC265X->spi_port            = spi_port;
  ptr_LTC265X->pin_chip_select_not = pin_chip_select_not;
  ptr_LTC265X->pin_load_dac_not    = pin_load_dac_not;  

  ptr_LTC265X->pin_dac_clear_not   = _PIN_NOT_CONNECTED;
  ptr_LTC265X->pin_por_select      = _PIN_NOT_CONNECTED;

  ETMSetPin(ptr_LTC265X->pin_dac_clear_not);
  ETMClearPin(ptr_LTC265X->pin_load_dac_not);
  ETMSetPin(ptr_LTC265X->pin_chip_select_not);
  if (ptr_LTC265X->por_select_value) {
    ETMSetPin(ptr_LTC265X->pin_por_select);
  } else {
    ETMClearPin(ptr_LTC265X->pin_por_select);
  }
  
  ETMPinTrisOutput(ptr_LTC265X->pin_dac_clear_not);
  ETMPinTrisOutput(ptr_LTC265X->pin_load_dac_not);  
  ETMPinTrisOutput(ptr_LTC265X->pin_chip_select_not);
  ETMPinTrisOutput(ptr_LTC265X->pin_por_select);

  ConfigureSPI(ptr_LTC265X->spi_port, LTC265X_SPI_CON_VALUE, LTC265X_SPI_CON2_VALUE, LTC265X_SPI_STAT_VALUE, spi_bit_rate, fcy_clk);  
}


void ClearOutputsLTC265X(LTC265X* ptr_LTC265X) {
  ETMClearPin(ptr_LTC265X->pin_dac_clear_not);
  __asm__ ("nop");
  __asm__ ("nop");
  __asm__ ("nop");
  __asm__ ("nop");
  ETMSetPin(ptr_LTC265X->pin_dac_clear_not);
}



unsigned char WriteLTC265X(LTC265X* ptr_LTC265X, unsigned int command_word, unsigned int data_word) {
  // See h File For Documentation
  
  unsigned char spi_error;
  unsigned int command_word_readback;
  unsigned int data_word_readback;
  unsigned long temp;
  
  spi_error = 0;
  
  ETMClearPin(ptr_LTC265X->pin_chip_select_not);

  temp = SendAndReceiveSPI(command_word, ptr_LTC265X->spi_port);
  if (temp == 0x11110000) {
    spi_error = 0b00000001;
  } 

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(data_word, ptr_LTC265X->spi_port);
    if (temp == 0x11110000) {
      spi_error |= 0b00000010;
    } 
  }
 

  ETMSetPin(ptr_LTC265X->pin_chip_select_not);
  __asm__ ("nop");
  __asm__ ("nop");
  __asm__ ("nop");
  __asm__ ("nop");
  ETMClearPin(ptr_LTC265X->pin_chip_select_not);

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(LTC265X_CMD_NO_OPERATION, ptr_LTC265X->spi_port);
    command_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00000100;
    } 
  }

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(0, ptr_LTC265X->spi_port);
    data_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00001000;
    } 
  }

  ETMSetPin(ptr_LTC265X->pin_chip_select_not);


  if (command_word_readback != command_word) {
    spi_error |= 0b00010000;
  }
  if (data_word_readback != data_word) {
    spi_error |= 0b00100000;
  }
  
  if (spi_error != 0) {
    LTC265X_single_channel_error_count++;
  }
  return spi_error;
}


unsigned char WriteLTC265XTwoChannels(LTC265X* ptr_LTC265X, unsigned int command_word_one, unsigned int data_word_one, unsigned command_word_two, unsigned int data_word_two) {
  // See h File For Documentation
  
  unsigned char spi_error;
  unsigned int command_word_readback;
  unsigned int data_word_readback;
  unsigned long temp;
  
  
  spi_error = 0;
  
  ETMClearPin(ptr_LTC265X->pin_chip_select_not);
  
  temp = SendAndReceiveSPI(command_word_one, ptr_LTC265X->spi_port);
  if (temp == 0x11110000) {
    spi_error = 0b00000001;
  } 

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(data_word_one, ptr_LTC265X->spi_port);
    if (temp == 0x11110000) {
      spi_error |= 0b00000010;
    } 
  }
 

  ETMSetPin(ptr_LTC265X->pin_chip_select_not);
  __asm__ ("nop");
  __asm__ ("nop");
  __asm__ ("nop");
  __asm__ ("nop");
  ETMClearPin(ptr_LTC265X->pin_chip_select_not);

  
  temp = SendAndReceiveSPI(command_word_two, ptr_LTC265X->spi_port);
  command_word_readback = temp & 0xFFFF;
  if (temp == 0x11110000) {
    spi_error = 0b00000001;
  } 

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(data_word_two, ptr_LTC265X->spi_port);
    data_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00000010;
    } 
  }


  if (command_word_readback != command_word_one) {
    spi_error |= 0b00010000;
  }
  if (data_word_readback != data_word_one) {
    spi_error |= 0b00100000;
  }


  ETMSetPin(ptr_LTC265X->pin_chip_select_not);
  __asm__ ("nop");
  __asm__ ("nop");
  __asm__ ("nop");
  __asm__ ("nop");
  ETMClearPin(ptr_LTC265X->pin_chip_select_not);


  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(LTC265X_CMD_NO_OPERATION, ptr_LTC265X->spi_port);
    command_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00000100;
    } 
  }

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(0, ptr_LTC265X->spi_port);
    data_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00001000;
    } 
  }

  ETMSetPin(ptr_LTC265X->pin_chip_select_not);

  if (command_word_readback != command_word_two) {
    spi_error |= 0b00010000;
  }
  if (data_word_readback != data_word_two) {
    spi_error |= 0b00100000;
  }

  if (spi_error != 0) {
    LTC265X_single_channel_error_count++;
  }
  return spi_error;

}


unsigned char WriteLTC2656AllDacChannels(LTC265X* ptr_LTC265X, unsigned int *dac_array) {
  // See h File For Documentation
  
  unsigned char spi_error;
  unsigned int command_word_readback;
  unsigned int data_word_readback;
  unsigned long temp;
  unsigned char dac_number;

  spi_error = 0;
  dac_number = 0;
  
  while ((spi_error == 0) && (dac_number < 8)) {
    // Send out two 16 bit words on the SPI BUS

    ETMClearPin(ptr_LTC265X->pin_chip_select_not);
    
    temp = SendAndReceiveSPI(LTC265X_CMD_WRITE_AND_UPDATE_N | dac_number, ptr_LTC265X->spi_port);
    command_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error = 0b00000001;
    } 
    
    if (spi_error == 0) { 
      temp = SendAndReceiveSPI(dac_array[dac_number], ptr_LTC265X->spi_port);
      data_word_readback = temp & 0xFFFF;
      if (temp == 0x11110000) {
	spi_error |= 0b00000010;
      } 
    }

    // Check that the command/data returned match the command/data sent out on the previous cycle
    if (dac_number > 0) {
      if (command_word_readback != (LTC265X_CMD_WRITE_AND_UPDATE_N | (dac_number - 1))) {
	spi_error |= 0b00000100;
      }
      if (data_word_readback != dac_array[dac_number-1]) {
	spi_error |= 0b00001000;
      }   
    }
  
    ETMSetPin(ptr_LTC265X->pin_chip_select_not);
    dac_number++;
  }

  // Read back the error check from the last command/data string sent out
  ETMClearPin(ptr_LTC265X->pin_chip_select_not);

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(LTC265X_CMD_NO_OPERATION, ptr_LTC265X->spi_port);
    command_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00000001;
    } 
  }

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(0, ptr_LTC265X->spi_port);
    data_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00000010;
    } 
  }
  
  ETMSetPin(ptr_LTC265X->pin_chip_select_not);
  
  if (command_word_readback != (LTC265X_CMD_WRITE_AND_UPDATE_N | (dac_number - 1))) {
    spi_error |= 0b00010000;
  }
  if (data_word_readback != dac_array[dac_number-1]) {
    spi_error |= 0b00100000;
  }

  if (spi_error != 0) {
    LTC265X_all_channel_error_count++;
  }
  return spi_error;
}
