/*  Diagnostics Routines

#include "defs.h"

void portTest() {
   Lcd_Out(1,1,"Diagnostics Run");
   Lcd_Out(2,1,"Starting in 10s");
   delay_ms(10000);

   LCD_Out(2,1,"Toggling 6M ON ");
   if (REV_F) {
      REVF_6M_RLY = 1;
   } else {
      _6M_RLY = 1;
   }
   delay_ms(4000);
   if (REV_F) {
      REVF_6M_RLY = 0;
   } else {
      _6M_RLY = 0;
   }
   LCD_Out(2,1,"Toggling 6M OFF");
   delay_ms(2000);
   
   LCD_Out(2,1,"Toggling 10M ON ");
   if (REV_F) {
      REVF_10_12_15M_RLY = 1;
   } else {
      _10_12_15M_RLY = 1;
   }
   delay_ms(4000);
   if (REV_F) {
      REVF_10_12_15M_RLY = 0;
   } else {
      _10_12_15M_RLY = 0;
   }
   LCD_Out(2,1,"Toggling 10M OFF");
   delay_ms(2000);
   
   LCD_Out(2,1,"Toggling 17M ON ");
   _17_20M_RLY = 1;
   delay_ms(4000);
   _17_20M_RLY = 0;
   LCD_Out(2,1,"Toggling 17M OFF");
   delay_ms(2000);
   
   LCD_Out(2,1,"Toggling 30M ON ");
   _30_40M_RLY = 1;
   delay_ms(4000);
   _30_40M_RLY = 0;
   LCD_Out(2,1,"Toggling 30M OFF");
   delay_ms(2000);
   
   LCD_Out(2,1,"Toggling 80M ON ");
   _80M_RLY = 1;
   delay_ms(4000);
   _80M_RLY = 0;
   LCD_Out(2,1,"Toggling 80M OFF");
   delay_ms(2000);
   
   LCD_Out(1,1,"Returning to    ");
   LCD_Out(2,1,"Main Program    ");
   delay_ms(2000);
}
*/