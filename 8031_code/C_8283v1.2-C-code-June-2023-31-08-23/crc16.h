/******************************************************************************
File name: crc16.h
Version number: V1.0
Author: pinot
Generation date: April 13, 2020
Function description: MODBUS crc16 calculation
Modification history:
Date :
author :
Modify content:
******************************************************************************/

#ifndef _CRC16_H_
#define _CRC16_H_

/*****************************************************************************
system library */

/*****************************************************************************
Custom header file */
#include "Dwin_T5L1H.h"

/*****************************************************************************
macro definition variable */

/*****************************************************************************
External function declaration */
u16 crc16table(u8 *ptr, u16 only);

#endif