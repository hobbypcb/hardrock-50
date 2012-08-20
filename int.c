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

// Interrupt function for HARDROCK-50

#include "defs.h"

unsigned short snapshotB;

void interrupt() {
     if (INTCON.RBIF)
     {  // RBIF set by change on PORTB b4-b7 (see Data Sheet P101)
        snapshotB = PORTB;      // end any mismatch before clear, take snapshot
        lastB = lastB ^ snapshotB; // xor previous value with current value to detect which pin changed


        if (lastB.key == 1)
        { // Key Line changed - check value and set flags
          if (snapshotB.key == active)
          {
           txState = 1;
           setTX_OUT();
           lcdFlag = 1;
           delay_ms(30);
          } else {
           txState = 0;
           setTX_OFF();
           lcdFlag = 1;
           delay_ms(30);
          }
        }

        if (lastB.cor == 1)
        { // COR Line changed - check value and set flags
          if (snapshotB.cor == 0)
          { // Carrier Detect when COR Line is LOW
           txState = 1;
           setTX_OUT();
           lcdFlag = 1;
           delay_ms(30);
          } else {
           txState = 0;
           setTX_OFF();
           lcdFlag = 1;
           delay_ms(30);
          }
        }
        INTCON.RBIF = 0;        // clear interrupt flag
        lastB = PORTB;
      }
      else if (INTCON.INT0IF)
      { // BAND-DOWN - INT0 interrupt
        changeBandDisplay(-1);
        delay_ms(30);
      //  setBandDelay();
        bandFlag = 1;
        lcdFlag = 1;
;
        INTCON.INT0IF = 0; // Clear interrupt flag bit

      }
      else if (INTCON3.INT1IF)
      { // BAND-UP - INT1 interrupt
        changeBandDisplay(+1);
        delay_ms(30);
      //  setBandDelay();
        bandFlag = 1;
        lcdFlag = 1;

        INTCON3.INT1IF = 0;  // Clear interrupt flag bit

      }
      else if (INTCON3.INT2IF)
      { // KEY MODE - INT2 interrupt
        changeKeyMode();
        delay_ms(30);

        lcdFlag = 1;

        INTCON3.INT2IF = 0;  // Clear interrupt flag bit

      }
      else if (INTCON.TMR0IF)
      {
        T0CON.TMR0ON = 0;   // Disable Timer0
        timer0Flag = 1;
        INTCON.TMR0IF = 0;
        bandFlag = 1;
      }
      
      

}