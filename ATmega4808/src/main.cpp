//#define myDebug
//#define displayDebug
//#define display160M       // alternate icons 160m-6m or 80m-4m
//#define useLM35           //else DS1820
//#define useMAX40010HAUT   //else Id from dxworld protection board
//#define A600_AMP          // A600 amp else dxworld or similar
//#define resetDebug
//#define SCREENROTATE      // see setupdisplay function
#define blinkLED            // if we are not using pin13 OPTOUT1 flash onboard led
#define SENSOR_DEBUG        // debug dallas sensors

#include <Arduino.h>
#include <DWIN_Arduino.h>
#include <Wire.h>
#include <MCP23017.h>
#include <avr/wdt.h>
#include <Ticker.h>
#include <Ewma.h>
//#include <EwmaT.h>
#ifdef useLM35
  #include <LM35.h>
#else
  #include "OneWireNg_CurrentPlatform.h"
  #include "drivers/DSTherm.h"
  #include "utils/Placeholder.h"
  static Placeholder<OneWireNg_CurrentPlatform> ow;
#endif
#include <EEPROM.h>
#include <includes.h>
#ifdef resetDebug
 #include <resets.h>
#endif
#include <setup_functions.hpp>
#include <errors_and_board_reading.hpp>
#include <calc_power_and_display.hpp>
#include <dwin_callback.hpp>
#include <eeprom_functions.hpp>
#include <ptt_actions.hpp>
#include <band_select_functions.hpp>


void setup() {
  setGlobalVars();
  setupPins();
  analogReference(INTERNAL4V3);
  analogWriteFrequency(16); // 16k
  analogWrite(FANPWM,0);
  delay(3000);
  Serial.begin(115200);
  #ifdef resetDebug
    printResetFlags();
    clearResetFlags();
  #endif
  Wire.begin();
  mcp23017_setup();
  readEEPROM();
  setupDisplay();

  #ifndef useLM35
    setup_dallas_sensors();
  #endif
  
  #ifdef A600_AMP
    a600_bias_off();
  #endif
  
  analogWrite(FANPWM,56);  //start fan

  houseKeeping.start();
  temperatureIDTicker.start();
  peakHoldTicker.start();
  sendPowerSwrRefTicker.start();
  dx_error_reset();
  wdt_enable(WDT_PERIOD_4KCLK_gc);      // set watchdog to 4 secs 
  
}

void loop() {
  wdt_reset();
  hmi.listen();
  // keep our tickers alive
  houseKeeping.update();
  temperatureIDTicker.update();
  sendPowerSwrRefTicker.update();
  if (peakHoldTicker.state() == RUNNING){
     peakHoldTicker.update();
  }
  tx_actions();
  calcPowerandDisplay();
  if (temp_id_reset){
   //this ticker is temperatureIDTicker
   temp_id_reset = false;
   float tempNow = readTemp();
   hmi.setFloatValue(temp_display,tempNow);
   hmi.setFloatValue(volt_display,read_volt());
   hmi.setFloatValue(current_display,readI());
   
   error_swr();error_i();error_po(); error_odrive();
  }  

}

void keepingHouse()
{
  // runs continuous

  #ifdef blinkLED
  static bool toggle;
  digitalWrite(13,toggle);
  toggle = !toggle;
  #endif
 
 if ((!tx_status) && (band_auto)){
  bcd_band();
 }else if (!band_auto){
  band_auto_touched = true;
 }
 
}



