/*
    Copyright 2012 HobbyPCB LLC

    This file is part of HARDROCK-50 Control Firmware

    HARDROCK-50 Control Firmware is free software: you can redistribute it and/or modify it under the terms of the
    GNU General Public License as published by the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    HARDROCK-50 Control Firmware is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
    Public License for more details.

    You should have received a copy of the GNU General Public License along with HARDROCK-50 Control Firmware. If not, see http://www.gnu.org/licenses/.
*/

// Functions

#include "defs.h"

unsigned int VOLT, TEMP, FWD_PWR, RFL_PWR;

void setTX_OUT() {

     if (bandFlag == 1) {
        setBand();
        bandFlag = 0;
        delay_ms(30);
     }
     TX_OUT = 1;

}

void changeKey() {
     keymode++;
}

void checkRXAnalogs() {
     unsigned int tmp_volt = 1;
     unsigned int tmp_temp = 1;
     float voltage, temperature;
     char volt_float_char[15];
     char temp_float_char[15];
     char tmp_char[8];
     tmp_volt = ADC_Read(VOLT_CH);

     if (tmp_volt != VOLT) {
        VOLT = tmp_volt;
        voltage = tmp_volt * 4;
        voltage = voltage / 1000;
        voltage = voltage * 4.191;
        FloatToStr(voltage, volt_float_char);
        memcpy(VOLT_STR, volt_float_char, 4);
        lcdFlag = 1;
     }
     delay_ms(5);
     // May want to change voltage reference to 1.024V instead of 4.096V
     // for better resolution
     tmp_temp = ADC_READ(TEMP_CH);
     if (tmp_temp != TEMP) {
        TEMP = tmp_temp;
        temperature = tmp_temp * 4;
        temperature = temperature / 10;
        temperature = temperature * 1.8;
        temperature = temperature + 32;
        FloatToStr(temperature, temp_float_char);
        memcpy(TEMP_STR, temp_float_char, 5);
        lcdFlag = 1;
     }

}

void checkTXAnalogs() {

}