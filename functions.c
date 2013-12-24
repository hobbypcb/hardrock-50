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

extern char ms_count;
unsigned int FWD_PWR, RFL_PWR, AVE_FWP = 0, AVE_RFP = 0, PEP_FWP = 0;
unsigned int bandUpFlag, bandDownFlag, keyModeFlag, rbDelayFlag;
unsigned int _10msCount = 0, _100msCount, _1000msCount, _1sCount, PEP_cnt = 0;
float fl_fwdpwr, fl_rflpwr, fl_vswr, fl_ratioPlus, fl_ratioMinus, fl_voltFwd, fl_voltRfl;
unsigned int tmp_fwdpwr = 1;
unsigned int tmp_rflpwr = 1;
char fwdpwr_float_char[15];
char vswr_float_char[15];
unsigned int vswr_len;
char tmp_char[8];
unsigned int last_STATE = 7;
unsigned int last_atemp = 0, last_avolt = 0;
unsigned int a_volt = 0, a_temp = 0;
float factor[11] = {1.354014 * SCALE, 1.217817 * SCALE, 1.208466 * SCALE, 1.168001 * SCALE, 1.157946 * SCALE,
                 1.135676 * SCALE, 1.105976 * SCALE, 1.081982 * SCALE, 1.0 * SCALE, 1.023131 * SCALE, 1.03489 * SCALE};



void setTX_ON() {
     if (bandFlag == 1) {
        setBand();
        bandFlag = 0;
        delay_ms(30);
     }
     TX_OUT = 1;
     TX_LED = 1;
     PEP_FWP = 0;
     PEP_cnt = 0;
     PWR_LED = 0;
     lcdFlag = 1;
     txState = 1;
     delay_ms(50);
}

void setTX_OFF() {
     txState = 0;
     TX_OUT = 0;
     TX_LED = 0;
     PWR_LED = 1;
     temperatureFlag = 1;
     voltageFlag = 1;
     lcdFlag = 1;
     delay_ms(50);
}


void checkTemperature() {

     int i, int_temp;
     unsigned int tmp_temp = 0;
     float temperature;
     char temp_float_char[15];
     char temp_int_char[7];


     tmp_temp = ADC_READ(TEMP_CH);
     a_temp = (a_temp + tmp_temp) >> 1;

     temperature = a_temp * 4;
     temperature = temperature / 10;
     if (a_temp != last_atemp){
      last_atemp = a_temp;
         if (tempmode == 0) {
            temperature = temperature * 1.8;
            temperature = temperature + 32;
         }
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
     tmp_volt = ADC_Read(VOLT_CH);

     a_volt = (a_volt + tmp_volt) >> 1;
     voltage = a_volt * 4;
     voltage = voltage / 1000;
     voltage = voltage * 4.191;
 
     if (a_volt != last_avolt){
        last_avolt = voltage;
        FloatToStr(voltage, volt_float_char);
        memcpy(VOLT_STR, volt_float_char, 4);
        lcdFlag = 1;
        voltageFlag = 0;
     }

}



void checkTXAnalogs() {
     int i, j;

     
 //    for (i = 1; i<=10; i++) {
 //        tmp_fwdpwr = tmp_fwdpwr + ADC_Read(FWD_PWR_CH);
 //    }
     tmp_fwdpwr = ADC_Read(FWD_PWR_CH);
     tmp_rflpwr = ADC_Read(RFL_PWR_CH);
     
     if (tmp_fwdpwr > AVE_FWP) AVE_FWP = (AVE_FWP * 2 + tmp_fwdpwr) / 3;
     else AVE_FWP = (AVE_FWP * 14 + tmp_fwdpwr) / 15;
     if (tmp_rflpwr > AVE_RFP) AVE_RFP = (AVE_RFP * 2 + tmp_rflpwr) / 3;
     else AVE_RFP = (AVE_RFP * 14 + tmp_rflpwr) / 15;

     if (tmp_fwdpwr > PEP_FWP) {
        PEP_cnt = 0;
        PEP_FWP = tmp_fwdpwr;
        fl_fwdpwr = PEP_FWP * 4;
        fl_voltFwd = fl_fwdpwr / 1000;
        fl_fwdpwr = fl_voltFwd / 2.7;
        fl_fwdpwr = fl_fwdpwr * fl_fwdpwr;
        fl_fwdpwr = fl_fwdpwr * 65;
        fl_fwdpwr *= factor[band];
        FloatToStr(fl_fwdpwr, fwdpwr_float_char);
         memcpy(PEP_STR, fwdpwr_float_char, 3);
        lcdFlag = 1;     }
     else {
        if (++PEP_cnt > 25) PEP_FWP = 0;
        }
     if (AVE_FWP != FWD_PWR) {
        FWD_PWR = AVE_FWP;
        fl_fwdpwr = AVE_FWP * 4;
        fl_voltFwd = fl_fwdpwr / 1000;
        fl_fwdpwr = fl_voltFwd / 2.7;
        fl_fwdpwr = fl_fwdpwr * fl_fwdpwr;
        fl_fwdpwr = fl_fwdpwr * 65;
        fl_fwdpwr *= factor[band];
        lcdFlag = 1;     }
 
 //    delay_ms(10);
 //    for (j = 1; j<=10; j++) {
 //      tmp_rflpwr = tmp_rflpwr + ADC_Read(RFL_PWR_CH);
 //    }


    if (AVE_RFP != RFL_PWR) {
        RFL_PWR = AVE_RFP;
        fl_rflpwr = AVE_RFP * 4;
        fl_voltRfl = fl_rflpwr / 1000;
        fl_rflpwr = fl_voltRfl / 2.7;
        fl_rflpwr = fl_rflpwr * fl_rflpwr;
        fl_rflpwr = fl_rflpwr * 65;
        fl_rflpwr *= factor[band];
        lcdFlag = 1;
     }
/*FloatToStr(fl_rflpwr, fwdpwr_float_char);
        memcpy(PEP_STR, fwdpwr_float_char, 2);
        lcdFlag = 1;*/
        setPowerMeter(fl_fwdpwr, fl_rflpwr);

}

void calculateVswr() {
     // Calculate VSWR only if fl_fwdpwr > 10
        if (fl_fwdpwr > 10) 
        {
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
     if (_10msCount == 5)
        {
         if (rbDelayFlag == 1) checkTxState();
        }
     if (_10msCount == 50)
        {
          voltageFlag = 1;
          calcSwrFlag = 1;
          temperatureFlag = 1;
          lcdFlag = 1;
          _10msCount = 0;
        }
}

void checkButtons() {
char STATE;

     STATE = PORTB & 0x0f;
     if (last_STATE == STATE) return;
     last_STATE = STATE;
 
     STATE = last_STATE & 0x01;
     if (STATE == 0) {   // Detect one-to-zero transition
         changeBandDisplay(-1);
         bandFlag = 1;
         lcdFlag = 1;
     }

     STATE = last_STATE & 0x02;
     if (STATE == 0) {
        changeBandDisplay(+1);
        bandFlag = 1;
        lcdFlag = 1;
     }

     STATE = last_STATE & 0x04;
     if (STATE == 0) {
        changeKeyMode();
        lcdFlag = 1;
     }
     delay_ms(20);
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

}

// copy const to ram string
char * CopyConst2Ram(char * dest, const char * src){
  char * d ;
  d = dest;
  for(;*dest++ = *src++;)
    ;
  return d;
}