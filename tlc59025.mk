# ---------- Begin TLC59025 Configuration Section ----------

# Defines the number of TLC59025 chips that are connected in series
TLC59025_N = 2

# Setting to select among, normal SPI Master mode, USART in MSPIM mode,
# or USI mode to communicate with the TLC59025. Refer to the schematics
# that have -spi-mode-0, -spi-mode-1, or -spi-mode-2 in their filenames
# for details on how to connect the hardware before changing this setting.
# One major advantage of using the USART in MSPIM mode is that its
# transmit register is double-buffered, so you can send data to the
# TLC59025 much faster.
#  0 = Use normal SPI Master mode to communicate with TLC59025 (slower)
#  1 = Use the USART in double-buffered MSPIM mode to communicate with
#      the TLC59025 (faster, but requires the use of different hardware
#      pins)
#  2 = Use the USI (Universal Serial Interface) found on the ATtiny*
#      family of chips.
#
#      Ex: CLOCK = 16000000
#          DEVICE = attiny85
#          FUSES = -U hfuse:w:0xdf:m -U lfuse:w:0xa1:m
#          OE_PIN = PB3
#          LE_PIN = PB0
#
# WARNING: If you change this setting, you must also change your physical
#          hardware configuration to match.
TLC59025_SPI_MODE = 2

# When OE is high, all outputs of the TLC59025 chip(s) will be
# disabled, and when OE is low, all outputs will be enabled. There
# must be an external 10K pull-up resistor attached to this pin. Choose
# this pin assignment wisely, so that during ICSP programming, OE
# remains high to keep the outputs disabled.
#
# WARNING: For an ATtiny85, when TLC59025_SPI_MODE = 2, OE_PIN
#          should be assigned to PB3 so the outputs are blanked during
#          programming.
#
OE_DDR = DDRB
OE_PORT = PORTB
OE_INPUT = PINB
OE_PIN = PB3

# DDR, PORT, and PIN connected to LE.
LE_DDR = DDRB
LE_PORT = PORTB
LE_INPUT = PINB
LE_PIN = PB0

# ---------- End TLC59025 Configuration Section ----------

# ---------- DO NOT MODIFY BELOW THIS LINE ----------

# This line integrates all options into a single flag called:
#     $(TLC59025_DEFINES)
# which should be appended to the definition of COMPILE in the Makefile
TLC59025_DEFINES = -D__DELAY_BACKWARD_COMPATIBLE__ \
                   -DTLC59025_N=$(TLC59025_N) \
                   -DTLC59025_SPI_MODE=$(TLC59025_SPI_MODE) \
                   -DOE_DDR=$(OE_DDR) \
                   -DOE_PORT=$(OE_PORT) \
                   -DOE_INPUT=$(OE_INPUT) \
                   -DOE_PIN=$(OE_PIN) \
                   -DLE_DDR=$(LE_DDR) \
                   -DLE_PORT=$(LE_PORT) \
                   -DLE_INPUT=$(LE_INPUT) \
                   -DLE_PIN=$(LE_PIN) \
