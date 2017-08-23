// volatile bool available;

#include <SPI.h>

#define PIN_SCK  13
#define PIN_MISO 12
#define PIN_DEVICE 9


void setup(){
    // available = false;
    pinMode(PIN_SCK,OUTPUT);
    pinMode(PIN_MISO,INPUT);
    pinMode(PIN_DEVICE,OUTPUT);
    pinMode(4,OUTPUT);


    Serial.begin(9600);
    SPI_begin();
    Serial.println("=======================");

}

void loop(){
    // Setting up transition for device to get data ready
    digitalWrite(PIN_DEVICE,LOW);
    delay(50);
    digitalWrite(PIN_DEVICE,HIGH);
    delay(50);
    SPI_beginTransaction(1400000, MSBFIRST, SPI_MODE2);
    //Start transfer
    SPI_transferStart(0x0);
    // while(!SPI.available()) ;
    while(!SPI_available()){
    //   Serial.print("") ;
        // asm volatile("nop");

        // delay(10);
        // delay(0);
        // delayMicroseconds(100);
    } 
    // SPI.transfer(0x0);
    Serial.print("Data is:");Serial.println((uint8_t)SPI_transferGetData(),BIN);
    SPI_endTransaction();
}
