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

const char uart4800[] = "4800bps";
const char uart9600[] = "9600bps";
const char uart19200[] = "19200bps";
const char uart38400[] = "38400bps";
const char uartsetto[] = "UART set to ";
const char onrestart[] = " on next restart";
extern const char crlfsemi[];
const char uartinvalid[] = "Invalid UART baud rate cmd proper format: HRBRX; (X = 0-3)";
const char kxenabled[] = "Elecraft KX receive mode enabled";
const char kxdisabled[] = "Elecraft KX receive mode disabled";
const char kxinvalid[] = "Invalid KX recieve mode set cmd proper format: HRKX0; or HRKX1;";
char version = 0x00;

//const char crlfsemi[];

void setBaudRate() {
   short acc_baud = 0;
   // Read saved baud from 4th position in eeprom
   acc_baud = EEPROM_Read(4);
   // Check to make sure it's a valid baud rate
   if (acc_baud > 3) { acc_baud = 2; }
   if (workingString[4] == 0x30) {
      UART1_Write_Text(copyConst2Ram(msg,uartsetto));
      UART1_Write_Text(copyConst2Ram(msg,uart4800));
      UART1_Write_Text(copyConst2Ram(msg,onrestart));
      UART1_Write_Text(copyConst2Ram(msg,crlfsemi));
      acc_baud = 0;
   } else if (workingString[4] == 0x31) {
      UART1_Write_Text(copyConst2Ram(msg,uartsetto));
      UART1_Write_Text(copyConst2Ram(msg,uart9600));
      UART1_Write_Text(copyConst2Ram(msg,onrestart));
      UART1_Write_Text(copyConst2Ram(msg,crlfsemi));
      acc_baud = 1;
   } else if (workingString[4] == 0x32) {
      UART1_Write_Text(copyConst2Ram(msg,uartsetto));
      UART1_Write_Text(copyConst2Ram(msg,uart19200));
      UART1_Write_Text(copyConst2Ram(msg,onrestart));
      UART1_Write_Text(copyConst2Ram(msg,crlfsemi));
      acc_baud = 2;
   } else if (workingString[4] == 0x33) {
      UART1_Write_Text(copyConst2Ram(msg,uartsetto));
      UART1_Write_Text(copyConst2Ram(msg,uart38400));
      UART1_Write_Text(copyConst2Ram(msg,onrestart));
      UART1_Write_Text(copyConst2Ram(msg,crlfsemi));
      acc_baud = 3;
   } else {
      UART1_Write_Text(copyConst2Ram(msg,uartinvalid));
      UART1_Write_Text(copyConst2Ram(msg,crlfsemi));
   }
   EEPROM_Write(4, acc_baud);
}

void setKxMode() {
   short kxmode;
     
   // Read saved kxmode from 5th position in eeprom
   kxmode = EEPROM_Read(5);
   // Check to make sure it's a valid kxmode
   if (kxmode > 1) { kxmode = 0; }

   if (workingString[4] == 0x31) {
      UART1_Write_Text(copyConst2Ram(msg,kxenabled));
      UART1_Write_Text(copyConst2Ram(msg,crlfsemi));
      kxmode = 1;
   } else if (workingString[4] == 0x30) {
      UART1_Write_Text(copyConst2Ram(msg,kxdisabled));
      UART1_Write_Text(copyConst2Ram(msg,crlfsemi));
      kxmode = 0;
   } else {
      UART1_Write_Text(copyConst2Ram(msg,kxinvalid));
      UART1_Write_Text(copyConst2Ram(msg,crlfsemi));
   }
   EEPROM_Write(5, kxmode);
}

void setTempLabel() {

   if (workingString[4] == 'F') {
      UART1_Write_Text("Temperature Label set to F\r\n");
      tempmode = 0;
   } else if (workingString[4] == 'C') {
      UART1_Write_Text("Temperature Label set to C\r\n");
      tempmode = 1;
   } else {
      UART1_Write_Text("Invalid Temperature set cmd proper format: HRTLC; or HRTLF;\r\n");
   }
   EEPROM_Write(3, tempmode);
}

void setCallSign() {
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