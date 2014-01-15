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

const char meterLeft_CHR[] = {16,16,16,0,16,16,16,0};
const char meterBoth_CHR[] = {31,31,31,0,31,31,31,0};
const char meterTop_CHR[] = {31,31,31,0,0,0,0,0};
const char meterBottom_CHR[] = {0,0,0,0,31,31,31,0};

char SPLASH_TOP[] = "HARDROCK-50 AMP";
char SPLASH_BOTTOM[] = "VER 3   FW:1.5D";


void LoadChars() {
    char i;
    Lcd_Cmd(64);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(meterLeft_CHR[i]);

    Lcd_Cmd(72);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(meterBoth_CHR[i]);

    Lcd_Cmd(80);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(meterTop_CHR[i]);
    
    Lcd_Cmd(88);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(meterBottom_CHR[i]);
}

void init() {

  int diagCounter, i;
  short baud;
  short kxmode;
  short blver;
  keymode = 2;
  ANSELA = 0b00000011;
  ANSELB = 0;                        // Configure PORTB pins as digital
  ANSELC = 0;
  ANSELD = 0b11000000;
  ANSELE = 0;
  
  // Configure ADC Options
  ADCON1 = 0b00001000;  // PVCFG<1:0> bit 3-2 10 for A/D VREF+ connected to internal signal, FVR BUF2
  VREFCON0 = 0b10110000;
  
  //  Set up timer0 for 1ms timing.  Will us this for timing various actions including the debounce functions.
  // Timer0 Registers:// 16-Bit Mode; Prescaler=1:1; TMRH Preset=C1; TMRL Preset=80; Freq=1,000.00Hz; Period=1.00 ms
  T0CON.TMR0ON = 0;// Timer0 On/Off Control bit:1=Enables Timer0 / 0=Stops Timer0
  T0CON.T08BIT = 0;// Timer0 8-bit/16-bit Control bit: 1=8-bit timer/counter / 0=16-bit timer/counter
  T0CON.T0CS   = 0;// TMR0 Clock Source Select bit: 0=Internal Clock (CLKO) / 1=Transition on T0CKI pin
  T0CON.T0SE   = 0;// TMR0 Source Edge Select bit: 0=low/high / 1=high/low
  T0CON.PSA    = 1;// Prescaler Assignment bit: 0=Prescaler is assigned; 1=NOT assigned/bypassed
  T0CON.T0PS2  = 0;// bits 2-0  PS2:PS0: Prescaler Select bits
  T0CON.T0PS1  = 0;
  T0CON.T0PS0  = 0;
  TMR0H = 0xd8;    // preset for Timer0 MSB register
  TMR0L = 0xef;    // preset for Timer0 LSB register
  INTCON.TMR0IF = 0;

  CM1CON0 = 0;
  CM2CON = 0;

  CCP1CON = 0;
  CCP2CON = 0;
  CCP3CON = 0;

  T3CON = 0;

  INTCON2.INTEDG0 = 1;               //Configure INT0-2 as rising edge interrupts
  INTCON.INT0IE = 0;                //Disable INT0-2
  INTCON2.INTEDG1 = 1;
  INTCON3.INT1IE = 0;
  INTCON2.INTEDG2 = 0;
  INTCON3.INT2IE = 1;
  
  INTCON.TMR0IE = 1;

  INTCON.RBIE = 1;                   //Enable PORTB Interrupt on Change
  IOCB.IOCB4 = 0;                    //Enable IOC on RB4/5
  IOCB.IOCB5 = 0;
  IOCB.IOCB6 = 0;
  IOCB.IOCB7 = 0;

  TRISA = 0b00000011;
  TRISB = 0b01110111;
  TRISC = 0;
  TRISD = 0b11000000;
  TRISE = 0;
  
  WPUB = 0b00100000;  // Enable Weak Pull up on COR RB5
  INTCON2.RBPU = 0;
  
  LATA = 0;
  LATC = 0;
  LATD = 0;
  LATE = 0;

  LATC = 0x02;
//  LATB.RB7 = 1;

  
  Do_LCD_Init();

  LoadChars();
  // Hold keymode to enter diagnostics mode
  if (!PORTB.B2) {
     diagCounter = 0;
     for (i = 1; i<=10; i++) {
         if (!PORTB.B2) {
            diagCounter++;
         } else {
            diagCounter--;
         }
         delay_ms(3);
     }
     if (diagCounter >= 7) {
        portTest();
     }
  }
  
  // Read saved values from eeprom or set defaults
  // Read saved tempmode from 3rd position in eeprom
  tempmode = EEPROM_Read(3);
  // Check to make sure it's a valid tempmode
  if (tempmode > 1 || tempmode < 0) { tempmode = 0; }
  
  // Read configured baud rate
  baud = EEPROM_Read(4);
  if (baud > 3 || baud < 0) { baud = 2; }
  
  // Read if kxmode is configured
  kxmode = EEPROM_Read(5);
  // Check to make sure it's a valid kxmode
  if (kxmode > 1 || kxmode < 0) { kxmode = 0; }

  // Read Bootloader Version
  blver = EEPROM_Read(6);
  // Check to make sure it's a valid kxmode
  if (blver > 1 || blver < 0) { blver = 0; }

  
  // Hold BAND-DOWN to change temperature display between F and C
  if (!PORTB.B1) {
        Lcd_Out(1,1,"Temp Mode: ");

        if (tempmode == 1) {
           tempmode = 0;
           Lcd_Out(1,12,"F ");
        }
        else {
           tempmode = 1;
           Lcd_Out(1,12,"C ");
        }
        EEPROM_Write(3, tempmode);
        delay_ms(3000);
     }
  // Enable UART

  // enable UART with standard UART setup
  if (!PORTB.B0) {
     TRISB6_bit = 1;
     UART1_Init(19200);
     BAUDCON1.DTRXP = 0;
     flags1.configMode = 1;

     if (blver < 1) {
        Lcd_Out(1,1,"UpdatingBTLDR");
        UART1_Write_Text("UpdatingBTLDR\r\n");
        delay_ms(2000);
        Start_Bootload();
        EEPROM_Write(6, 1);
        Lcd_Out(1,1,"DoneUpdating ");
        UART1_Write_Text("DoneUpdating\r\n");
        delay_ms(2000);
     }

  } else {
    if (baud == 0) {
     UART1_Init(4800);
    } else if (baud == 1) {
     UART1_Init(9600);
    } else if (baud == 2) {
     UART1_Init(19200);
    } else if (baud == 3) {
     UART1_Init(38400);
    }
    if (kxmode == 1) {
     BAUDCON1.DTRXP = 1;
     TRISB6_bit = 0;
     LATB6_bit = 0;
    }
    flags1.configMode = 0;
  }
  uartPtr = 0;
  readStart = 0;
  uartMsgs = 0;

  lastB = PORTB;
  INTCON.RBIF = 0;
/*INTCON.INT0IF = 0;
  INTCON3.INT2IF = 0;
  INTCON3.INT1IF = 0;*/


  Delay_ms(100);
  UART1_Write_Text("\r\n");            // diagnostic only - Tx not expected to be used
  UART1_Write_Text(SPLASH_TOP);
  UART1_Write_Text("\r\n");
  UART1_Write_Text(SPLASH_BOTTOM);
  UART1_Write_Text("\r\n");
  
  
  RC1IE_bit = 1;                                         // turn ON interrupt on UART1 receive
  RC1IF_bit = 0;                                         // Clear interrupt flag
  
  INTCON.PEIE = 1;
  INTCON.GIE = 1;                     //Global Interrupt Enable
  
  ADC_INIT();
  
  bandUpFlag = 0;
  bandDownFlag = 0;
  keyModeFlag = 0;

  T0CON.TMR0ON = 1;// Timer0 On/Off Control bit:1=Enables Timer0 / 0=Stops Timer0 ]

}