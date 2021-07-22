#include <Arduino.h>

#include "remote_control.h"


RemoteControl::RemoteControl(){
}

void RemoteControl::setup(int pino){
    pin = pino;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
}

void RemoteControl::acionar(void){
    digitalWrite(23,LOW);
    Serial.println("Portão aberto");
    delay(500);
    digitalWrite(23,HIGH);
}