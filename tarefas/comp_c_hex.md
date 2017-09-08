## Compilar código c, gerar arquivo HEX utilizando gcc-avr e programar no microcontrolador com avr-dude

Nesta tarefa será demonstrado o passo a passo para realizar a compilação de um código em C utilizando o compilador gcc-avr, criação do arquivo .hex e gravação do .hex em um microcontrolador ATMEGA328 utilizando o avrdude.
### Instalação do compilador e referências
Primeiramente é necessário instalar as bibliotecas que serão usadas no processo de compilação:

- sudo apt-get install avrdude-doc binutils-avr avr-libc gcc-avr
### Código para teste
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

### Processo de Compilação
Os seguintes comandos devem ser executados para realizar o processo, basta substituir led.c e led.hex para o nome dos arquivos que você queira compilar e gerar respectivamente.

- avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o led.o led.c
Compilo o arquivo e gero um *object file*. Flags -Os para optimização de tamanho, -DF_CPU para o clock do Atmega, -mmcu para o tipo de microcontrolador, -c para compilar e não linkar os objetos, -o nome do objeto.
- avr-gcc -mmcu=atmega328p led.o -o led
Linkar objeto a bibliotecas do sistema, gerando arquivo led
- avr-objcopy -O ihex -R .eeprom led led.hex && rm led && rm led.o
Converter o arquivo led, para formato .hex. Flags: -O formato do arquivo de saída, -R remove dados que não são úteis ao arquivo .hex

### Processo de Gravação

Com o arquivo .hex gerado é possível realizar a sua gravação no microcontrolador. Será utilizado como gravador um Raspberry Pi com ISP, o tutorial para configuração do raspberry como gravador está disponível nesse [link](https://github.com/Bianorz/gynga/blob/master/tarefas/prog_rasp_avr_isp.md)
Primeiramente testaremos a comunicação entre o avr e o gravador raspberry com esse comando:

-  sudo avrdude -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v
Flags -p tipo do microcontrolador, -C arquivo de configuração, -c nome do gravador, -v impressão do processo no console

Com o teste comunicação bem sucedido, agora será feito a gravação do arquivo no microcontrolador com o seguinte comando:

- sudo avrdude -p atmega328p -C ~/avrdude_gpio.conf -c pi_1 -v -U flash:w:led.hex:i
Flags -U memtype:op:filename:format, faz uma operação na memória, no caso acima, habilita a memória flash (memtype=flash) para escrita (w) utilizando o arquivo led.hex (filename=led.hex), que é um arquivo do tipo Intel Hex (format=i). O valor default para o formart é usar detecção automática para arquivos de entrada e o formato binário raw para arquivos de saída.

Com isso temos o arquivo led.hex gravado no microcontrolador AtMega328p

### Script para automação do processo (compilação+gravação)

Para automatizar essa sequência de passos descritos acima foi feito um simples shell script para realizar a compilação e gravação do código. O único parâmetro de entrada será o arquivo .c, então *feel free* para altera-lo em caso de mudança de microcontrolador, frequência da cpu, etc.
```shell
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
```




