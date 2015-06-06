/*

  main.c

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

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

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

void TLC59025_Init(void) {
  setOutput(CLK_DDR, CLK_PIN);
  setLow(CLK_PORT, CLK_PIN);

  setOutput(LE_DDR, LE_PIN);
  setLow(LE_PORT, LE_PIN);

  // setHigh called first to ensure OE doesn't briefly go low
  setHigh(OE_PORT, OE_PIN);
  setOutput(OE_DDR, OE_PIN);

  setOutput(SDI_DDR, SDI_PIN);

#if (TLC59025_SPI_MODE == 0)
#if (TLC59025_PB2_UNMAPPED == 1)
  setOutput(PORTB, PB2); // PB2 must be an output to remain in SPI Master Mode
#endif
#elif (TLC59025_SPI_MODE == 2)
  setLow(SDI_PORT, SDI_PIN); // since USI only toggles, start in known state
#endif // TLC59025_SPI_MODE

#if (TLC59025_SPI_MODE == 0)
  // Enable SPI, Master, set clock rate fck/2
  SPCR = (1 << SPE) | (1 << MSTR);
  SPSR = (1 << SPI2X);
#elif (TLC59025_SPI_MODE == 1)
  // Baud rate must be set to 0 prior to enabling the USART as SPI
  // master, to ensure proper initialization of the XCK line.
  UBRR0 = 0;
  // Set USART to Master SPI mode.
  UCSR0C = (1 << UMSEL01) | (1 << UMSEL00);
  // Enable TX only
  UCSR0B = (1 << TXEN0);
  // Set baud rate. Must be set _after_ enabling the transmitter.
  UBRR0 = 0;
#endif // TLC59025_SPI_MODE
}

uint8_t digits[] = {0b00111111,  // 0
                    0b00000110,  // 1
                    0b01011011,  // 2
                    0b01001111,  // 3
                    0b01100110,  // 4
                    0b01101101,  // 5
                    0b01111101,  // 6
                    0b00000111,  // 7
                    0b01111111,  // 8
                    0b01101111,  // 9
                    0b01000000}; // -

/* itoar:  convert n to characters in s, reversed */
void itoar(int32_t n, char s[])
{
  int32_t i, sign;
 
  if ((sign = n) < 0)  /* record sign */
    n = -n;            /* make n positive */
  i = 0;
  do {       /* generate digits in reverse order */
    s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);   /* delete it */
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
}

void TLC59025_DisplayNumber(const uint16_t number) {
  char buf[5] = {0};
  itoar(number, buf);
  for (uint8_t d = 0; d < 4; ++d) {
    if (buf[d] >= '0' && buf[d] <= '9')
      TLC59025_TX(digits[buf[d] - '0']);
    else
      TLC59025_TX(0x00);
  }
  pulse(LE_PORT, LE_PIN);
  setLow(OE_PORT, OE_PIN);
}

int main(void) {
  TLC59025_Init();

  for (;;) {
    for (uint16_t i = 0; i < 10000; ++i) {
      TLC59025_DisplayNumber(i);
      _delay_ms(25);
    }
  }
  /*
  for (;;) {
    for (uint16_t i = 0; i < 32; ++i) {
      uint32_t x = (uint32_t)1 << i;
      TLC59025_TX((uint8_t)x);
      TLC59025_TX((uint8_t)(x >> 8));
      TLC59025_TX((uint8_t)(x >> 16));
      TLC59025_TX((uint8_t)(x >> 24));
      pulse(LE_PORT, LE_PIN);
      setLow(OE_PORT, OE_PIN);
      _delay_ms(500);
    }
  }
  */

  /*
  TLC59025_TX(0xFF);
  TLC59025_TX(0xFF);
  TLC59025_TX(0xFF);
  TLC59025_TX(0xFF);
  pulse(LE_PORT, LE_PIN);
  setLow(OE_PORT, OE_PIN);

  for (;;);
  */
  return 0;
}
