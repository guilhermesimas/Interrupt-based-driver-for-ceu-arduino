#include <SPI.h>

#define PIN_SCK  13
#define PIN_MOSI 11
#define PIN_DEVICE 9

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
    pinMode(PIN_MOSI,INPUT);
    pinMode(PIN_DEVICE,OUTPUT);


    Serial.begin(9600);
    SPI.begin();
    // mark new program execution
    Serial.println("============================================");

}

void loop(){
    digitalWrite(PIN_DEVICE,LOW);
    // Read value to send
    Serial.println("Input data now:");
    while(!Serial.available() );
    char value = Serial.read();
    // Display data
    char value_display [9] = {0};
    sprintf(value_display,BYTE_TO_BINARY_PATTERN,BYTE_TO_BINARY(value));
    Serial.print("Data is <");Serial.print(value_display);Serial.println(">");
    // Setting up transition for device to get data ready
    SPI.beginTransaction(SPISettings(1400000, MSBFIRST, SPI_MODE0));
    // Output data in binary form, invert so 1 is ON and 0 is OFF
    SPI.transfer( ~( (uint8_t) value ) );
    delay(10);
    SPI.endTransaction();
    delay(50);
    // Transition from LOW to HIGH so data is latched
    digitalWrite(PIN_DEVICE,HIGH);
    delay(50);
}
