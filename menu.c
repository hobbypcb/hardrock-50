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

//  LCD Menu routines for HARDROCK-50

#include "defs.h"

const char * const msg_baud_rates[] = {"4800 ", "9600 ", "19200", "38400"};
const char * const msg_kx_modes[]   = {"NO ", "YES"};
const char * const msg_temp_modes[] = {"Fahrenheit", "Celsius   "};
const char * const msg_menu[] = {
   "1. Exit         ",
   "2. Baud Rate    ",
   "3. KX3 Serial   ",
   "4. Temp. Display"
};

const char            blank_line[] = "                ";
extern unsigned short buttons;
short                 baud, kxmode;
//extern unsigned short tempmode;
short                 new_baud, new_kxmode;
unsigned short        new_tempmode;
short                 menu_active;


// M A I N   M E N U
void displayMenu() {
   short item     = 1;
   short old_item = -1;

   // Read saved values from EEPROM.
   tempmode = EEPROM_Read(3); // 3rd position = saved tempmode (0=F, 1=C)
   baud     = EEPROM_Read(4); // 4th position = saved baud
   kxmode   = EEPROM_Read(5); // 5th position = saved kxmode

   // Make sure saved values are valid.  Use defaults if necessary.
   if (baud > 3     || baud < 0)     { baud = 2;     }
   if (kxmode > 1   || kxmode < 0)   { kxmode = 0;   }
   if (tempmode > 1 || tempmode < 0) { tempmode = 0; }

   Lcd_Cmd(_LCD_CLEAR); // Clear display

   // Menu loop.
   menu_active = 1;
   while (menu_active) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            //if (item < 4) { item++; }
            item++;
            if (item > 4) { item = 1; }
            break;
         case BTN_DN:
            //if (item > 1) { item--; }
            item--;
            if (item < 1) { item = 4; }
            break;
         case BTN_KY:
            switch (item) {
               case 1:
                  menu_active = 0;
                  break;
               case 2:
                  menuBaudRate();
                  break;
               case 3:
                  menuKxMode();
                  break;
               case 4:
                  menuTempMode();
                  break;
            } //endswitch
            break;
         case BTN_LONG_KY:
            menu_active = 0;
            break;
         default:
            delay_ms(1);
            backgroundTasks();
            break;
      } //endswitch

      // Update the LCD display when a new menu item is selected.
      if(item != old_item) {
         old_item = item;
         Lcd_Out(1,1,CopyConst2Ram(msg,msg_menu[item - 1]));
         Lcd_Out(2,1,CopyConst2Ram(msg,blank_line));
         // Add the current value to line two.
         switch(item) {
            case 1:
               break;
            case 2:
               // Display current baud rate.
               Lcd_Out(2,4,CopyConst2Ram(msg,msg_baud_rates[baud]));
               break;
            case 3:
               // Display current KX mode.
               Lcd_Out(2,4,CopyConst2Ram(msg,msg_kx_modes[kxmode]));
               break;
            case 4:
               // Display current temperature mode.
               Lcd_Out(2,4,CopyConst2Ram(msg,msg_temp_modes[tempmode]));
               break;
         }//endswitch
      } //endif
   } //endwhile
   lcdFlag = 1;
   checkTxState();
}

// B A U D   R A T E
// Display menu used to select new baud rate on line-2.
void menuBaudRate() {
   addMenuArrows();
   // Display current baud rate.
   new_baud = baud;
   Lcd_Out(2,4,CopyConst2Ram(msg,msg_baud_rates[new_baud]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_baud++;
            if (new_baud > 3) { new_baud = 3; }
            Lcd_Out(2,4,CopyConst2Ram(msg,msg_baud_rates[new_baud]));
            break;
         case BTN_DN:
            new_baud--;
            if (new_baud < 0) { new_baud = 0; }
            Lcd_Out(2,4,CopyConst2Ram(msg,msg_baud_rates[new_baud]));
            break;
         case BTN_LONG_KY:
            menu_active = 0;
            break;
         default:
            delay_ms(1);
            backgroundTasks();
            break;
      }//endswitch
   }
   removeMenuArrows();

   // User changed the baud rate.  Reconfigure the two UARTS.
   if (new_baud != baud) {
      baud = new_baud;
      EEPROM_Write(4, baud);

      // Disable both UARTs.
      RCSTA1.SPEN = 0;
      RCSTA2.SPEN = 0;

      // For REVE, use UART1.
      if (version != 0x46) {
         if      (baud == 0) { UART1_Init(4800 );} 
         else if (baud == 1) { UART1_Init(9600 );} 
         else if (baud == 2) { UART1_Init(19200);} 
         else if (baud == 3) { UART1_Init(38400);}
      // Ror REVF and above, use UART2.
      } else {
         if      (baud == 0) { UART2_Init(4800 );} 
         else if (baud == 1) { UART2_Init(9600 );} 
         else if (baud == 2) { UART2_Init(19200);} 
         else if (baud == 3) { UART2_Init(38400);}
      }

      // Clear out UART buffers.
      uartPtr    = 0;
      uartMsgs   = 0;
      readStart  = 0;
      flags1.UART_Buffer_Full  = 0;
      uartPtr2   = 0;
      uartMsgs2  = 0;
      readStart2 = 0;
      flags1.UART2_Buffer_Full = 0;

      // Re-enable both UARTs.
      RCSTA1.SPEN = 1;
      RCSTA2.SPEN = 1;
   }
}


