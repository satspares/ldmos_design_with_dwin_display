#include "Sys.h"

u16 xdata TimVal=0 ;
//u16 xdata Count_num1=0;
u16 xdata SysTick_RTC=0;
bit RTC_Flog=0;

//========================================
void Sys_Cpu_Init()
{
		   IEN0 = 0x00; //IEN0.7=EA interrupt total control.6=0,.5=ET2,.4=ES0,.3=ET1,.2=EX1,.1=ET0,.0=EX0 ES0 serial port 2 receive + send interrupt
			IEN1 = 0x00; //IEN1.7=0, .6=0, .5=serial port 5 receiving interrupt, .4=serial port 5 sending interrupt, .3=serial port 4 receiving interrupt, .2=serial port 4 sending interrupt,. 1=can interrupt
			IEN2 = 0x00; // .7-.1=0 , .0=ES1 serial port 3 receive + send interrupt
//===========================================MODE			
			DPC = 0x00; //Whether DPTR+1 C51=0
			CKCON = 0x00; //CPU running=0, 1T mode
		
			IP0 = 0x00;IP1 = 0x00; //interrupt priority default
			RAMMODE = 0x00;
//============================================== IO initialization 	        
			P0MDOUT =0x53; //p0.0 p0.1 is forced to be used for 485 direction control P0.4 leads to serial port 2 P0.6 leads to serial port 3
			//P2MDOUT =0xBD;  //Was C0
	    P3MDOUT =0x0C; //Open the output of P3_2 and P3_3 P3_2=RTC_SCL P3_3=RTC_SDA
		PORTDRV = 0x01; //IO drive capacity setting 4mA=0,8mA=1,16mA=2,32mA=3
	    P2MDOUT = 0xC0;  /*Turn on the output of P2_6 and P2_7*/
		P2 = 0X00;
	    P3 = 0x00;
			WDT_OFF(); //Close the door dog		
//============================================
			TH2 = 0x00;
			TL2 = 0x00;
			T2CON = 0x70;
			TRL2H = 0xBC; //1ms timer
			TRL2L = 0xCD;
			IEN0 |= 0x20; //start timer 2
			TR2 = 0x01;
      		ET2 = 1; //T2 timer interrupt enable control bit
			EA = 1;

			TMOD &= 0xF0;    // Clear 4bit field for timer0
			TMOD |= 0x01;    // Set timer0 in mode 1 = 16bit mode
			TH0 = 0;
			TL0 = 0;
			//TH0=T1MS>>8; //1ms timer
			//TL0=T1MS;
			ET0=1; //Enable timer 0 interrupt
			TR0=1; //Start the timer
}
/**************************************************** *****************************/
//void Sys_Timer2_isr() interrupt 5 //1MS
//{
//	        TF2=0;//Clear the interrupt flag bit of timer 2
//	        if(TimVal<255)TimVal++;
//			    SysTick_RTC++;
//	        if(T_O5>0)T_O5--;
//	        if(T_O4>0)T_O4--;
//	        if(T_O3>0)T_O3--;
//	        if(T_O2>0)T_O2--;
//	        if(Count_num1>0) Count_num1--;

//}

