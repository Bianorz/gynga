## Instalação do Optiboot no Arduino utilizando avr-dude

Algumas vezes pode ser necessário a instalação de um novo bootloader no arduino, isso pode ocorrer devido a uma gravação pelo ISP ou por uma atualização do dispositivo. 
Primeiramente devemos baixar/compilar o novo bootloader a ser instalado, neste exemplo usaremos o  [optiboot](https://raw.githubusercontent.com/Bianorz/gynga/master/codes/optiboot.hex). 

### Programação utilizando ISP (ex: raspberry+avr/arduino)
Para programar o bootloader devemos primeiro setar os fuses do atmega/arduino para a gravação ser feita no espaço de memória destinado ao bootloader. Isto é feito com o seguinte comando:

- sudo avrdude -b 19200 -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v -U efuse:w:0x05:m -U hfuse:w:0xD6:m -U lfuse:w:0xFF:m
Flags: 
-b baudrate 
-p tipo do microcontrolador, 
-C arquivo de configuração, 
-c nome do gravador, 
-v impressão do processo no console, 
-U memtype:op:filename:format, faz uma operação na memória. Neste caso seta que o .hex será gravado no espaço destinado ao bootloader

Com o seguinte comando é feita a gravação do optiboot.hex, onde o último -U seta que novos arquivos.hex não serão gravados na memória do bootloader.


- sudo avrdude -b 19200 -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v -U flash:w:optiboot.hex -U lock:w:0x0F:m
### Com isso seu arduino poderá ser programado pela porta usb novamente