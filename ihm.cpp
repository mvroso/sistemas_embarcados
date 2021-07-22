#include <Arduino.h>

#include "ihm.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

Ihm::Ihm(){
}

void Ihm::setup(int port){
/*get_config_parameters(ssid, password); //pega o ssid e senha do arquivo de setup

    Serial.println("Rede:");
    Serial.printf("%s\n",ssid);
    Serial.println("Rede:");
    Serial.printf("%s\n",password);
 

    WiFi.begin(ssid, password); //inicializa WiFi
    

    // WiFi.connect(ssid); //Conecta à rede WiFi
    while (WiFi.status() != WL_CONNECTED);
    server.begin();
    Serial.println(WiFi.localIP());
*/
}
