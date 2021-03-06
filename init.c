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

const char meterBoth_CHR[]   = {31,31,31,0,31,31,31,0};
const char meterTop_CHR[]    = {31,31,31,0,0,0,0,0};

const char SPLASH_TOP[]      = "HARDROCK-50  AMP";
const char SPLASH_BOTTOM[]   = "FW: 3.0A        ";
extern const char crlfsemi[];


void LoadChars() {
   char i;
   Lcd_Cmd(72);
   for (i = 0; i<=7; i++) Lcd_Chr_CP(meterBoth_CHR[i]);
   Lcd_Cmd(80);
   for (i = 0; i<=7; i++) Lcd_Chr_CP(meterTop_CHR[i]);
}

void init() {

   ANSELA = 0b00000011;
   ANSELB = 0;                        // Configure PORTB pins as digital
   ANSELC = 0;
   ANSELD = 0b11000000;
   ANSELE = 0;

   // Configure ADC Options.
   ADCON1 = 0b00001000;  // PVCFG<1:0> bit 3-2 10 for A/D VREF+ connected to internal signal, FVR BUF2
   VREFCON0 = 0b10110000;

   // Timer0 Registers:
   // 16-Bit Mode; Prescaler=1:1; TMRH Preset=C1; TMRL Preset=80; Freq=1,000.00Hz; Period=1.00 ms
   T0CON.TMR0ON  = 0;    // Timer0 On/Off Control bit:1=Enables Timer0 / 0=Stops Timer0
   T0CON.T08BIT  = 0;    // Timer0 8-bit/16-bit Control bit: 1=8-bit timer/counter / 0=16-bit timer/counter
   T0CON.T0CS    = 0;    // TMR0 Clock Source Select bit: 0=Internal Clock (CLKO) / 1=Transition on T0CKI pin
   T0CON.T0SE    = 0;    // TMR0 Source Edge Select bit: 0=low/high / 1=high/low
   T0CON.PSA     = 1;    // Prescaler Assignment bit: 0=Prescaler is assigned; 1=NOT assigned/bypassed
   T0CON.T0PS2   = 0;    // bits 2-0  PS2:PS0: Prescaler Select bits
   T0CON.T0PS1   = 0;  
   T0CON.T0PS0   = 0;  
   TMR0H         = 0xF0; // preset for Timer0 MSB register
   TMR0L         = 0x60; // preset for Timer0 LSB register
   INTCON.TMR0IF = 0;

   CM1CON0 = 0;
   CM2CON  = 0;

   CCP1CON = 0;
   CCP2CON = 0;
   CCP3CON = 0;

   T3CON   = 0;

   INTCON2.INTEDG0 = 1;                //Configure INT0-2 as rising edge interrupts
   INTCON.INT0IE   = 0;                //Disable INT0-2
   INTCON2.INTEDG1 = 1;
   INTCON3.INT1IE  = 0;
   INTCON2.INTEDG2 = 0;
   INTCON3.INT2IE  = 1;

   INTCON.TMR0IE = 1;

   INTCON.RBIE = 1;                    //Enable PORTB Interrupt on Change
   IOCB.IOCB4  = 0;                    //Enable IOC on RB4/5
   IOCB.IOCB5  = 0;
   IOCB.IOCB6  = 0;
   IOCB.IOCB7  = 0;

   TRISA = 0b00000011;
   TRISB = 0b01111111;
   TRISC = 0b00000000;
   TRISD = 0b11000000;
   TRISE = 0;

   getVersion();

  // REV F Changes to ports
   if (REV_F) {
      ANSELD = 0;
      ANSELE = 0b00000110;
      TRISD  = 0;
      TRISE  = 0b00000110;
   }

   WPUB = 0b00101000;  // Enable Weak Pull up on COR RB5 and RXD from ATU
   INTCON2.RBPU = 0;

   LATA = 0;
   LATC = 0x06;
   LATD = 0;
   LATE = 0;

   doLcdInit();
   LoadChars();
 
   // Read saved values from EEPROM.
   band      = EEPROM_Read(1);      // 1st position = saved band
   keymode   = EEPROM_Read(2);      // 2nd position = saved keymode
   tempmode  = EEPROM_Read(3);      // 3rd position = saved tempmode (0=F, 1=C)
   acc_baud  = EEPROM_Read(4);      // 4th position = saved ACC baud
   kxmode    = EEPROM_Read(5);      // 5th position = saved kxmode
   blver     = EEPROM_Read(6);      // 6th position = saved boot loader version
   usb_baud  = EEPROM_Read(7);      // 7th position = saved USB baud
   meter_adj = EEPROM_Read(8);      // 8th position = saved meter_adj (+/-25%)
   cor_htime = EEPROM_Read_int(9);  // 9th & 10th  positions = saved COR hang time (int)
   key_delay = EEPROM_Read_int(11); // 11th & 12th positions = saved key_up delay (int)
   // 13th position is the ATU mode but we only read it if the tuner is present
   ftmode    = EEPROM_Read(14);     // 14th position = saved FT-817 mode

   // Make sure saved values are valid.  Use defaults if necessary.
   if (band      > 10                     ) { band      = 10;}
   if (keymode   > 3                      ) { keymode   = 3; }
   if (tempmode  > 1    || tempmode  < 0  ) { tempmode  = 0; }
   if (acc_baud  > 3    || acc_baud  < 0  ) { acc_baud  = 2; }
   if (kxmode    > 1    || kxmode    < 0  ) { kxmode    = 0; }
   if (blver     > 1    || blver     < 0  ) { blver     = 0; }
   if (usb_baud  > 3    || usb_baud  < 0  ) { usb_baud  = 2; }
   if (meter_adj > 25   || meter_adj < -25) { meter_adj = 0; }
   if (cor_htime > 2000 || cor_htime < 0  ) { cor_htime = 0; }
   if (key_delay > 50   || key_delay < 0  ) { key_delay = 0; }
   if (ftmode    > 1    || ftmode    < 0  ) { ftmode    = 0; }

   set_baud = usb_baud;
   if (REV_E) set_baud = acc_baud;
   
   // Reset UART1 baud rate.
   switch (set_baud){
          case 0:
               UART1_Init(4800);
               break;
          case 1:
               UART1_Init(9600);
               break;
          case 2:
               UART1_Init(19200);
               break;
          case 3:
               UART1_Init(38400);
               break;
   }

   // enable UARTs using saved configuration.
   if (REV_E) {

      // Rev E doesn't support FT-817 mode
      ftmode = 0;

      // Set KX3 mode
      if(kxmode == 1) {
         BAUDCON1.DTRXP = 1;
         TRISB6_bit     = 0;
         LATB6_bit      = 0;
      } else {
         BAUDCON1.DTRXP = 0;
      }

   // For REV_F and above.
   } else {

      // Reset UART2 baud rate.
      switch (acc_baud){
             case 0:
                  UART2_Init(4800);
                  break;
             case 1:
                  UART2_Init(9600);
                  break;
             case 2:
                  UART2_Init(19200);
                  break;
             case 3:
                  UART2_Init(38400);
                  break;

      }

      // Set KX3 mode
      if(kxmode == 1) {
         BAUDCON2.DTRXP = 1;
      } else {
         BAUDCON2.DTRXP = 0;
      }

   }//endif

   flags1.configMode = 0;

   uartPtr    = 0;
   readStart  = 0;
   uartMsgs   = 0;
   uartPtr2   = 0;
   readStart2 = 0;
   uartMsgs2  = 0;

   lastB = PORTB;
   INTCON.RBIF = 0;

   Delay_ms(100);

   RC1IF_bit = 0;                                       // Clear interrupt flag
   RC1IE_bit = 1;                                       // turn ON interrupt on UART1 receive

   RC2IF_bit = 0;                                       // Clear interrupt flag
   RC2IE_bit = 1;                                       // turn ON interrupt on UART2 receive

   INTCON.PEIE = 1;
   INTCON.GIE  = 1;                                     //Global Interrupt Enable

   ADC_INIT();

   bandUpFlag   = 0;
   bandDownFlag = 0;
   keyModeFlag  = 0;

   T0CON.TMR0ON = 1;  // Timer0 On/Off Control bit:1=Enables Timer0 / 0=Stops Timer0.

   setFTmode();

}