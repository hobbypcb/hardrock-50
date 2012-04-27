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
unsigned short txState = 0, lastB = 0, timer0Flag = 0, bandFlag = 1;
unsigned short keymode = 0;
unsigned short band = 10;



void main(){
  OSCCON = 0b01110110;


  init();

  while(1) {                         // Endless loop
      if (lcdFlag == 1) {
         updateLCD();
      }
/*if (timer0Flag == 1) {
         timer0Flag = 0;
         if (bandFlag == 1) {
            bandFlag = 0;
            setBand();
         }

      }*/
//    if (i == 0)
//       Show_RX();
//       LATC = 0x01;
//       Delay_ms(5000);

//    if (i == 1)
//       Show_TX();
//       LATC = 0x02;
//       Delay_ms(5000);




  }
}