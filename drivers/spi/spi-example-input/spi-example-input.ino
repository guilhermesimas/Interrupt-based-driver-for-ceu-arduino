bool isAvailableFlag = 0;

void SPI_begin()
{
  SPCR |= _BV(MSTR);
  SPCR |= _BV(SPE);

  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
}

#define LSBFIRST 0
#define MSBFIRST 1

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

void SPI_config(uint32_t clock, uint8_t bitOrder, uint8_t dataMode){
  // Clock settings are defined as follows. Note that this shows SPI2X
  // inverted, so the bits form increasing numbers. Also note that
  // fosc/64 appears twice
  // SPR1 SPR0 ~SPI2X Freq
  //   0    0     0   fosc/2
  //   0    0     1   fosc/4
  //   0    1     0   fosc/8
  //   0    1     1   fosc/16
  //   1    0     0   fosc/32
  //   1    0     1   fosc/64
  //   1    1     0   fosc/64
  //   1    1     1   fosc/128

  // We find the fastest clock that is less than or equal to the
  // given clock rate. The clock divider that results in clock_setting
  // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
  // slowest (128 == 2 ^^ 7, so clock_div = 6).
  uint8_t clockDiv;

  // When the clock is known at compiletime, use this if-then-else
  // cascade, which the compiler knows how to completely optimize
  // away. When clock is not known, use a loop instead, which generates
  // shorter code.
  if (__builtin_constant_p(clock)) {
    if (clock >= F_CPU / 2) {
      clockDiv = 0;
    } else if (clock >= F_CPU / 4) {
      clockDiv = 1;
    } else if (clock >= F_CPU / 8) {
      clockDiv = 2;
    } else if (clock >= F_CPU / 16) {
      clockDiv = 3;
    } else if (clock >= F_CPU / 32) {
      clockDiv = 4;
    } else if (clock >= F_CPU / 64) {
      clockDiv = 5;
    } else {
      clockDiv = 6;
    }
  } else {
    uint32_t clockSetting = F_CPU / 2;
    clockDiv = 0;
    while (clockDiv < 6 && clock < clockSetting) {
      clockSetting /= 2;
      clockDiv++;
    }
  }

  // Compensate for the duplicate fosc/64
  if (clockDiv == 6)
  clockDiv = 7;

  // Invert the SPI2X bit
  clockDiv ^= 0x1;

  SPCR = _BV(SPE) | _BV(MSTR) | ((bitOrder == LSBFIRST) ? _BV(DORD) : 0) |
    (dataMode & SPI_MODE_MASK) | ((clockDiv >> 1) & SPI_CLOCK_MASK);
  SPSR = clockDiv & SPI_2XCLOCK_MASK;
}

// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
void SPI_transferStart(uint8_t data) {
  SPCR |= _BV(SPIE);
  isAvailableFlag = 0;
  SPDR = data;
}

uint8_t SPI_transferGetData( void ){
  
  return SPDR;
}

void SPI_setIsAvailable(bool flag) { 
  //Serial.println(flag);
  isAvailableFlag = flag; 
}

bool SPI_available() {
  //Serial.println(isAvailableFlag);
  return isAvailableFlag; 
}


ISR( SPI_STC_vect ) {
  SPCR &= ~_BV(SPIE);
  SPI_setIsAvailable(1);
}


#define PIN_SCK  13
#define PIN_MISO 12
#define PIN_DEVICE 9

void setup(){
    pinMode(PIN_SCK,OUTPUT);
    pinMode(PIN_MISO,INPUT);
    pinMode(PIN_DEVICE,OUTPUT);


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
    // Wait 3 seconds
    SPI_config(1400000, MSBFIRST, SPI_MODE2);
    delay(3000);
    //Start transfer
    SPI_transferStart(0x0);
    //Wait
    while(!SPI_available());
    Serial.print("Data is:");Serial.println((uint8_t)SPI_transferGetData(),BIN);
    delay(10);
}

