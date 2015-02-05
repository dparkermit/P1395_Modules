#include <libpic30.h>
#include <xc.h>
#include "ETM_EEPROM_INTERNAL.h"


__eds__ unsigned int internal_eeprom[INTERNAL_EEPROM_SIZE_WORDS] __attribute__ ((space(eedata))) = {};


void ETMEEPromInternalWriteWord(unsigned int register_location, unsigned int data) {
  _prog_addressT write_address;
  
  if (register_location < INTERNAL_EEPROM_SIZE_WORDS) {
    write_address = __builtin_tbladdress(internal_eeprom);
    _wait_eedata();
    _erase_eedata((write_address + register_location*2), _EE_WORD);
    _wait_eedata();
    _write_eedata_word((write_address + register_location*2), data);  
  }
}

unsigned int ETMEEPromInternalReadWord(unsigned int register_location) {
  if (register_location < INTERNAL_EEPROM_SIZE_WORDS) {
    _wait_eedata();
    return internal_eeprom[register_location];
  } else {
    return 0xFFFF;
  }
}


void ETMEEPromInternalReadPage(unsigned int page_number, unsigned int words_to_read, unsigned int *data) {
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

void ETMEEPromInternalWritePage(unsigned int page_number, unsigned int *data) {
  _prog_addressT write_address;

  if (page_number < (INTERNAL_EEPROM_SIZE_WORDS >> 4)) {
    write_address = __builtin_tbladdress(internal_eeprom);
    write_address += page_number << 5;
    // The requeted page address is within the boundry of this device.
    _wait_eedata();
    _erase_eedata(write_address, _EE_ROW);
    _wait_eedata();
    _write_eedata_row(write_address, (int*)data);
  }
}
