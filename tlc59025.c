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

#include "tlc59025.h"

void TLC59025_Init(void)
{
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
