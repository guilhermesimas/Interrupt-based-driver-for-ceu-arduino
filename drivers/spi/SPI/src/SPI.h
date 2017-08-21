#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>
#include <avr/interrupt.h>

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

/*
 * Enables the SPI Module
 */
void SPI_begin();

/*
 * Disables the SPI Module
 */
void SPI_end(void);

/*
 * Configures the SPI module and holds control of the driver
 */
void SPI_transactionBegin(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);

/*
 * Yields control of the driver
 */
void SPI_transactionEnd();

/*
 * Starts an 8-bit transfer asynchronously
 */
void SPI_transferStart(uint8_t data);

/*
 * Returns the latest received data
 */
uint8_t SPI_transferGetData( void );

/*
 * Returns whether Data is available or not
 */
bool SPI_available();

#endif

