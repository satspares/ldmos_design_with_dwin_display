#pragma once
#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif 


/* === Over temp protection and show temp error=== */
void error_temp(float temperature){
    
    static bool highTemp;   
    if(temperature >= ( (intSettingsArray[TEMPSETPOINT])-1)){
             highTemp = true;
        analogWrite(FANPWM,256); // Full Fan Speed;
    }

    if((temperature <= ((intSettingsArray[TEMPSETPOINT])-5))){
        highTemp = false;
        fanspeed(temperature); // fanspeed to normal
     }

    if ((error_temp_status == false) && (highTemp)){ 
        hmi.setVPByte(temp_error,RED_ICON);
        error_temp_status = true;
        usebeep?hmi.beepHMI(BEEP_ERROR):hmi.playSound(BEEPERROR);
        }
    
    if ((error_temp_status == true) && (!highTemp)){ 
         hmi.setVPByte(temp_error,GREEN_ICON);
         error_temp_status = false;
         }
       
    }
/* === Monitor and show over SWR error on display === */
void error_swr(){
  
    uint16_t error_swr_en = digitalRead(LED_SWR);
  
   // if (swrSoftTrip == true) error_swr_en = false;
  
    if (error_swr_en==LOW && error_swr_status == false){
      hmi.setVPByte(swr_error,RED_ICON);
      error_swr_status = true;
      usebeep?hmi.beepHMI(BEEP_ERROR):hmi.playSound(BEEPERROR);
      }
    else if(error_swr_en==HIGH && error_swr_status == true){
      hmi.setVPByte(swr_error,GREEN_ICON);
      error_swr_status = false;
    }
  }

  /* === Monitor and show over current error on display === */
void error_i(){
    uint16_t error_i_en = digitalRead(LED_I);
    if (error_i_en == LOW && error_i_status == false){
      hmi.setVPByte(current_error,RED_ICON);
      error_i_status = true;
      usebeep?hmi.beepHMI(BEEP_ERROR):hmi.playSound(BEEPERROR);
      }
       else if (error_i_en == HIGH && error_i_status == true){
        error_i_status = false;
        hmi.setVPByte(current_error,GREEN_ICON);
       }
  }
  
  /* === Monitor and show over Power error on display === */
  /* === we are only showing and sounding the error and not stopping tx === */
  void error_po(){
    uint16_t error_po_en = digitalRead(LED_PO);
    if (error_po_en==LOW && error_po_status == false){
      hmi.setVPByte(power_error,RED_ICON);
      error_po_status = true;
      usebeep?hmi.beepHMI(BEEP_ERROR):hmi.playSound(BEEPERROR);
     }
        else if (error_po_en==HIGH  && error_po_status == true){
        hmi.setVPByte(power_error,GREEN_ICON);
        error_po_status = false;
        }
  }

  void error_vo(float V)
{ 
    //V = 55; // test
    if((V >= intSettingsArray[VOLTSETPOINT]) && (error_vo_status == false)){
      hmi.setVPByte(volt_error,RED_ICON);
      error_vo_status = true;
      usebeep?hmi.beepHMI(BEEP_ERROR):hmi.playSound(BEEPERROR);
    }
    else if( (V < intSettingsArray[VOLTSETPOINT]) && (error_vo_status == true))
    {
      hmi.setVPByte(volt_error,GREEN_ICON);
      error_vo_status = false;
    }

}
/* ======= Read 48/65v Voltage ======== */
float read_volt(){
  float vcc;float Vin=0.00;
  float Vout=0.00;int v1_val=0;
  float Vresult=0;
  float mapResult;

  v1_val = analogRead(VIN50V);
  
  Vout =(float)((v1_val * 4.30) / 1024.00); 
  Vin = Vout / (Res50v_2/(Res50v_1+Res50v_2)); 
  Vresult = ((Vin * 10)+0.5);
  Vresult = voltFilter.filter(Vresult);

  if (glo_volt_setting <= 50){
    mapResult = map(glo_volt_setting,1,50,50,1);
    Vresult = Vresult - (float)(mapResult * 0.5);
  }else{
    mapResult = map(glo_volt_setting,51,100,1,50);
    Vresult = Vresult + (float)(mapResult * 0.5);
  }
  if (setting_volt_calc){
     hmi.setFloatValue(volt_calc_volt_display,(float)Vresult/10);
  }
  error_vo((float)Vresult/10);  
  return (float)Vresult/10;
} 
/* ======= Adjust Fanspeed to Temperature ======== */
void fanspeed(float temperature){
 
  //analogWrite(FANPWM,0);  //TEST
  //return;

  if(temperature>20.0 && temperature<32.0){  
    analogWrite(FANPWM,64);     // 25% duty cycle;
  }
  else if(temperature>=33.0 && temperature<40.0){
    analogWrite(FANPWM,128); //192 75% duty cycle) test 256 14-09-24
  }
  else if(temperature>=40.0 && temperature<45.0){
    analogWrite(FANPWM,256);     // Full;
  }
  else if(temperature>=45.0 && temperature < intSettingsArray[TEMPSETPOINT]){
    analogWrite(FANPWM,256);     // Full Fan Speed;
  }
   else if(temperature <= 20.0){
    analogWrite(FANPWM,0);     // no Fan Speed;
  } 
}

