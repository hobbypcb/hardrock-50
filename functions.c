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
#include "built_in.h"

extern char  ms_count = 0;
unsigned int bandUpFlag = 0, bandDownFlag = 0, keyModeFlag = 0;
unsigned int _10msCount = 0, _50msCount = 0, PEP_cnt = 0;
float        fl_fwdpwr  = 0.0, fl_rflpwr    = 0.0, fl_pepPwr     = 0.0;
float        fl_vswr    = 0.0, fl_ratioPlus = 0.0, fl_ratioMinus = 0.0;
float        fl_voltFwd = 0.0, fl_voltRfl   = 0.0, fl_voltPep    = 0.0;
unsigned int tmp_fwdpwr = 1;
unsigned int tmp_rflpwr = 1;
unsigned int last_STATE = 7;
unsigned int last_atemp = 0, last_avolt = 0;
unsigned int a_volt = 0, a_temp = 0;
char         tmpString[16+1];
float        factor[11];
const float  band_factor[11] = {1.354014, 1.217817, 1.208466, 1.168001, 1.157946, 1.135676, 1.105976, 1.081982, 1.0, 1.023131, 1.03489};

const unsigned short num_bits_set[8] = {0, 1, 2, 3, 1, 2, 2, 3};
unsigned short       buttons = 0;
const char ATU_TOP[]   = "   Tuning ...   ";
const char ATU_BYP[]   = " Tuner Bypassed ";
const char ATU_ACT[]   = "  Tuner Active  ";
const char ATU_S[]     = "   Tuned OK     ";
const char ATU_F[]     = "  Tune Failed   ";
const char ATU_L[]     = " Power too low  ";
const char ATU_A[]     = "  Tune Aborted  ";
const char ATU_T[]     = "   Timed Out    ";

void setTxOn() {

   // Supress TX if menu is active
   if (menu_active) return;

   // Supress TX if TX_delay is active
   if (TX_delay_ms != 0) return;

   // Operate band relays.
   if (bandFlag == 1) {
      setBand();
      bandFlag = 0;
      delay_ms(25);
   }

   if ((keymode != QR) && (band != _UNK)) TX_OUT = 1;
   TX_LED       = 1;
   PWR_LED      = 0;
   tx_lcdFlag   = 1;
   txState      = 1;
   delay_ms(40);
}

void setTxOff() {

   // Supress RX (TX_OFF) if RX_delay is active
   if(RX_delay_ms != 0) return;

   txState         = 0;
   TX_OUT          = 0;
   TX_LED          = 0;
   PWR_LED         = 1;
//   lcdFlag         = 1;
   _50msCount      = 0;
   _10msCount      = 0;

   delay_ms(40);
}
void SetFTmode(){
     if (REV_E) return;   //Rev E doen't support FT-817 Mode
     if (FTmode == 1){
        RCSTA2.SPEN = 0;  //disable ACC serial port.
        TRISD.B7 = 1;     //make RD7 an input.
        ANSELD.B7 = 1;    //make RD7 an analog pin.
     }
     else{
        ANSELD.B7 = 0;    //make RD7 a digital pin.
        TRISD.B7 = 1;     //make RD7 an input.
        RCSTA2.SPEN = 1;  //enable ACC serial port.
    }
}

void adjustWattMeter(short percent) {
   short i;
   float fl_scale = (percent==0) ? SCALE : (1.0 + (float)percent/100) * SCALE;

   for (i=0; i<11; i++) {
      factor[i] = band_factor[i] * fl_scale;
   }
}


void checkTemperature(short force) {
   unsigned int tmp_temp = 0;
   float    temperature;

   tmp_temp = ADC_READ(TEMP_CH);
   a_temp = (a_temp + tmp_temp) >> 1;

   temperature = a_temp * 4;
   temperature = temperature / 10;
   if (a_temp != last_atemp || force){
      last_atemp = a_temp;
      if (tempmode == 0) {
         temperature = temperature * 1.8;
         temperature = temperature + 32;
      }
      FloatToStr(temperature, tmpString);
      memcpy(TEMP_STR, tmpString, 3);
      rx_lcdFlag = 1;
    }

   temperatureFlag = 0;
}