// K X 3   S E R I A L   M O D E
// Display menu used to select new KX mode on line-2 .
void menuKxMode() {
   addMenuArrows();
   // Display current KX mode.
   new_kxmode = kxmode;
   Lcd_Out(2,4,CopyConst2Ram(msg,msg_kx_modes[new_kxmode]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_kxmode++;
            if (new_kxmode > 1) { new_kxmode = 1; }
            Lcd_Out(2,4,CopyConst2Ram(msg,msg_kx_modes[new_kxmode]));
            break;
         case BTN_DN:
            new_kxmode--;
            if (new_kxmode < 0) { new_kxmode = 0; }
            Lcd_Out(2,4,CopyConst2Ram(msg,msg_kx_modes[new_kxmode]));
            break;
         case BTN_LONG_KY:
            menu_active = 0;
            break;
         default:
            delay_ms(1);
            backgroundTasks();
            break;
      }//endswitch
   }
   removeMenuArrows();

   // User changed the KX3 serial mode.  Reconfigure the two UARTS.
   if (new_kxmode != kxmode) {
      kxmode = new_kxmode;
      EEPROM_Write(5, kxmode);

      // Disable both UARTs.
      RCSTA1.SPEN = 0;
      RCSTA2.SPEN = 0;

      // For REVE, use UART1 with custom settings.
      if (version != 0x46) {
         if(kxmode == 1) {
            BAUDCON1.DTRXP = 1;
            TRISB6_bit     = 0;
            LATB6_bit      = 0;
         } else {
            BAUDCON1.DTRXP = 0;
         }
      // Ror REVF and above, use UART2.
      } else {
         if(kxmode == 1) {
            BAUDCON2.DTRXP = 1;
         } else {
            BAUDCON2.DTRXP = 0;
         }
      }

      //Re-enable both UARTs.
      RCSTA1.SPEN = 1;
      RCSTA2.SPEN = 1;
   }
}

// T E M P E R A T U R E   M O D E
// Display menu used to select new temperature mode on line-2.
void menuTempMode() {
   addMenuArrows();
   // Display current temperature mode (F or C).
   new_tempmode = tempmode;
   Lcd_Out(2,4,CopyConst2Ram(msg,msg_temp_modes[new_tempmode]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_tempmode++;
            if (new_tempmode > 1) { new_tempmode = 1; }
            Lcd_Out(2,4,CopyConst2Ram(msg,msg_temp_modes[new_tempmode]));
            break;
         case BTN_DN:
            if (new_tempmode != 0) { new_tempmode--; }
            Lcd_Out(2,4,CopyConst2Ram(msg,msg_temp_modes[new_tempmode]));
            break;
         case BTN_LONG_KY:
            menu_active = 0;
            break;
         default:
            delay_ms(1);
            backgroundTasks();
            break;
      }//endswitch
   }
   removeMenuArrows();

   // User changed the temperature mode.  Reset F/C.
   if (new_tempmode != tempmode) {
      tempmode = new_tempmode;
      EEPROM_Write(3, tempmode);
      // Force the temperature to be recalculated.
      checkTemperature(1);
   }
}


// Add < and > to LCD line 2
void addMenuArrows() {
   Lcd_Chr(2,1,'<');
   Lcd_Chr(2,16,'>');
}

// Remove < and > from LCD line 2
void removeMenuArrows() {
   Lcd_Chr(2,1,' ');
   Lcd_Chr(2,16,' ');
}