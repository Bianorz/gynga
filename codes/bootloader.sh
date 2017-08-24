#!/bin/bash
# Test the communicattion between ISP programmer and AVR microcontroller
sudo avrdude -p atmega328p -C avrdude_gpio.conf -c pi_1 -v
# Set fuses for bootloader memory
sudo avrdude -b 19200 -p atmega328p -C avrdude_gpio.conf -c pi_1 -v -U efuse:w:0x05:m -U hfuse:w:0xD6:m -U lfuse:w:0xFF:m
# Program the bootloader file
sudo avrdude -b 19200 -p atmega328p -C avrdude_gpio.conf -c pi_1 -v -U flash:w:$1 -U lock:w:0x0F:m

