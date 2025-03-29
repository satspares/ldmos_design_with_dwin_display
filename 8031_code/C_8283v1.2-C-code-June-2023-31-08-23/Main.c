#include "Dwin_T5L1H.h"
#include "i2c.h"
#include "crc16.h"
#include "Sys.h"
#include "Uart.h"
//=============T5L_S4=115200=====================
#include "defines.h"
#include "adc.h"
#include "Functions.h"

void one_second_timer(void);
volatile u16 one_second_timer_count = 0;
volatile byte PWM = 40;
volatile byte LEDSet[2] = {0x00, 0x00}; // Start with Green LED 
volatile u16 Count_num1 = 0;

byte SystemSet[4] = (0x5A, 0x00, 0x00, 0x0F);
//byte PlayDing[4] = (0x0B, 0x01, 0x40, 0x00);
//byte PlayStartSound2[4] = {0x02, 0x01, 0x40, 0x00};

/****************************************************************************/
void Main()
{
	Sys_Cpu_Init();
	WDT_ON();
	uart_init();
  RedLED = 0;     // turn off leds
	GreenLED = 0;
  
	//WriteDGUS(SOUNDADDRESS, PlayStartSound2, sizeof(PlayStartSound2));
	
	while (1)
	{
		Clock();
		if (Count_num1 == 0)
		{
			Sw_Data_Send(); // Data upload automatically
			Count_num1 = 100;
		}
		uart_frame_deal(); // serial port data processing
		one_second_timer();
		WDT_RST();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////

// Timer0 ISR
void Timer0_ISR(void) interrupt 1
{
	u16 temp = 0;
	EA = 0;
	TR0 = 0; // Stop Timer 0
	if (LEDSet[1] == 0)
	{
		RedLED = 0;
		if (GreenLED)
		{
			GreenLED = 0;
			temp = (255 - PWM) * PWM_FREQ_NUM;
		}
		else
		{
			GreenLED = 1;
			temp = PWM * PWM_FREQ_NUM;
		}
	}
	else
	{
		GreenLED = 0;
		if (RedLED)
		{
			RedLED = 0;
			temp = (255 - PWM) * PWM_FREQ_NUM;
		}
		else
		{
			RedLED = 1;
			temp = PWM * PWM_FREQ_NUM;
		}
	}
	TL0 = 0xFF - temp & 0xFF;
	TH0 = 0xFF - (temp >> 8) & 0xFF;
	TF0 = 0; // Clear the interrupt flag
	TR0 = 1; // Start Timer 0
	EA = 1;
}

void one_second_timer(void)
{
	u16 ad;
	static u16 secondCount;
	static u16 old_adc_level;
	static u16 adc_level_trigger;

	if (one_second_timer_count >= 1000) // over or = 1 second
	{
		one_second_timer_count = 0;
		secondCount++;
		read_dgus_vp(LEDSETRAM, LEDSet, sizeof(LEDSet));
		//LOG((uint8_t *)&LEDSet,sizeof(LEDSet));
		ad = adc_read(ADC_CHANNEL7);

		if ( (adc_level_trigger < (ad - ADCCHANGE )) || (adc_level_trigger > (ad + ADCCHANGE) && (secondCount > 2) ))
		{
			secondCount = 0;
			adc_level_trigger = ad;
			SendADC(ad);
		}

		if ((ad <= ADCSWITCHLEVEL) && (ad != old_adc_level))
		{
			old_adc_level = ad;
			PWM = LEDHIGH;
		}
		else if (ad > ADCSWITCHLEVEL && (ad != old_adc_level))
		{
			old_adc_level = ad;
			PWM = LEDLOW;
		}
	}
}

/**************************************************** *****************************/
void Sys_Timer2_isr() interrupt 5 // 1MS
{
	TF2 = 0; // Clear the interrupt flag bit of timer 2
	if (TimVal < 255)
		TimVal++;
	SysTick_RTC++;
	if (T_O5 > 0)
		T_O5--;
	if (T_O4 > 0)
		T_O4--;
	if (T_O3 > 0)
		T_O3--;
	if (T_O2 > 0)
		T_O2--;
	if (Count_num1 > 0)
		Count_num1--;
	one_second_timer_count++;
}

