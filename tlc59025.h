/*

  tlc59025.h

  Copyright 2015 Matthew T. Pandina. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY MATTHEW T. PANDINA "AS IS" AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHEW T. PANDINA OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.

*/

#pragma once

#include <stdint.h>
#include <avr/io.h>

// These options are not configurable because they rely on specific hardware
// features of the ATmega328P that are only available on specific pins.
#if (TLC59025_SPI_MODE == 0)
#define SDI_DDR DDRB
#define SDI_PORT PORTB
#define SDI_PIN PB3

#define CLK_DDR DDRB
#define CLK_PORT PORTB
#define CLK_PIN PB5
#elif (TLC59025_SPI_MODE == 1)
#define SDI_DDR DDRD
#define SDI_PORT PORTD
#define SDI_PIN PD1

#define CLK_DDR DDRD
#define CLK_PORT PORTD
#define CLK_PIN PD4
#elif (TLC59025_SPI_MODE == 2)
#define SDI_DDR DDRB
#define SDI_PORT PORTB
#define SDI_PIN PB1

#define CLK_DDR DDRB
#define CLK_PORT PORTB
#define CLK_PIN PB2
#endif // TLC59025_SPI_MODE

#define setOutput(ddr, pin) ((ddr) |= (1 << (pin)))
#define setLow(port, pin) ((port) &= ~(1 << (pin)))
#define setHigh(port, pin) ((port) |= (1 << (pin)))
#define getValue(port, pin) ((port) & (1 << (pin)))
#define togglePin(input, pin) ((input) = (1 << (pin)))
#define pulse(port, pin) do {                       \
                           setHigh((port), (pin));  \
                           setLow((port), (pin));   \
                         } while (0)

// Define a macro for SPI Transmit
#if (TLC59025_SPI_MODE == 0)
#define TLC59025_TX(data) do {                              \
                           SPDR = (data);                  \
                           while (!(SPSR & (1 << SPIF)));  \
                         } while (0)
#elif (TLC59025_SPI_MODE == 1)
#define TLC59025_TX(data) do {                                 \
                           while (!(UCSR0A & (1 << UDRE0)));  \
                           UDR0 = (data);                     \
                         } while (0)
#elif (TLC59025_SPI_MODE == 2)
#define TLC59025_TX(data) \
do {                                                                         \
  USIDR = (data);                                                            \
  uint8_t lo = (1 << USIWM0) | (0 << USICS0) | (1 << USITC);                 \
  uint8_t hi = (1 << USIWM0) | (0 << USICS0) | (1 << USITC) | (1 << USICLK); \
  USICR = lo; USICR = hi; USICR = lo; USICR = hi;                            \
  USICR = lo; USICR = hi; USICR = lo; USICR = hi;                            \
  USICR = lo; USICR = hi; USICR = lo; USICR = hi;                            \
  USICR = lo; USICR = hi; USICR = lo; USICR = hi;                            \
 } while (0)
#endif // TLC59025_SPI_MODE

void TLC59025_Init(void);