/* ======= Read the LDMOS Drain Current ======== */
float readI(){    
  float currentI = 0;
  float mapResult;
  #ifndef useMAX40010HAUT
 // currentI = (analogRead(ID_IN) * 2); // even split on input
 currentI = (analogRead(ID_IN));
  currentI *= adcRef;  
  currentI = (currentI/2400); // need working out TO DO
  if (glo_current_setting <= 50){
    mapResult = map(glo_current_setting,1,50,50,1);
    currentI *= (13000 - (mapResult * ICALCMAJOR));
  }else{
    mapResult = map(glo_current_setting,51,100,1,50);
    currentI *= (13000 + (mapResult * ICALCMAJOR));
  }
  #else
  currentI = (analogRead(ID_IN));
  //currentI = 102.4; // test should be 4.30amps
  if (glo_current_setting <= 50){
    mapResult = map(glo_current_setting,1,50,50,1);
    currentI = currentI - (mapResult * (ICALCMAJOR/50));
    Serial.println(currentI);
  }else{
    mapResult = map(glo_current_setting,51,100,1,50);
    currentI = currentI + (mapResult * (ICALCMAJOR/50));
  }
  currentI *= adcRef;
  currentI /= 0.1;
  #endif

  if (setting_current_calc){
   hmi.setFloatValue(current_calc_current_display,currentI); 
  }
  return currentI;
}

/* ======= Read Temperature ======== */
float readTemp(bool starting){
  
  float temperature = 0; 
  #ifdef useLM35
  //R15 Not needed
  byte iterations = 5;
  int sum=0;int i=0;int j=0;
  while(i<iterations ){
    j = sensor.getTemp();
    delay(10); 
    sum += j;    
    i++;  
  }
  temperature = sum/iterations;  
  #else
  /* from OneWireNg example */
  DSTherm drv(ow);
  uint8_t sensorCount = 0;
  if (starting){
    drv.convertTempAll(DSTherm::MAX_CONV_TIME_START, false);
  }else{
    // we probably need more MAX_CONV_TIME for multiple sensors
    drv.convertTempAll(DSTherm::MAX_CONV_TIME, false); 
  }
  #if SENSOR_COUNT == 1
  static PlaceholderInit<DSTherm::Scratchpad> scrpd;
  OneWireNg::ErrorCode ec = drv.readScratchpadSingle(scrpd);
  if (ec == OneWireNg::EC_SUCCESS) {
      temperature = (DS18B20Temperature(scrpd));
   } else if (ec == OneWireNg::EC_CRC_ERROR){
    #ifdef SENSOR_DEBUG
    Serial.println("  DS18B20 CRC error.");
    #endif
   }
  #else  // #if SENSOR_COUNT > 1
  /* read sensors one-by-one from OneWireNg example */
  Placeholder<DSTherm::Scratchpad> scrpd;
  for (const auto& id: *ow) {
      Serial.print("sensor count ");Serial.println(sensorCount) ;   
      if (returnSensorId(id,sensorCount)) {
          if (drv.readScratchpad(id, scrpd) == OneWireNg::EC_SUCCESS){
              temperature = DS18B20Temperature(scrpd);
              error_temp(temperature); // check error on all sensors
              sensor_array[sensorCount].sensor_value = temperature;
              sensor_array[sensorCount].count = sensorCount+1;   // we can use 0 as a end stop if needed
              sensorCount +=1;
          }  
          else{
            #ifdef SENSOR_DEBUG
              Serial.println("  Read scratchpad error.");
            #endif  
          }
      }     
  }
  #ifdef SENSOR_DEBUG
  Serial.println("Sensor Array");
  for (int i =0; i< SENSOR_COUNT; i++){
      Serial.print(sensor_array[i].sensor_our_name);Serial.print(" "); 
      Serial.print(sensor_array[i].sensor_value);Serial.println(" "); 
      Serial.println(sensor_array[i].mac_address);
      Serial.print(sensor_array[i].sensor_real_name);Serial.println(" ");
  }
  #endif // #ifdef SENSOR_DEBUG
  #endif  // #if SENSOR_COUNT
 
#endif  //  #ifdef useLM35
//Serial.println(temperature);
// temperature = 55.00;  // test
error_temp(temperature);
return temperature;  // 18b20 return last sensor 
}


#ifndef useLM35
float DS18B20Temperature(const DSTherm::Scratchpad& scrpd)
{
    const uint8_t *scrpd_raw = scrpd.getRaw();   
    long temp = scrpd.getTemp2();
    return ((float)temp / 16);
}
static bool returnSensorId(const OneWireNg::Id& id,uint8_t sensorCount )
{
    const char *name = DSTherm::getFamilyName(id);
    String macAdress = "";
    for (size_t i = 1; i < sizeof(OneWireNg::Id); i++) {
      macAdress=checkHex(id[i]) + ":" + macAdress;
    }
    sensor_array[sensorCount].mac_address = macAdress;
    sensor_array[sensorCount].sensor_real_name = String(name); 
    return (name != NULL);
}
#endif //  ifndef useLM35 functions


void error_odrive()
{ 
float driveWattsIn = driveWatts();  
if ((driveWattsIn >= ((float)intSettingsArray[DRIVESETPOINT])) && (error_od_status == false)) {

  error_od_status = true;
  error_od_status_stop = true;
  hmi.setVPByte(overdrive_error,RED_ICON);
  #ifdef A600_AMP
   a600_bias_off();
  #else
  dx_bias_off();
  #endif
  usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
}
else if ((error_od_status_stop == false) && (error_od_status == true))
{
  error_od_status = false;
  hmi.setVPByte(overdrive_error,GREEN_ICON);
}
}

// format hex string for single digits
String checkHex(byte currentNo)
{
    if (currentNo < 0x10)
    {
        return "0" + String(currentNo, HEX);
    }
    return String(currentNo, HEX);
}


void dx_error_reset(){
  mcp23017.digitalWrite(DX_RESET,true);
  delay(100);
  mcp23017.digitalWrite(DX_RESET,false);
}