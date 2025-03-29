#ifndef __ADC_H__
#define __ADC_H__
#include "sys.h"
//#define read_adcavg

enum {
	ADC_CHANNEL0 = 0x00,
	ADC_CHANNEL1,
	ADC_CHANNEL2,
	ADC_CHANNEL3,
	ADC_CHANNEL4,
	ADC_CHANNEL5,
	ADC_CHANNEL6,
	ADC_CHANNEL7
};

u16 adc_read(uint16_t channel);
#ifdef read_adcavg
u16 adc_read_avg(uint16_t channel,uint16_t avg_cnt);
#endif
#endif


