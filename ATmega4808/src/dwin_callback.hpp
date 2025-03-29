#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif


// Event Occurs when response comes from HMI
void onHMIEvent(String address, int lastByte, String message, String response)
{  // start tag onHMIEvent
  #ifdef displayDebug
  Serial.println("OnEvent : [ A : " + address + " | D : " + String(lastByte, HEX) + " | M : " + message + " | R : " + response + " ]");
  #endif

  
  uint16_t hexAddress = strtol(address.c_str(),0,16);
  static uint16_t savePowerValue;  // for cancel button
  static uint16_t saveDrive;
  static uint16_t saveSWR;
  static uint16_t saveVolt;
  static uint16_t saveCurrent;  

  /* ======= Band Select ======== */ 
  if ((band_switch) == hexAddress){
   // if ((!band_auto)){   // Test
      if ((!tx_status) && (!band_auto)){        //dont change band on tx/band auto
      band_select = lastByte;
      select_band(lastByte);
      EEPROM.update(eeprom_band,band_select);
      usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    }else{
      usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
    } 
  }
   /* ======= Band Manual/Auto ======== */
  else if((band_manual_switch == hexAddress) && (!tx_status)) {   // band man. auto
    if (band_auto){
     band_auto = 0; 
     hmi.setVPWord(band_manual_display,BAND_MANUAL); //23 etc icon numbers
    }else{
     band_auto = 1;  
     hmi.setVPWord(band_manual_display,BAND_AUTO);  
    }
    EEPROM.update(eeprom_auto,band_auto);
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    }
  else if((band_manual_switch == hexAddress) && (tx_status)){
    usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
    }
    /* ======= Antenna Switch ======== */  
  else if((antenna_switch == hexAddress) && (!tx_status)) {  // Antenna switch
    if (which_antenna){
      which_antenna = 0;
      hmi.setVPWord(antenna_switch_display,ANTENNA_ONE);
      mcp23017.digitalWrite(ANTSWITCH,false);
    }else{
      which_antenna = 1; 
      hmi.setVPWord(antenna_switch_display,ANTENNA_TWO); 
      mcp23017.digitalWrite(ANTSWITCH,true);
    }
      EEPROM.update(eeprom_antenna,which_antenna);
      usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    }
    else if((antenna_switch == hexAddress) && (tx_status)){
      usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
    }  
  /* ======= DXWorld error reset relay or fet switch ======== */   
  else if(touch_reset == hexAddress){
     usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
      error_od_status_stop = false;
      swr_soft_trip = false; // reset swr software trip if in use
      dx_error_reset();
    }

 /* ======= SWR Display input swr/lpf  ======== */  
  else if(swr_meter_change == hexAddress){

    if(which_swr){
      which_swr = 0;
      hmi.setVPWord(swr_meter_switch_display,SWR_DISPLAY);
      swrOffset = 0;
    }else{
      which_swr = 1;
      hmi.setVPWord(swr_meter_switch_display,LPF_DISPLAY);  
      swrOffset = EEPROMROW;    //select lpf filter location in eeprom
    }
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
  } 

/* ======= Display Power Calc Settings ======== */ 
  // turn to page 1 power set
  else if (calc_power_touch == hexAddress){   
    setting_power_calc = true;  
    hmi.setVPWord(display_power_set_point,glo_power_set_value);
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    hmi.setPage(powerSetPage);
    setting_power_calc = true; 
    /*
    const uint16_t arraySize = (sizeof(powerCalcArray));
    for(int j = 0; j < arraySize/sizeof(uint16_t); j++)
    {
     Serial.print(" ");
     Serial.print(powerCalcArray[j]);
     Serial.print(" ");
     Serial.print(j);
    }
     */

    savePowerValue = powerCalcArray[calc_array_swr_offset+swrOffset];
    hmi.setText(startPage_band_text,bandStrings(calc_array_swr_offset));
    if (which_swr){
      hmi.setText(startPage_swr_text,stringLPF); 
    }else{
      hmi.setText(startPage_swr_text,stringSWR);
    }
  }
  // save button page 1 power set
  else if (save_power_calc == hexAddress){   
    setting_power_calc = false;
    eeprom_write_power_calc_values();
    glo_power_set_value = hmi.readVP(display_power_set_point);
    intSettingsArray[POWERSETPOINT] = glo_power_set_value;
    eeprom_write_intSetting_values();
    delay(50);
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
    eeprom_read_power_calc_values();
  }
  // page 1 power set cancel button
  else if (startPage_cancel == hexAddress)        
  {
    powerCalcArray[calc_array_swr_offset+swrOffset] = savePowerValue;
    usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
    setting_power_calc = false;
    tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
  }
  // test power calc button page 1 power set
  else if (test_power_calc == hexAddress){    
    uint16_t powerCalcSet;
    float Voltfwd;
    if (which_swr){
      Voltfwd = fwd1Voltage();
    }else{
      Voltfwd = fwd2Voltage();
    }
    glo_power_set_value = hmi.readVP(display_power_set_point);
 
    powerCalcSet = ((Voltfwd*Voltfwd) / (float)glo_power_set_value/MAXAMPPOWERCALC);   // glo_power_set_value normally 100/200 watt
    powerCalcSet = map(powerCalcSet,0,250,250,0);  // reverse it.
    delay(200); 
    powerCalcArray[calc_array_swr_offset+swrOffset] = powerCalcSet;  
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
  }   
/* ======= Drive level Display and Calc. ======== */ 
  // select page2 drive set
  else if (calc_drive_touch == hexAddress){
  // need if txstatus etc.
    hmi.setPage(driveSetPage);
    setting_power_calc = true;  
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    saveDrive = glo_drive_power;
    hmi.setVPWord(power_eeprom_display2,map(glo_drive_power,1,100,100,1));
    hmi.setFloatValue(power_display_page2, driveWatts());
  }
  // Save button page2 drive set
  else if (save_drive_calc == hexAddress){
    setting_power_calc = false;
    EEPROM.update(eeprom_drive,glo_drive_power);
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
  }
  // Test button Page 2 drive set
  else if (test_drive_calc == hexAddress){
    glo_drive_power = hmi.readVP(power_eeprom_display2);
    delay(200);
    glo_drive_power = map(glo_drive_power,1,100,100,1);
    hmi.setFloatValue(power_display_page2, driveWatts());
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
   
  }
  // page 2 drive set cancel button
  else if (page2_cancel == hexAddress){
    glo_drive_power = saveDrive;
    setting_power_calc = false;
    tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
    usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
  }

  /* ======= SWR Level Calc ======== */

  else if (calc_swr_touch == hexAddress){
      hmi.setPage(swrSetPage);
      setting_swr_calc = true;
      hmi.setVPWord(swr_calc_display,powerCalcArray[calc_array_swr_offset+swrOffset+(EEPROMROW*2)]);
      saveSWR = powerCalcArray[calc_array_swr_offset+swrOffset+(EEPROMROW*2)]; 
    //  Serial.println(calc_array_swr_offset+swrOffset+(EEPROMROW*2));
      hmi.setFloatValue(swr_display_glo_swr,glo_swr_display);
  }
  // swr calc cancel
  else if (swr_calc_cancel_control == hexAddress){
      powerCalcArray[calc_array_swr_offset+swrOffset+(EEPROMROW*2)] = saveSWR;
      usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
      setting_swr_calc = false;
      tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
  }
  // swr calc save
  else if (swr_calc_save_control == hexAddress){
      eeprom_write_power_calc_values();
      delay(50);
      usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
      setting_swr_calc = false;
      tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
      eeprom_read_power_calc_values();
  }
  // swr test control
  else if (swr_calc_test_control == hexAddress){
    //hmi.setFloatValue(swr_display_glo_swr,glo_swr_display);
    //Serial.println(hmi.r     (swr_calc_display));
      powerCalcArray[calc_array_swr_offset+swrOffset+(EEPROMROW*2)] = hmi.readVP(swr_calc_display);
      delay(200);
      usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
  }

  /* ======= Page 3 Volt calc ======== */
  else if (calc_volt_touch == hexAddress){
    hmi.setPage(voltSetPage);
    hmi.setVPWord(volt_calc_display,glo_volt_setting);
    saveVolt = glo_volt_setting;
    setting_volt_calc = true;
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
  }
  // volt calc save
  else if (volt_calc_save_control == hexAddress){
    setting_volt_calc = false;
    EEPROM.update(eeprom_volt,glo_volt_setting);
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
  }
  // Volt calc cancel
  else if (volt_calc_cancel_control == hexAddress){
    setting_volt_calc = false;
    glo_volt_setting = saveVolt;
    usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
    tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
  }
  // Volt test
  else if (volt_calc_test_control == hexAddress){
   // hmi.setFloatValue(volt_calc_volt_display,glo_volt_display);
    glo_volt_setting = hmi.readVP(volt_calc_display);
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    read_volt();
  }

  
  /* ======= Page 5 curent calc ======== */
  
  else if (calc_current_touch == hexAddress){
    hmi.setPage(currentSetPage);
    saveCurrent = glo_current_setting;
    hmi.setVPWord(current_calc_display,glo_current_setting);
    setting_current_calc = true;
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
  }
  else if (current_calc_save_control == hexAddress){
    setting_current_calc = false;
    EEPROM.update(eeprom_current,glo_current_setting);
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
    tx_status?hmi.setPage(txPage):hmi.setPage(startPage);

  }
  else if (current_calc_test_control == hexAddress){
    glo_current_setting = hmi.readVP(current_calc_display);
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(YES);
  } 
  else if (current_calc_cancel_control == hexAddress){
    glo_current_setting = saveCurrent;
    setting_current_calc = false;
    usebeep?hmi.beepHMI(BEEP_CANCEL):hmi.playSound(BEEPERROR);
    tx_status?hmi.setPage(txPage):hmi.setPage(startPage);
  }
  
  
} // end tag onHMIEvent

/* ======= Display String Display Functions ======== */ 
String swrStrings(bool whichSWR){
  if (whichSWR){
    return stringSWR;
  }else{
    return stringLPF;  
  }
  
}

String bandStrings(uint16_t arrayOffset){
String retString = "ERROR No Index"; 
switch(arrayOffset) {
  #ifdef display160M
  case offsetSWR160M:
  retString = string160M; 
  break;
  #endif
  case offsetSWR80M:
  retString = string80M;
  break;
  case offsetSWR60_40M:
  retString = string60_40M;
  break;
  case offsetSWR30_20M:
  retString = string30_20M;
  break;
  case offsetSWR17_15M:
  retString = string17_15M;
  break;
  case offsetSWR12_10M:
  retString = string12_10M;
  break;
  case offsetSWR6M:
  retString = string6M;
  break;
  #ifndef display160M
  case offsetSWR4M:
  retString = string4M;
  break;
  #endif
}
return retString;
}


