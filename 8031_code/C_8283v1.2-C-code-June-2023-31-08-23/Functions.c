#include "Functions.h"

void Norflash_Read(u16 nor_addr,u16 Dgus_addr,u16 len)//All parameters must be even
  {
   Write_Dgus(0x000b,len);
   Write_Dgus(0x000a,Dgus_addr);
   Write_Dgus(0x0009,nor_addr);
   Write_Dgus(0x0008,0x5A00);
   while(Read_Dgus(0X0008)&0xFF00);
  }
//// ======================================
  void Norflash_Write(u16 nor_addr,u16 Dgus_addr,u16 len)//All parameters must be even
  {
  Write_Dgus(0x000b,len);
  Write_Dgus(0x000a,Dgus_addr);
  Write_Dgus(0x0009,nor_addr);
  Write_Dgus(0x0008,0xA500);
  while(Read_Dgus(0X0008)&0xFF00);
  }
 //// ====================================== 

  void SendADC(uint16_t level)
{
  uart_send_byte(2, 0x5A);
	uart_send_byte(2, 0xA5);
	uart_send_byte(2, 0x06);
	uart_send_byte(2, 0x83);
	uart_send_byte(2, 0x41);
	uart_send_byte(2, 0x00);
	uart_send_byte(2, 0x01);
	uart_send_byte(2, (level >> 8));
	uart_send_byte(2, (level & 0xFF));
}

//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
/*
void DelayMs(uint16_t n)
{
	uint16_t i, j;
	for (i = 0; i < n; i++)
		for (j = 0; j < 7400; j++)
			;
}

*/
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


