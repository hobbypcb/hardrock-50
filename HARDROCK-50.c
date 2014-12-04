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

#include "defs.h"

char i;                              // Loop variable
unsigned short lcdFlag  = 1;
unsigned short txState  = 0, lastB = 0, timer0Flag = 0, bandFlag = 1;
unsigned short temperatureFlag = 0, voltageFlag = 0, eepromUpdateFlag = 0;
unsigned int AVE_FWP = 0, AVE_RFP = 0, PEP_FWP = 0, LAST_AVE_FWP = 0, LAST_AVE_RFP = 0;
short          menu_active = 0;
unsigned int   TX_delay_ms = 0;
unsigned int   RX_delay_ms = 0;
short atu_mode = 0, meter_cnt = 0;
char freqStr[6];
char ms_count;
Tflag_tag1 flags1 = {0,0,0,0,0,0,0,0};      // init all bits to zero
char msg[70];

// EEPROM variables:
unsigned short band     = 10;
unsigned short keymode  = 0;
unsigned short tempmode = 0; // 0 = F; 1 = C
short          acc_baud;
short          set_baud;
short          kxmode;
short          ftmode;
short          blver;
short          usb_baud;
short          meter_adj; // (+/- 25%)
int            cor_htime; // (2 bytes)
int            key_delay; // (2 bytes)
unsigned int   tuner_freq = 0;
unsigned short tx_lcdFlag, rx_lcdFlag;

void main(){
   OSCCON = 0b01110110;

   init();
   changeBandLCD(1);
   setKeyMode();
   adjustWattMeter(meter_adj);
   checkTemperature(0);
   checkVoltage();

   // Main process (endless) loop.
   ms_count = 0;
   while(1) {
      if (txState == 1) TXButtons();
      else processButtons();
      
      backgroundTasks();
      
      Update_LCD();
      
      delay_ms(1);
   }
}


void backgroundTasks() {
unsigned char fwd = 0, rfl = 0, k;

   // Transmit-only tasks.
   if (txState == 1) {

      // Disable UARTS during TX.
      if (RCSTA1.SPEN == 1) RCSTA1.SPEN = 0;
      if (RCSTA2.SPEN == 1) RCSTA2.SPEN = 0;

      if (keymode != QR) {
         checkTXAnalogs();
         calculateVswr();
      }
      else{
         if (meter_cnt++ >= 100){
            meter_cnt = 0;
            E_F = 0;
            
            // Get SWR
            Tuner_Snd_Char('*');
            Tuner_Snd_Char('W');
            Tuner_Snd_Char(13);
            Get_WM();
            PW_STR[3] = 0;
            if (E_F == 0) LCD_Out(2,5,PW_STR);

            //get Peak Power
            Tuner_Snd_Char('*');
            Tuner_Snd_Char('P');
            Tuner_Snd_Char('P');
            Tuner_Snd_Char(13);
            Get_WM();
            if (E_F == 0) LCD_Out(2,14,PW_STR);

            //get Forward Power
            Tuner_Snd_Char('*');
            Tuner_Snd_Char('P');
            Tuner_Snd_Char('F');
            Tuner_Snd_Char(13);
            Get_WM();
            if (E_F == 0){
               if (PW_STR[1] == '.') FWD = PW_STR[0] - 48;
               if (PW_STR[2] == '.') FWD = 10 *(PW_STR[0] - 48) + (PW_STR[1] - 48);
            }

            //get Reflected Power
            Tuner_Snd_Char('*');
            Tuner_Snd_Char('P');
            Tuner_Snd_Char('R');
            Tuner_Snd_Char(13);
            Get_WM();
            if (E_F == 0){
               if (PW_STR[1] == '.') RFL = PW_STR[0] - 48;
               if (PW_STR[2] == '.') RFL = 10 *(PW_STR[0] - 48) + (PW_STR[1] - 48);
            }
            Draw_BG(FWD,RFL);
         }
      }
   // Receive-only tasks.
   } else {

      if (temperatureFlag) {
         checkTemperature(0);
      }

      if (voltageFlag) {
         checkVoltage();
      }
   }

   // The remaining tasks occur during receive and transmit.

 // if (flags1.newdata) {
 //    UART1_Write(rxbuff[uartPtr]);
 //    flags1.newdata = 0;
 // }

   if (flags1.UART_Buffer_full) {
      uartGrabBuffer();
      findBand(1);
   }

   if (flags1.UART2_Buffer_full) {
      uartGrabBuffer2();
      findBand(2);
   }

   if (++ms_count == 10) {
      ms_count = 0;
      processTimerFlags();
   }

   if (eepromUpdateFlag == 1) {
      // Save band to first position in EEPROM.
      EEPROM_Write(1, band);
      eepromUpdateFlag = 0;
   }
}