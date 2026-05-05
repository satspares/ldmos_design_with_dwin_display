#include <Arduino.h>
#ifndef INCLUDES_H
    #include <includes.h>
#endif

/* === read ptt and set tx delay  === */

void tx_actions() {
    static bool tx_running;
    static bool display_dim;

    if ((digitalRead(PTT) == HIGH) || (error_vo_status)|| 
        (error_temp_status) || 
        #ifndef DRIVE_NO_STOP 
        (error_od_status_stop) || 
        #endif
        (error_i_status) || 
        (swr2IntActiveHigh) || 
        (swr1IntActiveHigh)) {
#ifndef BIAS_ON
#ifdef A600_AMP
        a600_bias_off();
#else
        bias_off();
#endif
#endif
        tx_status = false;
        tx_running = false;
        if (display_dim == true) {
            display_dim = false;
            hmi.setPage(startPage);
            hmi.writeLongValue(LCDBackLightAddress, LCDBackLightLow);
        }
    } else if ((digitalRead(PTT) == LOW) && (!tx_running) && (error_od_status_stop == false)) {
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
        bias_on();
#endif
        usebeep ? hmi.beepHMI(BEEP_YES) : hmi.playSound(BEEP);
        if (display_dim == false) {
            display_dim = true;
            hmi.writeLongValue(LCDBackLightAddress, LCDBackLightNormal);
            hmi.setPage(txPage);
        }

    }

}

#ifdef A600_AMP
void a600_bias_on() {
    digitalWrite(OPTOUT1, LOW);
    swr2IntCount = 0;
    swr1IntCount = 0;
    mcp23017.digitalWrite(A600BIAS,false); //inverse
}
void a600_bias_off() {
    mcp23017.digitalWrite(A600BIAS,true);
    digitalWrite(OPTOUT1, HIGH);
}
#else
void bias_on() {
    swr2IntCount = 0;
    swr1IntCount = 0;
    mcp23017.digitalWrite(DXBIAS, true);
}
#endif

void bias_off() {
    mcp23017.digitalWrite(DXBIAS, false);
}

void PTTservice() {
    pttIntActive = true;
}
