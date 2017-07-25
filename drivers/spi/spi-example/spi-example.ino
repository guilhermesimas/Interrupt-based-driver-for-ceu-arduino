#include <SPI.h>

#define PIN_SCK  13
#define PIN_MISO 12
#define PIN_DEVICE 9

void setup(){
    pinMode(PIN_SCK,OUTPUT);
    pinMode(PIN_MISO,INPUT);
    pinMode(PIN_DEVICE,OUTPUT);


    Serial.begin(9600);
    SPI.begin();

}

void loop(){
    // Setting up transition for device to get data ready
    digitalWrite(PIN_DEVICE,LOW);
    delay(50);
    digitalWrite(PIN_DEVICE,HIGH);
    delay(50);
    // Wait 3 seconds
    SPI.beginTransaction(SPISettings(1400000, MSBFIRST, SPI_MODE2));
    delay(3000);
    Serial.println((uint8_t)SPI.transfer(NULL),BIN);
    SPI.endTransaction();
    delay(10);
}