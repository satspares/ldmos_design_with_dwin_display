#ifndef __WENYU_T5L51_H__
#define __WENYU_T5L51_H__
//====================================================
#define  u8   unsigned char  
#define  s8     signed char
#define  u16  unsigned int 
#define  s16    signed int
#define  u32  unsigned long 
#define  s32    signed long
	
//Macro definition variable
#ifndef uint8_t
typedef unsigned char uint8_t;
typedef unsigned char BYTE;
typedef unsigned char byte;
#endif
#ifndef uint16_t
typedef unsigned short uint16_t;
typedef unsigned short WORD;
typedef unsigned short int;
#endif
#ifndef uint32_t
typedef unsigned int uint32_t;
#endif
#ifndef int8_t
typedef char int8_t;
#endif
#ifndef int16_t
typedef short int16_t;
#endif
#ifndef int32_t
typedef int int32_t;
#endif

//=====================================================
#include  "Parameter_Config.h"
//=====================================================
sfr P0 = 0x80; /********PO mouth********/
sfr SP = 0x81; /********stack pointer*******/
sfr DPL = 0x82; /********DPTR data pointer*******/
sfr DPH = 0x83; /********DPTR data pointer*******/
sfr PCON = 0x87; /********.7 UART2 baud rate setting*******/
sfr TCON = 0x88; /********T0 T1 control register********/

sbit TF1 = TCON^7; /********T1 interrupt trigger*******/
sbit TR1 = TCON^6;		
sbit TF0 = TCON^5; /********T0 interrupt trigger*******/
sbit TR0 = TCON^4;
sbit IE1 = TCON^3; /********External interrupt 1*******/
sbit IT1 = TCON^2; /********External interrupt 1 trigger mode 0: Low level trigger 1: Falling edge trigger********/
sbit IE0 = TCON^1; /********external interrupt 0*******/
sbit IT0 = TCON^0; /********External interrupt 0 trigger mode 0: Low level trigger 1: Falling edge trigger********/

sfr TMOD = 0x89; /********T0 T1 mode selection, same as 8051  Timer0 run 16bit mode Timer1 run 13bit mode**********/
sfr TH0 = 0x8C;		 
sfr TL0 	=	0x8A;
sfr TH1 	=	0x8D;
sfr TL1 	=	0x8B;

sfr CKCON = 0x8E; /********CPU running=0, 1T*******/
sfr	P1		=	0x90;
sfr DPC = 0x93; /********MOVX instruction, DPTR change mode 0: unchanged 1: +1 2: -1********/
sfr PAGESEL = 0x94; /********must be 0x01********/
sfr D_PAGESEL = 0x95; /********must be 0x02********/

sfr SCON2 = 0x98; /********UART2 control interface, same as 8051********/

sbit TI2 = SCON2^1;
sbit RI2 = SCON2^0;
sfr SBUF2 = 0x99; /********UART2 transceiver data interface********/
sfr SREL2H = 0xBA; /********Set baud rate, when ADCON is 0x80********/
sfr	SREL2L	=	0xAA;

sfr SCON3 = 0x9B; /********UART3 control interface*******/
sfr	SBUF3	=	0x9C;
sfr	SREL3H	=	0xBB;
sfr	SREL3L	=	0x9D;

sfr IEN2 = 0x9A; /******** interrupt enable controller SFR .7~.1 must write 0 .0 UART3 interrupt enable control bit********/
sfr	P2		=	0xA0;

//sbit    Red_LED1     =   P2^6;
//sbit    Green_LED2   =   P2^7;

sfr IEN0 = 0xA8; /********Interrupt enable controller 0********/
sbit EA = IEN0^7; /********Interrupt total control bit********/
sbit ET2 = IEN0^5; /********Timer 2 interrupt control bit*******/
sbit ES2 = IEN0^4; /********UART2*******/
sbit ET1 = IEN0^3; /********T1*******/
sbit EX1 = IEN0^2; /********External interrupt 1*******/
sbit ET0 = IEN0^1; /********T0******/
sbit EX0 = IEN0^0; /********external interrupt 0*******/

sfr IP0 = 0xA9; /********Interrupt Priority Controller 0*******/
sfr	P3		=	0xB0;

sbit RTC_SDA = P3^3; /****************clock****************/
sbit RTC_SCL	=	P3^2;

sfr IEN1 = 0xB8; /********Interrupt enable accept controller******/
sbit ES5R = IEN1^5; /*****UART5 receive interrupt enable control bit****/
sbit ES5T = IEN1^4; /*****UART5 accept interrupt enable control bit****/
sbit ES4R = IEN1^3; /*****UART4 accept interrupt enable control bit****/
sbit ES4T = IEN1^2; /*****UART4 accept interrupt enable control bit****/
sbit ECAN = IEN1^1; /********CAN interrupt enable control bit******/