/**************************************************** *****************************/
u16 Read_Dgus(u16 Dgus_Addr)
{ u16 R_Dgus=0;
// EA = 0;
			ADR_H = 0x00;
			ADR_M = (u8)((Dgus_Addr/2)>>8);
			ADR_L = (u8)(Dgus_Addr/2);
			RAMMODE = 0xAF; //read operation
			while(!APP_ACK);
			APP_EN = 1;
			while(APP_EN);
			if(Dgus_Addr%2) R_Dgus = (DATA1<<8)+DATA0;
			else R_Dgus=(DATA3<<8)+DATA2;
			RAMMODE = 0;
// EA = 1;
			return R_Dgus;		
}
/**************************************************** *****************************/
void Write_Dgus(u16 Dgus_Addr,u16 Val)
{          
// EA = 0;
			ADR_H = 0x00;
			ADR_M = (u8)((Dgus_Addr/2)>>8);
			ADR_L = (u8)(Dgus_Addr/2);
			RAMMODE = 0xAF; //read operation
			while(!APP_ACK);
			APP_EN = 1;
			while(APP_EN);
			RAMMODE = 0;
		   	ADR_M = (u8)((Dgus_Addr/2)>>8);
			ADR_L = (u8)(Dgus_Addr/2);
			RAMMODE = 0x8F; //write operation
		    while(!APP_ACK);
			if(Dgus_Addr%2){DATA1=(u8)(Val>>8); DATA0=(u8)(Val); }
			else { DATA3 =(u8)(Val>>8); DATA2 =(u8)(Val);}
		    APP_EN = 1;
			while(APP_EN);
			RAMMODE = 0;
// EA = 1;
}
//=========================================
//**************************************************** *****************************
// Function name: void write_dgus_vp(u32 addr,u8* buf,u16 len)
// Function description: Write dgus address data
//Input parameters: addr: write address value buf: write data storage buffer len: word length
// Output parameters :
// Modification history:
// 1. Date :
//   author :
//    Modified content: Modify the read and write dgus process, do not use nesting
//***************************************************** ***************************
void write_dgus_vp(unsigned int addr, unsigned char *buf, unsigned int len)
{
	unsigned char i;

// EA = 0;
	i=(unsigned char)(addr&0x01);
	addr=addr/2;
	ADR_H=0x00;
	ADR_M=(unsigned char)(addr>>8);
	ADR_L=(unsigned char)(addr);
	ADR_INC=0x01;
	RAMMODE=0x8F;
	while(APP_ACK==0);
	if(i && len>0)
	{
		RAMMODE=0x83;
		DATA1=*buf++;
		DATA0=*buf++;
		APP_EN=1;
		while(APP_EN==1);
		len--;
	}
	RAMMODE=0x8F;
	while(len>=2)
	{
		DATA3=*buf++;
		DATA2=*buf++;
		DATA1=*buf++;
		DATA0=*buf++;
		APP_EN=1;
		while(APP_EN==1);
		len=len-2;
	}
	if(len)
	{
		RAMMODE=0x8C;
		DATA3=*buf++;
		DATA2=*buf++;
		APP_EN=1;
		while(APP_EN==1);
	}
	RAMMODE=0x00;
// EA = 1;
}
/**************************************************** *****************************/
void Write_Dgusii_Vp_byChar(unsigned int addr, unsigned char *buf, unsigned int len){
	unsigned char i;
// bit EA_Bak;
// EA_Bak = EA;
// EA = 0;
	i = (unsigned char)(addr & 0x01);
	addr = addr / 2;
	ADR_H = 0x00;
	ADR_M = (unsigned char)(addr >> 8);
	ADR_L = (unsigned char)(addr);
	ADR_INC = 0x01;
	RAMMODE = 0x8F;
	while(APP_ACK == 0);
	if(i && len > 0){
		if(len == 1){
			RAMMODE = 0x82;
			DATA1 = *buf++;
			APP_EN = 1;
			while(APP_EN == 1);
			len--;
		} else {
			RAMMODE = 0x83;
			DATA1 = *buf++;
			DATA0 = *buf++;
			APP_EN = 1;
			while(APP_EN == 1);
			len -= 2;
		}
	}
	RAMMODE = 0x8F;
	while(len >= 4){
		DATA3 = *buf++;
		DATA2 = *buf++;
		DATA1 = *buf++;
		DATA0 = *buf++;
		APP_EN = 1;
		while(APP_EN == 1);
		len -= 4;
	}
	
	if(len == 3){
		RAMMODE = 0x8E;
		DATA3 = *buf++;
		DATA2 = *buf++;
		DATA1 = *buf++;
		APP_EN = 1;
		while(APP_EN == 1);
	} else if(len == 2){
		RAMMODE = 0x8C;
		DATA3 =*buf++;
		DATA2 =*buf++;
		APP_EN = 1;
		while(APP_EN == 1);
	} else if(len == 1){
		RAMMODE = 0x88;
		DATA3 = *buf++;
		APP_EN = 1;
		while(APP_EN == 1);
	}
	RAMMODE = 0x00;
// EA = EA_Bak;
}