void checkVoltage() {
   unsigned int tmp_volt = 0;
   float voltage;

   tmp_volt = ADC_Read(VOLT_CH);

   a_volt  = (a_volt + tmp_volt) >> 1;
   voltage = a_volt * 4;
   voltage = voltage / 1000;
   voltage = voltage * 4.191;

   if (a_volt != last_avolt){
      last_avolt = a_volt;
      FloatToStr(voltage, tmpString);
      memcpy(VOLT_STR, tmpString, 4);
      rx_lcdFlag     = 1;
      voltageFlag = 0;
   }
}
void readFT817(void){
     unsigned int FTdata;
     
     FTdata = ADC_Read(FT817);
     
     if (FTdata < 125) band = _160M;
     else if (FTdata <  208) band = _80M;
     else if (FTdata <  291) band = _40M;
     else if (FTdata <  375) band = _30M;
     else if (FTdata <  458) band = _20M;
     else if (FTdata <  541) band = _17M;
     else if (FTdata <  625) band = _15M;
     else if (FTdata <  708) band = _12M;
     else if (FTdata <  791) band = _10M;
     else if (FTdata <  875) band = _6M;
     else band = _UNK;
     
     bandFlag = 1;
     eepromUpdateFlag = 1;
     changeBandLCD(1);
}

void checkTXAnalogs() {

   if (REV_F) {
      tmp_fwdpwr = ADC_Read(REVF_FWD_PWR_CH);
      tmp_rflpwr = ADC_Read(REVF_RFL_PWR_CH);
   } else {
      tmp_fwdpwr = ADC_Read(FWD_PWR_CH);
      tmp_rflpwr = ADC_Read(RFL_PWR_CH);
   }

   // Compute average forward power.
   if (AVE_FWP == 0) {
      AVE_FWP = tmp_fwdpwr;
   } else if (tmp_fwdpwr > AVE_FWP) {
      AVE_FWP = (AVE_FWP * 2 + tmp_fwdpwr) / 3;
   } else {
      AVE_FWP = (AVE_FWP * 14 + tmp_fwdpwr) / 15;
   }

   // Compute average reverse power.
   if (AVE_RFP == 0) {
      AVE_RFP = tmp_rflpwr;
   } else if (tmp_rflpwr > AVE_RFP) {
      AVE_RFP = (AVE_RFP * 2 + tmp_rflpwr) / 3;
   } else {
      AVE_RFP = (AVE_RFP * 14 + tmp_rflpwr) / 15;
   }

   // Compute peak envelope power (PEP).
   if (tmp_fwdpwr > PEP_FWP) {
      PEP_cnt    = 0;
      PEP_FWP    = tmp_fwdpwr;
      fl_pepPwr  = PEP_FWP * 4;
      fl_voltPEP = fl_pepPwr / 1000;
      fl_pepPwr  = fl_voltPEP / 2.7;
      fl_pepPwr  = fl_pepPwr * fl_pepPwr;
      fl_pepPwr  = fl_pepPwr * 65;
      fl_pepPwr  *= factor[band];
      FloatToStr(fl_pepPwr, tmpString);
      memcpy(PEP_STR, tmpString, 3);
      tx_lcdFlag = 1;
   } else {
      if (++PEP_cnt > 25) PEP_FWP = 0;
   }

   // When average forward power changes, update variables.
   if (AVE_FWP != LAST_AVE_FWP) {
      LAST_AVE_FWP = AVE_FWP;
      fl_fwdpwr    = AVE_FWP * 4;
      fl_voltFwd   = fl_fwdpwr / 1000;
      fl_fwdpwr    = fl_voltFwd / 2.7;
      fl_fwdpwr    = fl_fwdpwr * fl_fwdpwr;
      fl_fwdpwr    = fl_fwdpwr * 65;
      fl_fwdpwr    *= factor[band];
      tx_lcdFlag      = 1;
   }

   // When average reverse power changes, update variables.
   if (AVE_RFP != LAST_AVE_RFP) {
      LAST_AVE_RFP = AVE_RFP;
      fl_rflpwr    = AVE_RFP * 4;
      fl_voltRfl   = fl_rflpwr / 1000;
      fl_rflpwr    = fl_voltRfl / 2.7;
      fl_rflpwr    = fl_rflpwr * fl_rflpwr;
      fl_rflpwr    = fl_rflpwr * 65;
      fl_rflpwr    *= factor[band];
      tx_lcdFlag      = 1;
   }

 //   FloatToStr(fl_rflpwr, tmpString);
 //   memcpy(PEP_STR, tmpString, 2);
 //   lcdFlag = 1;

   setPowerMeter(fl_fwdpwr, fl_rflpwr);
}


