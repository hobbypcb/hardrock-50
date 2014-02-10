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

// UART (RS232 Serial) Routines for HARDROCK-50

#include "defs.h"

//unsigned int uartPtr, uartPtr2;
//unsigned int uartMsgs, uartMsgs2, readStart, readStart2;
unsigned int uartPtr;
unsigned int uartMsgs, readStart;
char rxbuff[128];                                   // 256 byte circular Buffer for storing rx data
char workingString[128];
//char rxbuff2[128];                                   // 256 byte circular Buffer for storing rx data
//char workingString2[128];

long freqLong = 0;
unsigned short oldBand;
const char crlfsemi[] = ";\r\n";
const char hrcommand[] = "Hardrock Command: ";
const char detected[] = " detected";


void UART_grab_buffer(){

      int z;
      char lastChar[1];
      lastChar[0] = "C";
      
//      lclReadEnd = readEnd;
      z = 0;
      
      while (lastChar[0] != 0x3B) {

               workingString[z] = rxbuff[readStart];
               rxbuff[readStart] = 0x00;
               readStart++;
               lastChar[0] =  workingString[z];
               z++;
               
               if (readStart > 127)readStart = 0;
              }

      workingString[z] = 0x00;

      uartMsgs--;
      if (uartMsgs < 1) flags1.UART_Buffer_Full = 0;  // Clear the flag so that we dont come back
                                                     //   here unless we have a new data stream in the buffer
}

/*void UART_grab_buffer2(){

      int z;
      char lastChar[1];
      lastChar[0] = "C";

//      lclReadEnd = readEnd;
      z = 0;

      while (lastChar[0] != 0x3B) {

               workingString2[z] = rxbuff2[readStart2];
               rxbuff2[readStart2] = 0x00;
               readStart2++;
               lastChar[0] =  workingString2[z];
               z++;

               if (readStart2 > 127)readStart2 = 0;
              }

      workingString2[z] = 0x00;

      uartMsgs2--;
      if (uartMsgs2 < 1) flags1.UART2_Buffer_Full = 0;  // Clear the flag so that we dont come back
                                                     //   here unless we have a new data stream in the buffer
}*/

void findBand() {
     char *found;
     char* workStringPtr;
//     UART1_Write_Text("Starting findBand\r\n");
//     UART1_Write_Text(workingString);
//     UART1_Write_Text("\r\n");
//      if (uart == 1) {
         workStringPtr = workingString;
//      } else {
//         workStringPtr = workingString2;
//      }
     found = strstr(workStringPtr,"FA");
     if (found == 0) {
        found = strstr(workStringPtr,"IF");
     }
     if (found != 0) {
        oldBand = band;
//        UART1_Write_Text("Found FA\r\n");
        for (i = 0; i <= 5; i++)
            {
               freqStr[i] = found[i + 4];
            }
//        UART1_Write_Text(freqStr);
        freqLong = atol(freqStr);
        if (freqLong > 1800 && freqLong < 2000) {
//           UART1_Write_Text("Found 160M\r\n");
           band = _160M;
        } else if (freqLong > 3500 && freqLong < 4000) {
//           UART1_Write_Text("Found 80M\r\n");
           band = _80M;
        } else if (freqLong > 5325 && freqLong < 5410) {
//           UART1_Write_Text("Found 60M\r\n");
           band = _60M;
        } else if (freqLong > 7000 && freqLong < 7300) {
//           UART1_Write_Text("Found 40M\r\n");
           band = _40M;
        } else if (freqLong > 10100 && freqLong < 10150) {
//           UART1_Write_Text("Found 30M\r\n");
           band = _30M;
        } else if (freqLong > 14000 && freqLong < 14350) {
//           UART1_Write_Text("Found 20M\r\n");
           band = _20M;
        } else if (freqLong > 18068 && freqLong < 18168) {
//           UART1_Write_Text("Found 17M\r\n");
           band = _17M;
        } else if (freqLong > 21000 && freqLong < 21450) {
//           UART1_Write_Text("Found 15M\r\n");
           band = _15M;
        } else if (freqLong > 24890 && freqLong < 24990) {
//           UART1_Write_Text("Found 12M\r\n");
           band = _12M;
        } else if (freqLong > 28000 && freqLong < 29700) {
//           UART1_Write_Text("Found 10M\r\n");
           band = _10M;
        } else if (freqLong > 50000 && freqLong < 54000) {
//           UART1_Write_Text("Found 6M\r\n");
           band = _6M;
        } else {
//           UART1_Write_Text("Outside Band Boundry\r\n");
        }
        bandFlag = 1;
        eepromUpdateFlag = 1;
        changeBandLCD();
     }
     found = strstr(workStringPtr,"HR");
     if (found != 0) {
        flags1.newcmd = 1;
        UART1_Write_Text(CopyConst2Ram(msg,hrcommand));
        UART1_Write_Text(workingString);
        UART1_Write_Text(CopyConst2Ram(msg,detected));
        UART1_Write_Text(CopyConst2Ram(msg,crlfsemi));
        found = strstr(workingString,"HRBR");
        if (found != 0 && flags1.newcmd) {
           flags1.newcmd = 0;
           setBaudRate();
        }
        found = strstr(workStringPtr,"HRKX");
        if (found != 0 && flags1.newcmd) {
           flags1.newcmd = 0;
           setKXMode();
        }
        found = strstr(workStringPtr,"HRTL");
        if (found != 0 && flags1.newcmd) {
           flags1.newcmd = 0;
           setTempLabel();
        }
        found = strstr(workStringPtr,"HRCS");
        if (found != 0 && flags1.newcmd) {
           flags1.newcmd = 0;
           setCallSign();
        }



     }

}

void sendComma() {
     UART1_Write_Text(",");
}

void uartRxStatus() {
     UART1_Write_Text("RX");
     sendComma();
     UART1_Write_Text(KEY_STR);
     sendComma();
     rtrim(BAND_STR);
     UART1_Write_Text(BAND_STR);
     sendComma();
     UART1_Write_Text(TEMP_STR);
     sendComma();
     UART1_Write_Text(VOLT_STR);
     UART1_Write_Text(CopyConst2Ram(msg,crlfsemi));
}

void uartTxStatus() {
     UART1_Write_Text("TX");
     sendComma();
     UART1_Write_Text(VSWR_STR);
     sendComma();
     UART1_Write_Text(PEP_STR);
     UART1_Write_Text(CopyConst2Ram(msg,crlfsemi));
}