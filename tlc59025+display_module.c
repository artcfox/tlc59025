#include <stdint.h>

#include "tlc59025+display_module.h"

const uint8_t tlc59025_digits[] PROGMEM = {0b00111111,  // 0
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