/**************************************************** *****************************
 Function name: void read_dgus_vp(u32 addr,u8* buf,u16 len)
 Function description: read the value of dgus address
 Input parameters: addr: dgus address value len: read data length
 Output parameters: buf: data storage buffer
 Modification history:
  1. Date :
    author :
    Modified content: Modify the read and write dgus process, do not use nesting
***************************************************** *****************************/
void read_dgus_vp(unsigned int addr, unsigned char *buf, unsigned int len)
{
	unsigned char i;
	i=(unsigned char)(addr&0x01);
	addr=addr/2;
	ADR_H=0x00;
	ADR_M=(unsigned char)(addr>>8);
	ADR_L=(unsigned char)(addr);
	ADR_INC=0x01;
	RAMMODE=0xAF;
	while(APP_ACK==0);
	while(len>0)
	{
		APP_EN=1;
		while(APP_EN==1);
		if((i==0)&&(len>0))
		{
			*buf++=DATA3;
			*buf++=DATA2;
			i=1;
			len--;
		}
		if((i==1)&&(len>0))
		{
			*buf++=DATA1;
			*buf++=DATA0;
			i=0;
			len--;
		}
	}
	RAMMODE=0x00;
}



/*****************************************************************************
write DGUS register */
/*****************************************************************************
 Function name: WriteDGUS
 Function description: Read DGUS register
 Input parameters: uint32_t Addr DGUS register address
             uint8_t* pBuf receive buffer
             uint16_t Len Write data byte length (first high word and then low word)
 Output parameters: none
 Modification history:
 Date: November 04, 2019
 author :
 Modify content: create
*****************************************************************************/
void WriteDGUS(uint32_t Addr, uint8_t *pBuf, uint16_t Len)
{
  uint8_t Aoffset;
  if(NULL == pBuf){return;}
  if(0 == Len){return;}
  if((Addr+Len/2) > (0xFFFF*2)){return;}
  EA = 0;
  Aoffset = Addr&0x01; /*take bit0 as parity judgment*/
  Addr = Addr >> 1; /*Configuration address*/
  ADR_H = (uint8_t)(Addr >> 16);
  ADR_M = (uint8_t)(Addr >> 8);
  ADR_L = (uint8_t)(Addr);
  RAMMODE = 0x00;
  ADR_INC = 0x01;
  APP_REQ = 1; /*Occupy DGUS to read and write*/
  while(!APP_ACK);
  APP_RW = 0; /*write variable memory*/

  if(1 == Aoffset)
  { /*address is odd*/
    if(1 == Len){ APP_DATA3=0,APP_DATA2=0,APP_DATA1=1,APP_DATA0=0;}
    else{         APP_DATA3=0,APP_DATA2=0,APP_DATA1=1,APP_DATA0=1;}
    if(1 == Len){ DATA1=*pBuf++;Len=Len-1; }
    else{         DATA1=*pBuf++;DATA0=*pBuf++;Len=Len-2;}
    APP_EN = 1;
    while(APP_EN);
  }
  while(1) /*address is even*/
  {
    if(0 == Len) break;
    if(Len < 4)
    {
      switch(Len)
      {
        case 3: APP_DATA3=1,APP_DATA2=1,APP_DATA1=1,APP_DATA0=0;break;
        case 2: APP_DATA3=1,APP_DATA2=1,APP_DATA1=0,APP_DATA0=0;break;
        case 1: APP_DATA3=1,APP_DATA2=0,APP_DATA1=0,APP_DATA0=0;break;
      }
      switch(Len)
      {
        case 3: DATA3=*pBuf++;DATA2=*pBuf++;DATA1=*pBuf++;break;
        case 2: DATA3=*pBuf++;DATA2=*pBuf++;break;
        case 1: DATA3=*pBuf++;break;
      }
      APP_EN = 1;
      while(APP_EN);
      break;
    }
    else
    {
      APP_DATA3=1,APP_DATA2=1,APP_DATA1=1,APP_DATA0=1;
      DATA3=*pBuf++;DATA2=*pBuf++;DATA1=*pBuf++;DATA0=*pBuf++;
      APP_EN = 1;
      while(APP_EN);
      Len = Len - 4;
    }
  }

  RAMMODE = 0x00; /*Must be cleared when not occupied*/
  EA = 1;
}