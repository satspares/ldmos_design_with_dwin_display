#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "Dwin_T5L1H.h"
#include "Sys.h"


void Norflash_Read(u16 nor_addr,u16 Dgus_addr,u16 len);//All parameters must be even
void Norflash_Write(u16 nor_addr,u16 Dgus_addr,u16 len);//All parameters must be even
void SendADC(uint16_t level);
//void DelayMs(uint16_t n);


#endif
