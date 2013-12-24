#line 1 "C:/hardrock-50/portTest.c"
#line 1 "c:/hardrock-50/defs.h"
#line 20 "c:/hardrock-50/defs.h"
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
#line 100 "c:/hardrock-50/defs.h"
extern char SPLASH_TOP[];
extern char SPLASH_BOTTOM[];



extern unsigned short txState = 0, timer0Flag = 0, bandFlag = 10, bandDispFlag = 0, keyDispFlag = 0;
extern unsigned short keymode = 0;
extern unsigned short band = 10;
extern unsigned short lastB = 0 ;
extern char i;
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
extern unsigned short tempmode = 0;
extern char rxbuff[];
extern char workingString[];
extern char freqStr[6];
extern unsigned int uartPtr, BufferLength;
extern char msg[70];


typedef struct flag_tag1{

 unsigned int UART_Buffer_Full:1;
 unsigned int found:1;
 unsigned int newdata:1;
 unsigned int newcmd:1;
 unsigned int configMode:1;
 unsigned int Free5:1;
 unsigned int Free6:1;
 unsigned int Free7:1;

} Tflag_tag1;

extern Tflag_tag1 flags1;



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
#line 5 "C:/hardrock-50/portTest.c"
void portTest() {
 Lcd_Out(1,1,"Diagnostics Run");
 Lcd_Out(2,1,"Starting in 10s");
 delay_ms(10000);

 LCD_Out(2,1,"Toggling 6M ON ");
  LATE.B1  = 1;
 delay_ms(4000);
  LATE.B1  = 0;
 LCD_Out(2,1,"Toggling 6M OFF");
 delay_ms(2000);

 LCD_Out(2,1,"Toggling 10M ON ");
  LATE.B2  = 1;
 delay_ms(4000);
  LATE.B2  = 0;
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
