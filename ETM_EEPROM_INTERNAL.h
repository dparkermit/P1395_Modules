#ifndef __ETM_EEPROM_INTERNAL_H
#define __ETM_EEPROM_INTERNAL_H

#if defined(__dsPIC30F6014A__)
#define INTERNAL_EEPROM_SIZE_WORDS  2048
#endif



/*
  This module is designed to minic the ETM_EEPROM interface when using the internal eeprom instead of an external EEPROM
  This will only work on pics that have an internal EEPROM (like to 30F6014A)
*/

void ETMEEPromInternalWriteWord(unsigned int register_location, unsigned int data);
/*
  This writes a single word to the EEprom
  Register_location is the location in words (not the physical memory address)
*/

unsigned int ETMEEPromInternalReadWord(unsigned int register_location);
/*
  This reads a single word from the EEprom
  register_location is NOT the internal address. It is the position in words (not the physical memory address)
*/

void ETMEEPromInternalReadPage(unsigned int page_number, unsigned int words_to_read, unsigned int *data);
/*
  This reads data from the eeprom into the data array

  These are page(row) aligned instructions (16 words per page) even on devices that do not have pages.
  If words_to_read > 16 it will be truncated to 16.
  
  NOTE!!!!
  DO NOT READ MORE WORDS THAN YOU HAVE ALLOCATED TO YOUR DATA ARRAY
  IF YOU DO, THE VALUES IN RAM AT ADDRESSES PROCEEDING YOUR ARRAY WILL GET CLOBBERED 
*/


void ETMEEPromInternalWritePage(unsigned int page_number, unsigned int *data);
/*
  This writes data from the data array into the eeprom.

  These are page(row) aligned instructions (16 words per page(row)) even on devices that do not have pages.
  This will always erase and write and 16 words.
  If you don't want to rewrite all 16 words you have two options.
  1) Use the Write word command
  2) Read the values that you don't want to update before writing, and then add those to your write array
*/


#endif
