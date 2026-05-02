//#define MYDEBUG
#define DISPLAYDEBUG
//#define DISPLAY160M       // alternate icons 160m-6m or 80m-4m mine
//#define USELM35           //else DS1820
//#define DXWORLD_I           //Id from dxworld protection board else ACS712
//#define DXWORLD_ERROR_LEDS //errors from dxworld protection board
//#define A600_AMP          // A600 amp else dxworld or similar
//#define RESETDUBUG
//#define SCREENROTATE      // see setupdisplay function
//#define BIAS_ON           // keep bias on test etc
//#define MY_ALL_BAND_AMP   // my amp some different pinouts
//#define AUTO_BAND
//#define DRIVE_NO_STOP     // overdrive error only dont stop tx
#define BLINKLED            // if we are not using pin13 OPTOUT1 (a600_bias) flash onboard led
#define SENSOR_DEBUG        // debug dallas sensors

#include <Arduino.h>
#include <DWIN_Arduino.h>
#include <Wire.h>

#ifndef DXWORLD_I
    #include <ACS712.h>
#endif
#ifdef AUTO_BAND
    #include<ADS1015_WE.h> 
#endif
#include <SafeString.h>    
#include <MCP23017.h>
#include <avr/wdt.h>
#include <Ticker.h>
#include <Ewma.h>
//#include <EwmaT.h>
#ifdef USELM35
    #include <LM35.h>
#else
    #include "OneWireNg_CurrentPlatform.h"
    #include "drivers/DSTherm.h"
    #include "utils/Placeholder.h"
static Placeholder<OneWireNg_CurrentPlatform> ow;
DSTherm drv(ow);
#endif
#include <EEPROM.h>
#include <includes.h>
#ifdef RESETDUBUG
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
    setupPins();
    analogReference(INTERNAL4V3);
    analogWriteFrequency(16); // 16k
    analogWrite(FANPWM, 0);
    /*
      if we are using jtag2updi which is on tx0/rx0 to program
      the serial print functions also use the same port
      dont serial print early or we can't program easily
      this is why we need a big delay so we can jump in
      we can always use the updi port!  
    */
    delay(3000);
    Serial.begin(115200);
#ifdef RESETDUBUG
    printResetFlags();
    clearResetFlags();
#endif
    Wire.begin();
    //caution rf about
    Wire.setClock(400000UL);
    #ifndef DXWORLD_I
        ACS.autoMidPoint();
    #endif
    mcp23017_setup();
    readEEPROM();
    setupDisplay();

#ifndef USELM35
    setup_dallas_sensors();
#endif
#ifdef A600_AMP
    a600_bias_off();
#endif
    analogWrite(FANPWM, 56);  //start fan
    houseKeeping.start();
    temperatureIDTicker.start();
    peakHoldTicker.start();
    sendPowerSwrRefTicker.start();
    #ifdef DXWORLD_ERROR_LEDS
    dx_error_reset();
    #endif
//    attachInterrupt(PTT, PTTservice, FALLING); // Falling edge at PTT
    attachInterrupt(SWR2_INT, swr2IntRising, RISING);
    attachInterrupt(SWR1_INT, swr1IntRising, RISING);
    clearSWRInterruptFlags();
    wdt_enable(WDT_PERIOD_4KCLK_gc);      // set watchdog to 4 secs
    keepingHouse(); // if band auto pull band relays now
    delay(500);     // dont start just yet
    Serial.println("Starting...");
#ifdef BIAS_ON
#ifdef A600_AMP
    a600_bias_on();
#else
    bias_on();
#endif
#endif

#ifdef AUTO_BAND
    if (!adc.init()) {
        Serial.println("ADS1015 not connected!");
    }else{
        adc.setVoltageRange_mV(ADS1015_RANGE_4096);         
    }
#endif //AUTOBAND    

#ifdef MYDEBUG
    displayText.setOutput(Serial);
    safeStringFromEEPROM(eeprom_text1_address,30, displayText);
    displayText.debug();
    safeStringFromEEPROM(eeprom_text2_address,30, displayText);
    displayText.debug();
#endif

}

void loop() {
    wdt_reset();
    hmi.listen();
    // keep our tickers alive
    houseKeeping.update();
    temperatureIDTicker.update();
    sendPowerSwrRefTicker.update();
    peakHoldTicker.update();

    tx_actions();
    if (tx_status) {
       calcPowerandDisplay();  
    }

    if (temp_id_reset) {
        
        float tempNow;
        //this ticker is temperatureIDTicker
        tempNow = readTemp(false);
        hmi.setFloatValue(temp_display, tempNow);
        hmi.setFloatValue(volt_display, read_volt());
        hmi.setFloatValue(current_display, readI());
        error_swr(); 
        #ifdef DXWORLD_ERROR_LEDS
        error_i();
        // no longer used
        //if (!pttIntActive) error_po(); 
         #else
         //
         #endif 
        error_odrive();
        temp_id_reset = false;
        }
    }

void keepingHouse() {
    // runs continuous every 2000ms
#ifdef BLINKLED
    static bool toggle;
    digitalWrite(13, toggle);
    toggle = !toggle;
#endif // blinkLED

#ifdef AUTO_BAND    
    if ((!tx_status) && (band_auto)){
     bcd_band();
    }else if (!band_auto){
     band_auto_touched = true;
    }
#endif // AUTO_BAND    
    

}



