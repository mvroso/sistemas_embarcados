#include <Arduino.h>
#include <DS1307.h>

#include "rtc.h"

Rtc::Rtc(){
}

void Rtc::setup(int pinos[]){
    pinSDA = pinos[0];
    pinSCL = pinos[1];

    DS1307 rtc(26, 25);
}

String Rtc::receberData(void){
    // return rtc.getDateStr(FORMAT_SHORT)
}

String Rtc::receberHora(void){
    // return rtc.getTimeStr();
}
