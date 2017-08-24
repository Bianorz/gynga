# Repositório UFRN - Gynga

## Configuração Raspberry PI 2 como ISP AVR Programmer
<span style="color:red">ATENÇÃO, GRAVAR O CÓDIGO ATRAVÉS DO ISP APAGA O BOOTLOADER DO ARDUINO/AVR!
</span> <br />É mostrado uma forma de recuperar o bootloader mais adiante no texto.


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
### Ative o I2C na raspberry

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
```
