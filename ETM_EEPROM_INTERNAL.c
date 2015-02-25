#include <xc.h>
#include <libpic30.h>
#include "ETM_EEPROM.h"


#if defined(__dsPIC30F6014A__)
#define INTERNAL_EEPROM_SIZE_WORDS  2048
#endif

#if defined(__dsPIC30F6010A__)
#define INTERNAL_EEPROM_SIZE_WORDS  2048
#endif

#ifndef INTERNAL_EEPROM_SIZE_WORDS
#define INTERNAL_EEPROM_SIZE_WORDS 0
#endif


__eds__ unsigned int internal_eeprom[INTERNAL_EEPROM_SIZE_WORDS] __attribute__ ((space(eedata))) = {};


void ETMEEPromConfigureExternalDevice(unsigned int size_bytes, unsigned long fcy_clk, unsigned long i2c_baud_rate, unsigned char i2c_address, unsigned char i2c_port) {
}

void ETMEEPromWriteWord(unsigned int register_location, unsigned int data) {
  _prog_addressT write_address;
  
  if (register_location < INTERNAL_EEPROM_SIZE_WORDS) {
    write_address = __builtin_tbladdress(internal_eeprom);
    _wait_eedata();
    _erase_eedata((write_address + register_location*2), _EE_WORD);
    _wait_eedata();
    _write_eedata_word((write_address + register_location*2), data);  
  }
}


unsigned int ETMEEPromReadWord(unsigned int register_location) {
  if (register_location < INTERNAL_EEPROM_SIZE_WORDS) {
    _wait_eedata();
    return internal_eeprom[register_location];
  } else {
    return 0xFFFF;
  }
}


void ETMEEPromWritePage(unsigned int page_number, unsigned int words_to_write, unsigned int *data) {
  _prog_addressT write_address;
  int data_ram[16];
  
  data_ram[0] = (int)data[0];
  data_ram[1] = (int)data[1];
  data_ram[2] = (int)data[2];
  data_ram[3] = (int)data[3];
  data_ram[4] = (int)data[4];
  data_ram[5] = (int)data[5];
  data_ram[6] = (int)data[6];
  data_ram[7] = (int)data[7];
  data_ram[8] = (int)data[8];
  data_ram[9] = (int)data[9];
  data_ram[10] = (int)data[10];
  data_ram[11] = (int)data[11];
  data_ram[12] = (int)data[12];
  data_ram[13] = (int)data[13];
  data_ram[14] = (int)data[14];
  data_ram[15] = (int)data[15];

  if (page_number < (INTERNAL_EEPROM_SIZE_WORDS >> 4)) {
    write_address = __builtin_tbladdress(internal_eeprom);
    write_address += page_number << 5;
    // The requeted page address is within the boundry of this device.
    _wait_eedata();
    _erase_eedata(write_address, _EE_ROW);
    _wait_eedata();
    _write_eedata_row(write_address, data_ram);
  }
}


void ETMEEPromReadPage(unsigned int page_number, unsigned int words_to_read, unsigned int *data) {
  unsigned int starting_register;
  unsigned int n;
  
  starting_register = page_number*16;
  _wait_eedata();
  if (words_to_read > 0) {
    if (words_to_read > 16) {
      words_to_read = 16;
    }
    if (page_number < (INTERNAL_EEPROM_SIZE_WORDS >> 4)) {
      for (n = 0; n < words_to_read ; n++) {
	data[n] = internal_eeprom[starting_register + n];
      }
    }
  }
}
