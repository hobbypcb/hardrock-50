/* 
    Copyright 2012 HobbyPCB LLC

    This file is part of HARDROCK-50 Control Firmware

    HARDROCK-50 Control Firmware is free software: you can redistribute it and/or modify it under the terms of the

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
const char * const msg_ATU_modes[] = {"No ATU   ","Bypass   ", "Active   ","Wattmeter","Update FW","Erase Mem"};
const char * const msg_menu[] = {
   "1. Exit         ",
   "2. ACC Baud Rate",
   "3. USB Baud Rate",
   "4. KX3 Serial   ",
   "5. FT-817 Mode  ",
   "6. Temp. Display",
   "7. Adj Watt Metr",
   "8. COR Hang Time",
   "9. Key-up Delay ",
   "10. ATU Mode    "
};
const char msg_FTmode[] = " FT-817 MODE ON ";

const char TBOOT_TOP[]      = "   Update ATU   ";
const char TBOOT_BOTTOM[]   = "    Firmware    ";

const char TERASE_TOP[]      = " Erasing        ";
const char TERASE_BOTTOM[]   = " Memories...    ";

const char WATTMTR_TOP[]     = "FW:     PEP:    ";
const char WATTMTR_BOTTOM[]  = "RF:     SWR:    ";

const char            blank_line[]  = "                ";
const char            PERCENT_PAD[] = "%   ";
const char            NUMMSEC_PAD[] = " msec    ";
const char            COMPLETE[]    = "Complete";
const char            ERROR[]       = "Error";

extern unsigned short buttons;
unsigned short        new_tempmode;
short                 new_acc_baud;
short                 new_kxmode;
short                 new_ftmode;
short                 new_usb_baud;
short                 new_meter_adj;
int                   new_cor_htime;
int                   new_key_delay;
short                 new_atu_mode;
char                  PW_STR[5];
char                  E_F;


// M A I N   M E N U
void displayMenu() {
   short item     = 1;
   short old_item = 1;

   // Display menu item #1.
   Lcd_Out(1,1,copyConst2Ram(msg,msg_menu[0]));
   Lcd_Out(2,1,copyConst2Ram(msg,blank_line));
   waitButtonRelease();

   // Menu loop.
   menu_active = 1;
   while (menu_active) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:  //Step forward through menu skipping iten 3 for Rev E
            item++;
            if (item > 10) { item = 1; }
            if (item == 3 && REV_E) { item = 4; }
            if (item == 5 && REV_E) { item = 6; }
            break;
         case BTN_DN:  //Step backward through menu skipping iten 3 for Rev E
            item--;
            if (item < 1) { item = 10; }
            if (item == 3 && REV_E) { item = 2; }
            if (item == 5 && REV_E) { item = 4; }
            break;
         case BTN_KY:  //Menu item selected for update
            switch (item) {
               case 1:  //exit
                  menu_active = 0;
                  break;
               case 2:  //set ACC baudrate
                  menuAccBaudRate();
                  break;
               case 3:  //set USB baudrate
                  // REV-F and higher only.
                  menuUsbBaudRate();
                  break;
               case 4:  //KX3 data mode
                  menuKxMode();
                  break;
               case 5:  //FT-817 mode
                  menuFtMode();
                  break;
               case 6:  //temperature scale
                  menuTempMode();
                  break;
               case 7:  //calibrate the wattmeter
                  menuAdustMeter();
                  break;
               case 8:  //COR hang time
                  menuCorHangTime();
                  break;
               case 9:  //PTT delay
                  menuKeyupDelay();
                  break;
               case 10: //Tuner functions
                  menuATUMode();
                  break;
            } //endswitch
            break;
         case BTN_LONG_KY: //Holding the KY button for 2 seconds exits the menu
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
         Lcd_Out(1,1,copyConst2Ram(msg,msg_menu[item - 1]));
         Lcd_Out(2,1,copyConst2Ram(msg,blank_line));
         // Add the current value to line two.
         switch(item) {
            case 1:
               break;
            case 2:
               // Display current ACC baud rate.
               if (ftmode == 1) Lcd_Out(2,1,copyConst2Ram(msg,msg_FTmode));
               else Lcd_Out(2,4,copyConst2Ram(msg,msg_baud_rates[acc_baud]));
               break;
            case 3:
               // Display current USB baud rate.
               Lcd_Out(2,4,copyConst2Ram(msg,msg_baud_rates[usb_baud]));
               break;
            case 4:
               // Display current KX mode.
               if (ftmode == 1) Lcd_Out(2,1,copyConst2Ram(msg,msg_FTmode));
               else Lcd_Out(2,4,copyConst2Ram(msg,msg_kx_modes[kxmode]));
               break;
            case 5:
               // Display current KX mode.
               Lcd_Out(2,4,copyConst2Ram(msg,msg_kx_modes[ftmode]));
               break;
            case 6:
               // Display current temperature mode.
               Lcd_Out(2,4,copyConst2Ram(msg,msg_temp_modes[tempmode]));
               break;
            case 7:
               // Display current watt meter calibration.
               showPercent(meter_adj);
               break;
            case 8:
               // Display current COR hang time.
               showNumMsec(cor_htime);
               break;
            case 9:
               // Display current key-up delay.
               showNumMsec(key_delay);
               break;
            case 10:
               // Display current ATU mode.
               Lcd_Out(2,4,copyConst2Ram(msg,msg_ATU_modes[atu_mode]));
               break;
         }//endswitch
      } //endif
   } //endwhile
   Update_LCD();
   waitButtonRelease();
   checkTxState();
}
// A T U  M O D E
// Display menu on line-2 used to select new ATU mode.
void menuATUMode() {
   char C1, C2;
   int i;
   
   addMenuArrows();
   // Display current atu mode.
   new_atu_mode = atu_mode;
   Lcd_Out(2,4,copyConst2Ram(msg,msg_ATU_modes[new_atu_mode]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            if (atu_mode != 0){
               new_atu_mode++;
               if (new_atu_mode > 5) { new_atu_mode = 5; }
               Lcd_Out(2,4,copyConst2Ram(msg,msg_ATU_modes[new_atu_mode]));
            }
            break;
         case BTN_DN:
            if (atu_mode != 0){
               if (new_atu_mode != 1) { new_atu_mode--; }
               Lcd_Out(2,4,copyConst2Ram(msg,msg_ATU_modes[new_atu_mode]));
            }
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


   // ATU mode set to bypass.
   if (new_atu_mode == 1){
      atu_mode = new_atu_mode;
      EEPROM_Write(13, atu_mode);
      // bypass tuner
      Tuner_Byp(1);
   }

   // ATU mode set to active.
   if (new_atu_mode == 2){
      atu_mode = new_atu_mode;
      EEPROM_Write(13, atu_mode);
      // unbypass tuner
      Tuner_Byp(0);
   }
   // ATU mode set to Wattmeter.
   if (new_atu_mode == 3){
      
      //Bypass the tuner in Wattmeter mode if needed and set band to B11
      if (atu_mode == 2) Tuner_Byp(1);
      Tuner_Snd_Char('*');
      Tuner_Snd_Char('B');
      Tuner_Snd_Char('1');
      Tuner_Snd_Char('1');
      Tuner_Snd_Char(13);

      // display message
      Lcd_Out(1,1,copyConst2Ram(msg,WATTMTR_TOP));
      Lcd_Out(2,1,copyConst2Ram(msg,WATTMTR_BOTTOM));

      E_F = 0;

      while (E_F == 0){
          // get Forward Power:
          Tuner_Snd_Char('*');
          Tuner_Snd_Char('P');
          Tuner_Snd_Char('F');
          Tuner_Snd_Char(13);

          Get_WM();
          if (E_F == 0) LCD_Out(1,4,PW_STR);

          // get Reflected Power
          Tuner_Snd_Char('*');
          Tuner_Snd_Char('P');
          Tuner_Snd_Char('R');
          Tuner_Snd_Char(13);

          Get_WM();
          if (E_F == 0) LCD_Out(2,4,PW_STR);

          // Get SWR
          Tuner_Snd_Char('*');
          Tuner_Snd_Char('W');
          Tuner_Snd_Char(13);

          Get_WM();
          if (E_F == 0) LCD_Out(2,13,PW_STR);

          //get Peak Power
          Tuner_Snd_Char('*');
          Tuner_Snd_Char('P');
          Tuner_Snd_Char('P');
          Tuner_Snd_Char(13);
          Get_WM();
          if (E_F == 0) LCD_Out(1,13,PW_STR);

          Delay_ms(300);
          buttons = checkButtons();
          if (buttons != BTN_NONE) E_F = 1;
      }
      Lcd_Out(1,1,copyConst2Ram(msg,msg_menu[9]));
      Lcd_Out(2,1,copyConst2Ram(msg,blank_line));
      Lcd_Out(2,4,copyConst2Ram(msg,msg_ATU_modes[atu_mode]));
      
      //Un-Bypass the tuner in Wattmeter mode if needed
      if (atu_mode == 2) Tuner_Byp(0);
      changeBandLCD(2);
      
      //Clear keying flag
      INTCON.RBIF = 0;
      
  }
   // ATU mode set to update firmware.
   if (new_atu_mode == 4){
      // turn off USARTs
      RCSTA1 = 0;
      RCSTA2 = 0;
      // tristate the pins
      TRISC.B6 = 1;
      TRISC.B7 = 1;
      // tell tuner to wait for bootloader
      Tuner_Snd_Char('*');
      Tuner_Snd_Char('U');
      Tuner_Snd_Char(13);
      // display message
      Lcd_Out(1,1,copyConst2Ram(msg,TBOOT_TOP));      
      Lcd_Out(2,1,copyConst2Ram(msg,TBOOT_BOTTOM));
      // when ATU comes back it will be bypassed
      EEPROM_Write(13, 1);
      //the only way out is a power cycle...
      while(1);
   }

   // ATU mode set to erase memories.
   if (new_atu_mode == 5){

      // tell tuner to erase memories
      Tuner_Snd_Char('*');
      Tuner_Snd_Char('E');
      Tuner_Snd_Char(13);

      // display message
      Lcd_Out(1,1,copyConst2Ram(msg,TERASE_TOP));
      Lcd_Out(2,1,copyConst2Ram(msg,TERASE_BOTTOM));

      E_F = 0;
      i=0;
      C1 = 255;
      while ((i++ < 3000) && C1 == 255){C1 = Tuner_Get_Char();}
      if (C1 == 255) E_F = 1;

      i=0;
      C2 = 255;
      while ((i++ < 2) && C2 == 255){C2 = Tuner_Get_Char();}
      if (C2 == 255) E_F = 1;

      Lcd_Cmd(_LCD_CLEAR); // Clear display

      if (E_F != 0){Lcd_Out(1,5,copyConst2Ram(msg,ERROR));}
      else {Lcd_Out(1,5,copyConst2Ram(msg,COMPLETE));}

      Delay_ms(1500);
      Lcd_Out(1,1,copyConst2Ram(msg,msg_menu[9]));
      Lcd_Out(2,1,copyConst2Ram(msg,blank_line));
      Lcd_Out(2,4,copyConst2Ram(msg,msg_ATU_modes[atu_mode]));

   }
}

// A C C   B A U D   R A T E
// Display menu on line-2 used to select new ACC baud rate.
void menuAccBaudRate() {
   if(ftmode == 1){
        Lcd_Out(2,1,copyConst2Ram(msg,msg_FTmode));
        return;
   }
   addMenuArrows();
   // Display current ACC baud rate.
   new_acc_baud = acc_baud;
   Lcd_Out(2,4,copyConst2Ram(msg,msg_baud_rates[new_acc_baud]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_acc_baud++;
            if (new_acc_baud > 3) { new_acc_baud = 3; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_baud_rates[new_acc_baud]));
            break;
         case BTN_DN:
            new_acc_baud--;
            if (new_acc_baud < 0) { new_acc_baud = 0; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_baud_rates[new_acc_baud]));
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

   // User changed the baud rate.  Reconfigure the appropriate UART.
   if (new_acc_baud != acc_baud) {
      acc_baud = new_acc_baud;
      EEPROM_Write(4, acc_baud);

      // For REV_E, use UART1.
      if (REV_E) {

         // Disable UART1.
         RCSTA1.SPEN = 0;

         // Reset UART1 baud rate.
         if      (acc_baud == 0) { UART1_Init(4800 ); }
         else if (acc_baud == 1) { UART1_Init(9600 ); }
         else if (acc_baud == 2) { UART1_Init(19200); }
         else if (acc_baud == 3) { UART1_Init(38400); }

         // Clear out UART1 buffer.
         uartPtr    = 0;
         uartMsgs   = 0;
         readStart  = 0;
         flags1.UART_Buffer_Full  = 0;

         // Re-enable UART1.
         RCSTA1.SPEN = 1;

      // For REV_F and above, use UART2.
      } else {

         // Disable UART2.
         RCSTA2.SPEN = 0;

         // Reset UART2 baud rate.
         if      (acc_baud == 0) { UART2_Init(4800 ); }
         else if (acc_baud == 1) { UART2_Init(9600 ); }
         else if (acc_baud == 2) { UART2_Init(19200); }
         else if (acc_baud == 3) { UART2_Init(38400); }

         // Clear out UART2 buffer.
         uartPtr2   = 0;
         uartMsgs2  = 0;
         readStart2 = 0;
         flags1.UART2_Buffer_Full = 0;

         // Re-enable UART2.
         RCSTA2.SPEN = 1;
      }
   }
}


// U S B   B A U D   R A T E
// Display menu on line-2 used to select new USB baud rate.
void menuUsbBaudRate() {
   addMenuArrows();
   // Display current USB baud rate.
   new_usb_baud = usb_baud;
   Lcd_Out(2,4,copyConst2Ram(msg,msg_baud_rates[new_usb_baud]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_usb_baud++;
            if (new_usb_baud > 3) { new_usb_baud = 3; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_baud_rates[new_usb_baud]));
            break;
         case BTN_DN:
            new_usb_baud--;
            if (new_usb_baud < 0) { new_usb_baud = 0; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_baud_rates[new_usb_baud]));
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

   // User changed the baud rate.  Reconfigure the REV_F USB UART.
   if (new_usb_baud != usb_baud) {
      usb_baud = new_usb_baud;
      EEPROM_Write(7, usb_baud);

      // For REV_F, use UART1.
      if (REV_F) {

         // Disable UART1.
         RCSTA1.SPEN = 0;

         // Reset UART1 baud rate.
         if      (usb_baud == 0) { UART1_Init(4800 ); }
         else if (usb_baud == 1) { UART1_Init(9600 ); }
         else if (usb_baud == 2) { UART1_Init(19200); }
         else if (usb_baud == 3) { UART1_Init(38400); }

         // Clear out UART1 buffer.
         uartPtr    = 0;
         uartMsgs   = 0;
         readStart  = 0;
         flags1.UART_Buffer_Full  = 0;

         // Re-enable UART1.
         RCSTA1.SPEN = 1;
      }
   }
}

// K X 3   S E R I A L   M O D E
// Display menu on line-2 used to select new KX mode.
void menuKxMode() {
   if(ftmode == 1){
        Lcd_Out(2,1,copyConst2Ram(msg,msg_FTmode));
        return;
   }
   addMenuArrows();
   // Display current KX mode.
   new_kxmode = kxmode;
   Lcd_Out(2,4,copyConst2Ram(msg,msg_kx_modes[new_kxmode]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_kxmode++;
            if (new_kxmode > 1) { new_kxmode = 1; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_kx_modes[new_kxmode]));
            break;
         case BTN_DN:
            new_kxmode--;
            if (new_kxmode < 0) { new_kxmode = 0; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_kx_modes[new_kxmode]));
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
      setKX3mode();
   }
}
void setKX3mode(void){
      // For REV_E, use UART1 with custom settings.
      if (REV_E) {

         // Disable UART1.
         RCSTA1.SPEN = 0;

         if(kxmode == 1) {
            BAUDCON1.DTRXP = 1;
            TRISB6_bit     = 0;
            LATB6_bit      = 0;
         } else {
            BAUDCON1.DTRXP = 0;
         }

         //Re-enable UART1.
         RCSTA1.SPEN = 1;

      // For REV_F and above, use UART2.
      } 
      else {

         // Disable UART2.
         RCSTA2.SPEN = 0;

         if(kxmode == 1) {
            BAUDCON2.DTRXP = 1;
         } else {
            BAUDCON2.DTRXP = 0;
         }

         //Re-enable UART2.
         RCSTA2.SPEN = 1;
      }
}

// FT-817 Analog Band Detect  M O D E
// Display menu on line-2 used to select new FT mode.
void menuFtMode() {
   addMenuArrows();
   // Display current FT mode.
   new_ftmode = ftmode;
   Lcd_Out(2,4,copyConst2Ram(msg,msg_kx_modes[new_ftmode]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_ftmode++;
            if (new_ftmode > 1) { new_ftmode = 1; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_kx_modes[new_ftmode]));
            break;
         case BTN_DN:
            new_ftmode--;
            if (new_ftmode < 0) { new_ftmode = 0; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_kx_modes[new_ftmode]));
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
   if (new_ftmode != ftmode) {
      ftmode = new_ftmode;
      EEPROM_Write(14, ftmode);
      setFTmode();
  }
}


// T E M P E R A T U R E   M O D E
// Display menu on line-2 used to select new temperature mode.
void menuTempMode() {
   addMenuArrows();
   // Display current temperature mode (F or C).
   new_tempmode = tempmode;
   Lcd_Out(2,4,copyConst2Ram(msg,msg_temp_modes[new_tempmode]));

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_tempmode++;
            if (new_tempmode > 1) { new_tempmode = 1; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_temp_modes[new_tempmode]));
            break;
         case BTN_DN:
            if (new_tempmode != 0) { new_tempmode--; }
            Lcd_Out(2,4,copyConst2Ram(msg,msg_temp_modes[new_tempmode]));
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


// W A T T   M E T E R   C A L I B R A T I O N
// Display menu on line-2 used to change the watt meter adjustment.
void menuAdustMeter() {
   addMenuArrows();
   // Display current meter adj.
   new_meter_adj = meter_adj;
   showPercent(new_meter_adj);

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_meter_adj++;
            if (new_meter_adj > 25) { new_meter_adj = 25; }
            showPercent(new_meter_adj);
            break;
         case BTN_DN:
            new_meter_adj--;
            if (new_meter_adj < -25) { new_meter_adj = -25; }
            showPercent(new_meter_adj);
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

   // User changed the watt meter calibration.  Reconfigure.
   if (new_meter_adj != meter_adj) {
      meter_adj = new_meter_adj;
      EEPROM_Write(8, meter_adj);
      adjustWattMeter(meter_adj);
   }
}


// C O R   H A N G   T I M E
// Display menu on line-2 used to change the COR hang time.
void menuCorHangTime() {
   addMenuArrows();
   // Display current COR hang time.
   new_cor_htime = cor_htime;
   showNumMsec(new_cor_htime);

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_cor_htime += 50;
            if (new_cor_htime > 3000) { new_cor_htime = 3000; }
            showNumMsec(new_cor_htime);
            break;
         case BTN_DN:
            new_cor_htime -= 50;
            if (new_cor_htime < 0) { new_cor_htime = 0; }
            showNumMsec(new_cor_htime);
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

   // User changed the COR hang time.  Store new value in EEPROM.
   if (new_cor_htime != cor_htime) {
      cor_htime = new_cor_htime;
      EEPROM_Write_int(9, cor_htime);
   }
}


// K E Y - U P   D E L A Y
// Display menu on line-2 used to change the key-up delay.
void menuKeyupDelay() {
   addMenuArrows();
   // Display current COR hang time.
   new_key_delay = key_delay;
   showNumMsec(new_key_delay);

   buttons = 0;
   while (buttons != BTN_KY && buttons != BTN_LONG_KY) {
      buttons = checkButtons();
      switch (buttons) {
         case BTN_UP:
            new_key_delay++;
            if (new_key_delay > 50) { new_key_delay = 50; }
            showNumMsec(new_key_delay);
            break;
         case BTN_DN:
            new_key_delay--;
            if (new_key_delay < 0) { new_key_delay = 0; }
            showNumMsec(new_key_delay);
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

   // User changed the key-up delay.  Store new value in EEPROM.
   if (new_key_delay != key_delay) {
      key_delay = new_key_delay;
      EEPROM_Write_int(11, key_delay);
   }
}


// Display signed short value as a percentage on LCD line 2 (-128% .. +127%).
void showPercent(short percent) {
   short i;
   ShortToStr(percent, tmpString);
   // Add a leading plus sign (+).
   if(percent > 0) {
      for (i=2; i>=0; i--) {
         if(tmpString[i] == ' ') {tmpString[i] = '+'; break;}
      }//endfor   
   }//endif
   memcpy(tmpString+4, copyConst2Ram(msg,PERCENT_PAD), 5);
   Lcd_Out(2,4,Ltrim(tmpString));
}


// Display signed int value as a milli-second value on LCD line 2.
void showNumMsec(int num) {
   IntToStr(num, tmpString);
   memcpy(tmpString+6, copyConst2Ram(msg,NUMMSEC_PAD), 10);
   Lcd_Out(2,4,Ltrim(tmpString));
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

void Get_WM (void){
int i;

          E_F = 0;
          i=0;
          PW_STR[0] = 255;
          while ((i++ < 30) && PW_STR[0] == 255){PW_STR[0] = Tuner_Get_Char();}
          if (PW_STR[0] == 255) E_F = 1;

          i=0;
          PW_STR[1] = 255;
          while ((i++ < 30) && PW_STR[1] == 255){PW_STR[1] = Tuner_Get_Char();}
          if (PW_STR[1] == 255) E_F = 1;

          i=0;
          PW_STR[2] = 255;
          while ((i++ < 30) && PW_STR[2] == 255){PW_STR[2] = Tuner_Get_Char();}
          if (PW_STR[2] == 255) E_F = 1;

          i=0;
          PW_STR[3] = 255;
          while ((i++ < 30) && PW_STR[3] == 255){PW_STR[3] = Tuner_Get_Char();}
          if (PW_STR[3] == 255) E_F = 1;

          i=0;
          PW_STR[4] = 255;
          while ((i++ < 30) && PW_STR[4] == 255){PW_STR[4] = Tuner_Get_Char();}
          if (PW_STR[4] == 255) E_F = 1;
          else PW_STR[4] = 0;

}