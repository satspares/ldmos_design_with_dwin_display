#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif


  /* ======= Setup DWIN Display on Startup ======== */
  void setupDisplay(){ 
    hmi.initSerial(Serial2, DGUS_BAUD );
    hmi.hmiCallBack(onHMIEvent);  //set callback
    hmi.echoEnabled(false);  // dont want to see all the display transactions 
    hmi.ackDisabled(true);   //for our no ack kernel 
    #ifdef SCREENROTATE
  /* ======= Screen rotate should be set in .cfg config file ======== */
      const byte rotate0=0; const byte rotate90=1;
      const byte rotate180=2; const byte rotate270=3;
      setScreenRotate(rotate90);  // some display set to portrait when new!
    #endif
    hmi.setPage(0);
    // Clear the icons
    hmi.setVPByte(swr_error,GREEN_ICON); hmi.setVPByte(power_error,GREEN_ICON);
    hmi.setVPByte(current_error,GREEN_ICON); hmi.setVPByte(volt_error,GREEN_ICON);
    hmi.setVPByte(temp_error,GREEN_ICON); hmi.setVPByte(overdrive_error,GREEN_ICON);
    hmi.setFloatValue(current_display,0);
    hmi.setFloatValue(swr_digits,1.00);  hmi.setVPWord(swr_graph,100);   // 100 - 200
    hmi.setVPWord(band_manual_display,(band_manual+band_auto));   
    hmi.setVPWord(antenna_switch_display,(antenna_one+which_antenna));  
    which_antenna?mcp23017.digitalWrite(ANTSWITCH,true):mcp23017.digitalWrite(ANTSWITCH,false);
    hmi.setVPWord(swr_meter_switch_display,25);  
    hmi.setText(main_text1,MainText1);  
    hmi.setText(main_text2,MainText2);
    hmi.setTextColor(main_text1_sp,3,MainText1_Color);
    hmi.setTextColor(main_text2_sp,3,MainText2_Color);
    delay(500);
    hmi.setTPBeep(false); // turn off dwin touch beep use ours
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(STARTING3);
    hmi.writeLongValue(LCDBackLightAddress,LCDBackLightLow);
    #ifdef display160M
    select_band(band160Mtr_Selected);     // make sure we get the right icon set
    #else
    select_band(band4Mtr_Selected);
    #endif
    delay(500);     // debug
    select_band(band_select); //from eeprom
  }

  /* ======= MCP Initial Settings ======== */
  void mcp23017_setup(){
    mcp23017.init();
    //Port A as output
    //direction (IODIRA) - set direction of all lpf pins as outputs
    mcp23017.portMode(MCP23017Port::A, 0);          
    // Configure PORTB (switches)
    // unused pins should be set as outputs
    mcp23017.portMode(MCP23017Port::B,
      //0b00001100,   // direction (IODIRB) - set direction of pins
      //0b00001100,   // pull-up (GPPUB) - enable internal pull-up resistors on inputs
      //0b00001100);  // polarity (IPOLB) - invert logic polarity for inputs
      // All set for outputs at the moment we dont want floating inputs
      0b00000000,   // direction (IODIRB) - set direction of pins
      0b00000000,   // pull-up (GPPUB) - enable internal pull-up resistors on inputs
      0b00000000);  // polarity (IPOLB) - invert logic polarity for inputs
    // Reset MCP23017 ports
    mcp23017.writeRegister(MCP23017Register::GPIO_A, 0x00);
    mcp23017.writeRegister(MCP23017Register::GPIO_B, 0x00);
  }


   /* ======= Set Pin Mode on Startup ======== */
  void setupPins(){
    pinMode(FANPWM, OUTPUT);
    //pinMode(TEMPSENSOR,INPUT);
    #ifdef blinkLED
    pinMode(OPTOUT1,OUTPUT);      //13
    #endif
    pinMode(OPTOUT1,OUTPUT);      //13
    pinMode(VIN50V,INPUT);
    pinMode(SWR1,INPUT);
    pinMode(REF1,INPUT);
    pinMode(SWR2,INPUT);
    pinMode(REF2,INPUT);
    pinMode(ID_IN,INPUT);
    pinMode(RFSENSE,INPUT);
    pinMode(PTT,INPUT);
    pinMode(LED_I,INPUT);
    pinMode(LED_PO,INPUT);
    pinMode(LED_SWR,INPUT);
    pinMode(BCD_0, INPUT);
    pinMode(BCD_1, INPUT);
    pinMode(BCD_2, INPUT);
    pinMode(BCD_3, INPUT);
    pinMode(INTB, INPUT_PULLUP);

  }
  /* ======= Some Ticker Resets ======== */
  void tempIDTickerReset(){   
    temp_id_reset = true;
  }
  void peakHoldReset(){
    peak_hold_reset = true;
  }
  void powerSWRTickerReset(){
    power_swr_reset = true;
  }

  #ifndef useLM35 
  void setup_dallas_sensors(){
    new (&ow) OneWireNg_CurrentPlatform(OW_PIN, false);
    #if SENSOR_COUNT > 1
    sensor_array[0].sensor_our_name = sensor0;
    sensor_array[1].sensor_our_name = sensor1;
    sensor_array[2].sensor_our_name = sensor2;
    sensor_array[3].sensor_our_name = sensor3;
    #endif
  }
  #endif

