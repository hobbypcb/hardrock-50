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

//  Band Switching routines for HARDROCK-50

#include "defs.h"

void changeBandDisplay(int direction) {
   eepromUpdateFlag = 1;
   if (direction > 0) {
      if (band < 10)  band++;
      if (atu_mode == 0){
         if (band == _60M) { band = _80M; } // Skip over 60M going up, if there's no tuner
      }
   } 
   else {
      if (band > 0)  band--;
      if (atu_mode == 0){
         if (band == _60M) { band = _40M; } // Skip over 60M going down, if there's no tuner
      }
   }

   changeBandLCD(1);
}

void changeBandLCD(char snd_ATU) {
   if (snd_ATU < 2){
       switch (band) {
          case _6M:
             memcpy(BAND_STR,"6M ",3);
             break;
             // change interrupt here
          case _10M:
             memcpy(BAND_STR,"10M",3);
             break;
             // change interrupt here
          case _12M:
             memcpy(BAND_STR,"12M",3);
             break;
             // change interrupt here
          case _15M:
             memcpy(BAND_STR,"15M",3);
             break;
             // change interrupt here
          case _17M:
             memcpy(BAND_STR,"17M",3);
             break;
             // change interrupt here
          case _20M:
             memcpy(BAND_STR,"20M",3);
             break;
             // change interrupt here
          case _30M:
             memcpy(BAND_STR,"30M",3);
             break;
             // change interrupt here
          case _40M:
             memcpy(BAND_STR,"40M",3);
             break;
             // change interrupt here
          case _60M:
             if (atu_mode == 0) {memcpy(BAND_STR,"UNK",3);}
             else {memcpy(BAND_STR,"60M",3);}
             break;
             // change interrupt here
          case _80M:
             memcpy(BAND_STR,"80M",3);
             break;
             // change interrupt here
          case _160M:
             memcpy(BAND_STR,"160",3);
             break;
             // change interrupt here
          default:
             memcpy(BAND_STR,"UNK",3);
       }//endswitch
       rx_lcdFlag = 1;
   }
//If the ATU is installed, send the band data to the ATU
   if ((atu_mode != 0) && (snd_ATU == 1)){
      Tuner_Snd_Char('*');
      Tuner_Snd_Char('B');
      if (band < 10) {
         Tuner_Snd_Char(band + 48);
      }
      else {
         Tuner_Snd_Char('1');
         if (band == 10) Tuner_Snd_Char('0');
         else Tuner_Snd_Char('1');
      }
      Tuner_Snd_Char(13);
   }

}


void allBandRlyOFF() {
   if (REV_F) {
      REVF_6M_RLY = 0;
      REVF_10_12_15M_RLY = 0;
   } else {
      _6M_RLY = 0;
      _10_12_15M_RLY = 0;
   }
   _17_20M_RLY = 0;
   _30_40M_RLY = 0;
   _80M_RLY = 0;
   DTC1 = 0;
   DTC2 = 0;
}


void setBand() {
   // Turn off all relays
   allBandRlyOFF();

   switch (band) {
      case _6M:
         if (REV_F) {
            REVF_6M_RLY = 1;
         } else {
            _6M_RLY = 1;
         }
         break;
      
      case _10M:
      case _12M:
      case _15M:
         if (REV_F) {
            REVF_10_12_15M_RLY = 1;
         } else {
            _10_12_15M_RLY = 1;
         }
         DTC1 = 1;
         break;
      
      case _17M:
         _17_20M_RLY = 1;
         DTC1 = 1;
         break;
      
      case _20M:
         _17_20M_RLY = 1;
         DTC2 = 1;
         break;
      
      case _30M:
         _30_40M_RLY = 1;
         DTC1 = 1;
         DTC2 = 1;
         break;
      
      case _40M:
         _30_40M_RLY = 1;
         break;

      case _60M:
         _30_40M_RLY = 1;
         break;

      case _80M:
         _80M_RLY = 1;
         break;
   }//endswitch
}