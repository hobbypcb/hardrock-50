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

        INTCON.RBIF = 0;        // clear interrupt flag
        INTCON.RBIE = 0;        // Disable interrupt

        if (keyMode == PT)
        { // Key Line changed - check value and set flags
          if (snapshotB.key == active) {
           setTX_ON();
          } else {
           setTX_OFF();
          }
        }

        if (keyMode == CR)
        { // COR Line changed - check value and set flags
          if (snapshotB.cor == 0)
          { // Carrier Detect when COR Line is LOW
           setTX_ON();
          } else {
           setTX_OFF();
          }
        }
        rbDelayFlag = 1;
        lastB = PORTB;
      }
      else if (INTCON.INT0IF)
      { // BAND-DOWN - INT0 interrupt
        INTCON.INT0IF = 0;
        INTCON.INT0IE = 0;
      }
      else if (INTCON3.INT1IF)
      { // BAND-UP - INT1 interrupt
        INTCON3.INT1IF = 0;
        INTCON3.INT1IE = 0;
      }
      else if (INTCON3.INT2IF)
      { // KEY MODE - INT2 interrupt
        INTCON3.INT2IF = 0;
        INTCON3.INT2IE = 0;
      }
      else if (INTCON.TMR0IF)
      {
        INTCON.TMR0IF = 0;
        INTCON.TMR0IE = 0;
      }
      else if(RC1IF_bit == 1) 
      {                             // Checks for Receive Interrupt Flag bit
        rxbuff[uartPtr] = UART1_Read();   // Storing read data
        flags1.newdata = 1;
        if(rxbuff[uartPtr] == 0x3B)
         {
             flags1.UART_Buffer_Full = 1;
             uartMsgs++;
//             readEnd = uartPtr;
         }
        uartPtr++;                     // point to most recent character

        if(uartPtr > 127)uartPtr = 0;
        RC1IF_bit = 0;
      }
      INTCON.RBIE = 1;        // Enable interrupt

      

}