#ifndef __UART_H__
#define __UART_H__

#include "Dwin_T5L1H.h"
#include "Sys.h"
#include "crc16.h"


extern u8 xdata T_O2; //port 2 timeout counter
extern u8 xdata T_O3; //port 3 timeout counter
extern u8 xdata T_O4; //port 4 timeout counter
extern u8 xdata T_O5; //port 5 timeout counter


extern bit Response_flog; //Response
extern bit Auto_data_upload;//Used for automatic data upload
extern bit Crc_check_flog;//Crc check mark


void  uart_init();
void  Sw_Data_Send ();
void  uart_frame_deal();
void uart_send_byte(u8 Uart_number,u8 Dat);
void Uart2SendStr(uint8_t *pstr,uint8_t strlen);



#endif