void calculateVswr() {

   // Calculate VSWR only if fl_fwdpwr > 10
   if (fl_fwdpwr > 10.0) {
      fl_vswr = fl_voltRfl / fl_voltFwd;
      fl_ratioPlus  = 1.0 + fl_vswr;
      fl_ratioMinus = 1.0 - fl_vswr;
      fl_vswr = fl_ratioPlus / fl_ratioMinus;

      FloatToStr(fl_vswr, tmpString);
      // Pad VSWR string to 3 places;
      tmpString[3] = '\0';
      if (tmpString[2] == '\0') { tmpString[2] = ' '; };
      if (tmpString[1] == '\0') { tmpString[1] = ' '; };

      // Update display only when VSWR changes.
      if (strcmp(VSWR_STR, tmpString)) {
         memcpy(VSWR_STR, tmpString, 3);
         tx_lcdFlag     = 1;
      }
   }
}


void setPowerMeter(float fwdpwr, float rflpwr) {
   unsigned char f, r;
   f = (int)(fwdpwr / 3);
   r = (int)(rflpwr / 3);
   Draw_BG(f,r);
}
void Draw_BG(unsigned char f, unsigned char r){
   unsigned char i;
   
   for (i = 1; i <= 16; i++)  {
      if (f >= i){
         if (r >= i) {
           Lcd_Chr(1, i, meterBoth);
         }
         else {
            Lcd_Chr(1, i, meterTop);
         }
      }
      else {
         Lcd_Chr(1, i, 32);
      }
   }
}

void processTimerFlags() {
   _50msCount++;
   _10msCount++;
   // Every 50 msec, do this.
   if (_10msCount == 5) {
      checkTxState();
      _10msCount = 0;
   }
   // Every 1/2 sec, do this.
   if (_50msCount == 50) {
      voltageFlag     = 1;
      temperatureFlag = 1;
      _50msCount      = 0;

      if (txState == 1) tx_lcdFlag = 1;
      else rx_lcdFlag = 1;
      if (ftmode == 1) readFT817();
   }
}

void processButtons() {
   buttons = checkButtons();
   switch (buttons) {
      case BTN_DN:
         changeBandDisplay(+1);
         bandFlag = 1;
         break;
      case BTN_UP:
         changeBandDisplay(-1);
         bandFlag = 1;
         break;
      case BTN_KY:
         changeKeyMode();
         rx_lcdFlag  = 1;
         break;
      case BTN_LONG_KY:
         displayMenu();
         break;
   }//endswitch
}

void Tuner_Byp(char byp){
     TX_OUT = 0;
     Delay_ms(20);
     Tuner_Snd_Char('*');
     Tuner_Snd_Char('Y');
     Tuner_Snd_Char(byp + 48);
     Tuner_Snd_Char(13);
     Delay_ms(20);
     if ((txState == 1) && (keymode != 3)) TX_OUT = 1;
}

