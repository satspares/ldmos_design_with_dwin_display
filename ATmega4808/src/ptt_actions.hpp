#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif  

/* === read ptt and set tx delay  === */

void tx_actions()
{
  static bool tx_running;
  static bool display_dim;
 
     if ((digitalRead(PTT) == HIGH) || (error_vo_status) 
    || (error_temp_status) || (error_od_status_stop) || (swr_soft_trip) || (error_i_status)) 
    {  
    #ifdef A600_AMP
    a600_bias_off();
    #else
    dx_bias_off(); 
    #endif
    tx_status = false;
    tx_running = false;
    if (display_dim == true)
    {
     display_dim = false; 
     hmi.setPage(startPage);
     hmi.writeLongValue(LCDBackLightAddress,LCDBackLightLow);
    }
  }
  else if((digitalRead(PTT) == LOW) && (!tx_running) && (error_od_status_stop == false))
  {
 //   Serial.println("PTT_LOW");   // tx on
    tx_status = true;
    tx_running = true;
      // may not be needed if using solid state switching
     delay(TX_DELAY);
    // call for meters now
    temp_id_reset = true;
    peak_hold_reset = true;
    power_swr_reset = true;
    
    #ifdef A600_AMP
    a600_bias_on();
    #else
    dx_bias_on();
    #endif
    
    if (display_dim == false)
    {
      display_dim = true;
      hmi.writeLongValue(LCDBackLightAddress,LCDBackLightNormal);
      hmi.setPage(10);
    }
    usebeep?hmi.beepHMI(BEEP_YES):hmi.playSound(BEEP);

   }
  
}

void a600_bias_on(){
  digitalWrite(OPTOUT1,LOW);
  //mcp23017.digitalWrite(A600BIAS,false); //inverse
}
void a600_bias_off(){
  //mcp23017.digitalWrite(A600BIAS,true);
  digitalWrite(OPTOUT1,HIGH);
}
void dx_bias_on(){
  mcp23017.digitalWrite(DXBIAS,true);
}
void dx_bias_off(){
  mcp23017.digitalWrite(DXBIAS,false);
}