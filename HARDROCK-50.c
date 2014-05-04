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
//unsigned short temperatureFlag = 0, voltageFlag = 0, eepromUpdateFlag = 0, calcSwrFlag = 0;
unsigned short temperatureFlag = 0, voltageFlag = 0, eepromUpdateFlag = 0;
short          menu_active = 0;
unsigned int   TX_delay_ms = 0;
unsigned int   RX_delay_ms = 0;
char freqStr[6];
char ms_count;
Tflag_tag1 flags1 = {0,0,0,0,0,0,0,0};      // init all bits to zero
char msg[70];

// EEPROM variables:
unsigned short band     = 10;
unsigned short keymode  = 0;
unsigned short tempmode = 0; // 0 = F; 1 = C
short          acc_baud;
short          kxmode;
short          blver;
short          usb_baud;
short          meter_adj; // (+/- 25%)
int            cor_htime; // (2 bytes)
int            key_delay; // (2 bytes)

void main(){
   OSCCON = 0b01110110;

   init();
   changeBandLCD();
   setKeyMode();
   adjustWattMeter(meter_adj);
   checkTemperature(0);
   checkVoltage();

   // Main process (endless) loop.
   ms_count = 0;
   while(1) {
      if (txState != 1) {
         processButtons();
      }
      backgroundTasks();
      if (lcdFlag == 1) {
         updateLCD();
      }
      delay_ms(1);
   }
}


void backgroundTasks() {

   // Transmit-only tasks.
   if (txState == 1) {

      // Disable UARTS during TX.
      if (RCSTA1.SPEN == 1) RCSTA1.SPEN = 0;
      if (RCSTA2.SPEN == 1) RCSTA2.SPEN = 0;

      checkTXAnalogs();
      //if (calcSwrFlag) {
      calculateVswr();
      //}

   // Receive-only tasks.
   } else {

      // Re-Enable UARTS
      if (RCSTA1.SPEN == 0) RCSTA1.SPEN = 1;
      if (RCSTA2.SPEN == 0) RCSTA2.SPEN = 1;

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