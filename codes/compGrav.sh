#!/bin/bash

# compilar
avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o fileA.o $1.c
# linkar objetos
avr-gcc -mmcu=atmega328p fileA.o -o fileA
# converter para .hex
avr-objcopy -O ihex -R .eeprom fileA $1.hex
# limpar
rm fileA && rm fileA.o
# testar gravação
sudo avrdude -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v
# gravar
sudo avrdude -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v -U flash:w:$1.hex:i
