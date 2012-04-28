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

char SPLASH_TOP[] = "HARDROCK-50 AMP";
char SPLASH_BOTTOM[] = "VERSION 3 FW:1.0";

char RX_TOP_BAND[] = "BAND:";
char RX_TOP_KEY[] = "KEY:";
char BAND_STR[] = "160M";
char KEY_STR[] = "SB";
char RX_BOTTOM[] = "104.1      13.8V";
char TX_BOTTOM[] = "SWR:1.7  PEP:31W";
char VOLT_STR[] = "00.0V";
char TEMP_STR[] = "000.0F";


const char leftBracket[] = {31,16,16,16,16,16,16,31};
const char rightBracket[] = {31,1,1,1,1,1,1,31};

void ShowLeftBracket(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(64);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(leftBracket[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 0);
}

void ShowRightBracket(char pos_row, char pos_char) {
  char i;
    Lcd_Cmd(72);
    for (i = 0; i<=7; i++) Lcd_Chr_CP(rightBracket[i]);
    Lcd_Cmd(_LCD_RETURN_HOME);
    Lcd_Chr(pos_row, pos_char, 1);
}


void Show_RX() {                  // Function used for text moving
//  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  Lcd_Out(1,1,RX_TOP_KEY);
  LCD_Out(1,5,KEY_STR);
  Lcd_Out(1,8,RX_TOP_BAND);                 // Write text in first row
  Lcd_Out(1,13,BAND_STR);

  LCD_Out(2,1,TEMP_STR);
  Lcd_Chr(2,6,223);
  LCD_Out(2,7,"F    ");
  LCD_Out(2,12,VOLT_STR);
//  Lcd_Out(2,1,RX_BOTTOM);                 // Write text in second row
//  i = 1;
}

void Show_TX() {
  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  ShowLeftBracket(1,1);
  ShowRightBracket(1,16);
  Lcd_Out(2,1,TX_BOTTOM);                 // Write text in second row
  i = 0;
}

void Do_LCD_Init() {
  Lcd_Init();                        // Initialize Lcd

  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off

  Lcd_Out(1,1,SPLASH_TOP);           // Write Splash Screen in first row
  Lcd_Out(2,1,SPLASH_BOTTOM);        // Write Splash Screen in second row

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