// ETM MODULE:  REAL TIME CLOCK OVER I2C
#include <p30f6014A.h>
#include <libpic30.h>
#include "DS3231.h"
#include "ETM_I2C.h"

unsigned char slave_address = RTC_SLAVE_ADDRESS;

unsigned char ConvertToBCD(unsigned char decimal, unsigned char hour);
unsigned char ConvertFromBCD(unsigned char decimal, unsigned char hour);

unsigned char ConfigureDS3231(REAL_TIME_CLOCK* ptr_REAL_TIME_CLOCK, unsigned char I2Cport, unsigned char config){
    ptr_REAL_TIME_CLOCK->control_register = config;
    //unsigned char status;
    ptr_REAL_TIME_CLOCK->I2Cport = I2Cport;

    if (WaitForI2CBusIdle(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
        if (GenerateI2CStart(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
            if (WriteByteI2C(slave_address, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                if (WriteByteI2C(CONTROL_ADDRESS, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                    if (WriteByteI2C(ptr_REAL_TIME_CLOCK->control_register, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                        if (GenerateI2CStop(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                            return 0;
                        }
                        else
                            return 6;
                    }
                    else
                        return 5;
                }
                else
                    return 4;
            }
            else
               return 3;
        }
        else
            return 2;
    }
    else
        return 1;
}


unsigned char SetDateAndTime(REAL_TIME_CLOCK* ptr_REAL_TIME_CLOCK){
    if (WaitForI2CBusIdle(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
        if (GenerateI2CStart(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
            if (WriteByteI2C(slave_address, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                if (WriteByteI2C(SECONDS_ADDRESS, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                    if (WriteByteI2C(ConvertToBCD(ptr_REAL_TIME_CLOCK->second, 0), ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                        if (WriteByteI2C(ConvertToBCD(ptr_REAL_TIME_CLOCK->minute, 0), ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                            if (WriteByteI2C(ConvertToBCD(ptr_REAL_TIME_CLOCK->hour, 1), ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                                if (WriteByteI2C(ConvertToBCD(ptr_REAL_TIME_CLOCK->day, 0), ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                                    if (WriteByteI2C(ConvertToBCD(ptr_REAL_TIME_CLOCK->date, 0), ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                                        if (WriteByteI2C(ConvertToBCD(ptr_REAL_TIME_CLOCK->month, 0), ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                                            if (WriteByteI2C(ConvertToBCD(ptr_REAL_TIME_CLOCK->year, 0), ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                                                if (GenerateI2CStop(ptr_REAL_TIME_CLOCK->I2Cport) == 0)
                                                    return 0;
                                                else
                                                    return 0xC;
                                            }
                                            else
                                                return 0xB;
                                        }
                                        else
                                            return 0xA;
                                    }
                                    else
                                        return 9;
                                }
                                else
                                    return 8;
                            }
                            else
                                return 7;
                        }
                        else
                            return 6;
                    }
                    else
                        return 5;
                }
                else
                    return 4;
            }
            else
               return 3;
        }
        else
            return 2;
    }
    else
        return 1;
}

unsigned char ReadDateAndTime(REAL_TIME_CLOCK* ptr_REAL_TIME_CLOCK){
    unsigned int data;
    unsigned char temp;
    
    if (WaitForI2CBusIdle(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
        if (GenerateI2CStart(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
            if (WriteByteI2C(slave_address, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                if (WriteByteI2C(SECONDS_ADDRESS, ptr_REAL_TIME_CLOCK->I2Cport) == 0) {
                    if (GenerateI2CRestart(ptr_REAL_TIME_CLOCK->I2Cport) == 0) {
                        if (WriteByteI2C(slave_address + 1, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                            data = ReadByteI2C(ptr_REAL_TIME_CLOCK->I2Cport);
                            if (data == 0xFA00)
                                return 7;
                            temp = data & 0x00FF;
                            ptr_REAL_TIME_CLOCK->second = ConvertFromBCD(temp, 0);
                            if (ptr_REAL_TIME_CLOCK->second > 60) {
                                ptr_REAL_TIME_CLOCK->second = 0;
                                return 8;
                            }
                            if (GenerateACK(ptr_REAL_TIME_CLOCK->I2Cport) != 0)
                                return 9;
                            data = ReadByteI2C(ptr_REAL_TIME_CLOCK->I2Cport);
                            if (data == 0xFA00)
                                return 0xA;
                            temp = data & 0x00FF;
                            ptr_REAL_TIME_CLOCK->minute = ConvertFromBCD(temp, 0);
                            if (ptr_REAL_TIME_CLOCK->minute > 60) {
                                ptr_REAL_TIME_CLOCK->minute = 0;
                                return 0xB;
                            }
                            if (GenerateACK(ptr_REAL_TIME_CLOCK->I2Cport) != 0)
                                return 0xC;
                            data = ReadByteI2C(ptr_REAL_TIME_CLOCK->I2Cport);
                            if (data == 0xFA00)
                                return 0xD;
                            temp = data & 0x00FF;
                            ptr_REAL_TIME_CLOCK->hour = ConvertFromBCD(temp, 1);
                            if (ptr_REAL_TIME_CLOCK->hour > 24) {
                                ptr_REAL_TIME_CLOCK->hour = 0;
                                return 0xE;
                            }
                            if (GenerateACK(ptr_REAL_TIME_CLOCK->I2Cport) != 0)
                                return 0xF;
                            data = ReadByteI2C(ptr_REAL_TIME_CLOCK->I2Cport);
                            if (data == 0xFA00)
                                return 0x10;
                            temp = data & 0x00FF;
                            ptr_REAL_TIME_CLOCK->day = ConvertFromBCD(temp, 0);
                            if (ptr_REAL_TIME_CLOCK->day > 7) {
                                ptr_REAL_TIME_CLOCK->day = 0;
                                return 0x11;
                            }
                            if (GenerateACK(ptr_REAL_TIME_CLOCK->I2Cport) != 0)
                                return 0x12;
                            data = ReadByteI2C(ptr_REAL_TIME_CLOCK->I2Cport);
                            if (data == 0xFA00)
                                return 0x13;
                            temp = data & 0x00FF;
                            ptr_REAL_TIME_CLOCK->date = ConvertFromBCD(temp, 0);
                            if (ptr_REAL_TIME_CLOCK->date > 31) {
                                ptr_REAL_TIME_CLOCK->date = 0;
                                return 0x14;
                            }
                            if (GenerateACK(ptr_REAL_TIME_CLOCK->I2Cport) != 0)
                                return 0x15;
                            data = ReadByteI2C(ptr_REAL_TIME_CLOCK->I2Cport);
                            if (data == 0xFA00)
                                return 0x16;
                            temp = data & 0x00FF;
                            ptr_REAL_TIME_CLOCK->month = ConvertFromBCD(temp, 0);
                            if (ptr_REAL_TIME_CLOCK->month > 12) {
                                ptr_REAL_TIME_CLOCK->month = 0;
                                return 0x17;
                            }
                            if (GenerateACK(ptr_REAL_TIME_CLOCK->I2Cport) != 0)
                                return 0x18;
                            data = ReadByteI2C(ptr_REAL_TIME_CLOCK->I2Cport);
                            if (data == 0xFA00)
                                return 0x19;
                            temp = data & 0x00FF;
                            ptr_REAL_TIME_CLOCK->year = ConvertFromBCD(temp, 0);
                            if (ptr_REAL_TIME_CLOCK->year > 99) {
                                ptr_REAL_TIME_CLOCK->year = 0;
                                return 0x1A;
                            }
                            if (GenerateNACK(ptr_REAL_TIME_CLOCK->I2Cport) != 0)
                                return 0x1B;
                            if (GenerateI2CStop(ptr_REAL_TIME_CLOCK->I2Cport) == 0) {
                                return 0;
                            }
                            else
                                return 0x1C;
                        }
                        else
                            return 6;
                    }
                    else
                        return 5;
                }
                else
                    return 4;
            }
            else
               return 3;
        }
        else
            return 2;
    }
    else
        return 1;
}

unsigned int ReadRTCTemperature(REAL_TIME_CLOCK* ptr_REAL_TIME_CLOCK) {
    if (WaitForI2CBusIdle(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
        if (GenerateI2CStart(ptr_REAL_TIME_CLOCK->I2Cport) == 0){
            if (WriteByteI2C(slave_address, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                if (WriteByteI2C(TEMPERATURE_ADDRESS_MSB, ptr_REAL_TIME_CLOCK->I2Cport) == 0) {
                    if (GenerateI2CRestart(ptr_REAL_TIME_CLOCK->I2Cport) == 0) {
                        if (WriteByteI2C(slave_address + 1, ptr_REAL_TIME_CLOCK->I2Cport) == 0){
                            ptr_REAL_TIME_CLOCK->temperature = ReadByteI2C(ptr_REAL_TIME_CLOCK->I2Cport);
                            if (GenerateNACK(ptr_REAL_TIME_CLOCK->I2Cport) != 0)
                                return 8;
                            if (GenerateI2CStop(ptr_REAL_TIME_CLOCK->I2Cport) == 0) {
                                return 0;
                            }
                            else
                                return 9;
                        }
                        else
                            return 6;
                    }
                    else
                        return 5;
                }
                else
                    return 4;
            }
            else
                return 3;
        }
        else
            return 2;
    }
    else
        return 1;
}

unsigned char ConvertFromBCD(unsigned char bcd, unsigned char hour){
    unsigned char twenty = 0;
    unsigned char ten = 0;
    unsigned char decimal = 0;

    if (hour){
        twenty = bcd & 0x20;
        ten = bcd & 0x10;
    }
    else {
        twenty = 0;
        ten = bcd & 0xF0;
    }

    decimal = ((twenty >> 5) * 20) + ((ten >> 4) * 10) + (bcd & 0xF);

    return decimal;
}

unsigned char ConvertToBCD(unsigned char decimal, unsigned char hour){
    unsigned char twenty = 0;
    unsigned char ten = 0;
    unsigned char bcd = 0;
    
    if (hour){
        twenty = decimal / 20;
        decimal -= twenty * 20;
        ten = decimal / 10;
        decimal -= ten * 10;
        if (twenty)
            bcd = 0x20;
        if (ten)
            bcd |= ten << 4;
        bcd |= decimal;
    }
    else {
        ten = decimal / 10;
        decimal -= ten * 10;
        if (ten)
            bcd = ten << 4;
        bcd |= decimal;
    }
    return bcd;
}
unsigned int ReadRTCTimeDifference(REAL_TIME_CLOCK* ptr_rtc_old, REAL_TIME_CLOCK* ptr_rtc_new) {
    unsigned int seconds;

    if (ptr_rtc_old->year == ptr_rtc_new->year) {
        if (ptr_rtc_old->month == ptr_rtc_new->month) {
            if (ptr_rtc_old->date == ptr_rtc_new->date) {
                if (ptr_rtc_old->day == ptr_rtc_new->day) {
                    seconds = (ptr_rtc_new->hour - ptr_rtc_old->hour);
                    if (seconds > 17) //max integer 0xFFFF value
                        return 0xFD20;
                    else
                        seconds *= 3600;

                    if (ptr_rtc_old->minute > ptr_rtc_new->minute)
                        seconds -= (ptr_rtc_old->minute - ptr_rtc_new->minute) * 60;
                    else
                        seconds += (ptr_rtc_new->minute - ptr_rtc_old->minute) * 60;

                    if (ptr_rtc_old->second > ptr_rtc_new->second)
                        seconds -= (ptr_rtc_old->second - ptr_rtc_new->second);
                    else
                        seconds += (ptr_rtc_new->second - ptr_rtc_old->second);
                           
                    return seconds;
                }
                else
                    seconds = 0xFD20;
            }
            else
                seconds = 0xFD20;
        }
        else
            seconds = 0xFD20;
    }
    else
        seconds = 0xFD20;

    if (seconds > 0xFD20)
        seconds = 0xFFFF;

    return seconds;
}

