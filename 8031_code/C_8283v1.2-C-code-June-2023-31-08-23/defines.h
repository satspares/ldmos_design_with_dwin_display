#define USERTC 
//#define TESTLCD
#define LOG(x,y) {Uart2SendStr(x,y);}
//#define LOG4(x,y) {Uart4SendStr(x,y);}
#define START_WIN_NUM_VP 0x2000
#define BACKLIGHTLOW ((byte)0x10)
#define BACKLIGHTHIGH ((byte)0x40)
#define ADCSWITCHLEVEL 0x640
#define PWM_FREQ_NUM  0x40
#define LEDLOW 0x40
#define LEDHIGH 0xF0
#define ADCCHANGE 0x100  //send adc when change is greater than this
#define DHW_LEDCFG 0x0082
#define SOUNDADDRESS 0x00A0
#define CHANGEPAGEADDRESS 0x0084
#define SYSTEMSETADDRESS 0x0080
#define NORADDRESS 0x0008
#define RTCSETADDRESS 0x009C
#define SYSTEMRESETADDRESS 0x0004
#define LCDBackLightAddress 0x0082

#define LEDSETRAM 0x4000  // ram address for Green/Red Leds