void setGlobalVars() {     // redo incase of watchdog reboot
  #ifdef display160M
    const uint8_t band_display_offset = 7;   //display second band icons 160m-6m 7 icons per. display
  #else
    const uint8_t band_display_offset = 0;   // display first band icons 80m-4m
  #endif
  bool tx_status = false;             // true when tx on
  bool band_auto = false;             // band man auto
  bool which_antenna = false;         // Antenna one
  bool which_swr = false;             // which swr result to display
  bool band_auto_touched = false; // detect band auto to manual change - used = bcd band select
  uint8_t swrOffset = 0;           // offset into eeprom for swr/lpf
  bool temp_id_reset = false;     // ticker reset for temp and ID
  bool peak_hold_reset = false;   // ticker reset for power peak hold etc
  bool power_swr_reset = false;   // power swr ticker reset
// error globaal vars
  bool error_i_status = false;
  bool error_swr_status = false;
  bool error_po_status = false;
  bool error_temp_status = false;
  bool error_vo_status = false;
  bool error_od_status = false;
  bool error_od_status_stop = false; // stop tx and need reset
  bool swr_soft_trip = false;
  bool setting_power_calc = false;   // we are in the power calc page
  bool setting_swr_calc =false;      // we are in the swr calc page
  bool setting_volt_calc = false;    // we are in the volt calc page
  bool setting_current_calc = false; // we are in the I calc page
  uint16_t glo_power_fwd = 0;        // power_fwd copy
  uint16_t glo_drive_power = 0;      // drive power from eeprom  
  uint16_t glo_volt_setting = 0;      // volt calc read from eeprom
  uint16_t glo_current_setting = 0;   // current calc read from eeprom
  uint16_t glo_power_set_value = 0;
  float glo_swr_display = 0;
  float glo_volt_display = 0;
  }


  //rotate the screen  eg. setScreenRotate(rotate270); // flip the screen
void setScreenRotate(byte angle)
{
   hmi.setVPWord(0x0081,((hmi.readVP(0x0081) & 0xFFFC) + angle));
   hmi.setVPWord(0x0080,0x5A00);  // set it.
}


void configureInterrupts() {
  mcp23017.interruptMode(MCP23017InterruptMode::Separated); 
  mcp23017.interrupt(MCP23017Port::B, CHANGE);  // Port B change int.
  mcp23017.clearInterrupts();  // reset ints
  attachInterrupt(digitalPinToInterrupt(INTB), mcp23017ChangeOnPortB, FALLING);  //active LOW
}

void mcp23017ChangeOnPortB(){
// todo not needed on this
}