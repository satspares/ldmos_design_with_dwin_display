#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif 

/* ======= Read EEPROM on boot and set bands etc. ======== */  
void readEEPROM(){
    if (EEPROM.read(eeprom_new_on_address) != 0x5a){
      for (int i = 1 ; i < EEPROM_SIZE; i++) {  
        EEPROM.write(i, 0xff); // wipe eeprom
      }
      EEPROM.update(eeprom_new_on_address,0x5a);
      EEPROM.update(eeprom_band,band80Mtr_Selected);   // set to 80m when new 
      band_select = band80Mtr_Selected;
      EEPROM.update(eeprom_auto,0); EEPROM.update(eeprom_band,0); EEPROM.update(eeprom_antenna,0);
      band_auto = 0;
      which_antenna = 0;
      eeprom_write_power_calc_values(); 
      eeprom_write_intSetting_values();
       
    }else{
      band_select = EEPROM.read(eeprom_band);
      band_auto = EEPROM.read(eeprom_auto);
      which_antenna = EEPROM.read(eeprom_antenna);
      glo_drive_power = EEPROM.read(eeprom_drive);
      glo_volt_setting = EEPROM.read(eeprom_volt);
      glo_current_setting = EEPROM.read(eeprom_current);
      
      if (glo_current_setting > 100) glo_current_setting = 50;
      if (glo_volt_setting > 100) glo_volt_setting = 50;
      if (glo_drive_power > 100) glo_drive_power = 50;  // set drive power display page2 if out of range
      //eeprom_write_power_calc_values();  //TEST FORCE WRITE
      //eeprom_write_intSetting_values();    //TEST FORCE WRITE
      eeprom_read_power_calc_values();
      // check for out of range
      for(int j = 0; j < (sizeof(powerCalcArray)/sizeof(uint16_t)); j++)
      {
         if (powerCalcArray[j] > 2000) powerCalcArray[j] = 50; 
        // Serial.print(powerCalcArray[j]);
        // Serial.print(" ");
      }
    
      eeprom_read_intSetting_values();
      glo_power_set_value = intSettingsArray[POWERSETPOINT];
      if (glo_power_set_value > 800 || glo_power_set_value < 60) glo_power_set_value = 200;
    }       
}

void eeprom_write_power_calc_values(){
  EEPROM_writeAnything(eeprom_power_calc_address,powerCalcArray);   
}  
void eeprom_read_power_calc_values(){
     EEPROM_readAnything(eeprom_power_calc_address, powerCalcArray);  
}

void eeprom_write_intSetting_values(){
     EEPROM_writeAnything(eeprom_intSettings_address, intSettingsArray);         
}  

void eeprom_read_intSetting_values(){      
      EEPROM_readAnything(eeprom_intSettings_address, intSettingsArray);
}      
   