sfr IP1 = 0xB9; /********Interrupt Priority Controller 0********/
sfr IRCON2 = 0xBF;
sfr	IRCON 	=	0xC0;
sbit TF2 = IRCON^6; /********T2 interrupt trigger flag********/
sfr T2CON = 0xC8; /********T2 control register********/
sbit TR2 = T2CON^0; /***********T2 enable************/
sfr	TRL2H	=	0xCB;
sfr	TRL2L	=	0xCA;
sfr	TH2 	=	0xCD;
sfr	TL2 	=	0xCC;

sfr PSW = 0xD0;
sbit CY = PSW^7;
sbit AC = PSW^6;
sbit F0 = PSW^5;
sbit RS1 = PSW^4;
sbit RS0 = PSW^3;
sbit OV = PSW^2;
sbit F1 = PSW^1;
sbit P = PSW^0;
sfr	ADCON	=	0xD8;
sfr	ACC		=	0xE0;
sfr	B 		=	0xF0;


/******Hardware Extension Definition********/
/******DGUS variable memory access*********/
sfr RAMMODE = 0xF8; /******DGUS variable memory access interface control register**********/
sbit APP_REQ = RAMMODE^7;
sbit APP_EN = RAMMODE^6;
sbit	APP_RW	=	RAMMODE^5;
sbit	APP_ACK	=	RAMMODE^4;
sbit APP_DATA3 = RAMMODE^3; /*1=corresponding to Byte writing, 0=corresponding to Byte not writing*/
sbit APP_DATA2 = RAMMODE^2; /*same as above*/
sbit APP_DATA1 = RAMMODE^1; /*same as above*/
sbit APP_DATA0 = RAMMODE^0; /*Same as above*/
sfr ADR_H	=	0xF1;
sfr ADR_M	=	0xF2;
sfr ADR_L	=	0xF3;
sfr ADR_INC	=	0xF4;
sfr DATA3	=	0xFA;
sfr DATA2	=	0xFB;
sfr DATA1	=	0xFC;
sfr DATA0 = 0xFD;


//UART4
sfr SCON4T = 0x96; /******UART4 send control********/
sfr SCON4R = 0x97; /******UART4 receive control**********/
sfr BODE4_DIV_H = 0xD9; /******baud rate setting********/
sfr	BODE4_DIV_L	=	0xE7;
sfr SBUF4_TX = 0x9E; /******UART4 send data interface********/
sfr SBUF4_RX = 0x9F; /******UART4 receiving data interface**********/
sbit TR4 = P0^0; /******485 direction control of port 4**********/            
//UART5
sfr	SCON5T	=	0xA7;
sfr	SCON5R	=	0xAB;
sfr	BODE5_DIV_H	=	0xAE;
sfr	BODE5_DIV_L	=	0xAF;
sfr	SBUF5_TX	=	0xAC;
sfr	SBUF5_RX	=	0xAD;
sbit TR5 = P0^1; /******port 5 485 direction control**********/
// CAN communication
sfr	CAN_CR	=	0x8F;
sfr	CAN_IR	=	0x91;
sfr	CAN_ET	=	0xE8;

//GPIO
sfr	P0MDOUT	=	0xB7;
sfr	P1MDOUT	=	0xBC;
sfr	P2MDOUT	=	0xBD;
sfr	P3MDOUT	=	0xBE;
sfr	MUX_SEL	=	0xC9;
sfr PORTDRV = 0xF9; /******Output drive strength*********/

sbit GreenLED = P2^7;
sbit RedLED = P2^6;

//MAC&DIV
sfr	MAC_MODE	=	0xE5;
sfr	DIV_MODE	=	0xE6;

//SFR extension interface
sfr EXADR = 0xFE;
sfr	EXDATA	=	0xFF;

//watchdog macro definition
#define WDT_ON() MUX_SEL|=0x02 //Enable watchdog
#define WDT_OFF() MUX_SEL&=0xFD //Turn off watchdog
#define WDT_RST() MUX_SEL|=0x01 // ??
//System main frequency and 1ms timing value definition
//#define FOSC     				206438400UL
//#define T1MS (65536-FOSC/12/1000)
#define DTHD1 0X5A //frame header 1
#define DTHD2 0XA5 //frame header 2  
#define       FOSC          206438400UL
#define FRAME_LEN 255 //frame length
//===========================================
void Write_Dgus(u16 Dgus_Addr,u16 Val);
u16 Read_Dgus(u16 Dgus_Addr);
void write_dgus_vp(unsigned int addr,unsigned char *buf,unsigned int len);
void read_dgus_vp(unsigned int addr,unsigned char *buf,unsigned int len);
void Write_Dgusii_Vp_byChar(unsigned int addr,unsigned char *buf,unsigned int len);
void WriteDGUS(uint32_t Addr, uint8_t *pBuf, uint16_t Len);

#endif 
