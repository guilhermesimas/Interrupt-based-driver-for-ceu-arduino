
#include "SPI.h"

bool isAvailableFlag = 0;
uint8_t interruptSave = 0;
uint8_t usersCount = 0;
bool busy = false;

void SPI_setIsAvailable(bool flag);

void SPI_begin()
{
  // If first begin, initialize the application
  if(!usersCount){
    // Set SS to high so a connected chip will be "deselected" by default
    uint8_t port = digitalPinToPort(SS);
    uint8_t bit = digitalPinToBitMask(SS);
    volatile uint8_t *reg = portModeRegister(port);
    
    // if the SS pin is not already configured as an output
    // then set it high (to enable the internal pull-up resistor)
    if(!(*reg & bit)){
        digitalWrite(SS, HIGH);
    }
    
    // When the SS pin is set as OUTPUT, it can be used as
    // a general purpose output port (it doesn't influence
    // SPI operations).
    pinMode(SS, OUTPUT);
    
    SPCR |= _BV(MSTR);
    SPCR |= _BV(SPE);

    pinMode(SCK, OUTPUT);
    pinMode(MOSI, OUTPUT);
  }
  usersCount++;
}

void SPI_transactionBegin(uint32_t clock, uint8_t bitOrder, uint8_t dataMode){
  // In Arduino, operations to an 8-bit variable are atomic
  // Block untill available
  while(busy == true);

  busy = true;

  interruptSave = SREG;
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

void SPI_transactionEnd(){
  busy = false;
  SREG = interruptSave;
}

void SPI_end(void) {
    usersCount--;
    if(usersCount == 0){
      // disable SPI Module
      SPCR &= ~_BV(SPE);
    }
}

// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
void SPI_transferStart(uint8_t data) {
  Serial.println(">>>> tranferStart"); Serial.flush();
  isAvailableFlag = 0;
  SPCR |= _BV(SPIE);
  SPDR = data;
  // If this line is uncommented, application works
  // Serial.println("<<<< tranferStart"); Serial.flush();
}

uint8_t SPI_transferGetData( void ){
  SPI_setIsAvailable(false);
  SPCR &= ~_BV(SPIF);
  return SPDR;
}

void SPI_setIsAvailable(bool flag) { 
  isAvailableFlag = flag; 
}

bool SPI_available() {
  return isAvailableFlag; 
}


ISR( SPI_STC_vect ) {
  // Serial.println(">>>> INT"); Serial.flush();
  SPCR &= ~_BV(SPIE);
  SPI_setIsAvailable(true);
  // Serial.println("<<<< INT"); Serial.flush();
}
