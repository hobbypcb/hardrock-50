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

   //---------------------------------------------------------------------------
   // Timer 0 interrupt.
   if (INTCON.TMR0IF) {

      if(TX_delay_ms != 0) {              // If TX is delayed
         TX_delay_ms--;                   // count down
         if(TX_delay_ms == 0) {           //    to zero
            setTxOn();                    //    then TX
         }
      }
      if(RX_delay_ms != 0) {              // If RX is delayed
         RX_delay_ms--;                   // count down
         if(RX_delay_ms == 0) {           //    to zero
            setTxOff();                   //    then RX
         }
      }

      TMR0H = 0xF0;                       // preset for Timer0 MSB register
      TMR0L = 0x60;                       // preset for Timer0 LSB register
      INTCON.TMR0IF = 0;                  // Re-enable interrupt

   //---------------------------------------------------------------------------
   // PTT/COR interrupt.
   } else if (INTCON.RBIF) {              // RBIF set by change on PORTB b4-b7 (see Data Sheet P101)
      snapshotB = PORTB;                  // end any mismatch before clear, take snapshot
      lastB = lastB ^ snapshotB;          // xor previous value with current value to detect which pin changed

      INTCON.RBIF = 0;                    // clear interrupt flag
      INTCON.RBIE = 0;                    // Disable interrupt

      if (keyMode == PT) {                // Key Line changed - check value and set flags
         if (snapshotB.key == active) {   // PTT active
            if(key_delay) {
               TX_delay_ms = key_delay;   // If key_delay is set, request TX
            } else {                      // .  in 'TX_delay_ms' milli-seconds,
               setTxOn();                 // .  otherwise, transmit now.
            }
         } else {
            setTxOff();
         }
      }
      if (keyMode == CR) {                // COR Line changed - check value and set flags
         if (snapshotB.cor == 0) {        // Carrier Detect when COR Line is LOW
            setTxOn();
         } else {
            if(cor_htime) {               //
               RX_delay_ms = cor_htime;   // If cor_htime is set, request RX
            } else {                      // .  in 'RX_delay_ms' milli-seconds,
               setTxOff();                // .  otherwise, end transmit now.
            }
         }
      }
      lastB = PORTB;

   //---------------------------------------------------------------------------
   // Button interrupts.
   }  else if (INTCON.INT0IF) {           // BAND-DOWN - INT0 interrupt
      INTCON.INT0IF  = 0;
      INTCON.INT0IE  = 0;
   } else if (INTCON3.INT1IF) {           // BAND-UP - INT1 interrupt
      INTCON3.INT1IF = 0;
      INTCON3.INT1IE = 0;
   } else if (INTCON3.INT2IF) {           // KEY MODE - INT2 interrupt
      INTCON3.INT2IF = 0;
      INTCON3.INT2IE = 0;

   //---------------------------------------------------------------------------
   // UART1 interrupt
   } else if(RC1IF_bit == 1) {            // Checks for Receive Interrupt Flag bit
      rxbuff[uartPtr] = UART1_Read();     // Storing read data
      flags1.newdata = 1;
      if(rxbuff[uartPtr] == 0x3B) {
         flags1.UART_Buffer_Full = 1;
         uartMsgs++;
//       readEnd = uartPtr;
      }
      uartPtr++;                          // point to most recent character
      if(uartPtr > 127) uartPtr = 0;
      RC1IF_bit = 0;                      // Re-enable interrupt

   //---------------------------------------------------------------------------
   // UART2 interrupt
   } else if(RC2IF_bit == 1) {            // Checks for Receive Interrupt Flag bit
      rxbuff2[uartPtr2] = UART2_Read();   // Storing read data
//    flags1.newdata = 1;
      if(rxbuff2[uartPtr2] == 0x3B) {
         flags1.UART2_Buffer_Full = 1;
         uartMsgs2++;
//       readEnd = uartPtr;
      }
      uartPtr2++;                         // point to most recent character
      if(uartPtr2 > 127) uartPtr2 = 0;
      RC2IF_bit = 0;                      // Re-enable interrupt
   }

   INTCON.RBIE = 1;                       // Enable interrupt
}