void TXButtons() {
   unsigned int j;
   char C1,C2;

   buttons = checkButtons();
   switch (buttons) {
      case BTN_DN:
       if (atu_mode !=0){
         atu_mode = 1;
         Tuner_Byp(1);   // Bypass tuner
         Lcd_Out(1,1,copyConst2Ram(msg,ATU_BYP));
         EEPROM_Write(13, atu_mode);
         Delay_ms(1500);
       }
       break;
      case BTN_UP:
       if (atu_mode != 0){
         atu_mode = 2;
         Tuner_Byp(0);   // Unbypass tuner
         Lcd_Out(1,1,copyConst2Ram(msg,ATU_ACT));
         EEPROM_Write(13, atu_mode);
         Delay_ms(1500);
        }
        break;
      case BTN_KY:
         if (atu_mode == 1){
            atu_mode = 2;
            Tuner_Byp(0);
            EEPROM_Write(13, atu_mode);
         }
         if (atu_mode == 2){
            TX_OUT = 0; // Amp Off

            // display 'Tuning ...' message
            Lcd_Cmd(_LCD_CLEAR); // Clear display
            Lcd_Out(1,1,copyConst2Ram(msg,ATU_TOP));

            // tell tuner to tune
            Tuner_Snd_Char('*');
            Tuner_Snd_Char('A');
            Tuner_Snd_Char(13);

            j=0;
            C1 = 255;
            // wait up to 6 seconds for tuner to respond
            while ((j++ < 1500) && C1 == 255){
                  C1 = Tuner_Get_Char();
                  if (PORTB.B2 == 0) Tuner_Snd_Char('A'); // If 'KEY MODE' pressed abort tuning
            }
            C1 = Tuner_Get_Char();
            Delay_ms(25);
            
            Tuner_Snd_Char('*');
            Tuner_Snd_Char('S');
            Tuner_Snd_Char(13);

            C1 = Tuner_Get_Char();
            C2 = Tuner_Get_Char();

            if (j > 999) Lcd_Out(1,1,copyConst2Ram(msg,ATU_T));
            else{
                 switch (C1){
                        case 'A':
                        case 'a':
                             Lcd_Out(1,1,copyConst2Ram(msg,ATU_A));
                             break;
                        case 'L':
                        case 'l':
                             Lcd_Out(1,1,copyConst2Ram(msg,ATU_L));
                             break;
                        case 'S':
                        case 's':
                             Lcd_Out(1,1,copyConst2Ram(msg,ATU_S));
                             break;
                        case 'F':
                        case 'f':
                             Lcd_Out(1,1,copyConst2Ram(msg,ATU_F));
                             break;
                 }
            }
            if ((keymode == PT)||(keymode == CR)) TX_OUT = 1;
            Delay_ms(1500);
            Update_LCD();
         }
         break;
      case BTN_LONG_KY:
         break;
   }//endswitch
}

unsigned short checkButtons() {
   unsigned short state;
   unsigned int   cnt;

   buttons = ~PORTB & 0x07;
   state   = buttons;
   cnt     = 0;

   // Debounce switches.
   while (state != 0) {
      // Allow multi-press buttons to "pile-on".
      if ( num_bits_set[state] > num_bits_set[buttons] ) {
         buttons = state;
      }
      delay_ms(1);
      backgroundTasks();
      state = ~PORTB & 0x07;

      // Indicate long button press.
      if (cnt++ > 1000) {
         buttons =  buttons | 0x08;
         break;
      }
   }
   // Note, we exit early on a long press and the user may still have his finger
   // on the button.  The calling routine needs to deal with this!
   return buttons;
}


// Wait for all buttons to be released
void waitButtonRelease() {
   buttons = ~PORTB & 0x07;
   while (buttons != 0) {
      delay_ms(1);
      backgroundTasks();
      buttons = ~PORTB & 0x07;
   }
}

