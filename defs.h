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
#define PWR_LED   LATC.B1
#define TX_LED    LATC.B0

#define TX_OUT    LATE.B0

#define BAND_UP   PORTB.B0
#define BAND_DOWN PORTB.B1
#define KEY_MODE  PORTB.B2


// PORT B button connections
#define key    F4
#define cor    F5
#define active 1

// Key Mode constants
#define SB 0
#define PT 1
#define CR 2

// Band constants
#define _6M   0
#define _10M  1
#define _12M  2
#define _15M  3
#define _17M  4
#define _20M  5
#define _30M  6
#define _40M  7
#define _60M  8
#define _80M  9
#define _160M 10

// Band Ports
#define REVF_6M_RLY        LATA.B7
#define _6M_RLY            LATE.B1
#define REVF_10_12_15M_RLY LATA.B6
#define _10_12_15M_RLY     LATE.B2
#define _17_20M_RLY        LATA.B3
#define _30_40M_RLY        LATC.B5
#define _80M_RLY           LATA.B5

// Analog Channels
#define VOLT_CH            0
#define TEMP_CH            1

#define FWD_PWR_CH         26
#define RFL_PWR_CH         27

#define REVF_FWD_PWR_CH    7
#define REVF_RFL_PWR_CH    6

// DTC PORTS
#define DTC1               LATA.B2
#define DTC2               LATA.B4

// LCD Special Chars
#define meterLeft          0
#define meterBoth          1
#define meterTop           2
#define meterBottom        3

// Button change constants
#define POSITIVE_COUNT     8

// Wattmeter correction scale
#define SCALE 0.87719

// checkButtons constants
#define BTN_NONE       0x0
#define BTN_UP         0x1
#define BTN_DN         0x2
#define BTN_DN_UP      0x3
#define BTN_KY         0x4
#define BTN_KY_UP      0x5
#define BTN_KY_DN      0x6
#define BTN_ALL_3      0x7
#define BTN_LONG_UP    0x9
#define BTN_LONG_DN    0xA
#define BTN_LONG_DN_UP 0xB
#define BTN_LONG_KY    0xC
#define BTN_LONG_KY_UP 0xD
#define BTN_LONG_KY_DN 0xE
#define BTN_LONG_ALL_3 0xF

//extern char SPLASH_TOP[];
//extern char SPLASH_BOTTOM[];

// Global variables

extern unsigned short txState = 0, timer0Flag = 0, bandFlag = 10, bandDispFlag = 0, keyDispFlag = 0;
extern unsigned short keymode = 0;  // 0=SB, 1=PT, 2=CR
extern unsigned short band = 10;    // see below for values 0-10
extern unsigned short lastB = 0 ;
extern char i;                      // Loop variable
extern unsigned short lcdFlag;
extern char BAND_STR[4];
extern char VOLT_STR[5];
extern char TEMP_STR[5];
extern char PEP_STR[3];
extern char VSWR_STR[3];
extern char KEY_STR[2];
extern unsigned int VOLT, TEMP, FWD_PWR, RFL_PWR;
extern unsigned int bandUpFlag, bandDownFlag, keyModeFlag, rbDelayFlag; 
extern unsigned short eepromUpdateFlag;
extern unsigned int _100msCount;
extern unsigned short temperatureFlag = 0, voltageFlag = 0, calcSwrFlag = 0;
extern unsigned short tempmode = 0; // 0 - F; 1 = C
extern char rxbuff[];               // 128 byte circular Buffer for storing UART1 rx data
extern char workingString[];
extern char rxbuff2[];              // 128 byte circular Buffer for storing UART2 rx data
extern char workingString2[];

extern char freqStr[6];
extern unsigned int uartPtr    = 0;
extern unsigned int uartMsgs   = 0;
extern unsigned int readStart  = 0;
extern unsigned int uartPtr2   = 0;
extern unsigned int uartMsgs2  = 0;
extern unsigned int readStart2 = 0;
extern const char crlfsemi[];
extern char msg[70]; //declare array set to max size required plus 1 [for terminator] for copying into
extern char version;


typedef struct flag_tag1{  // 8 bit flags

        unsigned int UART_Buffer_Full:1;  // bit0, used for uart buffer full indication
        unsigned int found:1;             // bit1
        unsigned int newdata:1;           // bit2
        unsigned int newcmd:1;            // bit3
        unsigned int configMode:1;        // bit4
        unsigned int UART2_Buffer_Full:1; // bit5
        unsigned int Free6:1;             // bit6
        unsigned int Free7:1;             // bit7

} Tflag_tag1;

extern Tflag_tag1 flags1;

// Function Prototypes

char * CopyConst2Ram(char * dest, const char * src);
void Do_LCD_Init(void) ;
void Start_Bootload();
void UART_grab_buffer();
void UART_grab_buffer2();
void addMenuArrows();
void backgroundTasks();
void calculateVswr();
void changeBandDisplay(int direction);
void changeBandLCD();
void changeKeyMode(void);
void changeKeyModeLCD();
unsigned short checkButtons();
void checkTXAnalogs(void);
void checkTemperature(void);
void checkTxState();
void checkVoltage(void);
void display(void) ;
void displayMenu();
void findBand(short uart);
void getVersion();
void init(void);
void interrupt(void);
unsigned short mask(unsigned short) ;
void menuBaudRate();
void menuKxMode();
void menuTempMode();
void outDigit(unsigned short) ;
void portTest();
void processButtons();
void processTimerFlags();
void removeMenuArrows();
void setBand(void);
void setBandDelay(void);
void setBaudRate();
void setCallSign();
void setKXMode();
void setKeyLcd(char mode);
void setPowerMeter(float fwdpwr, float rflpwr);
void setTX_OFF(void);
void setTX_ON(void);
void setTempLabel();
void uartRxStatus();
void uartTxStatus();
void updateLCD(void);
