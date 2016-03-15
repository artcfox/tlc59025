/*

  tlc59025+display_module.h

  Copyright 2015-2016 Matthew T. Pandina. All rights reserved.

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
#include <avr/pgmspace.h>

#include "tlc59025.h"

#define DIGITS_IN_DISPLAY 4

extern const uint8_t tlc59025_digits[] PROGMEM;

// This function assumes buffer contains DIGITS_IN_DISPLAY characters
static inline void TLC59025_DisplayString(const char *buffer) {
  for (uint8_t d = DIGITS_IN_DISPLAY; d != 0; d--) {
    if (buffer[d - 1] >= '0' && buffer[d - 1] <= '9')
      TLC59025_TX(pgm_read_byte(&tlc59025_digits[buffer[d - 1] - '0']));
    else if (buffer[d - 1] == '-')
      TLC59025_TX(pgm_read_byte(&tlc59025_digits[10]));
    else
      TLC59025_TX(0x00);
  }

  TLC59025_WaitUntilTransmitComplete();
  pulse(LE_PORT, LE_PIN);
  setLow(OE_PORT, OE_PIN);
}

/* itoar:  convert n to characters in s, reversed */
static inline void itoar(int32_t n, char s[])
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

static inline void TLC59025_DisplayNumber(const int16_t number) {
  char buffer[12] = {0};
  itoar(number, buffer);
  for (uint8_t d = 0; d < DIGITS_IN_DISPLAY; ++d) {
    if (buffer[d] >= '0' && buffer[d] <= '9')
      TLC59025_TX(pgm_read_byte(&tlc59025_digits[buffer[d] - '0']));
    else if (buffer[d] == '-')
      TLC59025_TX(pgm_read_byte(&tlc59025_digits[10]));
    else
      TLC59025_TX(0x00);
  }

  TLC59025_WaitUntilTransmitComplete();
  pulse(LE_PORT, LE_PIN);
  setLow(OE_PORT, OE_PIN);
}
