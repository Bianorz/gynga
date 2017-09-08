## Comunicação I2C entre Raspberry II e Arduino

### Configuração Raspberry
Primeiramente é preciso editar o arquivo /etc/modules e adicionar ```i2c-dev``` para realizar o carregamento do módulo i2c no startup da raspberry
### Instalar I2C tools
A biblioteca encarregada de fazer o gerenciamento dos dispositivos i2c é a ```i2c-tools```, instale utilizando o seguite comando: 

- sudo apt-get install i2c-tools
### Autorizar acesso aos dispositivos I2C
É necessário dar autorização de acesso aos dispositivos i2c ao usuário, neste caso o usuário pi, para fazer isso utilize:

- sudo adduser pi i2c
### Ativar o I2C na raspberry
O i2c vem por default desabilitado na placa, siga os seguintes passos para realizar a sua ativação:

- sudo raspi-config
- Selecione opção 8 - *Advanced Options*
- Vá para *A7 I2C*
- Responda *yes* para habilitar I2C
- Marque *yes* de novo para carregar o kernel automaticamente
- Selecione *Finish*
- Selecione yes para reiniciar a placa.
### Teste de comunicação Raspberry - Arduino
Com o I2C ativado na raspberry, vamos agora testar a comunicação com um simples código onde a raspberry controla o estado de uma led no arduino. 
Grave o seguinte código no arduino:
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
Como usuremos um código em python para ser o mestre na raspberry, é preciso instalar o sm-bus, sm-bus é outro nome para i2c.

- sudo apt-get install python-smbus

E então executar o seguinte código na raspberry:
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

Se tudo ocorrer bem você vai ver o led 13 do arduino piscando, enquanto que no console do raspberry será impressa a mensagem ```Arduino answer to RPI:10``` 