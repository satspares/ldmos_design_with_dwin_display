#ifndef __PARAMETER_CONFIG_H__
#define __PARAMETER_CONFIG_H__
#include "Dwin_T5L1H.h"

#define SELECT_RTC_TYPE 1 //RX8130=1;SD2058=2 ;No RTC=0

#define UART2_ENABLE 1 //T5L serial port 2 on/off setting, 1 is on, 0 is off
#define UART3_ENABLE 0 //T5L serial port 3 on/off setting, 1 on, 0 off
#define UART4_ENABLE 0 //T5L serial port 4 on and off settings, 1 on, 0 off
#define UART5_ENABLE 1 //T5L serial port 5 on/off setting, 1 on, 0 off

#define UART2_RX_LENTH 2048 //T5L serial port 2 receiving array length
#define UART3_RX_LENTH 2048 //T5L serial port 3 receiving array length
#define UART4_RX_LENTH 2048 //T5L serial port 4 receiving array length
#define UART5_RX_LENTH 2048 //T5L serial port 5 receiving array length

#define BAUD_UART2 115200 //T5L serial port 2 baud rate setting
#define BAUD_UART3 115200 //T5L serial port 3 baud rate setting
#define BAUD_UART4 115200 //T5L serial port 4 baud rate setting
#define BAUD_UART5 115200 //T5L serial port 5 baud rate setting

#define RESPONSE_UART2 0 //Serial port 2 response 4F4B open and close setting, RESPONSE_UART2=1 open, RESPONSE_UART2=0 close
#define RESPONSE_UART3 1 //Serial port 3 response 4F4B open and close setting, RESPONSE_UART3=1 open, RESPONSE_UART3=0 close
#define RESPONSE_UART4 1 //Serial port 4 response 4F4B open and close setting, RESPONSE_UART4=1 open, RESPONSE_UART4=0 close
#define RESPONSE_UART5 1 //Serial port 5 response 4F4B open and close setting, RESPONSE_UART5=1 open, RESPONSE_UART5=0 close

#define DATA_UPLOAD_UART2 1 //Serial port 2 data automatic upload setting, 1 uploads, 0 does not upload
#define DATA_UPLOAD_UART3 1 //Serial port 3 data automatic upload setting, 1 uploads, 0 does not upload
#define DATA_UPLOAD_UART4 1 //Serial port 4 data automatic upload setting, 1 uploads, 0 does not upload
#define DATA_UPLOAD_UART5 1 //Serial port 5 data automatic upload setting, 1 uploads, 0 does not upload

#define CRC_CHECK_UART2 0 //Serial port 2 data CRC check setting, 1 enables CRC, 0 disables CRC
#define CRC_CHECK_UART3 0 //Serial port 3 data CRC check setting, 1 enables CRC, 0 disables CRC
#define CRC_CHECK_UART4 0 //Serial port 4 data CRC check setting, 1 enables CRC, 0 disables CRC
#define CRC_CHECK_UART5 0 //Serial port 5 data CRC check setting, 1 enables CRC, 0 disables CRC
#endif