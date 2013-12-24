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
unsigned short lcdFlag = 1;
unsigned short txState = 0, lastB = 0, timer0Flag = 0, bandFlag = 1, bandDispFlag = 1, keyDispFlag;
unsigned short keymode = 0;
unsigned short band = 10;
unsigned short tempmode = 0; // 0 - F; 1 = C
unsigned short temperatureFlag = 0, voltageFlag = 0, eepromUpdateFlag = 0, calcSwrFlag = 0;

char freqStr[6];
char ms_count;
Tflag_tag1 flags1 = {0,0,0,0,0,0,0,0};      // init all bits to zero
char msg[70];




void main(){
  OSCCON = 0b01110110;


  init();
  // Read saved band value from the first position in eeprom
  band = EEPROM_Read(1);
  // Check to make sure it is a valid band
  if (band > 10) { band = 10; }
  changeBandLCD();
  // Read saved keymode value from the 2nd position in eeprom
  keymode = EEPROM_Read(2);
  // Check to make sure it is a valid keymode
  if (keymode > 2) { keymode = 2; }
  changeKeyMode();

  checkTemperature();
  
  checkVoltage();
  ms_count = 0;

  while(1) {                         // Endless loop

      if (txState == 1) {
         checkTXAnalogs();
         if (calcSwrFlag) {
            calculateVswr();
         }


      } else {
        checkButtons();
        if (temperatureFlag) {
           checkTemperature();
        }
        if (voltageFlag) {
           checkVoltage();
         }
        if (keyDispFlag == 1) {
           changeKeyModeLCD();
         }
      }
      
//      if (flags1.newdata) {
//         UART1_Write(rxbuff[uartPtr]);
//         flags1.newdata = 0;
//      }
      
      if (flags1.UART_Buffer_full) {
         UART_grab_buffer();
         findBand();
      }

      if (++ms_count == 10) {
         ms_count = 0;
         processTimerFlags();
      }

      if (lcdFlag == 1) {
         updateLCD();
      }
      
      if (eepromUpdateFlag == 1) {
          // Save band to first position in eeprom
          EEPROM_Write(1, band);
          eepromUpdateFlag = 0;
      }
      delay_ms(1);
  }
}