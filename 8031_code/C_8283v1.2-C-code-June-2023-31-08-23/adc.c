#include "adc.h"
//#include "dgus.h"


#define ADC_START_ADDR 0x32

//Read the ad value of the channel, no need to start the command, after the screen is powered on, just read it directly
//channel:0-7
u16 adc_read(uint16_t channel)
{
//sys_read_vp(ADC_START_ADDR+channel,(u8*)&channel,1);
	
read_dgus_vp(ADC_START_ADDR+channel, (u8*)&channel,1);

return channel;
}

#ifdef read_adcavg
//Read the average ad value of the channel
//channel:0-7
//avg_cnt: average number of times
//Returns the ad value after stability
uint16_t adc_read_avg(uint16_t channel,uint16_t avg_cnt)
{
u16 cnt;
u16 ad;
u16 max_ad = 0;
u16 min_ad = 0xFFFF;
float ad_sum = 0;

for(cnt=0;cnt<avg_cnt;cnt++)
{
//Note, because the ADC sampling speed in our T5L is slow, a sampling result is generated in almost 80ms, and T5L does not
//Provide us with any flags for judging the completion of ADC conversion, so only about the delay can be used here, of course you can
//Using the timer method to do it, the advantage is that it will not cause the program to block, if you do not delay the read directly,
//The read is the last conversion result, so our average filter algorithm loses its meaning
DelayMs(100);

ad = adc_read(channel);
ad_sum += ad;//Calculate the sum
if(ad>max_ad)
max_ad = ad;//Find the maximum ad value
if(ad<min_ad)
min_ad = ad;//Find the smallest ad value
}
if(avg_cnt>2)
{
ad_sum = ad_sum-max_ad-min_ad;//Remove the maximum and minimum values ​​from the sum
avg_cnt -= 2;
}
ad = (u16)(ad_sum/avg_cnt+0.5f);//adding 0.5f is for rounding

return ad;
}
#endif














