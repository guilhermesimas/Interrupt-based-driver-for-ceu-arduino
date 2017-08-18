#include <SPI.h>

#define PIN_SCK  13
#define PIN_MISO 12
#define PIN_MOSI 11
#define PIN_DEVICE_INPUT 9
#define PIN_DEVICE_OUTPUT 8

// Macro used to print bytes in 8 digits format
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


void setup(){
    pinMode(PIN_SCK,OUTPUT);
    pinMode(PIN_MISO,INPUT);
    pinMode(PIN_MOSI,INPUT);
    pinMode(PIN_DEVICE_INPUT,OUTPUT);
    pinMode(PIN_DEVICE_OUTPUT,OUTPUT);


    Serial.begin(9600);
    SPI_begin();
    Serial.println("=======================");

}

void loop(){
    // Setting up transition for device to get data ready
    digitalWrite(PIN_DEVICE_INPUT,LOW);
    digitalWrite(PIN_DEVICE_OUTPUT,LOW);
    delay(50);
    digitalWrite(PIN_DEVICE_INPUT,HIGH);
    delay(50);
    // Wait 3 seconds
    SPI_config(1400000, MSBFIRST, SPI_MODE0);
    delay(3000);
    //Start transfer
    SPI_transferStart(0x0);
    //Wait
    delay(1);
    while(!SPI_available());
    uint8_t value = (uint8_t)SPI_transferGetData();
    delay(10);
    //SPI_end();

    // Display data
    char value_display [9] = {0};
    sprintf(value_display,BYTE_TO_BINARY_PATTERN,BYTE_TO_BINARY(value));
    Serial.print("Data is <");Serial.print(value_display);Serial.println(">");
    // Setting up transition for device to get data ready
    SPI_config(1400000, MSBFIRST, SPI_MODE0);
    // Output data in binary form, invert so 1 is ON and 0 is OFF
    SPI_transferStart( ~( (uint8_t) value ) );
    int nCycles = 0;
    delay(1);
    while(!SPI_available()){
        //delay(1000);
        Serial.println("Iteration");
        nCycles++;
    }
    Serial.print("Cycles saved <");Serial.print(nCycles);Serial.println(">");
    delay(10);
    //SPI_end();
    // Transition from LOW to HIGH so data is latched
    digitalWrite(PIN_DEVICE_OUTPUT,HIGH);
    delay(50);
}