void checkTxState() {
   if (keyMode == PT) {
      if (PORTB.key == 0 && txState == 1) {
         // The PTT line is off, need to turn TX off
         setTxOff();
         TX_delay_ms = 0;
      }
      if (PORTB.key == 1 && txState == 0 && TX_delay_ms == 0) {
         // The PTT line is on, we aren't in TX or waiting to TX, we need to turn TX on.
         if(key_delay) {
            TX_delay_ms = key_delay;   // If key_delay is set, request TX
         } else {                      // .  in 'TX_delay_ms' milli-seconds,
            setTxOn();                 // .  otherwise, transmit now.
         }
      }
   }
   if (keyMode == CR) {
      // Carrier Detect when COR Line is LOW
      if (PORTB.cor == 1 && txState == 1 && RX_delay_ms == 0) {
         // The COR line is HIGH (OFF), need to turn TX off.
         if(cor_htime) {
            RX_delay_ms = cor_htime;   // If cor_htime is set, request RX
         } else {                      // .  in 'RX_delay_ms' milli-seconds,
            setTxOff();                // .  otherwise, end transmit now.
         }
      }
      if (PORTB.cor == 0 && txState == 0) {
         // The COR line is on (LOW), we aren't in TX, we need to turn TX on
         setTxOn();
         RX_delay_ms = 0;
      }
   }
   if (keyMode == QR) {
      // Carrier Detect when COR or PTT Line is LOW
      if (PORTB.cor == 1 && txState == 1 && PORTB.key == 0) {
         // The COR line is HIGH (OFF), need to turn TX off.
         setTxOff();
      }
      if ((PORTB.cor == 0 || PORTB.key == 1) && txState == 0) {
         // The COR line is on (LOW), we aren't in TX, we need to turn TX on
         setTxOn();
      }
   }
}


// Copy ROM constant to RAM string.
char * copyConst2Ram(char * dest, const char * src){
   char * d = dest;
   for(;*dest++ = *src++;)
      ;
   return d;
}


// Write int to EEPROM.
void EEPROM_Write_int(unsigned int address, unsigned int num) {
   EEPROM_Write(address  , Lo(num));
   EEPROM_Write(address+1, Hi(num));
  // Note that the EEPROM delay is only needed between write and read functions.
}


// Read int from EEPROM.
unsigned int EEPROM_Read_int (unsigned int address) {
   unsigned int num = 0;
   Lo(num) = EEPROM_Read(address);
   Hi(num) = EEPROM_Read(address+1);
   return num;
}

// Sends a single character at 19200,N,8,1 out pin RC2.
void Tuner_Snd_Char(char c){
     unsigned short j;

     INTCON.GIE  = 0;  //Global Interrupt Disable
     Delay_us(104);
     T_TXD = 0;
     Delay_us(49);
     for (j = 0; j < 8; j++){
         if ((c & 0x01) == 0) T_TXD = 0;
         else T_TXD = 1;
         c >>= 1;
         Delay_us(48);
     }
     T_TXD = 1;
     INTCON.GIE  = 1;  //Global Interrupt Enable
}

char Tuner_Get_Char(void){
     int i = 0;
     unsigned short L_RXD = 0x04, j;
     char c = 0;
     
     INTCON.GIE  = 0;  //Global Interrupt Disable

     //wait for RXD to go high (stop bit)
     while ((T_RXD == 0) && (i++ < 1000)) Delay_us(1);
     if (i > 999){
        INTCON.GIE  = 1;
        return 255;
     }
     i = 0;
     //wait for RXD to go low (start bit)
     while ((T_RXD == 1) && (i++ < 1000)) Delay_us(1);
     if (i > 999){
        INTCON.GIE  = 1;
        return 255;
     }

     Delay_us(70); //wait until center of 1st bit
     PORTB.B0 = 1;
     
     for (j = 0; j < 8; j++){
         c >>= 1;
         PORTB.B0 = 0;
         if (T_RXD != 0) c += 0x80;
         Delay_us(49);
     }
     INTCON.GIE  = 1;  //Global Interrupt Ensable
     
     return c;
}