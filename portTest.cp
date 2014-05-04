#line 1 "C:/hardrock-50/portTest.c"
#line 1 "c:/hardrock-50/defs.h"
#line 36 "c:/hardrock-50/defs.h"
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
#line 144 "c:/hardrock-50/defs.h"
extern unsigned short band;
extern unsigned short keymode;
extern unsigned short tempmode;
extern short acc_baud;
extern short kxmode;
extern short blver;
extern short usb_baud;
extern short meter_adj;
extern int cor_htime;
extern int key_delay;

extern unsigned short txState, timer0Flag, bandFlag;
extern unsigned short lastB;
extern char i;
extern unsigned short lcdFlag;
extern char BAND_STR[4+1];
extern char VOLT_STR[5+1];
extern char TEMP_STR[3+1];
extern char PEP_STR[3+1];
extern char VSWR_STR[3+1];
extern char KEY_STR[2+1];
extern unsigned int VOLT, TEMP, LAST_AVE_FWP, RFL_PWR;
extern unsigned int bandUpFlag, bandDownFlag, keyModeFlag;
extern unsigned short eepromUpdateFlag;
extern unsigned short temperatureFlag, voltageFlag, calcSwrFlag;

extern char rxbuff[];
extern char rxbuff2[];
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
extern char msg[69+1];
extern char tmpString[16+1];
extern char version;
extern short menu_active;
extern int TX_delay_ms;
extern int RX_delay_ms;

typedef struct flag_tag1{
 unsigned int UART_Buffer_Full:1;
 unsigned int found:1;
 unsigned int newdata:1;
 unsigned int newcmd:1;
 unsigned int configMode:1;
 unsigned int UART2_Buffer_Full:1;
 unsigned int Free6:1;
 unsigned int Free7:1;
} Tflag_tag1;

extern Tflag_tag1 flags1;



char * copyConst2Ram(char * dest, const char * src);
void addMenuArrows();
void adjustWattMeter(short percent);
void backgroundTasks();
void calculateVswr();
void changeBandDisplay(int direction);
void changeBandLCD();
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
void outDigit(unsigned short) ;
void portTest();
void processButtons();
void processTimerFlags();
void removeMenuArrows();
void setBand(void);
void setBaudRate();
void setCallSign();
void setKxMode();
void setKeyMode();
void setPowerMeter(float fwdpwr, float rflpwr);
void setTxOff(void);
void setTxOn(void);
void setTempLabel();
void showNumMsec(int num);
void showPercent(short percent);
void startBootload();
void uartRxStatus();
void uartTxStatus();
void uartGrabBuffer();
void uartGrabBuffer2();
void updateLCD(void);
void waitButtonRelease();
#line 5 "C:/hardrock-50/portTest.c"
void portTest() {
 Lcd_Out(1,1,"Diagnostics Run");
 Lcd_Out(2,1,"Starting in 10s");
 delay_ms(10000);

 LCD_Out(2,1,"Toggling 6M ON ");
 if ( (version == 0x46) ) {
  LATA.B7  = 1;
 } else {
  LATE.B1  = 1;
 }
 delay_ms(4000);
 if ( (version == 0x46) ) {
  LATA.B7  = 0;
 } else {
  LATE.B1  = 0;
 }
 LCD_Out(2,1,"Toggling 6M OFF");
 delay_ms(2000);

 LCD_Out(2,1,"Toggling 10M ON ");
 if ( (version == 0x46) ) {
  LATA.B6  = 1;
 } else {
  LATE.B2  = 1;
 }
 delay_ms(4000);
 if ( (version == 0x46) ) {
  LATA.B6  = 0;
 } else {
  LATE.B2  = 0;
 }
 LCD_Out(2,1,"Toggling 10M OFF");
 delay_ms(2000);

 LCD_Out(2,1,"Toggling 17M ON ");
  LATA.B3  = 1;
 delay_ms(4000);
  LATA.B3  = 0;
 LCD_Out(2,1,"Toggling 17M OFF");
 delay_ms(2000);

 LCD_Out(2,1,"Toggling 30M ON ");
  LATC.B5  = 1;
 delay_ms(4000);
  LATC.B5  = 0;
 LCD_Out(2,1,"Toggling 30M OFF");
 delay_ms(2000);

 LCD_Out(2,1,"Toggling 80M ON ");
  LATA.B5  = 1;
 delay_ms(4000);
  LATA.B5  = 0;
 LCD_Out(2,1,"Toggling 80M OFF");
 delay_ms(2000);

 LCD_Out(1,1,"Returning to    ");
 LCD_Out(2,1,"Main Program    ");
 delay_ms(2000);
}
