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

// Header file for HARDROCK-50

// Lcd module connections
sbit LCD_RS at LATD4_bit;
sbit LCD_EN at LATD5_bit;
sbit LCD_D4 at LATD3_bit;
sbit LCD_D5 at LATD2_bit;
sbit LCD_D6 at LATD1_bit;
sbit LCD_D7 at LATD0_bit;

sbit LCD_RS_Direction at TRISD4_bit;
sbit LCD_EN_Direction at TRISD5_bit;
sbit LCD_D4_Direction at TRISD3_bit;
sbit LCD_D5_Direction at TRISD2_bit;
sbit LCD_D6_Direction at TRISD1_bit;
sbit LCD_D7_Direction at TRISD0_bit;
// End Lcd module connections



// LED connections
#define PWR_LED LATC.B1
#define TX_LED LATC.B0

#define TX_OUT LATE.B0

#define BAND_UP PORTB.B0
#define BAND_DOWN PORTB.B1
#define KEY_MODE  PORTB.B2


// PORT B button connections
#define key F4
#define cor F5
#define active 1

// Key Mode constants
#define SB 0
#define PT 1
#define CR 2

// Band constants
#define _6M    0
#define _10M   1
#define _12M   2
#define _15M   3
#define _17M   4
#define _20M   5
#define _30M   6
#define _40M   7
#define _60M   8
#define _80M   9
#define _160M  10

// Band Ports
#define _6M_RLY             LATE.B1
#define _10_12_15M_RLY      LATE.B2
#define _17_20M_RLY         LATA.B3
#define _30_40M_RLY         LATC.B5
#define _80M_RLY            LATA.B5

// Analog Channels
#define VOLT_CH             0
#define TEMP_CH             1
#define FWD_PWR_CH             26
#define RFL_PWR_CH             27

// DTC PORTS
#define DTC1                LATA.B2
#define DTC2                LATA.B4

// LCD Special Chars
#define meterLeft 0
#define meterBoth 1
#define meterTop 2
#define meterBottom 3

// Button change constants
#define POSITIVE_COUNT   8

// Wattmeter correction scale
#define SCALE 0.87719

extern char SPLASH_TOP[];
extern char SPLASH_BOTTOM[];

// Global variables

extern unsigned short txState = 0, timer0Flag = 0, bandFlag = 10, bandDispFlag = 0, keyDispFlag = 0;
extern unsigned short keymode = 0;  //0=SB, 1=PT, 2=CR
extern unsigned short band = 10;  //see below for values 0-10
extern unsigned short lastB = 0 ;
extern char i;                              // Loop variable
extern unsigned short lcdFlag;
extern char BAND_STR[4];
extern char VOLT_STR[5];
extern char TEMP_STR[5];
extern char PEP_STR[3];
extern char VSWR_STR[3];
extern char KEY_STR[2];
extern unsigned int VOLT, TEMP, FWD_PWR, RFL_PWR;
extern unsigned int bandUpFlag, bandDownFlag, keyModeFlag, rbDelayFlag, eepromUpdateFlag;
extern unsigned int _100msCount;
extern unsigned short temperatureFlag = 0, voltageFlag = 0, calcSwrFlag = 0;
extern unsigned short tempmode = 0; // 0 - F; 1 = C
extern char rxbuff[];                                   // 256 byte circular Buffer for storing rx data
extern char workingString[];
extern char freqStr[6];
extern unsigned int uartPtr, BufferLength;
extern char msg[70]; //declare array set to max size required plus 1 [for terminator] for copying into


typedef struct flag_tag1{  // 8 bit flags

        unsigned int UART_Buffer_Full:1; // bit0, used for uart buffer full indication
        unsigned int found:1; // bit1
        unsigned int newdata:1; // bit2
        unsigned int newcmd:1; // bit3
        unsigned int configMode:1; // bit4
        unsigned int Free5:1; // bit5
        unsigned int Free6:1; // bit6
        unsigned int Free7:1; // bit7

} Tflag_tag1;

extern Tflag_tag1 flags1;

// Function Prototypes

unsigned short mask(unsigned short) ;
void init(void);
void outDigit(unsigned short) ;
void display(void) ;
void Do_LCD_Init(void) ;
void updateLCD(void);
void interrupt(void);
void changeKeyMode(void);
void setKeyLcd(char mode);
void changeBandDisplay(int direction);
void setBandDelay(void);
void setBand(void);
void setTX_ON(void);
void setTX_OFF(void);
void checkTemperature(void);
void checkVoltage(void);
void checkTXAnalogs(void);
void changeKeyModeLCD();
void changeBandLCD();
void setPowerMeter(float fwdpwr, float rflpwr);
void portTest();
void processTimerFlags();
void checkButtons();
void checkTxState();
void calculateVswr();
void UART_grab_buffer();
void findBand();
void uartRxStatus();
void uartTxStatus();
void setBaudRate();
void setKXMode();
void setTempLabel();
void setCallSign();
char * CopyConst2Ram(char * dest, const char * src);
void Start_Bootload();