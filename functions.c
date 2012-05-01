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

unsigned int VOLT = 1000;
unsigned int TEMP = 1000;
unsigned int FWD_PWR, RFL_PWR;

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
     int i, j, int_temp;
     unsigned int tmp_volt = 0;
     unsigned int tmp_temp = 0;
     float voltage, temperature;
     char volt_float_char[15];
     char temp_float_char[15];
     char temp_int_char[7];
     char tmp_char[8];
     for (i = 1; i<=10; i++) {
         tmp_volt = tmp_volt + ADC_Read(VOLT_CH);
     }
     tmp_volt = tmp_volt / 10;

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
     for (i = 1; i<=10; i++) {
         tmp_temp = tmp_temp + ADC_READ(TEMP_CH);
     }
     tmp_temp = tmp_temp / 10;
     tmp_temp = ADC_READ(TEMP_CH);
     if (tmp_temp != TEMP) {
        TEMP = tmp_temp;
        temperature = tmp_temp * 4;
        temperature = temperature / 10;
        temperature = temperature * 1.8;
        temperature = temperature + 32;
        FloatToStr(temperature, temp_float_char);
        memcpy(TEMP_STR, temp_float_char, 3);
        lcdFlag = 1;
     }

}

void checkTXAnalogs() {
     int i, j;
     unsigned int tmp_fwdpwr = 1;
     unsigned int tmp_rflpwr = 1;
     float fl_fwdpwr, fl_rflpwr, fl_vswr, fl_ratioPlus, fl_ratioMinus, fl_voltFwd, fl_voltRfl;
     char fwdpwr_float_char[15];
     char vswr_float_char[15];
     char tmp_char[8];
     
     for (i = 1; i<=10; i++) {
         tmp_fwdpwr = tmp_fwdpwr + ADC_Read(FWD_PWR_CH);
     }
     tmp_fwdpwr = tmp_fwdpwr / 10;

     if (tmp_fwdpwr != FWD_PWR) {
        FWD_PWR = tmp_fwdpwr;
        fl_fwdpwr = tmp_fwdpwr * 4;
        fl_voltFwd = fl_fwdpwr / 1000;
        fl_fwdpwr = fl_voltFwd / 2.7;
        fl_fwdpwr = fl_fwdpwr * fl_fwdpwr;
        fl_fwdpwr = fl_fwdpwr * 50;
        FloatToStr(fl_fwdpwr, fwdpwr_float_char);
        memcpy(PEP_STR, fwdpwr_float_char, 2);
        lcdFlag = 1;     }
     delay_ms(10);
     for (j = 1; j<=10; j++) {
       tmp_rflpwr = tmp_rflpwr + ADC_Read(RFL_PWR_CH);
     }
     tmp_rflpwr = tmp_rflpwr / 10;
     if (tmp_rflpwr != RFL_PWR) {
        RFL_PWR = tmp_rflpwr;
        fl_rflpwr = tmp_rflpwr * 4;
        fl_voltRfl = fl_rflpwr / 1000;
        fl_rflpwr = fl_voltRfl / 2.7;
        fl_rflpwr = fl_rflpwr * fl_rflpwr;
        fl_rflpwr = fl_rflpwr * 50;
        
     // Calculate VSWR
        fl_vswr = fl_voltRfl / fl_voltFwd;
        fl_ratioPlus = 1.0 + fl_vswr;
        fl_ratioMinus = 1.0 - fl_vswr;
        fl_vswr = fl_ratioPlus / fl_ratioMinus;
        
        FloatToStr(fl_vswr, vswr_float_char);
        memcpy(VSWR_STR, vswr_float_char, 3);
        lcdFlag = 1;
     }
/*FloatToStr(fl_rflpwr, fwdpwr_float_char);
        memcpy(PEP_STR, fwdpwr_float_char, 2);
        lcdFlag = 1;*/
        setPowerMeter(fl_fwdpwr, fl_rflpwr);

}

void setPowerMeter(float fwdpwr, float rflpwr) {

     int i;
     float iFlt;
     if (fwdpwr > 0.0) {
        if (rflpwr > 0.0) {
           Lcd_Chr(1, 1, meterBoth);
        } else {
           Lcd_Chr(1, 1, meterTop);
        }
     }
     for (i = 2; i<=16; i++) {
         iFlt = i * 3.125;
         if (fwdpwr > iFlt) {
            if (rflpwr > iFlt) {
               Lcd_Chr(1, i, meterBoth);
            } else {
               Lcd_Chr(1, i, meterTop);
            }
         } else {
            Lcd_Chr(1, i, 32);
         }
         

     }
}