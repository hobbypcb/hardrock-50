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

unsigned int uartPtr = 0, uartPtr2 = 0;
unsigned int uartMsgs = 0, uartMsgs2 = 0, readStart = 0, readStart2 = 0;
char rxbuff[128];          // 128 byte circular Buffer for storing rx data
char workingString[128];
char rxbuff2[128];         // 128 byte circular Buffer for storing rx data
char workingString2[128];

long freqLong = 0;
unsigned short oldBand;
unsigned int old_tuner_freq;
const char crlfsemi[] = ";\r\n";
const char HRTM[] = "HRTM";

void uartGrabBuffer(){
   int z = 0;
   char lastChar[1];
   lastChar[0] = "C";
// lclReadEnd = readEnd;

   while (lastChar[0] != 0x3B) {
      workingString[z] = rxbuff[readStart];
      rxbuff[readStart] = 0x00;
      readStart++;
      lastChar[0] =  workingString[z];
      z++;
      if (readStart > 127)readStart = 0;
   }//endwhile

   workingString[z] = 0x00;

   uartMsgs--;
   if (uartMsgs < 1) flags1.UART_Buffer_Full = 0;  // Clear the flag so that we dont come back
                                                   //   here unless we have a new data stream in the buffer
}

void uartGrabBuffer2(){
   int z = 0;
   char lastChar[1];
   lastChar[0] = "C";
// lclReadEnd = readEnd;

   while (lastChar[0] != 0x3B) {
      workingString2[z] = rxbuff2[readStart2];
      rxbuff2[readStart2] = 0x00;
      readStart2++;
      lastChar[0] =  workingString2[z];
      z++;
      if (readStart2 > 127) readStart2 = 0;
   }//endwhile

   workingString2[z] = 0x00;

   uartMsgs2--;
   if (uartMsgs2 < 1) flags1.UART2_Buffer_Full = 0;  // Clear the flag so that we dont come back
                                                     //   here unless we have a new data stream in the buffer
}


