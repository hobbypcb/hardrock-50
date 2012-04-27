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
     if (direction > 0) {
        band = band + 1;
        if (band > 10) { band = 10; }
        if (band == _60M) { band = _80M; } // Skip over 60M going up
     } else if (direction < 0) {
        band = band -1;
        if (band < 0) { band = 0; }
        if (band == _60M) { band = _40M; } // Skip over 60M going down
     }

     switch (band) {
            case _6M:
                 memcpy(BAND_STR,"6M  ",4);
                 break;
                 // change interrupt here
            case _10M:
                 memcpy(BAND_STR,"10M ",4);
                 break;
                 // change interrupt here
            case _12M:
                 memcpy(BAND_STR,"12M ",4);
                 break;
                 // change interrupt here
            case _15M:
                 memcpy(BAND_STR,"15M ",4);
                 break;
                 // change interrupt here
            case _17M:
                 memcpy(BAND_STR,"17M ",4);
                 break;
                 // change interrupt here
            case _20M:
                 memcpy(BAND_STR,"20M ",4);
                 break;
                 // change interrupt here
            case _30M:
                 memcpy(BAND_STR,"30M ",4);
                 break;
                 // change interrupt here
            case _40M:
                 memcpy(BAND_STR,"40M ",4);
                 break;
                 // change interrupt here
            case _80M:
                 memcpy(BAND_STR,"80M ",4);
                 break;
                 // change interrupt here
            case _160M:
                 memcpy(BAND_STR,"160M",4);
                 break;
                 // change interrupt here
     }

}

void setBandDelay() {
                     //  Load Timer0 with Preset value for 1 Second Delay
  TMR0H = 0xB;       // preset for Timer0 MSB register
  TMR0L = 0xDC;      // preset for Timer0 LSB register
  INTCON.TMR0IF = 0; //Make sure interrupt flag is clear
  T0CON.TMR0ON = 1;  // Timer0 On
}

void allBandRlyOFF() {
     _6M_RLY = 0;
     _10_12_15M_RLY = 0;
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
                 _6M_RLY = 1;
                 break;

            case _10M:
            case _12M:
            case _15M:
                 _10_12_15M_RLY = 1;
                 DTC1 = 1;
                 break;

            case _17M:
            case _20M:
                 _17_20M_RLY = 1;
                 DTC2 = 1;
                 break;

            case _30M:
            case _40M:
                 _30_40M_RLY = 1;
                 DTC1 = 1;
                 DTC2 = 1;
                 break;

            case _80M:
                 _80M_RLY = 1;
                 break;
     }
}