#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif

float driveWatts()
{
  float Voltage;
  float Power;
  Voltage = analogRead(RFSENSE);
  Voltage = map(Voltage,0,1023,1,4300);
  Voltage = Voltage + diodeLossMVdrive;
  Voltage = driveFilter.filter(Voltage);
  if (Voltage <= diodeLossMVdrive +1 ){
    return 0;
  }else{
     Voltage = (Voltage / (DRIVECALCMAJOR - map(glo_drive_power,1,100,1,1000) ));
     Power = (pow(Voltage,2.00)); // should be * 50 but we would only have to div. again by 50
    if (Power > 9.90){            // drive set point max is 10 watts
      return(9.90);
    }else{
      return Power;
    }
  }
}

/* ======= Get Voltage from SWR1/REF1 SWR2/REF2 ======== */
float fwd1Voltage()
{
  float Voltage;
  Voltage = analogRead(SWR1);
  Voltage = map(Voltage,0,1023,1,4300);
  return Voltage;
}
float ref1Voltage()
{
  float Voltage;
  Voltage = analogRead(REF1);
  Voltage = map(Voltage,0,1023,lpfMapLow,lpfMapHigh);
  return Voltage;
}
float fwd2Voltage()
{
  float Voltage;
  Voltage = analogRead(SWR2);
  Voltage = map(Voltage,0,1023,1,4300);
  return Voltage;
}
float ref2Voltage()
{
  float Voltage;
  Voltage = analogRead(REF2);
  Voltage = map(Voltage,0,1023,swrMapLow,swrMapHigh);
  return Voltage;
}

/* ======= Calc and Display Output Power/REF/SWR etc. ======== */
void calcPowerandDisplay()
{
  float rawFwdVoltage; float rawRefVoltage;
  float fwdPower; float refPower;
  static float fwdPower_max; static float refPower_max;
  float powerCalc;
  uint8_t swr_calc_major;

  if ((which_swr == false))  // false the antenna tandem match
  { 
    rawFwdVoltage = fwd2Voltage(); rawRefVoltage = ref2Voltage();
    swr_calc_major = SWRCALCMAJORSWR;
  }
  else
  {
    rawFwdVoltage = fwd1Voltage(); rawRefVoltage = ref1Voltage();
    swr_calc_major = SWRCALCMAJORLPF;
  }
  
  powerCalc = powerCalcArray[calc_array_swr_offset+swrOffset];
  powerCalc = map(powerCalc,0,250,250,0);  // reverse it

  if ((rawRefVoltage >= rawFwdVoltage) || (rawFwdVoltage <= (diodeLossMV)))
  {
    rawRefVoltage = 0.00;rawFwdVoltage = 0.01;
  }
  // swr/ref power adjustment 
  rawRefVoltage = correctRefVoltage(rawRefVoltage+diodeLossMV, rawFwdVoltage, swr_calc_major);
  fwdPower = pow((rawFwdVoltage+diodeLossMV),2.00); refPower = pow((rawRefVoltage),2.00);
  fwdPower = fwdPower/powerCalc/MAXAMPPOWERCALC; refPower = refPower/powerCalc/MAXAMPPOWERCALC;

  if (fwdPower >= fwdPower_max)
  {
    fwdPower_max = fwdPower; refPower_max = refPower;
  }
  if (peak_hold_reset){
    peak_hold_reset = false;
    fwdPower_max = fwdPower; refPower_max = refPower;
  }
  //Test both should work out the same
  const float SWR = (rawFwdVoltage+rawRefVoltage)/(rawFwdVoltage-rawRefVoltage);
  //const float SWR = (1.00 + sqrt(refPower_max/fwdPower_max)) / (1.00 - sqrt(refPower_max/fwdPower_max));    
  float swr_display = ((SWR * 10.00 )); // Float x 10 for our display
  if ((swr_display < 10.00) || isNegative(swr_display)){
    swr_display = 10;  
  } 
                   
  if (setting_power_calc){
    hmi.setVPWord(power_display_startPage, (int)fwdPower_max); // update page 1 display 
    glo_power_fwd = (int)fwdPower_max; 
//    power_swr_reset = false;            // dont need below on power set page
  }
  if (setting_swr_calc){
    glo_swr_display = swr_display/10.00;   // used by calc swr
    hmi.setFloatValue(swr_display_glo_swr,glo_swr_display);
   // power_swr_reset = false;   
    if (which_swr == false){
      hmi.setVPWord(fwd_millivolts,analogRead(SWR2));
      hmi.setVPWord(ref_millivolts,analogRead(REF2));
    }else{
      hmi.setVPWord(fwd_millivolts,analogRead(SWR1));
      hmi.setVPWord(ref_millivolts,analogRead(REF1));
    }
  }  // end setting_swr_calc

  //if (true)  //test
  if(power_swr_reset)
  {        
   // Ticker timeout to update display
   if (fwdPower_max < 5) fwdPower_max = 0;
      power_swr_reset = false; // ticker reset
      float driveWattsIn = driveWatts(); 
      hmi.setVPWord(power_graph, (int)fwdPower_max);          //1-powerRangeMax
      hmi.setVPWord(swr_graph, ((int)swr_display * 10));      // 100-swrRangeMax
      hmi.setVPWord(power_display, (int)fwdPower_max);        // int 4 digits
      hmi.setFloatValue(rev_display,refPower_max);            // float
      hmi.setFloatValue(swr_digits, (float)swr_display / 10); // float int 1 decimal 2
      hmi.setFloatValue(drive_display, driveWattsIn);
 
#ifdef myDebug
    Serial.print("SWR ");
    Serial.println(SWR);
    Serial.print("FWD ");
    Serial.println(fwdPower);
    Serial.print("REF ");
    Serial.println(refPower);
#endif
  }
}

float correctRefVoltage(float refVoltage, float fwdVoltage, uint8_t swr_calc_major){
  if (refVoltage > 1){
    float swrCalc;
    //swrCalc = (swrCalc / 1.5)  // test
    swrCalc = powerCalcArray[calc_array_swr_offset+swrOffset+(EEPROMROW*2)];    
    if (swrCalc <= 500){                                // 500 will be about 0 
      swrCalc = map(swrCalc,500,1,1,500); 
     refVoltage = refVoltage - (float)(swrCalc*swr_calc_major); 
     if (isNegative(refVoltage)) refVoltage = 0;
    }
    else{
      swrCalc = map(swrCalc,505,1000,1,500);
      refVoltage = refVoltage + (float)(swrCalc*swr_calc_major);
      if (refVoltage > fwdVoltage) refVoltage = 0;
    }
  }
  return refVoltage;
}

static int isNegative(float swr)
{
    return ((static_cast<int>(swr+1) > 0) ? 0 :1);
}
