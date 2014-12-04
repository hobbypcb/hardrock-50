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

// EEPROM Map:
//  1: band
//  2: keymode
//  3: tempmode
//  4: acc_baud (DB9 serial plug)
//  5: kxmode
//  6: blver
//  7: usb_baud
//  8. meter_adj (+/- 25%)
//  9. cor_htime (2 bytes)
// 11. key_delay (2 bytes)
// 12. ATU mode
// 13. FT-817 mode

// Macros
#define REV_E (version != 0x46)
#define REV_F (version == 0x46)

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
#define inactive 0

// Key Mode constants
#define SB 0
#define PT 1
#define CR 2
#define QR 3

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
#define _UNK  99

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

#define FT817              27

// DTC PORTS
#define DTC1               LATA.B2
#define DTC2               LATA.B4

// LCD Special Chars
#define meterBoth          1
#define meterTop           2

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

// soft UART constants
#define T_TXD PORTC.B2
#define T_RXD PORTB.B3

//extern char SPLASH_TOP[];
//extern char SPLASH_BOTTOM[];

// Global variables

//EEPROM variables:
extern unsigned short band;
extern unsigned short keymode;
extern unsigned short tempmode;
extern short          acc_baud;
extern short          set_baud;
extern short          kxmode;
extern short          ftmode;
extern short          blver;
extern short          usb_baud;
extern short          meter_adj; // (+/- 25%)
extern int            cor_htime; // (2 bytes)
extern int            key_delay; // (2 bytes)
extern short          atu_mode;
extern char           E_F;
extern char           PW_STR[5];
extern unsigned int   tuner_freq;

extern unsigned short txState, timer0Flag, bandFlag;
extern unsigned short lastB;
extern char i;                      // Loop variable
extern unsigned short tx_lcdFlag, rx_lcdFlag;
extern char BAND_STR[4+1];
extern char VOLT_STR[5+1];
extern char TEMP_STR[3+1];
extern char PEP_STR[3+1];
extern char VSWR_STR[3+1];
extern char KEY_STR[2+1];
extern unsigned int VOLT, TEMP, LAST_AVE_FWP, RFL_PWR, PEP_cnt;
extern unsigned int LAST_AVE_RFP, AVE_FWP, AVE_RFP, PEP_FWP;
extern unsigned int bandUpFlag, bandDownFlag, keyModeFlag; 
extern unsigned short eepromUpdateFlag;
extern unsigned short temperatureFlag, voltageFlag, calcSwrFlag;

extern char rxbuff[];               // 128 byte circular Buffer for storing UART1 RX data
extern char rxbuff2[];              // 128 byte circular Buffer for storing UART2 RX data
extern char workingString[];
extern char workingString2[];
extern unsigned int uartPtr;
extern unsigned int uartPtr2;
extern unsigned int uartMsgs;
extern unsigned int uartMsgs2;
extern unsigned int readStart;
extern unsigned int readStart2;

extern char freqStr[6];
extern const char crlfsemi[];
extern char msg[69+1]; //declare array set to max size required plus 1 [for terminator] for copying into
extern char tmpString[16+1];
extern char version;
extern short menu_active;
extern int TX_delay_ms;
extern int RX_delay_ms;

typedef struct flag_tag1{            // 8 bit flags
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

char * copyConst2Ram(char * dest, const char * src);
void addMenuArrows();
void adjustWattMeter(short percent);
void backgroundTasks();
void calculateVswr();
void changeBandDisplay(int direction);
void changeBandLCD(char);
void changeKeyMode(void);
unsigned short checkButtons();
void checkTXAnalogs(void);
void checkTemperature(short force);
void checkTxState();
void checkVoltage(void);
void display(void);
void displayMenu();
void doLcdInit(void);
unsigned int EEPROM_Read_int (unsigned int address);
void EEPROM_Write_int(unsigned int address, unsigned int num);
void findBand(short uart);
void getVersion();
void init(void);
void interrupt(void);
unsigned short mask(unsigned short) ;
void menuAccBaudRate();
void menuAdustMeter();
void menuCorHangTime();
void menuKeyupDelay();
void menuKxMode();
void menuTempMode();
void menuUsbBaudRate();
void menuATUMode();
void outDigit(unsigned short) ;
void portTest();
void processButtons();
void TXButtons();
void processTimerFlags();
void removeMenuArrows();
void setBand(void);
void setBaudRate(char, char *);
void setKxMode(char, char *);
void setKeyMode();
void setPowerMeter(float fwdpwr, float rflpwr);
void setTxOff(void);
void setTxOn(void);
void showNumMsec(int num);
void showPercent(short percent);
void startBootload();
void uartRxStatus(char);
void uartGrabBuffer();
void uartGrabBuffer2();
void waitButtonRelease();
void Tuner_Snd_Char(char);
char Tuner_Get_Char(void);
void Show_TX(void);
void Show_RX(void);
void Get_WM (void);
void Tuner_Byp(char);
void Draw_BG(unsigned char, unsigned char);
void menuFtMode(void);
void SetFTmode(void);
void readFT817(void);
void Update_LCD(void);
void UART_send(char,char *);
void setBandser(char, char *);
void setModeser(char, char *);
void setKX3mode(void);
void setTempser(char, char *);
void setVoltser(char, char *);
void setATUser(char, char *);