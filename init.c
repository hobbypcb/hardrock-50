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

// Init for HARDROCK-50
#include "defs.h"

void init() {
  keymode = 2;
  ANSELB = 0;                        // Configure PORTB pins as digital
  ANSELC = 0;
  ANSELD = 0;
  ANSELE = 0;
  
  // Timer0 Registers:// 16-Bit Mode; Prescaler=1:256; TMRH Preset=B; TMRL Preset=DC; Freq=1.00Hz; Period=1,000,000,000 ns
  T0CON.TMR0ON = 0;// Timer0 On/Off Control bit:1=Enables Timer0 / 0=Stops Timer0
  T0CON.T08BIT = 0;// Timer0 8-bit/16-bit Control bit: 1=8-bit timer/counter / 0=16-bit timer/counter
  T0CON.T0CS   = 0;// TMR0 Clock Source Select bit: 0=Internal Clock (CLKO) / 1=Transition on T0CKI pin
  T0CON.T0SE   = 0;// TMR0 Source Edge Select bit: 0=low/high / 1=high/low
  T0CON.PSA    = 0;// Prescaler Assignment bit: 0=Prescaler is assigned; 1=NOT assigned/bypassed
  T0CON.T0PS2  = 1;// bits 2-0  PS2:PS0: Prescaler Select bits
  T0CON.T0PS1  = 1;// Set 1:256
  T0CON.T0PS0  = 1;
  INTCON.TMR0IE = 1;
//  TMR0H = 0xB;    // preset for Timer0 MSB register
//  TMR0L = 0xDC;    // preset for Timer0 LSB register

  CM1CON0 = 0;
  CM2CON = 0;

  CCP1CON = 0;
  CCP2CON = 0;
  CCP3CON = 0;

  T3CON = 0;

  INTCON2.INTEDG0 = 1;               //Configure INT0-2 as rising edge interrupts
  INTCON.INT0IE = 1;                //Enable INT0-2
  INTCON2.INTEDG1 = 1;
  INTCON3.INT1IE = 1;
  INTCON2.INTEDG2 = 1;
  INTCON3.INT2IE = 1;

  INTCON.RBIE = 1;                   //Enable PORTB Interrupt on Change
  IOCB.IOCB4 = 0;                    //Enable IOC on RB4/5
  IOCB.IOCB5 = 0;
  IOCB.IOCB6 = 0;
  IOCB.IOCB7 = 0;

  TRISA = 0;
  TRISB = 0b00110111;
  TRISC = 0;
  TRISD = 0;
  TRISE = 0;
  
  WPUB = 0b00100000;  // Enable Weak Pull up on COR RB5
  INTCON2.RBPU = 0;
  
  LATA = 0;
  LATC = 0;
  LATD = 0;
  LATE = 0;

  LATC = 0x02;
  LATB.RB7 = 1;
  LATB.RB6 = 1;
  Do_LCD_Init();
  LATB.RB7 = 0;
  LATB.RB6 = 0;
  LATB = 0;
  INTCON.GIE = 1;                     //Global Interrupt Enable
  lastB = PORTB;
  INTCON.RBIF = 0;
  INTCON.INT0IF = 0;
  INTCON3.INT2IF = 0;
  INTCON3.INT1IF = 0;

}