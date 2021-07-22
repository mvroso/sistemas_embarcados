#include <Arduino.h>

#include "memory_card.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

MemoryCard::MemoryCard(){
}

void MemoryCard::setup(int pino){
    pin = pino;
    pinMode(pin,OUTPUT);

    if(!SD.begin(pin)){
    Serial.println("Card Mount Failed");
    return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    // writeFile(SD, "/log.txt", "O Sistema foi iniciado!\n");
}

void MemoryCard::escrever_log(String mensagem){
    // appendFile(SD, "/log.txt", mensagem);
    // appendFile(SD, "/log.txt", "\n");
}

String MemoryCard::ler_log(void){
    // return readFile(SD, "/log.txt");
}
