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
unsigned int bandUpFlag, bandDownFlag, keyModeFlag, rbDelayFlag;
unsigned int _10msCount, _100msCount, _1000msCount, _1sCount;
float fl_fwdpwr, fl_rflpwr, fl_vswr, fl_ratioPlus, fl_ratioMinus, fl_voltFwd, fl_voltRfl;
unsigned int tmp_fwdpwr = 1;
unsigned int tmp_rflpwr = 1;
char fwdpwr_float_char[15];
char vswr_float_char[15];
char tmp_char[8];


void setTX_ON() {
     txState = 1;
     if (bandFlag == 1) {
        setBand();
        bandFlag = 0;
        delay_ms(30);
     }
     TX_OUT = 1;
     TX_LED = 1;
     PWR_LED = 0;
     lcdFlag = 1;
}

void setTX_OFF() {
     txState = 0;
     TX_OUT = 0;
     TX_LED = 0;
     PWR_LED = 1;
     temperatureFlag = 1;
     voltageFlag = 1;
     lcdFlag = 1;
}

void changeKey() {
     keymode++;
}

void checkTemperature() {

     int i, int_temp;
     unsigned int tmp_temp = 0;
     float temperature;
     char temp_float_char[15];
     char temp_int_char[7];

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
     temperatureFlag = 0;
}

void checkVoltage() {
     int i;
     unsigned int tmp_volt = 0;
     float voltage;
     char volt_float_char[15];
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
     voltageFlag = 0;

}



void checkTXAnalogs() {
     int i, j;

     
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
        
        lcdFlag = 1;
     }
/*FloatToStr(fl_rflpwr, fwdpwr_float_char);
        memcpy(PEP_STR, fwdpwr_float_char, 2);
        lcdFlag = 1;*/
        setPowerMeter(fl_fwdpwr, fl_rflpwr);

}

void calculateVswr() {
     // Calculate VSWR only if fl_fwdpwr > 10
        if (fl_fwdpwr > 10) {
          fl_vswr = fl_voltRfl / fl_voltFwd;
          fl_ratioPlus = 1.0 + fl_vswr;
          fl_ratioMinus = 1.0 - fl_vswr;
          fl_vswr = fl_ratioPlus / fl_ratioMinus;

          FloatToStr(fl_vswr, vswr_float_char);
          memcpy(VSWR_STR, vswr_float_char, 3);
          lcdFlag = 1;
        }
        calcSwrFlag = 0;
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

void processTimerFlags() {
     _10msCount++;
     if (_10msCount > 9) {
        _100msCount = _100msCount + 10;
     }
     if (_100msCount > 99) {
        _1000msCount = _1000msCount + 100;
        _100msCount = 0;
        if (rbDelayFlag == 1) {
           checkTxState();
        }
     }
     if (_1000msCount > 999) {
        _1sCount++;
        _1000msCount = 0;
        voltageFlag = 1;
        calcSwrFlag = 1;
     }
     if (_1sCount > 4) {
        temperatureFlag = 1;
        _1sCount = 0;
     }
     timer0Flag = 0;

}

void checkButtons() {
      if (Button(&PORTB, 0, 20, 1)) {               // Detect logical one
        bandUpFlag = 1;                              // Update flag
      }
      if (bandUpFlag && Button(&PORTB, 0, 20, 0)) {   // Detect one-to-zero transition
         changeBandDisplay(+1);
         bandFlag = 1;
         lcdFlag = 1;                           // Invert PORTC
         bandUpFlag = 0;                              // Update flag
      }

      if (Button(&PORTB, 1, 20, 1)) {               // Detect logical one
        bandDownFlag = 1;                              // Update flag
      }
      if (bandDownFlag && Button(&PORTB, 1, 20, 0)) {
         changeBandDisplay(-1);
         bandFlag = 1;
         lcdFlag = 1;
         bandDownFlag = 0;                              // Update flag
      }
      if (Button(&PORTB, 2, 20, 1)) {               // Detect logical one
         keyModeFlag = 1;                              // Update flag
      }
      if (keyModeFlag && Button(&PORTB, 2, 20, 0)) {
         changeKeyMode();
         lcdFlag = 1;
         keyModeFlag = 0;
      }
}


void checkTxState() {
   if (keyMode == PT) {

      if (PORTB.key == 0 && txState == 1) {
           // The PTT line is off, need to turn TX off
           txState = 0;
           setTX_OFF();
           lcdFlag = 1;
      }
      if (PORTB.key == 1 && txState == 0) {
         // The PTT line is on, we aren't in TX, we need to turn TX on
           setTX_ON();
      }
   }
   if (keyMode == CR) {
      // Carrier Detect when COR Line is LOW
      if (PORTB.cor == 1 && txState == 1) {
           // The COR line is HIGH (OFF), need to turn TX off
           setTX_OFF();
      }
      if (PORTB.cor == 0 && txState == 0) {
         // The COR line is on (LOW), we aren't in TX, we need to turn TX on
           setTX_ON();
      }
   }
   // Clear Interrupt Flag and re-enable interrupts
   INTCON.RBIF = 0;        // clear interrupt flag
   INTCON.RBIE = 1;        // Enable interrupt

}