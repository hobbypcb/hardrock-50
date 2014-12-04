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

// Configuration Routines for HARDROCK-50

#include "defs.h"

const char HRBR[] = "HRBR";
const char HRBN[] = "HRBN";
const char HRKX[] = "HRKX";
const char HRMD[] = "HRMD";
const char HRTP[] = "HRTP";
const char HRVT[] = "HRVT";
const char HRAT[] = "HRAT";

extern const char crlfsemi[];
char version = 0x00;

//const char crlfsemi[];

void setBaudRate(char uart, char *found) {
   short acc_baud = 0;
   // Read saved baud from 4th position in eeprom
   acc_baud = EEPROM_Read(4);
   // Check to make sure it's a valid baud rate
   if (acc_baud > 3) { acc_baud = 2; }

   if (found[4] == 0x30) {
      acc_baud = 0;
   } else if (found[4] == 0x31) {
      acc_baud = 1;
   } else if (found[4] == 0x32) {
      acc_baud = 2;
   } else if (found[4] == 0x33) {
      acc_baud = 3;
   } else if (found[4] == ';') {
      UART_send(uart, copyConst2Ram(msg,HRBR));
      ShortToStr(acc_baud, msg);
      UART_send(uart, Ltrim(msg));
      UART_send(uart, copyConst2Ram(msg,crlfsemi));
      return;
   }
   if (acc_baud <= 3) EEPROM_Write(4, acc_baud);
}

void setKxMode(char uart, char *found) {
   short new_kxmode;

   if (found[4] == 0x31) {
      new_kxmode = 1;
   } else if (found[4] == 0x30) {
      new_kxmode = 0;
   } else if (found[4] == ';') {
      UART_send(uart, copyConst2Ram(msg,HRKX));
      ShortToStr(kxmode, msg);
      UART_send(uart, Ltrim(msg));
      UART_send(uart, copyConst2Ram(msg,crlfsemi));
      return;
   }
   if (new_kxmode != kxmode) {
      kxmode = new_kxmode;
      EEPROM_Write(5, kxmode);
      setKX3mode();
   }
}
void setBandser(char uart, char *found){
   unsigned char new_band;
   
   if (found[4] == ';'){
      UART_send(uart, copyConst2Ram(msg,HRBN));
      ShortToStr(band, msg);
      UART_send(uart,Ltrim(msg));
      UART_send(uart, copyConst2Ram(msg,crlfsemi));
      return;
   }

   if (found[5] == ';') new_band = found[4] - 0x30;
   else new_band = (found[4] - 0x30) * 10 + (found[5] - 0x30);

   if (new_band > 10) new_band = _UNK;

   if (new_band != band)
      band = new_band;
      bandFlag = 1;
      eepromUpdateFlag = 1;
      changeBandLCD(1);
}

void setModeser(char uart, char *found){
   char new_mode;

   if (found[4] == ';'){
      UART_send(uart, copyConst2Ram(msg,HRMD));
      ShortToStr(keymode, msg);
      UART_send(uart,Ltrim(msg));
      UART_send(uart, copyConst2Ram(msg,crlfsemi));
      return;
   }
   new_mode = found[4] - 0x30;
   if (new_mode != keymode){
      keymode = new_mode;
      if ((atu_mode == 0) && (keymode > 2)) { keymode = 0; }
      if ((atu_mode != 0) && (keymode > 3)) { keymode = 0; }
      EEPROM_Write(2, keymode);
      setKeyMode();
   }
}

void setTempser(char uart, char *found){
   char new_temp;
   char TMPC[] = "C";
   char TMPF[] = "F";

   if (found[4] == ';'){
      UART_send(uart, copyConst2Ram(msg,HRTP));
      UART_send(uart,Ltrim(TEMP_STR));
      if (tempmode == 0) UART_send(uart,TMPF);
      else UART_send(uart,TMPC);
      UART_send(uart, copyConst2Ram(msg,crlfsemi));
      return;
   }
   
   new_temp = found[4] - 0x30;
   if (new_temp > 1) new_temp = 1;
   if (new_temp != tempmode){
      tempmode = new_temp;
      EEPROM_Write(3, tempmode);
      // Force the temperature to be recalculated.
      checkTemperature(1);
   }
}

void setATUser(char uart, char *found){
   char new_atu;

   if (found[4] == ';'){
      UART_send(uart, copyConst2Ram(msg,HRAT));
      ShortToStr(atu_mode, msg);
      UART_send(uart,Ltrim(msg));
      UART_send(uart, copyConst2Ram(msg,crlfsemi));
      return;
   }
   new_atu = found[4] - 0x30;

   if ((atu_mode != 0) || (new_atu != atu_mode)){
      if (new_atu == 1){
         atu_mode = new_atu;
         EEPROM_Write(13, atu_mode);
         // bypass tuner
         Tuner_Byp(1);
      }
      if (new_atu == 2){
         atu_mode = new_atu;
         EEPROM_Write(13, atu_mode);
         // unbypass tuner
         Tuner_Byp(0);
      }
   }
}

void setVoltser(char uart, char *found){
   char new_temp;

   if (found[4] == ';'){
      UART_send(uart, copyConst2Ram(msg,HRVT));
      UART_send(uart,Ltrim(Volt_STR));
      UART_send(uart, copyConst2Ram(msg,crlfsemi));
      return;
   }
}

void getVersion() {
   EECON1.EEPGD = 1;
   TBLPTRU = 0x20;
   TBLPTRH = 0x00;
   TBLPTRL = 0x00;
   asm {
      TBLRD*+
   }
   version = TABLAT;
   EECON1.EEPGD = 0;
}