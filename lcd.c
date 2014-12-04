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

// LCD Routines for HARDROCK-50

#include "defs.h"

const char RX_TOP_BAND[]   = " BAND:";
const char RX_TOP_KEY[]    = "KEY:";
const char TX_BOTTOM_SWR[] = "SWR:";
const char TX_BOTTOM_PEP[] = "  PEP:";
const char VSWR_0_0[]      = "0.0 ";
const char TEMP_STR_PAD2[] = "  ";
const char TEMP_STR_PAD6[] = "      ";

char BAND_STR[] = "160";
char KEY_STR[]  = "OFF";
char VOLT_STR[] = "00.0V";
char TEMP_STR[] = "000";
char PEP_STR[]  = "00W";
char VSWR_STR[] = "-.-";
char *res;

extern const char SPLASH_TOP[];
extern const char SPLASH_BOTTOM[];

void Update_LCD(void){
      if (tx_lcdFlag == 1) Show_TX();
      if (rx_lcdFlag == 1) Show_RX();
}

void Show_RX() {
   short pos = 4;
   
   // Restore default VSWR.
   memcpy(VSWR_STR, "-.-", 3);
   
   // Line 1: Display key mode.
   Lcd_Out(1,1,copyConst2Ram(msg,RX_TOP_KEY));
   LCD_Out(1,5,KEY_STR);
   
   // Line 1: Display band.
   Lcd_Out(1,8,copyConst2Ram(msg,RX_TOP_BAND));  // Write text in first row
   Lcd_Out(1,14,BAND_STR);
  
   // Line 2: Display temperature.
   LCD_Out(2,1,TEMP_STR);
   if (TEMP_STR[2] == '.') TEMP_STR[2] = 0x00;
   if (TEMP_STR[1] == '.') TEMP_STR[1] = 0x00;
   if (TEMP_STR[2] == 0x00) pos--;
   if (TEMP_STR[1] == 0x00) pos--;
   Lcd_Chr(2,pos,223);
   
   if (tempmode == 0) LCD_Out(2,pos+1,"F");
   else LCD_Out(2,pos+1,"C");
   
   Lcd_Out(2,pos+2,copyConst2Ram(msg,TEMP_STR_PAD2));
   Lcd_Out(2,6    ,copyConst2Ram(msg,TEMP_STR_PAD6));

   // Line 2: Display voltage.
   LCD_Out(2,12,VOLT_STR);
   
   // Clear the TX meters
   LAST_AVE_FWP = 0;
   LAST_AVE_RFP = 0;
   AVE_FWP      = 0;
   AVE_RFP      = 0;
   PEP_FWP      = 0;
   rx_lcdFlag   = 0;
   
   // Re-Enable UARTS
   if (RCSTA1.SPEN == 0) {
      RCSTA1.SPEN = 1;
      flags1.UART_Buffer_full = 0;
      uartPtr = 0;
   }
   if ((RCSTA2.SPEN == 0) && (ftmode == 0) && (REV_F)) {
      RCSTA2.SPEN = 1;
      flags1.UART2_Buffer_full = 0;
      uartPtr2 = 0;
   }

}


void Show_TX() {
   char b_UNK[] = "----";
   Lcd_Out(2,1,copyConst2Ram(msg,TX_BOTTOM_SWR));
   if (keymode != QR) Lcd_Out(2,5,VSWR_STR);
   Lcd_Out(2,8,copyConst2Ram(msg,TX_BOTTOM_PEP)); 
   if (keymode != QR) Lcd_Out(2,14,PEP_STR);
   if (band == _UNK) Lcd_Out(2,14,B_UNK);
   tx_lcdFlag = 0;
//   i = 0;
}


void doLcdInit() {
   char C1, C2, C3, E_F = 0;
   
   Lcd_Init();                        // Initialize Lcd
   
   Lcd_Cmd(_LCD_CLEAR);               // Clear display
   Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
   
   Lcd_Out(1,1,copyConst2Ram(msg,SPLASH_TOP));    // Write Splash Screen in first row
   Lcd_Out(2,1,copyConst2Ram(msg,SPLASH_BOTTOM)); // Write Splash Screen in second row

   Tuner_Snd_Char('*');
   Tuner_Snd_Char('V');
   Tuner_Snd_Char(13);

   C1 = Tuner_Get_Char();
   if (C1 == 255) E_F = 1;
   C2 = Tuner_Get_Char();
   if (C2 == 255) E_F = 1;
   C3 = Tuner_Get_Char();
   if (C3 == 255) E_F = 1;

   if (E_F == 0){
      Lcd_Chr(2,10,'A');
      Lcd_Chr_Cp('T');
      Lcd_Chr_Cp('U');
      Lcd_Chr_Cp(' ');
      Lcd_Chr_Cp(C1);
      Lcd_Chr_Cp(C2);
      Lcd_Chr_Cp(C3);
      atu_mode  = EEPROM_Read(13);     // 13th position = saved ATU mode
      if (atu_mode  > 2    || atu_mode  < 1  ) { atu_mode  = 1; }
      if (atu_mode == 1){
            // bypass tuner
            Tuner_Snd_Char('*');
            Tuner_Snd_Char('Y');
            Tuner_Snd_Char('1');
            Tuner_Snd_Char(13);
      }
   }
   else{
      atu_mode = 0;
      if (keymode > 2) keymode == 2;
   }
   Delay_ms(3000);
   Lcd_Cmd(_LCD_CLEAR);               // Clear display
}

void changeKeyMode() {
   keymode++;
   if ((atu_mode == 0) && (keymode > 2)) { keymode = 0; }
   if ((atu_mode != 0) && (keymode > 3)) { keymode = 0; }
   EEPROM_Write(2, keymode);
   setKeyMode();
}


void setKeyMode() {
   switch (keymode) {
      case SB:
         IOCB.IOCB4 = 0;         //Disable IOC on RB4/5
         IOCB.IOCB5 = 0;
         memcpy(KEY_STR,"OFF",3);
         break;
      case PT:
         IOCB.IOCB4 = 1;         //Disable IOC on RB5, enable on RB4
         IOCB.IOCB5 = 0;
         memcpy(KEY_STR,"PTT",3);
         break;
      case CR:
         IOCB.IOCB4 = 0;         //Disable IOC on RB4, enable on RB5
         IOCB.IOCB5 = 1;
         memcpy(KEY_STR,"COR",3);
         break;
      case QR:
         IOCB.IOCB4 = 1;         //Enable IOC on RB4, enable on RB5
         IOCB.IOCB5 = 1;
         memcpy(KEY_STR,"QRP",3);
         break;
   }//endswitch
}