# Repositório UFRN - Gynga

## Configuração Raspberry PI 2 como ISP AVR Programmer
<span style="color:red">ATENÇÃO, GRAVAR O CÓDIGO ATRAVÉS DO ISP APAGA O BOOTLOADER DO ARDUINO/AVR!
</span> <br />Para recuperar o bootloader, siga esse [link](https://github.com/Bianorz/gynga/blob/master/docs.md#instala%C3%A7%C3%A3o-do-optiboot-no-arduino-utilizando-avr-dude). 


### Adicionar repositório da Adafruit
- curl -sLS https://apt.adafruit.com/add | sudo bash
### Instalar o avrdude
- sudo apt-get install avrdude
### Verificar instalação
- avrdude -v
### Conectar cabos VCC, GROUND, MOSI, MISO, SCK e RESET do arduino/avr nos GPIO da raspberry. Nesse exemplo faremos a seguinte conexão:
- Arduino ICSP VCC para Raspberry Pi 5 volt pin.
- Arduino ICSP GND para Raspberry Pi ground pin.
- Arduino ICSP RESET para Raspberry Pi GPIO #12.
- Arduino ICSP SCK para Raspberry Pi GPIO #24.
- Arduino ICSP MOSI para Raspberry Pi GPIO #23.
- Arduino ICSP MISO para Raspberry Pi GPIO #18.
### É necessário editar um arquivo .conf especificando quais os pinos da raspberry serão utilizados para programar o arduino/avr.
- cp /etc/avrdude.conf ~/avrdude_gpio.conf <br />
- nano ~/avrdude_gpio.conf
### Quando o editor abrir o arquivo, desça até o final dele e adicione o código abaixo
    # Linux GPIO configuration for avrdude.
    # Change the lines below to the GPIO pins connected to the AVR.
    programmer
      id    = "pi_1";
      desc  = "Use the Linux sysfs interface to bitbang GPIO lines";
      type  = "linuxgpio";
      reset = 12;
      sck   = 24;
      mosi  = 23;
      miso  = 18;
    ;
### Tudo pronto para iniciar a gravação do arduino/avr. Utilize o seguinte comando para testar a comunicação arduino/avr e raspberry
- sudo avrdude -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v
### Para programar o arduino/avr utilize o comando abaixo, substituindo 'Blink.cpp.hex' pelo nome do arquivo .hex que se deseja programar.
- sudo avrdude -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v -U flash:w:Blink.cpp.hex:i

## Comunicação I2C entre Raspberry e Arduino

### Para realizar a comunicação I2C entre raspberry e arduino, primeiramente devemos ativar o I2C na raspberry. Edite o arquivo /etc/modules adicionando *i2c-dev* ao fim do arquivo
- sudo nano /etc/modules
### Instalar I2C tools
- sudo apt-get install i2c-tools
### Autorizar acesso aos dispositivos I2C pelo usuário pi
- sudo adduser pi i2c
### Ativar o I2C na raspberry

- sudo raspi-config
- Selecione opção 8 - *Advanced Options*
- Vá para *A7 I2C*
- Responda *yes* para habilitar I2C
- Marque *yes* de novo para carregar o kernel automaticamente
- Selecione *Finish*
- Selecione yes para reiniciar a placa.
### Foi utilizado o seguinte código para testar a comunicação. No arduino temos:
```c
#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int number = 0;
int state = 0;

void setup(){
  pinMode(13, OUTPUT);
  Serial.begin(9600); // start serial for output
// initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);
// define callbacks for i2c communication
  digitalWrite(13, LOW);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("Ready!");
}

void loop() {
delay(100);
}

// callback for received data
void receiveData(int byteCount){
  while(Wire.available()) {
    number = Wire.read();
    Serial.print("data received: ");
    Serial.println(number);
      if (number == 1){
          digitalWrite(13, HIGH);
      }// set the LED on
      else{
          digitalWrite(13, LOW); // set the LED off
        }
      }
   }

// callback for sending data
void sendData(){
Wire.write(10);
}
```
### Na raspberry temos primeiro que instalar o smBus
- sudo apt-get install python-smbus
### E então executar o seguinte código:
```python
import RPi.GPIO as gpio
import smbus
import time
import sys
 
bus = smbus.SMBus(1)
address = 0x04
 
def main():
    gpio.setmode(gpio.BCM)
    gpio.setup(17, gpio.OUT)
    status = False
    while 1:
        gpio.output(17, status)
        status = not status
        bus.write_byte(address, 1 if status else 0)
        print "Arduino answer to RPI:", bus.read_byte(address)
        time.sleep(1)
 
 
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print 'Interrupted'
        gpio.cleanup()
        sys.exit(0)
```
## Compilar código C e gerar arquivo HEX utilizando gcc-avr 

### Instalação do compilador e referências
- sudo apt-get install avrdude-doc binutils-avr avr-libc gcc-avr
### Neste exemplo será gerado o arquivo HEX do seguinte código:
```c
#include <avr/io.h>                        
#include <util/delay.h>                    

int main(void) {

  // Configurar PB5 (Digital 13 no arduino) como saída
  DDRB |= 0b00100000;            
 
  while (1) {

    PORTB = 0b00100000;          // Ligar PB5
    _delay_ms(250);                                           

    PORTB = 0b00000000;          // Desligar PB5
    _delay_ms(250);                                          

  }                                                  
  return 0;                            
}
```
### Substitua led.c e led.hex para os nomes dos arquivos que você deseja compilar(.c) e criar(.hex):

- avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o led.o led.c
- avr-gcc -mmcu=atmega328p led.o -o led
- avr-objcopy -O ihex -R .eeprom led led.hex && rm led && rm led.o
## Instalação do Optiboot no Arduino utilizando avr-dude

### Baixe o arquivo do bootloader: [optiboot.hex](https://raw.githubusercontent.com/Bianorz/gynga/master/codes/led.hex) 
### Programe utilizando ISP (ex: raspberry+avr/arduino) com os seguintes comandos
- sudo avrdude -b 19200 -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v -U efuse:w:0x05:m -U hfuse:w:0xD6:m -U lfuse:w:0xFF:m
- sudo avrdude -b 19200 -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v -U flash:w:optiboot.hex -U lock:w:0x0F:m
### Com isso seu arduino poderá ser programado pela porta usb novamente
