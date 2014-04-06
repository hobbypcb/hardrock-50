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


const char RX_TOP_BAND[] = " BAND:";
const char RX_TOP_KEY[] = "KEY:";
char BAND_STR[] = "160M";
char KEY_STR[] = "SB";
const char TX_BOTTOM[] = "SWR:-.-  PEP:";
char VOLT_STR[] = "00.0V";
char TEMP_STR[] = "000";
char PEP_STR[] = "00W";
char VSWR_STR[] = "-.-";
char *res;
extern const char SPLASH_TOP[];
extern const char SPLASH_BOTTOM[];


void Show_RX() {                  // Function used for text moving
//  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  memcpy(VSWR_STR, "-.-", 3);
  Lcd_Out(1,1,CopyConst2Ram(msg,RX_TOP_KEY));
  LCD_Out(1,5,KEY_STR);
  Lcd_Out(1,7,CopyConst2Ram(msg,RX_TOP_BAND));                 // Write text in first row
  Lcd_Out(1,13,BAND_STR);

  LCD_Out(2,1,TEMP_STR);
  res = 0;
  res = strchr(TEMP_STR,'.');
  if (res != 0) {
     Lcd_Chr(2,3,223);
     if (tempmode == 0) LCD_Out(2,4,"F       ");
     if (tempmode == 1) LCD_Out(2,4,"C       ");
  } else {
     Lcd_Chr(2,4,223);
     if (tempmode == 0) LCD_Out(2,5,"F      ");
     if (tempmode == 1) LCD_Out(2,5,"C      ");
  }
  LCD_Out(2,12,VOLT_STR);
//  i = 1;
//  if (!flags1.configMode) {
//     uartRxStatus();
//  }
}

void Show_TX() {
//  Lcd_Cmd(_LCD_CLEAR);               // Clear display
/*Lcd_Chr(1, 1, meterLeft);
  Lcd_Chr(1, 2, meterBoth);
  Lcd_chr(1, 3, meterTop);
  Lcd_chr(1, 4, meterBottom);*/
  Lcd_Out(2,1,CopyConst2Ram(msg,TX_BOTTOM));                 // Write text in second row
  Lcd_Out(2,5,"0.0 ");
  Lcd_Out(2,5,VSWR_STR);
  Lcd_Out(2,14,PEP_STR);
//  if (!flags1.configMode) {
//     uartTxStatus();
//  }

  i = 0;
}

void Do_LCD_Init() {
  Lcd_Init();                        // Initialize Lcd

  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off

  Lcd_Out(1,1,CopyConst2Ram(msg,SPLASH_TOP));           // Write Splash Screen in first row
  Lcd_Out(2,1,CopyConst2Ram(msg,SPLASH_BOTTOM));        // Write Splash Screen in second row

  Delay_ms(3000);
  Lcd_Cmd(_LCD_CLEAR);               // Clear display
}

void updateLCD() {
     if (txState == 0) {
        Show_RX();
     } else {
        Show_TX();
     }
     lcdFlag = 0;
}

void changeKeyMode() {
     EEPROM_Write(2, keymode);
     keymode = keymode + 1;
     if (keymode > 2) { keymode = 0; }
     keyDispFlag = 1;
}

void changeKeyModeLCD() {
     switch (keymode) {
            case SB:
                 IOCB.IOCB4 = 0;                    //Disable IOC on RB4/5
                 IOCB.IOCB5 = 0;
                 memcpy(KEY_STR,"SB",2);
                 break;

            case PT:
                 IOCB.IOCB4 = 1;                    //Disable IOC on RB5 enable on RB4
                 IOCB.IOCB5 = 0;
                 memcpy(KEY_STR,"PT",2);
                 break;

            case CR:
                 IOCB.IOCB4 = 0;                    //Disable IOC on RB4 enable on RB5
                 IOCB.IOCB5 = 1;
                 memcpy(KEY_STR,"CR",2);
                 break;

     }
     keyDispFlag = 0;

}