void findBand(short uart) {
   char *found;
   char* workStringPtr;
   unsigned int C, t_freq;
   int i, wsl;
   
   if (uart == 1) {
      workStringPtr = workingString;
   } else {
      workStringPtr = workingString2;
   }
   // convert working string to uppercase...
   wsl = strlen(workStringPtr);
   for (i = 0; i <= wsl; i++) workStringPtr[i] = toupper(workStringPtr[i]);

   found = strstr(workStringPtr,"FA");
   if (found == 0) {
      found = strstr(workStringPtr,"IF");
   }
   if (found != 0) {
      oldBand = band;
      old_tuner_freq = tuner_freq;
      for (i = 0; i <= 5; i++) {
         freqStr[i] = found[i + 4];
      }

      freqLong = atol(freqStr);
      if (freqLong >= 1800 && freqLong <= 2000) {
         band = _160M;
         tuner_freq = 10 + (freqLong - 1800)/10;
      } else if (freqLong >= 3500 && freqLong <= 4000) {
           band = _80M;
           tuner_freq = 40 + (freqLong - 3500)/10;
      } else if (freqLong >= 5330 && freqLong <= 5410) {
           band = _60M;
           tuner_freq = 100 + (freqLong - 5330)/10;
      } else if (freqLong >= 7000 && freqLong <= 7300) {
           band = _40M;
           tuner_freq = 110 + (freqLong - 7000)/10;
      } else if (freqLong >= 10100 && freqLong <= 10150) {
           band = _30M;
           tuner_freq = 150 + (freqLong - 10100)/10;
      } else if (freqLong >= 14000 && freqLong <= 14350) {
           band = _20M;
           tuner_freq = 160 + (freqLong - 14000)/10;
      } else if (freqLong >= 18068 && freqLong <= 18168) {
           band = _17M;
           tuner_freq = 200 + (freqLong - 18060)/10;
      } else if (freqLong >= 21000 && freqLong <= 21450) {
           band = _15M;
           tuner_freq = 220 + (freqLong - 21000)/10;
      } else if (freqLong >= 24890 && freqLong <= 24990) {
           band = _12M;
           tuner_freq = 270 + (freqLong - 24890)/10;
      } else if (freqLong >= 28000 && freqLong <= 29700) {
           band = _10M;
           tuner_freq = 290 + (freqLong - 28000)/25;
      } else if (freqLong >= 50000 && freqLong <= 54000) {
           band = _6M;
           tuner_freq = 360 + (freqLong - 50000)/100;
      } else {
           band = _UNK;
           tuner_freq = 999;
      }//endif

      if (band != oldBand) {
         bandFlag = 1;
         eepromUpdateFlag = 1;
         changeBandLCD(0);
      }

      if (tuner_freq != old_tuner_freq){

         Tuner_Snd_Char('*');
         Tuner_Snd_Char('F');
         C = tuner_freq/100;
         Tuner_Snd_Char(C + 48);
         t_freq = tuner_freq - C * 100;
         C = t_freq/10;
         Tuner_Snd_Char(C + 48);
         t_freq = t_freq - C * 10;
         Tuner_Snd_Char(t_freq + 48);
         Tuner_Snd_Char(13);
      }
   }

   found = strstr(workStringPtr,"HR");
   if (found != 0) {

      flags1.newcmd = 1;

      //set acc baud rate:
      found = strstr(workStringPtr,"HRBR");
      if (found != 0 && flags1.newcmd) {
         flags1.newcmd = 0;
         setBaudRate(uart, found);
      }
      //set KX3 mode:
      found = strstr(workStringPtr,"HRKX");
      if (found != 0 && flags1.newcmd) {
         flags1.newcmd = 0;
         setKxMode(uart, found);
      }
      //set band:
      found = strstr(workStringPtr,"HRBN");
      if (found != 0 && flags1.newcmd) {
         flags1.newcmd = 0;
         setBandser(uart, found);
      }
      //set mode:
      found = strstr(workStringPtr,"HRMD");
      if (found != 0 && flags1.newcmd) {
         flags1.newcmd = 0;
         setModeser(uart, found);
      }
      //read temp /set F or C:
      found = strstr(workStringPtr,"HRTP");
      if (found != 0 && flags1.newcmd) {
         flags1.newcmd = 0;
         setTempser(uart, found);
      }
      //read volts:
      found = strstr(workStringPtr,"HRVT");
      if (found != 0 && flags1.newcmd) {
         flags1.newcmd = 0;
         setVoltser(uart, found);
      }      
      //RX Status:
      found = strstr(workStringPtr,"HRRX");
      if (found != 0 && flags1.newcmd) {
         flags1.newcmd = 0;
         if (found[4] == ';') uartRxStatus(uart);
      }
      //ATU mode:
      found = strstr(workStringPtr,"HRAT");
      if (found != 0 && flags1.newcmd) {
         flags1.newcmd = 0;
         setATUser(uart, found);
      }
      //Tuner pass-thru:
      found = strstr(workStringPtr,"HRTM");
      if (found != 0 && flags1.newcmd) {
         UART_send(uart, copyConst2Ram(msg,HRTM));
         flags1.newcmd = 0;
         Tuner_Snd_Char('*');
         i = 4;
         while (found[i] != ';') Tuner_Snd_Char(found[i++]);
         Tuner_Snd_Char(13);
         i = 0;
         msg[0] = Tuner_Get_Char();
         if (msg[0] != 255){
            while (msg[i] != 13){
                  msg[++i] = Tuner_Get_Char();
            }
            msg[i] = 0;
            UART_send(uart, msg);
         }
         UART_send(uart, copyConst2Ram(msg,crlfsemi));
      }
   }
}

void UART_send(char uart,char *send_str){
     if (uart == 1) UART1_Write_Text(send_str);
     else UART2_Write_Text(send_str);
}

void sendComma() {
   UART1_Write_Text(",");
}

void uartRxStatus(char uart) {
   char RX[] = "RX";
   char TMPC[] = "C";
   char TMPF[] = "F";
   char comma[] = ",";
   
   UART_send(uart, RX);
   UART_send(uart, comma);
   UART_send(uart, KEY_STR);
   UART_send(uart, comma);
   rtrim(BAND_STR);
   UART_send(uart, BAND_STR);
   UART_send(uart, comma);
   UART_send(uart, TEMP_STR);
   if (tempmode == 0) UART_send(uart,TMPF);
   else UART_send(uart,TMPC);
   UART_send(uart, comma);
   UART_send(uart, VOLT_STR);
   UART_send(uart, copyConst2Ram(msg,crlfsemi));
}